#!/bin/sh

mkdir -p build
g++ -c src/render.cpp -o build/common.o
ar rcs build/common.a build/common.o
