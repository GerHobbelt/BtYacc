#
# Build all btyacc test grammars and compile them into test applications
#

TEST_GRAMMARS := $(wildcard test*.y)
TEST_APPLICATIONS := $(patsubst %.y,%,$(TEST_GRAMMARS))
TEST_OUTPUT_SOURCEFILES := $(patsubst %.y,%.tab.c,$(TEST_GRAMMARS))

BTYACC_OPTIONS = -v -l

FRAMEWORK_DEPS = \
	test-framework-header.h \
	test-framework-tail.h \
	test-framework-porting-lib.h \
	portability-config.h


.PHONY: all clean

all: ../btyacc $(TEST_OUTPUT_SOURCEFILES) $(TEST_APPLICATIONS) 

%.tab.c: %.y ../btyacc
	../btyacc $(BTYACC_OPTIONS) $<

%: %.tab.c $(FRAMEWORK_DEPS)
	$(CC) -ggdb -O0 -Wall -Wno-unused-function -o $@ $<


#portability-config.h: portability-config.h.in Makefile.in configure
#	@./configure


# just a helper rule for development: make sure we use the latest BTYACC build
../btyacc: 							  \
			$(wildcard ../*.[ch])     \
			$(wildcard ../*.skeleton) \
			../skel2c                 \
			../Makefile
	@cd ..; make
	@cd .; make clean



clean:
	@rm -f $(TEST_APPLICATIONS)
	@rm -f $(patsubst %.y,%.tab.c,$(TEST_GRAMMARS))
	@rm -f $(patsubst %.y,%.output,$(TEST_GRAMMARS))
	@rm -f *.o

