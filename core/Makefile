ifndef (prj)
prj := all
endif

# detect MAC OS
OSUPPER = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
OSLOWER = $(shell uname -s 2>/dev/null | tr [:upper:] [:lower:])
# 'linux' is output for Linux system, 'darwin' for OS X
DARWIN = $(strip $(findstring DARWIN, $(OSUPPER)))
ifneq ($(DARWIN),)
	LIB_EXT=dylib
else
	LIB_EXT=so
endif

#installation parameters
ifeq ($(DIR),)
 INSTALL_DIR=/usr
else
  FIRST=$(shell echo $(DIR) | cut -c1)
  ifeq ("$(FIRST)","/")
   INSTALL_DIR="$(DIR)"
  else
   HERE=$(shell pwd)
   INSTALL_DIR=$(HERE)/$(DIR)
  endif
endif

INCLUDE_INSTALL=$(INSTALL_DIR)/include/eblearn/
LIB_INSTALL=$(INSTALL_DIR)/lib/
BIN_INSTALL=$(INSTALL_DIR)/bin

LIB_DIR=../bin/

INCLUDES="libidx/include/*" "libeblearn/include/*"
LIBS=libidx.${LIB_EXT} libeblearn.${LIB_EXT}

all: release

release:
	@echo __________________________________________________________________
	@echo
	@echo "Compiling ${prj} project(s) in release mode..."
	@echo "Use (e.g.) 'make release prj=idx' to compile a specific project."
	@echo __________________________________________________________________
	mkdir -p build/Release 2> /dev/null
	cd build/Release && cmake -DCMAKE_BUILD_TYPE=Release ../.. \
	&& make ${prj} && cd ../..

libs:
	@echo ${LIB_EXT}
	@echo ${ARCH2}
	@echo __________________________________________________________________
	@echo
	@echo "Compiling libraries in release mode..."
	@echo __________________________________________________________________
	mkdir -p build/Release 2> /dev/null
	cd build/Release &&  cmake -DCMAKE_BUILD_TYPE=Release ../.. \
	&& make idx eblearn && cd ../..

debug:
	@echo __________________________________________________________________
	@echo
	@echo "Compiling ${prj} project(s) in debug mode..."
	@echo "Use (e.g.) 'make debug prj=idx' to compile a specific project."
	@echo __________________________________________________________________
	mkdir -p build/Debug 2> /dev/null
	cd build/Debug &&  cmake -DCMAKE_BUILD_TYPE=Debug ../.. \
	&& make ${prj} && cd ../..

.PHONY: doc	

doc:
	cd doc/documentation && ./generate.sh

clean:
	rm -rf build
	rm -rf ${LIB_DIR}/libidx.${LIB_EXT}
	rm -rf ${LIB_DIR}/libeblearn.${LIB_EXT}
	rm -rf ../doc/documentation/libidx_html
	rm -rf ../doc/documentation/libeblearn_html

install: prepare_install $(INCLUDES) $(LIBS) $(BINS)

prepare_install:
	@echo "Installation directory: $(INSTALL_DIR)"
	mkdir -p $(INCLUDE_INSTALL) $(LIB_INSTALL) $(BIN_INSTALL)

$(LIBS):
	@if test -e ${LIB_DIR}/$@; \
	then echo "cp -f ../lib/$@ $(LIB_INSTALL)" \
	     && cp -f ${LIB_DIR}/$@ $(LIB_INSTALL); \
	else echo "warning: ../lib/$@ not found, not installed"; \
	fi

$(INCLUDES):
	@echo "cp -f $@ $(INCLUDE_INSTALL)" && cp -f "$@" $(INCLUDE_INSTALL)

$(BINS):
	@if test -e ../bin/$@; \
	then echo "cp -f ../bin/$@ $(BIN_INSTALL)" \
	     && cp -f ../bin/$@ $(BIN_INSTALL); \
	else echo "warning: ../bin/$@ not found, not installed"; \
	fi

uninstall:
	rm -Rf $(INCLUDE_INSTALL)
	cd $(LIB_INSTALL) && rm -Rf ${LIBS}
	cd $(BIN_INSTALL) && rm -Rf ${BINS}

