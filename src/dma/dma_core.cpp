#include "dma_core.hpp"
#include "../bus.hpp"

static dma::state_t state;

static inline uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}

static inline uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}

uint32_t dma::mmio_read(int size, uint32_t address) {
  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: return state.dpcr;
      case 1: return state.dicr;
      case 2: return 0x7ffac68b;
      case 3: return 0x00fffff7;
    }
  }
  else {
    switch ((address >> 2) & 3) {
      case 0: return state.channels[channel].address;
      case 1: return state.channels[channel].counter;
      case 2: return state.channels[channel].control;
    }
  }

  return 0;
}

void dma::mmio_write(int size, uint32_t address, uint32_t data) {
  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: state.dpcr = data & 0xffffffff; break;

      case 1:
        state.dicr &=  (       0xff000000);
        state.dicr |=  (data & 0x00ff803f);
        state.dicr &= ~(data & 0x7f000000);
        break;

      case 2: break;
      case 3: break;
    }
  }
  else {
    switch ((address >> 2) & 3) {
      case 0: state.channels[channel].address = data & 0x00ffffff; break;
      case 1: state.channels[channel].counter = data & 0xffffffff; break;
      case 2: state.channels[channel].control = data & 0x71770703; break;
    }
  }

  dma::main();
}

void dma::main() {
  if (state.dpcr & 0x08000000) { run_channel(6); }
  if (state.dpcr & 0x00800000) { run_channel(5); }
  if (state.dpcr & 0x00080000) { run_channel(4); }
  if (state.dpcr & 0x00008000) { run_channel(3); }
  if (state.dpcr & 0x00000800) { run_channel(2); }
  if (state.dpcr & 0x00000080) { run_channel(1); }
  if (state.dpcr & 0x00000008) { run_channel(0); }
}

static void run_channel_2_data_read() {
  auto address = state.channels[2].address & 0x00ffffff;
  auto bs = (state.channels[2].counter >>  0) & 0xffff;
  auto ba = (state.channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (int a = 0; a < ba; a++) {
    for (int s = 0; s < bs; s++) {
      auto data = bus::read_word(0x1f801810);
      bus::write_word(address, data);
      address += 4;
    }
  }

  state.channels[2].control &= ~0x01000000;
}

static void run_channel_2_data_write() {
  auto address = state.channels[2].address & 0x00ffffff;
  auto bs = (state.channels[2].counter >>  0) & 0xffff;
  auto ba = (state.channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (int a = 0; a < ba; a++) {
    for (int s = 0; s < bs; s++) {
      auto data = bus::read_word(address);
      bus::write_word(0x1f801810, data);
      address += 4;
    }
  }

  state.channels[2].control &= ~0x01000000;
}

static void run_channel_2_list() {
  auto address = state.channels[2].address & 0xffffff;

  while (address != 0xffffff) {
    auto value = bus::read_word(address);
    auto count = value >> 24;

    for (auto index = 0; index < count; index++) {
      auto command = bus::read_word(address += 4);
      bus::write_word(0x1f801810, command);
    }

    address = value & 0xffffff;
  }

  state.channels[2].control &= ~0x01000000;
}

static void run_channel_6() {
  auto address = state.channels[6].address & 0xffffff;
  auto counter = state.channels[6].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (int i = 1; i < counter; i++) {
    bus::write_word(address, address - 4);
    address -= 4;
  }

  bus::write_word(address, 0x00ffffff);

  state.channels[6].control &= ~0x11000000;
}

void dma::run_channel(int n) {
  if (n == 2 && state.channels[2].control == 0x01000200) { return run_channel_2_data_read(); }
  if (n == 2 && state.channels[2].control == 0x01000201) { return run_channel_2_data_write(); }
  if (n == 2 && state.channels[2].control == 0x01000401) { return run_channel_2_list(); }
  if (n == 6 && state.channels[6].control == 0x11000002) { return run_channel_6(); }
}
