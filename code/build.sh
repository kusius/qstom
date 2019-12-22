#!/bin/sh
# Build handmade engine
#
#
LIBRARY_PATH="$PWD/../build/debug/"
DEFINES="-D_THREAD_SAFE -DQSTOM_UNIX"
INCLUDE_DIRS="-I/usr/local/include -I$PWD"
mkdir -p ../build/debug
g++ -g -rpath='$ORIGIN' \
    glfw3_qstom.cpp glad.c -o ../build/debug/qstom $INCLUDE_DIRS\
    $DEFINES -pthread\
    -L$LIBRARY_PATH -lglfw -lGL -ldl
#clang -g glfw3_qstom.cpp -o ../build/debug/qstom\
 #   -rpath '$ORIGIN' -I/usr/local/include\
  #  -D_THREAD_SAFE -pthread\
   # -L$PWD/../build/debug/ -L/usr/local/lib/ -lglfw -lGL
