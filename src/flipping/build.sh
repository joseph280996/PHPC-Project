#!/bin/bash
rm -rf build
mkdir build && cd build
cmake ..
make
mv parallel_flip ..