#!/bin/bash

if [[ "$1" == "-d" ]] ; then
    gdb bin/tetris
else
    bin/tetris
fi
