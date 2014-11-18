#include "project.h"

Project::Project(QObject *parent) : QObject(parent)
{
        m_platforms << new Platform("Platform one");
        m_platforms << new Platform("Platform two");
        m_platforms << new Platform("Platform three");
}

void Project::setName(const QString &arg) {
        m_name = arg;
}
