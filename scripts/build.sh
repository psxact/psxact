#!/bin/bash

set -e

# Get packages from apt

packages='build-essential cmake git libsdl2-dev'

apt -yqq update
apt -yqq install $packages

# Build

mkdir -p bin
pushd bin

cmake .. -DCMAKE_BUILD_TYPE=Release && make

popd
