BUILD := build

LIBPSXACT := $(BUILD)/libpsxact.a
LIBPSXACT_CLI := $(BUILD)/libpsxact-cli.a
LIBPSXACT_TEST := $(BUILD)/libpsxact-tests.a

PSXACT := $(BUILD)/psxact
PSXACT_TEST := $(BUILD)/psxact-tests

.PHONY: all clean

all: $(PSXACT) $(PSXACT_TEST)

clean:
	@rm -rf $(BUILD)

$(PSXACT): $(LIBPSXACT_CLI) $(LIBPSXACT)
	@mkdir -p $(@D)
	$(CXX) $^ -o $@ -lSDL2

$(PSXACT_TEST): $(LIBPSXACT_TEST) $(LIBPSXACT)
	@mkdir -p $(@D)
	$(CXX) $^ -o $@ -lgtest_main -lgtest -pthread

$(LIBPSXACT): ALWAYS
	$(MAKE) -C lib

$(LIBPSXACT_CLI): ALWAYS
	$(MAKE) -C src

$(LIBPSXACT_TEST): ALWAYS
	$(MAKE) -C test

ALWAYS:
