#!/bin/bash -ex

make CONFIGURATION=release clean
make CONFIGURATION=release -j $(nproc)
