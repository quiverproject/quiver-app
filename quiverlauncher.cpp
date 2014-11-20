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

        QFileInfo script_info("Quiver/quiver");
        if (!script_info.exists()) {
                qDebug() << this << "launch(): fatal: quiver script not found!";
                return;
        }

        foreach (QObject *o, project->platforms()) {
                auto platform = qobject_cast<Platform *>(o);
                if (!platform) continue;
                if (!platform->enabled()) continue;

                foreach (QObject *o, platform->configs()) {
                        auto config = qobject_cast<Config *>(o);
                        if (!config) continue;
                        if (!config->enabled()) continue;

                        QStringList args;
                        args << "-d" << project->id();
                        args << "-p" << platform->name();
                        args << "-c" << config->id();
                        args << "-q" << "/Users/njahnke/Qt/5.3/clang_64/bin/qmake"; //FIXME

                        QProcess launcher_process;
                        launcher_process.setProgram(script_info.absoluteFilePath());
                        launcher_process.setArguments(args);
                        launcher_process.start();
                        launcher_process.waitForFinished();
                }
        }
}
