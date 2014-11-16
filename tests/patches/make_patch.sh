#!/bin/sh 
for i in * ; do
    if [ -d "$i" ]; then
        sed -i s=/opt/ausmt/tests/files=$1=g $PWD/$i/unified_diff.patch
    fi
done