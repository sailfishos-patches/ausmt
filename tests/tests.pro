TEMPLATE = app
TARGET = ausmttests

QT = core testlib

system($$PWD/create-ausmt-copy.sh)


SOURCES += main.cpp

#SCRIPTS = ausmt-install \
#    ausmt-remove

#TEST_FILES = files/simple.qml

#ORIGINAL_FILES = original/simple.qml

#PATCHED_FILES = patched/simple-patch1.qml \
#    patched/simple-patch2.qml \
#    patched/simple-patch12.qml

#SIMPLEPATCH1 = patches/simple-patch1/unified_diff.patch
#SIMPLEPATCH2 = patches/simple-patch2/unified_diff.patch

#OTHER_FILES = $${SCRIPTS} \
#    $${TEST_FILES} \
#    $${ORIGINAL_FILES} \
#    $${PATCHED_FILES} \
#    $${SIMPLEPATCH1} \
#    $${SIMPLEPATCH2}

RESOURCES += \
    res.qrc
