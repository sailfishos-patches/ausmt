TEMPLATE = app
TARGET = ausmt-notify

QT = core dbus

CONFIG += plugin

SOURCES += \
    main.cpp \
    notification.cpp \
    notificationmanagerproxy.cpp

HEADERS += \
    notification.h \
    notificationmanagerproxy.h


OTHER_FILES = ausmt-notify-wrapper

target.path = /opt/ausmt
wrapper.files = $${OTHER_FILES}
wrapper.path = /opt/ausmt

INSTALLS += target wrapper

system(qdbusxml2cpp org.freedesktop.Notifications.xml -p notificationmanagerproxy -c NotificationManagerProxy -i notification.h)
