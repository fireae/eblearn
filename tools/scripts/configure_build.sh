#!/bin/sh

if [ "$#" = "1" ]; then
    ans=$1
else
    echo "Enter 'unix' for command line build system and 'eclipse' for Eclipse build system"
    read ans
    echo "Preparing for $ans"
fi

if [ "$ans" = "unix" ]
then 
    mkdir build 2> /dev/null
    mkdir build/Debug 2> /dev/null
    mkdir build/Release 2> /dev/null
    mkdir build/Demos 2> /dev/null
    cp scripts/Makefile.unix Makefile
    exit 0
fi
if [ "$ans" = "eclipse" ]
then 
    cmake -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" .
    exit 0
fi
echo "please enter unix or eclipse"
