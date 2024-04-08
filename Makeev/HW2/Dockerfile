FROM ubuntu:bionic
LABEL authors="Konstantin Chaika"
LABEL description="Dockerfile for simulavr based on https://www.nongnu.org/simulavr/build_simple.html#build-in-docker-recommended"

RUN apt-get update  \
    && apt-get upgrade -y \
    && apt-get install -y g++ make cmake git python3 gcc-avr avr-libc vim \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root
RUN git clone -b master https://git.savannah.nongnu.org/git/simulavr.git
WORKDIR /root/simulavr
# optional, if you want to build a dedicated release (uncomment the hash)
#git checkout rel-X.Y.Z
RUN make build
# optional, just to check the build (uncomment the hash)
#make check
ENV PATH="${PATH}:/root/simulavr/build/app"

ENTRYPOINT ["bash", "-c", "sleep infinity"]
