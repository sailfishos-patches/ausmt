TEMPLATE = app
TARGET = ausmt-notify

QT = core dbus

CONFIG += link_pkgconfig plugin
PKGCONFIG += nemonotifications-qt5

SOURCES += \
    main.cpp

target.path = /opt/ausmt

INSTALLS += target

