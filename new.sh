#!/usr/bin/bash

FILE=$1
DELETE=$2

if [ -n "$DELETE" ]; then
    ./_shell/del.sh
fi

for ((i=1;i<=$FILE;i++))
do
    if [ ! -e $i.cpp ]; then
        cp -i file/new.cpp $i.cpp
    fi
    code $i.cpp
done
for ((i=$FILE;0<i;i--))
do
    code -g $i.cpp:8
done