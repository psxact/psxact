#!/bin/bash

for file in `find src/* -type d`; do
  cpplint \
    --root=src \
    --headers=hpp,h \
    --linelength=120 \
    --verbose=0 \
    "$1/*"
done
