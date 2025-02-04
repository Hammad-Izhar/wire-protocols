FROM vookimedlo/ubuntu-qt:qt6_latestdistroofficial_gcc_noble
WORKDIR /home/cs2620/wire-protocols

RUN apt-get update

RUN apt-get install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    gdb \
    valgrind

RUN useradd cs2620
RUN chown -R cs2620 /home/cs2620
RUN chmod -R 777 /tmp
USER cs2620