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
        QList<QObject *> projects() const { return m_projects; }
signals:
        void projectsChanged();
public slots:
        void fileDropped(const QList<QUrl> &urls);
        void addProjectWithDirpath(QString dirpath);
        void addProjectWithUrl(QUrl url);
        void addProject(Project *project);
        void launch(const QString &project_id);
private slots:
        void read_process();
        void launch_next_process();
        void process_finished();
private:
        QList<QObject *> m_projects;
        QProcess *process = nullptr;
        QString process_out;
        typedef QHash<QString,QString> Process_Template;
        QList<Process_Template> processes_to_launch;
        QString builddirpath;
};

#endif // QUIVERLAUNCHER_H
