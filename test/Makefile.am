#
# Build all btyacc test grammars and compile them into test applications
#

TEST_GRAMMARS := $(wildcard test*.y)
TEST_APPLICATIONS := $(patsubst %.y,%,$(TEST_GRAMMARS))

BTYACC = ../btyacc
BTYACC_OPTIONS = -tt -v

FRAMEWORK_DEPS = \
	test-framework-header.h \
	test-framework-tail.h \
	portability-config.h


.PHONY: all clean

all: $(TEST_APPLICATIONS)

%.c: %.y 
	$(BTYACC) $(BTYACC_OPTIONS) -o $@ $<

%: %.c $(FRAMEWORK_DEPS)
	$(CC) -O2 -o $@ $<


#portability-config.h: portability-config.h.in Makefile.in configure
#	@./configure
