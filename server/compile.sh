#!/bin/sh

mkdir -p build
bear -- g++ \
  -I../common/include\
  -Iinclude \
  src/main.cpp \
  src/tcp_server.cpp \
  -o build/server
