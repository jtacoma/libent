# Configuration

DEBUG = 1
COVERAGE = 1
CC = clang

# Version

VERSION := $(shell git describe --tags --abbrev=0 | sed -e 's/^v//' )
ENT_VERSION := '$(shell git describe --tags --always) ($(shell git log --pretty=format:%cd --date=short -n1), branch \"$(shell git describe --tags --always --all | sed s:heads/::)\")'

MAJOR_VERSION := $(shell echo ${VERSION} | cut -d '.' -f 1)
MINOR_VERSION := $(shell echo ${VERSION} | cut -d '.' -f 2)
PATCH_VERSION := $(shell echo ${VERSION} | cut -d '.' -f 3)
ifeq (${PATCH_VERSION},)
PATCH_VERSION := 0
endif

# Generic Flags

ifdef DEBUG
CFLAGS ?= -g
else
CFLAGS ?= -O3
endif

ENT_CFLAGS  = $(CFLAGS) -std=c11
ENT_CFLAGS += -Wall
ENT_CFLAGS += -Wpedantic
ENT_CFLAGS += -Werror
ENT_CFLAGS += -Iinclude

ENT_CPPFLAGS  = -DENT_VERSION=\"${ENT_VERSION}\"
ENT_CPPFLAGS += -DMAJOR_VERSION=${MAJOR_VERSION}
ENT_CPPFLAGS += -DMINOR_VERSION=${MINOR_VERSION}
ENT_CPPFLAGS += -DPATCH_VERSION=${PATCH_VERSION}

ENT_LIBS = -Llib -lent

# Code Coverage

ifdef COVERAGE
COVERAGE_CFLAGS += -fprofile-arcs -ftest-coverage
ifeq ($(CC),clang)
ENT_LIBS += --coverage
endif
endif
