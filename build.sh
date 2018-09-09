#!/bin/bash

set -e

# Get packages from apt

apt-get update > /dev/null
apt-get install -y build-essential cmake git libsdl2-dev > /dev/null

# Build

mkdir -p bin
pushd bin

cmake .. && make

