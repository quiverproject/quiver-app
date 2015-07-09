#include <QApplication>
#include <QDir>

#include "qml/BSComponents/mactoolbar.h"

#include "Quiver.h"

#include "quiverlauncher.h"

int main(int argc, char *argv[]) {
        QApplication app(argc, argv);

        QCoreApplication::setOrganizationName("BSCM");
        QCoreApplication::setOrganizationDomain("bscmdesign.com");
        QCoreApplication::setApplicationName("Quiver");
        QCoreApplication::setApplicationVersion("1");

        QDir::setCurrent(QCoreApplication::applicationDirPath());

        MacToolBar::registerTypes();

        Quiver quiver;

        auto client = new QuiverLauncher;
        quiver.addProperty("instance", client);
#ifdef Q_OS_MAC
        bool deploy = quiver.deploy();
        deploy = false; //override what quiver thinks - for working on quiver-app in qt creator
        quiver.setDeploy(deploy);
        QDir::setCurrent(deploy ? "../Resources" : "../../../../quiver-app");
#endif
        quiver.setSource("qml");

        return app.exec();
}
