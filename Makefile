#################################
# GLOABLS #######################
#################################

# The project to build (just the basename).
MODULE := noobos

# This version-strategy uses git tags to set the version string
# Currently only used to version the docker image based on the git history
# This is slightly broken in that the image is not tied to all of the source
# of the repo, only what is necessary to build/run/test the project from source.
TAG := $(shell git describe --tags --always --dirty)

UID := $(shell id -u)
GID := $(shell id -g)
BLUE='\033[0;34m'
NC='\033[0m' # No Color


export ROOT_DIR = $(shell pwd)

export DEFAULT_HOST = $(shell $(ROOT_DIR)/default_host.sh)
export HOST ?= $(DEFAULT_HOST)
export HOST_ARCH = $(shell $(ROOT_DIR)/target_triplet_to_arch.sh $(HOST))
export BIN_DIR = $(ROOT_DIR)/bin
export BUILD_DIR = $(ROOT_DIR)/build
export ARCH_DIR = arch/$(HOST_ARCH)

export AR := $(shell which $(HOST)-ar)
export CC := $(shell which $(HOST)-gcc)
export CXX := $(shell which $(HOST)-g++)

export CFLAGS = -ggdb \
				-ffreestanding \
				-falign-jumps \
				-falign-functions \
				-falign-labels \
				-falign-loops \
				-fstrength-reduce \
				-fomit-frame-pointer \
				-finline-functions \
				-fvar-tracking \
				-Wno-unused-function \
				-fno-builtin \
				-Werror \
				-Wno-unused-label \
				-Wno-unused-parameter \
				-nostdlib \
				-nostartfiles \
				-nodefaultlibs \
				-Wall \
				-O0

export CXXFLAGS = -ggdb \
				-std=c++20 \
				-fno-rtti \
				-fno-exceptions \
				-fno-use-cxa-atexit


##################################
# LOCAL BUILD, RUN, TEST TARGETS #
##################################

.PHONY: bindir
bindir:
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/$(ARCH_DIR)

.PHONY: builddir
builddir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(ARCH_DIR)

.PHONY: version
version:
	@echo $(TAG)

.PHONY: boot
boot: bindir
	$(MAKE) -C $(ROOT_DIR)/boot

.PHONY: kernel
kernel: builddir
	$(MAKE) -C $(ROOT_DIR)/kernel

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all
all: boot kernel
	rm -rf $(BIN_DIR)/$(ARCH_DIR)/os.bin
	dd if=$(BIN_DIR)/$(ARCH_DIR)/boot.bin >> $(BIN_DIR)/$(ARCH_DIR)/os.bin
	dd if=$(BIN_DIR)/$(ARCH_DIR)/kernel.bin >> $(BIN_DIR)/$(ARCH_DIR)/os.bin
	dd if=/dev/zero bs=512 count=100 >> $(BIN_DIR)/$(ARCH_DIR)/os.bin # pad up to the rest of the 100 sectors with nulls

#################################
# DOCKER RELATED ################
#################################

# Where to push the docker image.
REGISTRY ?= ghcr.io/tn259
IMAGE := $(REGISTRY)/$(MODULE)

.PHONY: docker-build
docker-build:
	@echo "\n${BLUE}Building docker image with labels:\n"
	@echo "name: $(MODULE)"
	@echo "version: $(TAG)${NC}\n"
	docker build -t $(IMAGE) -f- . < Dockerfile	

# Example: make docker-run CMD="-c 'date > datefile'"
.PHONY: docker-run
docker-run:
	@echo "\n${BLUE}Launching a shell in the docker container environment...${NC}\n"
		docker run	\
			-ti	 \
			--rm \
			-v $$PWD:/home/devuser/N00bOS \
			--user devuser \
			$(IMAGE) \
			${CMD}

.PHONY: docker-push
docker-push: docker-build
	@echo "\n${BLUE}Pushing image to GitHub Docker Registry...${NC}\n"
	docker push $(IMAGE):$(TAG)


# Example: make docker-pull VERSION=<Hash>-dirty
.PHONY: docker-pull
docker-pull:
	@echo "\n${BLUE}PUlling image from Github Docker Registry...${NC}\n"
	docker pull $(IMAGE):$(VERSION)

.PHONY: docker-clean
docker-clean:
	docker rmi -f $(shell docker images -q $(IMAGE))
