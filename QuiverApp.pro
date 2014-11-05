QT       += core network gui sql qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quiver
TEMPLATE = app
CONFIG += c++11

include(Quiver/Quiver.pri)


SOURCES += main.cpp

OTHER_FILES += \
    qml/osx/main.qml \
    qml/osx/MainUI.qml \
    qml/osx/MainMenu.qml

# Add more folders to ship with the application, here
folder_01.source = qml/osx
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += common
