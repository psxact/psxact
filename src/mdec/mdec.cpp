#include <cstdio>
#include "mdec/mdec.hpp"


static uint32_t read_data() {
  printf("mdec::read_data()\n");
  return 0;
}


static uint32_t read_stat() {
  printf("mdec::read_stat()\n");
  return 0;
}


uint32_t mdec_t::io_read(bus_width_t width, uint32_t address) {
  switch (address - 0x1f801820) {
  case 0:
    return read_data();

  case 4:
    return read_stat();
  }

  return 0;
}


static void write_command(uint32_t data) {
  printf("mdec::write_command(0x%08x)\n", data);
}


static void write_control(uint32_t data) {
  printf("mdec::write_control(0x%08x)\n", data);
}


void mdec_t::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  switch (address - 0x1f801820) {
  case 0:
    return write_command(data);

  case 4:
    return write_control(data);
  }
}
