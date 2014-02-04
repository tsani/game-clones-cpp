#!/bin/bash

cd bin

if [[ "$1" == "-d" ]] ; then
    gdb tetris
else
    ./tetris
fi
