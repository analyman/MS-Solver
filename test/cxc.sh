#!/bin/bash

CXX="g++"
CXX_FLAGS="-g --std=c++14 -x c++ -O0"

[ ! $# -eq 1 ] && exit 1
[ ! -f $1 ] && echo "File <$1> doesn't exsit."

eval "${CXX} ${CXX_FLAGS} $1"
exit $?
