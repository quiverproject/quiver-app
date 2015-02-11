#ifndef QUIVERLAUNCHER_H
#define QUIVERLAUNCHER_H

#include <QObject>
#include <QtQml>

#include "project.h"

#ifdef Q_OS_MAC
#include "macurlconvert.h"
#endif

class QuiverLauncher : public QObject
{
        Q_OBJECT
        Q_PROPERTY(QList<QObject *> projects READ projects NOTIFY projectsChanged)
public:
        explicit QuiverLauncher(QObject *parent = 0);
        QList<QObject *> projects() const {
                QList<QObject *> retval;
                foreach (auto object, m_projects) {
                        retval << object;
                }
                return retval;
        }
signals:
        void projectsChanged();
public slots:
        void fileDropped(const QList<QUrl> &urls);
        void addProjectWithDirpath(QString dirpath);
        void addProjectWithUrl(QUrl url);
        void addProject(Project *project);
        void launch(const QString &project_id);
        void remove(const QString &project_id);
        void deploy(const QString &project_id);
private slots:
        void read_process();
        void launch_next_process();
        void process_finished();
private:
        QList<Project *> m_projects;
        QProcess *process = nullptr;
        QString process_out;
        typedef QHash<QString,QString> Process_Template;
        QList<Process_Template> processes_to_launch;
        QString builddirpath;
        QSettings *settings;
        Project *get_project_from_project_id(const QString &project_id);
        QStringList get_files_in_dir_recursive(const QString &dirpath);
};

#endif // QUIVERLAUNCHER_H
