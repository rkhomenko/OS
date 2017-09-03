#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Bad arguments!"
    exit 1
fi

BUILD_DIR=$1

LIBRARY_DIR=$BUILD_DIR/lab05/library

LD_LIBRARY_PATH=$LIBRARY_DIR
export LD_LIBRARY_PATH

echo "Compile time linked program"
$BUILD_DIR/lab05/program1/program1

LD_LIBRARY_PATH=
export LD_LIBRARY_PATH

echo "Program with dlopen"
$BUILD_DIR/lab05/program2/program2 $LIBRARY_DIR/libvector.so
