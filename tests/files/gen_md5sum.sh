#!/bin/sh
rm -f md5sums > /dev/null 2>&1
for i in * ; do
    if [ -f "$i" ]; then
        md5=`md5sum $i | awk '{print $1}'`
        echo "$PWD/$i $md5" >> md5sums
    fi
done
