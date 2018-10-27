#include "memory-component.hpp"

#include <cstdio>


using namespace psx;

memory_component_t::memory_component_t(const char *name)
  : name(name) {
}


uint32_t memory_component_t::io_read_byte(uint32_t address) {
  printf("[%s] io_read_byte(0x%08x)\n", name, address);
  return 0;
}


uint32_t memory_component_t::io_read_half(uint32_t address) {
  printf("[%s] io_read_half(0x%08x)\n", name, address);
  return 0;
}


uint32_t memory_component_t::io_read_word(uint32_t address) {
  printf("[%s] io_read_word(0x%08x)\n", name, address);
  return 0;
}


void memory_component_t::io_write_byte(uint32_t address, uint32_t data) {
  printf("[%s] io_write_byte(0x%08x, 0x%08x)\n", name, address, data);
}


void memory_component_t::io_write_half(uint32_t address, uint32_t data) {
  printf("[%s] io_write_half(0x%08x, 0x%08x)\n", name, address, data);
}


void memory_component_t::io_write_word(uint32_t address, uint32_t data) {
  printf("[%s] io_write_word(0x%08x, 0x%08x)\n", name, address, data);
}
