TEMPLATE = aux

OTHER_FILES = ausmt-install \
    ausmt-remove \
    ausmt-verify

ausmt.files = $${OTHER_FILES}
ausmt.path = /opt/ausmt

INSTALLS += ausmt
