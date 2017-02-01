#ifndef QUIVERLAUNCHER_H
#define QUIVERLAUNCHER_H

#include <QObject>
#include <QtQml>

#include "project.h"

#ifdef Q_OS_MAC
#include "macurlconvert.h"
#endif


class QuiverWorker : public QObject
{
        Q_OBJECT
public:
        explicit QuiverWorker(QObject *parent = 0);
public slots:
        void deploy(const Project *project);
        void launch(const Project *project);
signals:
        void completed();
        void process_output(QString text);
private slots:
        void process_finished();
private:
        void build_ios(const Project *project);
        void build_osx(const Project *project);
        void deploy_ios(const Project *project);
        void deploy_osx(const Project *project);
        void start_wait_process(QProcess &process);
        void update_qrc(const Project *project);
        QStringList get_files_in_dir_recursive(const QString &dirpath);
        QString builddirpath;
        QString qt_version = "5.8";

        void launch_next_process();
        QProcess *process = nullptr;
        QString process_out;
        typedef QHash<QString,QString> Process_Template;
        QList<Process_Template> processes_to_launch;

        void show_in_finder(QString path);
};


class QuiverLauncher : public QObject
{
        Q_OBJECT
        Q_PROPERTY(QList<QObject *> projects READ projects NOTIFY projectsChanged)
        Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged)
public:
        explicit QuiverLauncher(QObject *parent = 0);
        ~QuiverLauncher();
signals:
        void projectsChanged();
        void busyChanged();
        void processOutput(QString text);

        void deploy_in_thread(const Project *project);
        void launch_in_thread(const Project *project);
public slots:
        QList<QObject *> projects() const {
                QList<QObject *> retval;
                foreach (auto object, m_projects) {
                        retval << object;
                }
                return retval;
        }
        bool busy() { return _busy; }
        void setBusy(const bool &boolean) { _busy = boolean; emit busyChanged(); }
        void fileDropped(const QList<QUrl> &urls);
        void addProjectWithDirpath(QString dirpath);
        void addProjectWithUrl(QUrl url);
        void addProject(Project *project);
        void launch(const QString &project_id);
        void remove(const QString &project_id);
        void deploy(const QString &project_id);
        void openInQtCreator(const QString &filepath);
private slots:
private:
        QList<Project *> m_projects;
        QSettings *settings;
        Project *get_project_from_project_id(const QString &project_id);
        bool _busy = false;
        QuiverWorker worker;
        QThread worker_thread;
};

#endif // QUIVERLAUNCHER_H
