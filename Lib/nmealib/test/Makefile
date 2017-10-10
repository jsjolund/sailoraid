TOPDIR = ..

include $(TOPDIR)/Makefile.inc

#
# Settings
#

TESTDYNAMICLINK ?= 0

H_FILES = $(wildcard $(TOPDIR)/include/nmealib/*.h *.h)
C_FILES = $(wildcard *.c)

MODULES = $(C_FILES:%.c=%)

OBJ = $(MODULES:%=build/%.o)

CFLAGS += -I $(TOPDIR)/include
LDLAGS += -L $(TOPDIR)/lib -lm -lcunit
STATICLIBS =

ifneq ($(TESTDYNAMICLINK),0)
  LDLAGS += -lnmea
else
  STATICLIBS += $(TOPDIR)/lib/$(LIBNAMESTATIC)
endif


#
# Targets
#

all: default_target

default_target: all-before lib/main

remake: clean all

lib/main: $(OBJ) $(STATICLIBS)
ifeq ($(VERBOSE),0)
	@echo "[LD] $@"
endif
	$(MAKECMDPREFIX)$(CC) $(CFLAGS) $(LDLAGS) $(LIBRARIES) -o $@ $(OBJ) $(STATICLIBS)

build/%.o: %.c $(H_FILES) Makefile $(TOPDIR)/Makefile.inc
ifeq ($(VERBOSE),0)
	@echo "[CC] $<"
endif
	$(MAKECMDPREFIX)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

check: default_target
ifdef COV
	@lcov --base-directory .. --directory .. --zerocounters -q
endif
	@./lib/main
ifdef COV
	@lcov -q --base-directory .. --directory .. -c -o build/gov.info
	@lcov -q --remove build/gov.info "/usr*" "test/*" -o build/gov.info
	@rm -rf lib/coverage
	@genhtml -q -o lib/coverage -t "Test Coverage" --num-spaces 4 build/gov.info
endif


#
# Phony Targets
#

.PHONY: all-before clean

all-before:
	$(MAKECMDPREFIX)mkdir -p build lib $(OBJDIRS)

clean:
	$(MAKECMDPREFIX)rm -fr build lib