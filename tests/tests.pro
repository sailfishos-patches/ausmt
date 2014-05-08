TEMPLATE = app
TARGET = ausmttests

QT = core testlib

$$system($$PWD/create-ausmt-copy.sh)


SOURCES += main.cpp

SCRIPTS = ausmt-install \
    ausmt-remove

TEST_FILES = files/simple.qml

ORIGINAL_FILES = original/simple.qml

PATCHED_FILES = patched/simple-patch1.qml \
    patched/simple-patch2.qml \
    patched/simple-patch12.qml

SIMPLEPATCH1 = patches/simple-patch1/unified_diff.patch
SIMPLEPATCH2 = patches/simple-patch2/unified_diff.patch

OTHER_FILES = $${SCRIPTS} \
    $${TEST_FILES} \
    $${ORIGINAL_FILES} \
    $${PATCHED_FILES} \
    $${SIMPLEPATCH1} \
    $${SIMPLEPATCH2}

target.path = /opt/ausmt/tests
scripts.files = $${SCRIPTS}
scripts.path = /opt/ausmt/tests
testFiles.files = $${TEST_FILES}
testFiles.path = /opt/ausmt/tests/files
original.files = $${ORIGINAL_FILES}
original.path = /opt/ausmt/tests/original
patched.files = $${PATCHED_FILES}
patched.path = /opt/ausmt/tests/patched
simplepatch1.files = $${SIMPLEPATCH1}
simplepatch1.path = /opt/ausmt/tests/patches/simple-patch1
simplepatch2.files = $${SIMPLEPATCH2}
simplepatch2.path = /opt/ausmt/tests/patches/simple-patch2

INSTALLS += target scripts testFiles original patched simplepatch1 simplepatch2
