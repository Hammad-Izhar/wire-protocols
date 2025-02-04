FROM ubuntu:latest
WORKDIR /home/cs2620/wire-protocols

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    gdb \
    valgrind \
    libgtk-3-dev \
    g++ \
    pkg-config

RUN useradd cs2620
USER cs2620