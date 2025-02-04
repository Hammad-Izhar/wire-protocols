# wire-protocols

COMPSCI 2620 Design Exercise 1: Wire Protocols

## Running

To run the code, you'll need Docker and an X11 server. On Linux, you most likely already have an X11 server installed. On Mac/Windows consider using [https://www.xquartz.org/](XQuartz) or [https://sourceforge.net/projects/xming/](Xming), respectively.

**NOTE** This code has only been verified on Mac, and therefore, some environment variables may not bet set in the docker-compose.yml for Linux/Windows.

```bash
git clone https://github.com/Hammad-Izhar/wire-protocols && cd wire-protocols
docker compose up --build -d
docker attach wire-protocols-app-1 # replace this with the container name
```

To build the code:

```
cd ~/cs2620/wire-protocols/build
cmake ..
make
```
