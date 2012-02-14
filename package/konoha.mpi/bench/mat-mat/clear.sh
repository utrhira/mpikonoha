#!/bin/sh

for log in `find . -name "mm_*"`; do
    if [ `wc -l $log` = 0 ]; then
        rm $log
    fi
done

now=`date "+%m%d_%H%M%S"`
tmp=mm_${now}
mkdir $tmp
mv mm_* ${tmp}/.
