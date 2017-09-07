#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Bad arguments!"
    exit 1
fi

BUILD_DIR=$1

LIBRARY_DIR=$BUILD_DIR/lab067/libzmq/lib

LD_LIBRARY_PATH=$LIBRARY_DIR
export LD_LIBRARY_PATH

$BUILD_DIR/lab067/lab067
