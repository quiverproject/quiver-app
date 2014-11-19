#include "quiverlauncher.h"

QuiverLauncher::QuiverLauncher(QObject *parent) : QObject(parent)
{
}

void QuiverLauncher::fileDropped(const QList<QUrl> &urls) {
        foreach (const QUrl &in_url, urls) {
                QUrl url = in_url;
#ifdef Q_OS_MAC
                url = MacUrlConvert::convertUrl(in_url);
#endif
                QString dirpath = url.toLocalFile();
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

                auto project = new Project(directory.dirName());

                //add platforms to project
                QDir qmldir(dirpath + "/qml");
                foreach (const QString &filename, qmldir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                        if (filename == "common") continue;
                        project->addPlatform(new Platform(filename));
                }

                //FIXME add configs to project

                addProject(project);
        }
}

void QuiverLauncher::addProject(Project *project) {
        m_projects << project;
        emit projectsChanged();
}
