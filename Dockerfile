FROM ubuntu:latest

MAINTAINER T N <tjnewman2013@gmail.com>

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
  sudo \
  git \
  nasm \
  qemu-system-x86 \
  qemu-system-arm \
  gcc \
  cmake \
  make \
  gdb \
  valgrind \
  python3

RUN addgroup --gid {GROUPID} devuser

# Add 'devuser' with password 'devuser' with home directory and add to sudo group
RUN useradd --home-dir /home/devuser --create-home --shell /bin/bash --uid {USERID} --gid {GROUPID} devuser \
  && echo "devuser:devuser" | chpasswd \
  && adduser devuser sudo

RUN mkdir -p /home/devuser/N00bOS

WORKDIR /home/devuser/N00bOS

USER devuser

ENTRYPOINT ["/bin/bash"]

LABEL name={NAME}
LABEL version={VERSION}
