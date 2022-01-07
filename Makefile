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

BIN_DIR=$(shell pwd)/bin
export BIN_DIR

##################################
# LOCAL BUILD, RUN, TEST TARGETS #
##################################

.PHONY: bindir
bindir:
	@mkdir -p $(BIN_DIR)

.PHONY: version
version:
	@echo $(TAG)

.PHONY: boot
boot: bindir
	$(MAKE) -C ./boot

.PHONY: run-x86
run-x86: boot
	@qemu-system-x86_64 -nographic -hda $(BIN_DIR)/boot.bin # "Ctrl-A X" to exit from nographical

.PHONY: all
all: boot

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
			--entrypoint /bin/bash \
			$(IMAGE):$(TAG)	\
			$(CMD)

# Example: make docker-push VERSION=0.0.2
.PHONY: docker-push
docker-push: docker-build
	@echo "\n${BLUE}Pushing image to GitHub Docker Registry...${NC}\n"
	@docker push $(IMAGE):$(VERSION)

.PHONY: docker-clean
docker-clean:
	@docker system prune -f --filter "label=name=$(MODULE)"
