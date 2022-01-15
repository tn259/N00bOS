#################################
# GLOABLS #######################
#################################

# The project to build (just the basename).
MODULE := noobos

# This version-strategy uses git tags to set the version string
TAG := $(shell git describe --tags --always --dirty)

UID := $(shell id -u)
GID := $(shell id -g)

BLUE='\033[0;34m'
NC='\033[0m' # No Color

export ROOT_DIR = $(shell pwd)
export BIN_DIR = $(ROOT_DIR)/bin
export BUILD_DIR = $(ROOT_DIR)/build

##################################
# LOCAL BUILD, RUN, TEST TARGETS #
##################################

.PHONY: bindir
bindir:
	@mkdir -p $(BIN_DIR)

.PHONY: builddir
builddir:
	@mkdir -p $(BUILDDIR)

.PHONY: version
version:
	@echo $(TAG)

.PHONY: boot
boot: bindir
	$(MAKE) -C $(ROOT_DIR)/boot

.PHONY: kernel
kernel: builddir
	$(MAKE) -C $(ROOT_DIR)/kernel

.PHONY: run-x86
run-x86: boot
	qemu-system-x86_64 -nographic -hda $(BIN_DIR)/boot.bin # "Ctrl-A X" to exit from nographical

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all
all: kernel boot
	@rm -rf $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/boot.bin >> $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/kernel.bin >> $(BIN_DIR)/os.bin
	dd if=/dev/zero bs=512 count=100 >> $(BIN_DIR)/os.bin # pad up to the rest of the 100 sectors with nulls

#################################
# DOCKER RELATED ################
#################################

# Where to push the docker image.
REGISTRY ?= docker.pkg.github.com/tn259/noobos
IMAGE := $(REGISTRY)/$(MODULE)

.PHONY: docker-build
docker-build:
	@echo "\n${BLUE}Building docker image with labels:\n"
	@echo "name: $(MODULE)"
	@echo "version: $(TAG)${NC}\n"
	@sed								\
		-e 's|{NAME}|$(MODULE)|g'		\
		-e 's|{VERSION}|$(TAG)|g'		\
		-e 's|{USERID}|$(UID)|g'		\
		-e 's|{GROUPID}|$(GID)|g'		\
		Dockerfile | docker build -t $(IMAGE):$(TAG) -f- .					

# Example: make docker-run CMD="-c 'date > datefile'"
.PHONY: docker-run
docker-run: docker-build
	@echo "\n${BLUE}Launching a shell in the docker container environment...${NC}\n"
		@docker run	\
			-ti	 \
			--rm \
			-v $$PWD:/home/devuser/N00bOS \
			--user $(UID):$(GID) \
			$(IMAGE):$(TAG)	\
			${CMD}

# Example: make docker-push VERSION=0.0.2
.PHONY: docker-push
docker-push: docker-build
	@echo "\n${BLUE}Pushing image to GitHub Docker Registry...${NC}\n"
	@docker push $(IMAGE):$(VERSION)

.PHONY: docker-clean
docker-clean:
	@docker system prune -f --filter "label=name=$(MODULE)"
