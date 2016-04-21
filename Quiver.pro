QT       += core network gui sql qml quick
macx: QT += macextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quiver
TEMPLATE = app
CONFIG += c++11

include(Quiver/Quiver.pri)


SOURCES += main.cpp \
    quiverlauncher.cpp \
    project.cpp

OTHER_FILES += \
    qml/osx/main.qml \
    qml/osx/MainUI.qml \
    qml/osx/MainMenu.qml \
    qml/osx/MainToolBar.qml

# Add more folders to ship with the application, here
folder_01.source = qml/osx
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += common

HEADERS += \
    quiverlauncher.h \
    project.h \
    qml/BSComponents/macwindowblur.h

macx {
HEADERS += macurlconvert.h
OBJECTIVE_SOURCES += macurlconvert.mm
LIBS += -framework Foundation
}

QMAKE_MAC_SDK = macosx10.11 #added 20151218 to make build work under 10.10

#for BSComponents
macx {

HEADERS += qml/BSComponents/mactoolbutton.h \
    qml/BSComponents/mactoolbar.h
SOURCES += qml/BSComponents/mactoolbutton.cpp
OBJECTIVE_SOURCES += qml/BSComponents/mactoolbar.mm
LIBS += -framework Foundation -framework AppKit
}

RESOURCES += \
    resources.qrc \
    Quiver.qrc

OBJECTIVE_SOURCES += \
    qml/BSComponents/macwindowblur.mm

DISTFILES += \
    qml/osx/CustomBorder.qml
