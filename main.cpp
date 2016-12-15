#include <QApplication>
#include <QDir>

#include "qml/BSComponents/mactoolbar.h"

//#include "qml/com.afmx.debug/afmxdebug_plugin.h"
//#include "qml/com.afmx.debug/peekaboolib.h"

#include "Quiver.h"

#include "quiverlauncher.h"

//
// FIXME: Command+W hides application window. Clicking app dock icon opens window
// SEE: https://github.com/KDAB/Charm/blob/c6244ad730057ec8f89ca4b4555df3c386756209/Charm/MacApplicationCore.mm
// SEE: https://github.com/petroules/petroules-utilities-qt/blob/d05e0c6312b47a2d870d9e6d515ab1142dda9630/src/integratedapplication_mac.mm

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
