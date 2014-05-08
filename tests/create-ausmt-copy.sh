#!/bin/sh
cp $PWD/../ausmt/ausmt-* $PWD
for ausmt in `ls $PWD/ausmt*`; do
    sed -e 's/AUSMT_VAR_DIR=[a-z\/]*/AUSMT_VAR_DIR=\/opt\/ausmt\/tests\/var\/lib\/patchmanager\/ausmt/g' $ausmt > $ausmt.new
    rm $ausmt
    sed -e 's/PATCH_ROOT_DIR=[a-z\/]*/PATCH_ROOT_DIR=\/opt\/ausmt\/tests\/patches/g' $ausmt.new > $ausmt
    rm $ausmt.new
    chmod +x $ausmt
done

# Make qmake happy
echo "ok"
exit 0
