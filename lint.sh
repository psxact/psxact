#!/bin/bash

function lint() {
  cpplint \
    --root=src \
    --headers=hpp,h \
    --linelength=120 \
    --verbose=0 \
    "$1"
}

for file in `find src -name '*.hpp'`; do
  lint $file
done

for file in `find src -name '*.cpp'`; do
  lint $file
done
