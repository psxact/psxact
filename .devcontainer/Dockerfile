FROM debian:buster

RUN apt -y update && apt -y install --no-install-recommends --no-install-suggests \
        build-essential \
        gdb \
        git \
        git-lfs \
        libgtest-dev \
        libsdl2-dev \
 && apt -y autoremove \
 && apt -y clean \
 && rm -rf /var/lib/apt/lists/*

RUN groupadd -g 1000 dev \
 && useradd -g 1000 -u 1000 -m -s /bin/bash dev

USER dev
