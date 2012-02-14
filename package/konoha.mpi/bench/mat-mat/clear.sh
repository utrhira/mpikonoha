#!/bin/sh

now=`date "+%m%d_%H%M%S"`
tmp=${now}
mkdir $tmp
flag=0
for log in `find . -maxdepth 1 -mindepth 1 -name "mm_*"`; do
    ln=(`wc -l $log`)
    if [ $ln = 0 ]; then
        rm $log
    else
        flag=1
        mv $log ${tmp}/.
    fi
done

if [ $flag = 0 ]; then
    rmdir $tmp
fi