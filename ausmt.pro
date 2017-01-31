TEMPLATE = subdirs
SUBDIRS = ausmtsrc ausmt tests
ausmt.depends = ausmtsrc

OTHER_FILES += rpm/ausmt.spec

