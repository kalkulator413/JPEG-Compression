#!/bin/sh

mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make

cd ..
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
