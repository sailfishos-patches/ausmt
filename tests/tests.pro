TEMPLATE = app
TARGET = ausmttests

QT = core testlib

system($$PWD/create-ausmt-copy.sh)

SOURCES += main.cpp

RESOURCES += \
    res.qrc

target.path = /opt/ausmt/tests
INSTALLS += target
