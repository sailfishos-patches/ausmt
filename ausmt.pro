TEMPLATE = subdirs
SUBDIRS = ausmtsrc ausmt tests
ausmt.depends = ausmtsrc

!CONFIG(desktop): SUBDIRS += bin

OTHER_FILES += rpm/ausmt.yaml \
    rpm/ausmt.spec

