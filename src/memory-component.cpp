#include "memory-component.hpp"
#include <cstdio>


memory_component_t::memory_component_t(const char *name)
  : name(name) {
}


uint32_t memory_component_t::io_read(memory_size_t size, uint32_t address) {
  printf("[%s] io_read(%d, 0x%08x)\n", name, size, address);
  return 0;
}


void memory_component_t::io_write(memory_size_t size, uint32_t address, uint32_t data) {
  printf("[%s] io_write(%d, 0x%08x, 0x%08x)\n", name, size, address, data);
}
