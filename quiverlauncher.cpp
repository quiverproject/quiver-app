#include "quiverlauncher.h"

QuiverLauncher::QuiverLauncher(QObject *parent) : QObject(parent)
{
        //FIXME load projects list from QSettings and run them through addProjectWithDirpath()
}

void QuiverLauncher::fileDropped(const QList<QUrl> &urls) {
        foreach (const QUrl &in_url, urls) {
                QUrl url = in_url;
#ifdef Q_OS_MAC
                url = MacUrlConvert::convertUrl(in_url);
#endif
                QString dirpath = url.toLocalFile();
                addProjectWithDirpath(dirpath);
        }
}

void QuiverLauncher::addProjectWithUrl(QUrl url) {
        return addProjectWithDirpath(url.toLocalFile());
}

void QuiverLauncher::addProjectWithDirpath(QString dirpath) {
        QFileInfo info(dirpath);
        //if this is a .pro file, switch to using the directory containing it
        if (info.isFile()) {
                if (!info.fileName().endsWith(".pro")) {
                        return;
                }
                dirpath = info.path();
                info.setFile(info.path());
        }

        //directory must contain a directory called quiver and exactly one .pro file
        QDir directory(dirpath);
        bool found_quiver = false;
        bool found_qml_dir = false;
        int found_pro_files = 0;
        foreach (const QString &filename, directory.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (filename.toLower() == "quiver" and QFileInfo(filename).isDir()) {
                        found_quiver = true;
                } else if (filename.toLower() == "qml" and QFileInfo(filename).isDir()) {
                        found_qml_dir = true;
                } else if (filename.toLower().endsWith(".pro") && !QFileInfo(filename).isDir()) {
                        ++found_pro_files;
                }
        }
        if (!found_quiver) return;
        if (found_pro_files != 1) return;

        auto project = new Project(directory.dirName(), directory.path());

        //add platforms to project
        QDir qmldir(dirpath + "/qml");
        foreach (const QString &os, qmldir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (os == "common") continue;

                auto platform = new Platform(os);

                //add configs to platform
                QDir configdir(dirpath + "/Quiver/config");
                foreach (const QFileInfo &info, configdir.entryInfoList(QDir::Files)) {
                        if (!info.fileName().endsWith(".conf")) continue;

                        //parse config
                        QFile config_file(info.filePath());
                        if (!config_file.open(QIODevice::ReadOnly)) continue;
                        QTextStream stream(&config_file);
                        QHash<QString,QString> config;
                        while (!stream.atEnd()) {
                                QString line = stream.readLine();
                                QRegExp name_string_regex("^\\s*(\\w+)\\s*:\\s*\"(.*)\"\\s*$");
                                if (line.indexOf(name_string_regex) != -1) {
                                        QString key = name_string_regex.cap(1);
                                        QString value = name_string_regex.cap(2).replace("\\", "");
                                        config[key] = value;
                                }
                        }
                        config_file.close();

                        if (config.value("os").isEmpty() || config.value("name").isEmpty()) continue;
                        if (config.value("os") != os) continue;

                        platform->addConfig(new Config(config.value("name"),
                                                       config.value("os"),
                                                       info.fileName().replace(QRegExp("\\.conf$"), "")
                                                       ));
                }

                project->addPlatform(platform);
        }

        //FIXME save dirpath in our QSettings list of projects

        addProject(project);
}

void QuiverLauncher::addProject(Project *project) {
        m_projects << project;
        emit projectsChanged();
}


void QuiverLauncher::launch(const QString &project_id) {
        //FIXME
        QString qmake_path("/Users/njahnke/Qt/5.3/clang_64/bin/qmake");

        //look up the project object based on the project_id
        Project *project = nullptr;
        foreach (QObject *o, m_projects) {
                auto p = qobject_cast<Project *>(o);
                if (!p) continue;
                if (p->id() == project_id) {
                        project = p;
                        break;
                }
        }
        if (!project) {
                qDebug() << this << "launch(): fatal: project id" << project_id << "not found!";
                return;
        }


        //build the project
        builddirpath = QString("%1/../build-quiver-%2").arg(project->id()).arg(project->name());
        QDir builddir(builddirpath);
        if (!builddir.exists()) builddir.mkpath(builddir.path());

        QProcess qmake_process;
        qmake_process.setWorkingDirectory(builddir.path());
        qmake_process.setProgram(qmake_path);
        qmake_process.setArguments(QStringList()<<project->id());
        qmake_process.start();
        qmake_process.waitForStarted();
        qmake_process.waitForFinished();

        QProcess make_process;
        make_process.setWorkingDirectory(builddir.path());
        make_process.setProgram("/usr/bin/make");
        make_process.setArguments(QStringList()<<"-j"<<"4"); //FIXME
        make_process.start();
        make_process.waitForStarted();
        //FIXME handle errors
        make_process.waitForFinished();

        //launch an instance of the project with each checked platform and config
        processes_to_launch.clear();
        foreach (QObject *o, project->platforms()) {
                auto platform = qobject_cast<Platform *>(o);
                if (!platform) continue;
                if (!platform->enabled()) continue;

                foreach (QObject *o, platform->configs()) {
                        auto config = qobject_cast<Config *>(o);
                        if (!config) continue;
                        if (!config->enabled()) continue;

                        Process_Template process_template;
                        process_template["project_id"] = project->id();
                        process_template["project_name"] = project->name();
                        process_template["config_id"] = config->id();
                        process_template["platform_name"] = platform->name();
                        processes_to_launch << process_template;
                }
        }

        launch_next_process();
}

void QuiverLauncher::launch_next_process() {
        if (!processes_to_launch.size()) {
                return;
        }

        Process_Template process_template = processes_to_launch.takeFirst();

        QString project_id = process_template.value("project_id");
        QString project_name = process_template.value("project_name");
        QString config_id = process_template.value("config_id");
        QString platform_name = process_template.value("platform_name");


        //read mainqmltemplate
        QFile mainqmltemplatefile(QString("%1/Quiver/mainqmltemplate").arg(project_id));
        if (!mainqmltemplatefile.exists()) {
                qDebug() << this << "fatal: mainqmltemplate does not exist in project directory's Quiver directory";
                return;
        }
        mainqmltemplatefile.open(QIODevice::ReadOnly);
        QTextStream maintemplatestream(&mainqmltemplatefile);
        QString mainqmltemplate = maintemplatestream.readAll();
        mainqmltemplatefile.close();


        QString qml = mainqmltemplate;
        QFile config_file(QString("%1/Quiver/config/%2.conf").arg(project_id).arg(config_id));
        if (!config_file.open(QIODevice::ReadOnly)) {
                qDebug() << this << "fatal: could not open config file";
                return;
        }
        QTextStream configstream(&config_file);
        QString config_string;
        while (!configstream.atEnd()) {
                QString line = configstream.readLine();
                config_string += "property var "+line+"\n";
        }
        config_file.close();
        qml.replace("QUIVERENV", config_string);

        QDir platformdir(QString("%1/qml/%2").arg(project_id).arg(platform_name));
        if (!platformdir.exists()) {
                qDebug() << this << "fatal: platform dir" << platform_name << "does not exist";
                return;
        }
        QString main_qml_filepath = QString("%1/main.qml").arg(platformdir.path());
        QFile main_qml_file(main_qml_filepath);
        if (!main_qml_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                qDebug() << this << "fatal: can not open main qml file" << main_qml_filepath << "for writing";
                return;
        }
        QTextStream outstream(&main_qml_file);
        outstream << qml;
        main_qml_file.close();

        QString executable_dirpath = QString("%1/%2.app/Contents/MacOS").arg(builddirpath).arg(project_name);
        QString program = QString("%1/%2").arg(executable_dirpath).arg(project_name);
        QStringList environment; environment << QString("%1=%2").arg("Quiver_platformName").arg(platform_name);

        process = new QProcess;
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(read_process()));
        connect(process, SIGNAL(finished(int)), this, SLOT(process_finished()));
        process->setWorkingDirectory(executable_dirpath);
        process->setProgram(program);
        process->setEnvironment(environment);
        process->start();
        process->waitForStarted();
}

void QuiverLauncher::read_process() {
        if (!process) return;
        process_out.append(process->readAllStandardOutput());
        if (process_out.contains("Quiver: main.qml loaded")) {
                disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(read_process()));
                process_out.clear();
                launch_next_process();
        }
}

void QuiverLauncher::process_finished() {
        auto process = qobject_cast<QProcess *>(QObject::sender());
        if (!process) return;
        process->deleteLater();
}
