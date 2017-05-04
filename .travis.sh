#/bin/bash


travis_before_install() {
  cd ..
  if [ "$TARGET_OS" = "Linux" ]; then
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test;
    sudo apt-get update -qq;
    sudo apt-get install -qq ;
    sudo apt-get install -qq libsdl2-dev gcc-5 g++-5 cmake;
  elif [ "$TARGET_OS" = "OSX" ]; then
    brew update
    brew install sdl2
  fi;
}


travis_script() {
  mkdir build
  pushd build

  if [ "$TARGET_OS" = "Linux" ]; then
    if [ "$CXX" = "g++" ]; then export CXX="g++-5" CC="gcc-5"; fi
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
