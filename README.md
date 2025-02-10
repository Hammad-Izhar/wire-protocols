# wire-protocols

COMPSCI 2620 Design Exercise 1: Wire Protocols

## Running

To run the code, you'll need Docker and an X11 server. On Linux, you most likely already have an X11 server installed. On Mac/Windows consider using [XQuartz](https://www.xquartz.org/) or [Xming](https://sourceforge.net/projects/xming/), respectively.

**NOTE** This code has only been verified on Mac, and briefly tested on Linux. Your mileage may very.

>[!INFO]
> On Linux, `docker.host.internal` is not natively supported. Therefore, it is recommended to first verify the value of `$DISPLAY` locally and then export it in the docker container. Also be sure to verify xhost connections are allowed by `xhost +local:`

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
