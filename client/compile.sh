#!/bin/sh

mkdir -p build
bear -- g++ \
  -I../common/include \
  ../common/src/render.cpp \
  -Iinclude \
  src/main.cpp \
  src/tcp_client.cpp \
  -o build/server
