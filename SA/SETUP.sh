#!/bin/bash

mkdir lib
mkdir include
mkdir bin
mkdir testcases
# go to engine/
cd engine/

# install "abc"
./abc.script

# back to lsv_final/
cd ../../

# make approximate
make
