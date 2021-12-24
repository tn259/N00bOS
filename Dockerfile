FROM ubuntu:latest

MAINTAINER T N <tjnewman2013@gmail.com>

RUN apt-get update && apt-get install -y \
  nasm \
  qemu-system-x86 \
  qemu-system-arm \
  gcc \
  cmake \
  make \
  gdb \
  valgrind \
  python3

WORKDIR /N00bOS

COPY . /N00bOS

ENTRYPOINT ["/bin/bash"]

LABEL name={NAME}
LABEL version={VERSION}
