FROM vookimedlo/ubuntu-qt:qt6_latestdistroofficial_gcc_noble
WORKDIR /home/cs2620/wire-protocols

RUN apt-get update

RUN apt-get install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    gdb \
    valgrind \
    libgtest-dev \
    x11-apps \
    qt6-base-examples

RUN apt install -y \
    libx11-xcb1 \
    libxcb1 \
    libxcb-util1 \
    libxkbcommon-x11-0

RUN useradd cs2620
RUN chown -R cs2620 /home/cs2620
RUN chmod -R 777 /tmp
USER cs2620