#!/bin/sh

mkdir -p build
bear -- g++ \
  -Iinclude \
  src/main.cpp \
  src/tcp_server.cpp \
  -o build/server
