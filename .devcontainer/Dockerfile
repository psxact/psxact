FROM debian:buster

RUN apt -y update && apt -y install --no-install-recommends \
        build-essential \
        cmake \
        git \
        git-lfs \
        libsdl2-dev \
 && apt -y autoremove \
 && apt -y clean \
 && rm -rf /var/lib/apt/lists/*
