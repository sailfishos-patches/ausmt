TEMPLATE = app
TARGET = ausmt-notify

QT = core dbus

CONFIG += link_pkgconfig plugin
PKGCONFIG += nemonotifications-qt5

SOURCES += \
    main.cpp

OTHER_FILES = ausmt-notify-wrapper

target.path = /opt/ausmt
wrapper.files = $${OTHER_FILES}
wrapper.path = /opt/ausmt

INSTALLS += target wrapper

