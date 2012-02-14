#!/bin/sh

now=`date "+%m%d_%H%M%S"`
tmp=${now}
mkdir $tmp
for log in `find . -name "mm_*"`; do
    if [ `wc -l $log` = 0 ]; then
        rm $log
    else
        mv $log ${tmp}/.
    fi
done
