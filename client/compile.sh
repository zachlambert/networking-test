#!/bin/sh

mkdir -p build
bear -- g++ \
  -Iinclude \
  src/main.cpp \
  src/tcp_client.cpp \
  -o build/server
