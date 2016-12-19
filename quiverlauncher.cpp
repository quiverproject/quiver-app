#include "quiverlauncher.h"

QuiverWorker::QuiverWorker(QObject *parent) : QObject(parent)
{
}

QuiverLauncher::QuiverLauncher(QObject *parent) : QObject(parent)
{
        settings = new QSettings(this);
        //settings->clear();
        QVariantList variants = settings->value("projects").value<QVariantList>();
        foreach (const QVariant &variant, variants) {
                auto project = new Project(variant.value<QHash<QString, QVariant>>());
                connect(project, SIGNAL(platformsChanged()), this, SIGNAL(projectsChanged()));
                m_projects << project;
        }

        connect(this, &QuiverLauncher::projectsChanged, [=](){
                QVariantList projects;
                foreach (auto project, m_projects) {
                        projects << project->serialize();
                }
                settings->setValue("projects", projects);
        });

        worker.moveToThread(&worker_thread);
        worker_thread.start();

        connect(this, SIGNAL(deploy_in_thread(const Project*)),
                &worker, SLOT(deploy(const Project*)));
        connect(this, SIGNAL(launch_in_thread(const Project*)),
                &worker, SLOT(launch(const Project*)));
        connect(&worker, &QuiverWorker::completed,
                this, [this]() { setBusy(false); qDebug() << this << "finished!"; });
        connect(&worker, SIGNAL(process_output(QString)),
                this, SIGNAL(processOutput(QString)));
}

QuiverLauncher::~QuiverLauncher() {
        worker_thread.quit();
        worker_thread.wait();
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

void QuiverLauncher::addProject(Project *project) {
        m_projects << project;
        emit projectsChanged();
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

        //directory must contain exactly one .pro file
        QDir directory(dirpath);
        int found_pro_files = 0;
        foreach (const QString &filename, directory.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (filename.toLower().endsWith(".pro") && !QFileInfo(filename).isDir()) {
                        ++found_pro_files;
                }
        }
        if (found_pro_files != 1) return;

        QString id = directory.path();
        foreach (auto project, m_projects) {
                if (project->id() == id) {
                        qDebug() << this << "project already exists";
                        return;
                }
        }
        auto project = new Project(directory.dirName(), id);

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


void QuiverLauncher::remove(const QString &project_id) {
        QList<Project *> new_projects;
        foreach (auto project, m_projects) {
                if (project->id() != project_id) {
                        new_projects << project;
                }
        }
        m_projects = new_projects;
        emit projectsChanged();
}

Project *QuiverLauncher::get_project_from_project_id(const QString &project_id) {
        Project *project = nullptr;

        foreach (QObject *o, m_projects) {
                auto p = qobject_cast<Project *>(o);
                if (!p) continue;
                if (p->id() == project_id) {
                        project = p;
                        break;
                }
        }

        return project;
}

QStringList QuiverWorker::get_files_in_dir_recursive(const QString &dirpath) {
        QStringList filepaths;

        QDirIterator it(dirpath, QDirIterator::Subdirectories);
        while (it.hasNext()) {
                QString the_file = it.next();
                QFileInfo the_file_info(the_file);
                if (the_file_info.isFile()) {
                        filepaths << the_file;
                }
        }
        qSort(filepaths);

        return filepaths;
}

void QuiverWorker::update_qrc(const Project *project) {
        QString qmldirpath = QString("%1/qml").arg(project->id());
        QDir qmldir(qmldirpath);
        if (!qmldir.exists()) {
                qDebug() << this << "update_qrc(): qml dir in project id" << project->id() << "not found!";
                return;
        }

        QStringList filepaths = get_files_in_dir_recursive(qmldirpath);
        QStringList relative_filepaths;
        foreach (QString filepath, filepaths) {
                relative_filepaths << "qml"+filepath.replace(qmldirpath, "");
        }

        QFile qrc_file(QString("%1/Quiver.qrc").arg(project->id()));
        if (!qrc_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                qDebug() << this << "update_qrc(): cannot open output qrc file" << qrc_file.fileName();
                return;
        }

        QTextStream stream(&qrc_file);
        stream << "<RCC><qresource prefix=\"/\">\n";
        foreach (const QString &filepath, relative_filepaths) {
                stream << QString("<file>%1</file>").arg(filepath);
                stream << "\n";
        }
        stream << "</qresource></RCC>\n";

        qrc_file.close();
}

void QuiverWorker::start_wait_process(QProcess &process) {
        process.start();
        process.waitForStarted();
        process.waitForFinished(-1);
        emit process_output(QString("%1 %2").arg(process.program()).arg(process.arguments().join(" ")));

        emit process_output("stdout:");
        foreach (auto line, QString(process.readAllStandardOutput()).split("\n")) {
                emit process_output(line);
        }

        emit process_output("stderr:");
        foreach (auto line, QString(process.readAllStandardError()).split("\n")) {
                emit process_output(line);
        }
}

void QuiverWorker::deploy_ios(const Project *project) {
        QDir builddir(builddirpath);


        QProcess archive_process;
        archive_process.setWorkingDirectory(builddir.path());
        archive_process.setProgram("xcodebuild");
        archive_process.setArguments(QStringList()
                                     << "-scheme"
                                     << project->name()
                                     << "-configuration"
                                     << "AdHoc"
                                     << "clean"
                                     << "archive"
                                     << "-archivePath"
                                     << QString("build/%1").arg(project->name())
                                     );
        start_wait_process(archive_process);
        

        //start by blowing away whatever is already there - xcodebuild will not overwrite it (20150806)
        QString archive_path = QString("build/%1.ipa").arg(project->name());
        QString archive_full_path = QString("%1/%2").arg(builddir.path()).arg(archive_path);
        if (!QFile(archive_full_path).remove()) {
                qDebug() << this << "removing the ipa failed!" << archive_full_path;
        }


        //copy the contents of :/options.plist (the xcodebuild exportOptionsPlist file) to a temporary file that xcodebuild will read
        QString options_plist_path = "/tmp/quiver-options.plist";
        QFile options_file(options_plist_path);
        options_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        QTextStream options_stream(&options_file);

        QFile options_in_file(":/options.plist");
        options_in_file.open(QIODevice::ReadOnly);
        QTextStream options_in_stream(&options_in_file);

        options_stream << options_in_stream.readAll();

        options_in_file.close();
        options_file.close();


        QProcess export_process;
        export_process.setWorkingDirectory(builddir.path());
        export_process.setProgram("xcodebuild");
        export_process.setArguments(QStringList()
                                    << "-configuration"
                                    << "AdHoc"
                                    << "-exportArchive"
                                    << "-archivePath"
                                    << QString("build/%1.xcarchive/").arg(project->name())
                                    << "-exportPath"
                                    << "build/"
                                    << "-exportOptionsPlist"
                                    << options_plist_path
                                    );
        start_wait_process(export_process);
        QFile(options_plist_path).remove();


        show_in_finder(QString("%1/build/%2.ipa").arg(builddir.absolutePath()).arg(project->name()));
}

void QuiverWorker::deploy_osx(const Project *project) {
        QDir builddir(builddirpath);


        QProcess process;
        process.setWorkingDirectory(QString("%1/%2.app/Contents/MacOS").arg(builddir.path()).arg(project->name()));


        //add Frameworks rpath to executable
        process.setProgram("install_name_tool");
        process.setArguments(QStringList()
                             << "-add_rpath"
                             << "@executable_path/../Frameworks"
                             << project->name()
                             );
        start_wait_process(process);


        //make Frameworks dir
        process.setProgram("mkdir");
        process.setArguments(QStringList() << "../Frameworks");
        start_wait_process(process);

        //copy qt frameworks into bundle
        QStringList framework_names = QStringList()
                        << "Core"
                        << "DBus"
                        << "Gui"
                        << "Multimedia"
                        << "MultimediaQuick_p"
                        << "Network"
                        << "OpenGL"
                        << "PrintSupport"
                        << "Qml"
                        << "Quick"
                        << "Sql"
                        << "Svg"
                        << "Widgets"
                           ;
        foreach (const QString &framework_name, framework_names) {
                process.setProgram("rsync");
                process.setArguments(QStringList()
                                     << "-a"
                                     << QString("%1/Qt/%2/clang_64/lib/Qt%3.framework")
                                     .arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                                     .arg(qt_version)
                                     .arg(framework_name)
                                     << "../Frameworks/"
                                     );
                start_wait_process(process);
        }

        //copy qt plugins and qml stuff into bundle
        QStringList dirs_to_copy = QStringList() << "plugins" << "qml";
        foreach (const QString &dir_to_copy, dirs_to_copy) {
                process.setProgram("rsync");
                process.setArguments(QStringList()
                                     << "-a"
                                     << QString("%1/Qt/%2/clang_64/%3")
                                     .arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                                     .arg(qt_version)
                                     .arg(dir_to_copy)
                                     << "../"
                                     );
                start_wait_process(process);
        }

        //get rid of space-consuming debug libs and headers dirs - deleting the .prl files is necessary for code signing
        QStringList filemasks_to_delete = QStringList() << "*_debug.dylib" << "*_debug" << "Headers" << "*.prl";
        foreach (const QString &filemask_to_delete, filemasks_to_delete) {
                process.setProgram("find");
                process.setArguments(QStringList()
                                     << ".."
                                     << "-name"
                                     << filemask_to_delete
                                     << "-exec"
                                     << "rm"
                                     << "-rf"
                                     << "{}"
                                     << ";"
                                     );
                start_wait_process(process);
        }


        //make qt.conf in Resources
        process.setProgram("mkdir");
        process.setArguments(QStringList() << "../Resources");
        start_wait_process(process);

        QFile qtconf_file(QString("%1/%2.app/Contents/Resources/qt.conf").arg(builddir.path()).arg(project->name()));
        if (qtconf_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QTextStream stream(&qtconf_file);

                stream << "[Paths]\n";
                stream << "Plugins = plugins\n";

                qtconf_file.close();
        }


        //sign the bundle
        process.setProgram("codesign");
        process.setArguments(QStringList()
                             << "--force"
                             << "--verify"
                             << "--verbose"
                             << "--deep"
                             << "--sign"
                             << "Developer ID Application: Grinbath LLC (KW6BRTLKNE)"
                             << QString("../../../%1.app").arg(project->name())
                             );
        start_wait_process(process);


        show_in_finder(QString("%1/%2.app").arg(builddir.absolutePath()).arg(project->name()));
}

void QuiverWorker::build_ios(const Project *project) {
        QString qmake_path = QString("%1/Qt/%2/ios/bin/qmake").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(qt_version); //FIXME let the user configure it somehow (20141125)


        builddirpath = QString("%1/../build-quiver-ios-%2").arg(project->id()).arg(project->name());
        QDir builddir(builddirpath);
        if (!builddir.exists()) builddir.mkpath(builddir.path());

        QProcess qmake_process;
        qmake_process.setWorkingDirectory(builddir.path());
        qmake_process.setProgram(qmake_path);
        qmake_process.setArguments(QStringList()
                                   << project->id()
                                   << "-r"
                                   << "-spec"
                                   << "macx-ios-clang"
                                   << "CONFIG+=release"
                                   << "CONFIG+=iphoneos"
                                   );
        start_wait_process(qmake_process);

        QProcess make_process;
        make_process.setWorkingDirectory(builddir.path());
        make_process.setProgram("/usr/bin/make");
        //make_process.setArguments(QStringList() << "-f" << "Makefile.ReleaseDevice");
        start_wait_process(make_process);
}

void QuiverWorker::build_osx(const Project *project) {
        QString qmake_path = QString("%1/Qt/%2/clang_64/bin/qmake").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(qt_version); //FIXME let the user configure it somehow (20141125)


        builddirpath = QString("%1/../build-quiver-osx-%2").arg(project->id()).arg(project->name());
        QDir builddir(builddirpath);
        if (!builddir.exists()) builddir.mkpath(builddir.path());

        QProcess qmake_process;
        qmake_process.setWorkingDirectory(builddir.path());
        qmake_process.setProgram(qmake_path);
        qmake_process.setArguments(QStringList()<<project->id());
        qmake_process.start();
        qmake_process.waitForStarted();
        qmake_process.waitForFinished(-1);

        QProcess make_process;
        make_process.setWorkingDirectory(builddir.path());
        make_process.setProgram("/usr/bin/make");
        make_process.setArguments(QStringList()<<"-j"<<"8"); //FIXME
        make_process.start();
        make_process.waitForStarted();
        //FIXME handle errors
        make_process.waitForFinished(-1);
}

void QuiverWorker::deploy(const Project *project) {
        update_qrc(project);


        //build and deploy each checked platform
        foreach (QObject *o, project->platforms()) {
                auto platform = qobject_cast<Platform *>(o);
                if (!platform) continue;
                if (!platform->enabled()) continue;

                if ("osx" == platform->name()) {
                        build_osx(project);
                        deploy_osx(project);
                } else if ("ios" == platform->name()) {
                        build_ios(project);
                        deploy_ios(project);
                } else {
                        qDebug() << this << "build and deploy to platform" << platform->name() << "not supported at this time!";
                }
        }

        //if there are no checked platforms (because this is not a quiver project), just deploy for os x
        if (!project->platforms().size()) {
                build_osx(project);
                deploy_osx(project);
        }

        emit completed();
}

void QuiverLauncher::deploy(const QString &project_id) {
        Project *project = get_project_from_project_id(project_id);
        if (!project) {
                qDebug() << this << "deploy(): fatal: project id" << project_id << "not found!";
                return;
        }

        setBusy(true);
        emit deploy_in_thread(project);
}

void QuiverLauncher::openInQtCreator(const QString &filepath) {
        QProcess::startDetached(QString("%1/Qt/Qt Creator.app/Contents/MacOS/Qt Creator").arg(QDir::homePath()), QStringList() << "-client" << filepath);
}

void QuiverLauncher::launch(const QString &project_id) {
        //look up the project object based on the project_id
        Project *project = get_project_from_project_id(project_id);
        if (!project) {
                qDebug() << this << "launch(): fatal: project id" << project_id << "not found!";
                return;
        }

        setBusy(true);
        emit launch_in_thread(project);
}

void QuiverWorker::launch(const Project *project) {
        build_osx(project);


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

void QuiverWorker::launch_next_process() {
        if (!processes_to_launch.size()) {
                emit completed();
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
        QStringList environment;
        environment << QString("%1=%2").arg("Quiver_platformName").arg(platform_name);
        environment << QString("%1=%2").arg("Quiver_deploy").arg("0");

        process = new QProcess;

        connect(process, &QProcess::readyReadStandardOutput, [=]() {
                if (!process) return;

                QString text = process->readAllStandardError();

                emit process_output(QString("%1 %2 (%3) stdout: %4")
                                    .arg(project_name)
                                    .arg(platform_name)
                                    .arg(config_id)
                                    .arg(text)
                                    );
        });

        connect(process, &QProcess::readyReadStandardError, [=]() {
                if (!process) return;

                QString text = process->readAllStandardError();

                process_out.append(text);
                if (process_out.contains("Quiver: main.qml loaded")) {
                        process_out.clear();
                        launch_next_process();
                }

                emit process_output(QString("%1 %2 (%3) stderr: %4")
                                    .arg(project_name)
                                    .arg(platform_name)
                                    .arg(config_id)
                                    .arg(text)
                                    );
        });

        connect(process, SIGNAL(finished(int)), this, SLOT(process_finished()));

        process->setWorkingDirectory(executable_dirpath);
        process->setProgram(program);
        process->setEnvironment(environment);
        process->start();
        process->waitForStarted();
}

void QuiverWorker::process_finished() {
        auto process = qobject_cast<QProcess *>(QObject::sender());
        if (!process) return;
        process->deleteLater();
        process = nullptr;
}

void QuiverWorker::show_in_finder(QString path) { //from http://lynxline.com/show-in-finder-show-in-explorer/ (20130204)
#ifdef Q_OS_MAC
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\"";
        args << "-e";
        args << "activate";
        args << "-e";
        args << "select POSIX file \""+path+"\"";
        args << "-e";
        args << "end tell";
        QProcess::startDetached("osascript", args);
#endif

#ifdef Q_OS_WIN32
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(path);
        QProcess::startDetached("explorer", args);
#endif

#ifndef Q_OS_WIN32
#ifndef Q_OS_MAC
        Q_UNUSED(path) //don't know how to do this under linux right now (20130209)
#endif
#endif
}
