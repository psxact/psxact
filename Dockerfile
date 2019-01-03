FROM debian:stretch

RUN groupadd -g 1000 -r psxact && \
    useradd -u 1000 --no-log-init -r -g psxact psxact

RUN apt-get update -y && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsdl2-dev \
    python-pip \
 && rm -rf /var/lib/apt/lists/*

RUN pip install cpplint

USER psxact

VOLUME /workspace
WORKDIR /workspace
