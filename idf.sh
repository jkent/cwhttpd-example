#!/bin/bash

if [ -f /.dockerenv ]; then
    export LC_ALL=C.UTF-8
    uid=$1; shift
    gid=$1; shift
    addgroup --gid $gid group
    adduser --uid $uid --gid $gid --disabled-password --gecos "" user
    adduser user dialout
    adduser user tty
    pip install -r components/libespfs/requirements.txt
    su -c "export PATH=$PATH; idf.py $*" user
else
    args=("$@")
    while [ $# -gt 0 ]; do
        if [ "$1" = "-p" ]; then
        shift
            tty="$1"
            break
        fi
        shift
    done

    if [ -z "$tty" ]; then
        docker run --rm -t -i -v $PWD:/project -w /project espressif/idf /project/idf.sh $(id -u) $(id -g) "${args[@]}"
    else
        docker run --rm -t -i -v $PWD:/project -w /project --device=$tty espressif/idf /project/idf.sh $(id -u) $(id -g) "${args[@]}"
    fi
fi
