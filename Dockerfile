FROM ubuntu:latest

MAINTAINER T N <tjnewman2013@gmail.com>

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
  sudo \
  git \
  tar \
  nasm \
  wget \
  qemu-system-x86 \
  qemu-system-arm \
  # GCC cross compiler
  build-essential \
  bison \
  flex \
  libgmp3-dev \
  libmpc-dev \
  libmpfr-dev \
  texinfo \
  libisl-dev \
  # the rest
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

COPY install_cross_compiler.sh /home/devuser/N00bOS/

WORKDIR /home/devuser/N00bOS
RUN chown -R devuser:devuser /home/devuser/N00bOS

USER devuser

ENV HOME="/home/devuser"
ENV PREFIX="$HOME/opt/cross"
ENV TARGET="i686-elf"
ENV PATH="$PREFIX/bin:$PATH"

RUN echo $PREFIX
RUN echo $PATH

RUN /bin/bash install_cross_compiler.sh

# At this point bash is in $PATH and entrypoint is in the current directory
ENTRYPOINT ["/bin/bash"]

LABEL name={NAME}
LABEL version={VERSION}
