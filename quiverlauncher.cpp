#include "quiverlauncher.h"

QuiverLauncher::QuiverLauncher(QObject *parent) : QObject(parent)
{
        //qmlRegisterType<Project>("com.bscmdesign.qmlcomponents", 1, 0, "Project");

        auto one = new Project;
        one->setName("First project");
        m_projects << one;
}
