# The project to build (just the basename).
MODULE := noobos

# Where to push the docker image.
REGISTRY ?= docker.pkg.github.com/tn259/noobos

IMAGE := $(REGISTRY)/$(MODULE)

# This version-strategy uses git tags to set the version string
TAG := $(shell git describe --tags --always --dirty)

BLUE='\033[0;34m'
NC='\033[0m' # No Color

docker-build:
	@echo "\n${BLUE}Building docker image with labels:\n"
	@echo "name: $(MODULE)"
	@echo "version: $(TAG)${NC}\n"
	@sed                                 \
	    -e 's|{NAME}|$(MODULE)|g'        \
	    -e 's|{VERSION}|$(TAG)|g'        \
	    Dockerfile | docker build -t $(IMAGE):$(TAG) -f- .

# Example: make docker-run CMD="-c 'date > datefile'"
docker-run: docker-build
	@echo "\n${BLUE}Launching a shell in the docker container environment...${NC}\n"
		@docker run                                                 \
			-ti                                                     \
			--rm                                                    \
			--entrypoint /bin/bash                                  \
			-u $$(id -u):$$(id -g)                                  \
			$(IMAGE):$(TAG)										    \
			$(CMD)

# Example: make docker-push VERSION=0.0.2
docker-push: docker-build
	@echo "\n${BLUE}Pushing image to GitHub Docker Registry...${NC}\n"
	@docker push $(IMAGE):$(VERSION)

version:
	@echo $(TAG)

docker-clean:
	@docker system prune -f --filter "label=name=$(MODULE)"
