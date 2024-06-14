#!/bin/bash

mkdir lib
mkdir include
mkdir bin

# go to engine/
cd engine/

# install "abc"
./abc.script

# back to SA/
cd ../../

# make cadcontest
make
