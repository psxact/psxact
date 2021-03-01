ifeq ($(CONFIGURATION), release)
  CXXFLAGS += -g -O3 -DNDEBUG
  CXXFLAGS += -march=native
else
	CONFIGURATION := debug
  CXXFLAGS += -g -O0 -DDEBUG -D_DEBUG
  CXXFLAGS += -DUNIT_TESTS
endif

CXXFLAGS += -MMD -std=c++17 -I../lib -Wall -Wextra -flto=full -c

CPP := $(shell find . -type f -name '*.cpp')
DEP := $(CPP:./%.cpp=$(OUTDIR)/$(CONFIGURATION)/%.d)
OBJ := $(CPP:./%.cpp=$(OUTDIR)/$(CONFIGURATION)/%.o)

.PHONY: all

all: $(OUTDIR)/$(CONFIGURATION)/$(OUT)

$(OUTDIR)/$(CONFIGURATION)/$(OUT): $(OBJ)
	ar rcs $@ $^

$(OUTDIR)/$(CONFIGURATION)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@

-include $(DEP)
