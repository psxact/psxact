#!/bin/bash


travis_before_install() {
  cd ..
  if [ "$TARGET_OS" = "Linux" ]; then
    sudo apt-get update -y;
    sudo apt-get install -y libegl1-mesa-dev libgles2-mesa-dev libsdl2-dev;
  elif [ "$TARGET_OS" = "OSX" ]; then
    brew update
    brew install sdl2
  fi;
}


travis_script() {
  mkdir build
  pushd build

  if [ "$TARGET_OS" = "Linux" ]; then
    export CXX="clang++";
    export CC="clang";

    cmake .. -G"$BUILD_TYPE" -DCMAKE_BUILD_TYPE=Release;
    cmake --build .
  elif [ "$TARGET_OS" = "OSX" ]; then
    cmake .. -G"$BUILD_TYPE"
    cmake --build . --config Release
  fi;

  popd
}


set -e
set -x

$1;

