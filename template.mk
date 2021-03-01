ifeq ($(configuration), release)
  CXXFLAGS += -g -O3 -DNDEBUG
  CXXFLAGS += -march=native
else
  CXXFLAGS += -g -O0 -DDEBUG -D_DEBUG
  CXXFLAGS += -DUNIT_TESTS
endif

CXXFLAGS += -MMD -std=c++17 -I../lib -c

CPP := $(shell find . -type f -name '*.cpp')
DEP := $(CPP:./%.cpp=$(OUT_DIR)/%.d)
OBJ := $(CPP:./%.cpp=$(OUT_DIR)/%.o)

.PHONY: all

all: $(OUT_DIR)/$(OUT)

$(OUT_DIR)/$(OUT): $(OBJ)
	ar rcs $@ $^

$(OUT_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@

-include $(DEP)
