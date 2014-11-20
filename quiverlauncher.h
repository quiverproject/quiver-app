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
        void addProject(Project *project);
        void launch(const QString &project_id);
private:
        QList<QObject *> m_projects;
        void addProjectWithDirpath(QString dirpath);
};

#endif // QUIVERLAUNCHER_H
