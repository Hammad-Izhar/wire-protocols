FROM vookimedlo/ubuntu-qt:qt6_latestdistroofficial_gcc_noble
WORKDIR /home/cs2620/wire-protocols

RUN apt-get update -y

RUN apt-get install -y \
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
    clangd

RUN groupadd -r wireshark && useradd -m -s /bin/bash -G wireshark cs2620
RUN usermod -aG ubuntu cs2620


RUN chgrp wireshark /usr/bin/dumpcap && \
    chmod 750 /usr/bin/dumpcap && \
    setcap cap_net_raw,cap_net_admin+eip /usr/bin/dumpcap

RUN echo "cs2620 ALL=(ALL) NOPASSWD: /usr/bin/dumpcap" >> /etc/sudoers

RUN chown -R cs2620 /home/cs2620 && chmod -R 777 /tmp

USER cs2620
CMD ["/bin/bash"]