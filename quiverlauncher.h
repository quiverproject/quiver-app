#ifndef QUIVERLAUNCHER_H
#define QUIVERLAUNCHER_H

#include <QObject>
#include <QtQml>

#include "project.h"

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
private:
         QList<QObject *> m_projects;
};

#endif // QUIVERLAUNCHER_H
