FROM vookimedlo/ubuntu-qt:qt6_latestdistroofficial_gcc_noble
WORKDIR /home/cs2620/wire-protocols

RUN apt-get update -y && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    gdb \
    valgrind \
    libgtest-dev \
    x11-apps \
    qt6-base-examples \
    libx11-xcb1 \
    libxcb1 \
    libxcb-util1 \
    libxkbcommon-x11-0 \
    wireshark \
    libssl-dev \
    clang-format \
    clang-tidy  \
    clangd \
    git

RUN apt-get install -y \
    net-tools

RUN groupadd -r wireshark && useradd -m -s /bin/bash -G wireshark cs2620
RUN usermod -aG ubuntu cs2620


RUN chgrp wireshark /usr/bin/dumpcap && \
    chmod 750 /usr/bin/dumpcap && \
    setcap cap_net_raw,cap_net_admin+eip /usr/bin/dumpcap

RUN echo "cs2620 ALL=(ALL) NOPASSWD: /usr/bin/dumpcap" >> /etc/sudoers

RUN chown -R cs2620 /home/cs2620 && chmod -R 777 /tmp

RUN git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc /home/cs2620/grpc

RUN export MY_INSTALL_DIR=/home/cs2620/.local
RUN export PATH="$MY_INSTALL_DIR/bin:$PATH"

RUN cd /home/cs2620/grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_CXX_STANDARD=17 \ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR ../.. && \
    make -j4 && \
    make install

RUN apt-get install -y \
    python3-pip tmux

RUN pip3 install --break-system-packages pyyaml

USER cs2620
CMD ["/bin/bash"]