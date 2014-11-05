#include <QApplication>
#include <QDir>

#include "Quiver.h"

//#include "goathubclient.h"

int main(int argc, char *argv[]) {
        QApplication app(argc, argv);

        QCoreApplication::setOrganizationName("BSCM");
        QCoreApplication::setOrganizationDomain("bscmdesign.com");
        QCoreApplication::setApplicationName("Quiver");
        QCoreApplication::setApplicationVersion("1");

        QDir::setCurrent(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
        QDir::setCurrent("../Resources");
        //FIXME remove for deployment
        QDir::setCurrent("../../../../quiver-app");
        //
#endif

        Quiver quiver;

        //auto client = new GoatHubClient;
        //quiver.addProperty("instance", client);

        quiver.setSource("qml");


        return app.exec();
}
