#!/bin/sh

for log in `find . -name "pp_*"`; do
    if [ `wc -l $log` = 0 ]; then
        rm $log
    fi
done

now=`date "+%m%d_%H%M%S"`
tmp=pp_${now}
mkdir $tmp
mv mm_* ${tmp}/.
