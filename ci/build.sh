#!/bin/bash

set -e

# Build

mkdir -p bin
pushd bin

cmake .. -DCMAKE_BUILD_TYPE=Release
make

popd
