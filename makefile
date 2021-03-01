ifneq ($(CONFIGURATION), release)
	CONFIGURATION := debug
endif

OUTDIR := build/$(CONFIGURATION)

LIBPSXACT := $(OUTDIR)/libpsxact.a
LIBPSXACT_CLI := $(OUTDIR)/libpsxact-cli.a
LIBPSXACT_TEST := $(OUTDIR)/libpsxact-tests.a

PSXACT := $(OUTDIR)/psxact
PSXACT_TEST := $(OUTDIR)/psxact-tests

.PHONY: all clean

all: $(PSXACT) $(PSXACT_TEST)

clean:
	@rm -rf $(OUTDIR)

$(PSXACT): $(LIBPSXACT_CLI) $(LIBPSXACT)
	@mkdir -p $(@D)
	$(CXX) $^ -o $@ -flto=full -lSDL2

$(PSXACT_TEST): $(LIBPSXACT_TEST) $(LIBPSXACT)
	@mkdir -p $(@D)
	$(CXX) $^ -o $@ -flto=full -lgtest_main -lgtest -pthread

$(LIBPSXACT): ALWAYS
	$(MAKE) -C lib

$(LIBPSXACT_CLI): ALWAYS
	$(MAKE) -C src

$(LIBPSXACT_TEST): ALWAYS
	$(MAKE) -C test

ALWAYS:
