CC := clang++
CCFLAGS := -Wall --std=c++11 -c
LDFLAGS := 

OBJS := obj/cpu/cpu_core.o
OBJS += obj/cpu/memory.o
OBJS += obj/gpu/gpu_core.o
OBJS += obj/spu/spu_core.o
OBJS += obj/bus.o
OBJS += obj/utility.o
OBJS += obj/psxact.o

FILE := bin/psxact.exe

all: dirs $(FILE)
	$(CC) $^ -o bin/psxact.exe

dirs:
	@mkdir -p bin/
	@mkdir -p obj/
	@mkdir -p obj/cpu/
	@mkdir -p obj/gpu/
	@mkdir -p obj/spu/

$(FILE): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.cpp
	$(CC) $(CCFLAGS) $^ -o $@

clean:
	-rm -rf $(OBJS) $(FILE)

