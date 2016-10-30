#include "dma_core.hpp"
#include "../bus.hpp"

static dma::state_t state;

static inline uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}

static inline uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}

void dma::initialize() {
  state.dpcr = 0x07654321;
  state.dicr = 0x00000000;
}

uint32_t dma::mmio_read(int size, uint32_t address){
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
      case 1: return state.channels[channel].block_control;
      case 2: return state.channels[channel].channel_control;
    }
  }

  return 0;
}

void dma::mmio_write(int size, uint32_t address, uint32_t data) {
  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: state.dpcr = data & 0xffffffff; break;
      case 1: state.dicr = data & 0x0fff803f; break;
      case 2: break;
      case 3: break;
    }
  }
  else {
    switch ((address >> 2) & 3) {
      case 0: state.channels[channel].address = data; break;
      case 1: state.channels[channel].block_control = data; break;
      case 2: state.channels[channel].channel_control = data; break;
    }
  }

  dma::main();
}

static void dma_sync_mode_0(dma::channel_t &channel) {
  if ((channel.channel_control & 0x10000000) == 0) {
    return;
  }

  auto count = channel.block_control & 0xffff;
  auto index = 0;

  auto address = channel.address & 0xffffff;
  auto address_step = (channel.channel_control & 2) ? 0xfffffffc : 0x00000004;

  while (true) {
    bus::write(WORD, address, 0x00ffffff);

    address += address_step;

    index = (index + 1) & 0xffff;

    if (index == count) {
      break;
    }
  }

  channel.channel_control &= ~0x11000000;
}

static void dma_sync_mode_2(dma::channel_t &channel) {
  if ((channel.channel_control & 0x01000000) == 0) {
    return;
  }

  auto address = channel.address & 0xffffff;

  while (true) {
    auto header = bus::read(WORD, address);
    auto count = header >> 24;

    for (auto index = 0; index < count; index++) {
      address += 4;
      auto command = bus::read(WORD, address);

      bus::write(WORD, 0x1f801810, command);
    }

    address = header & 0xffffff;

    if (address == 0xffffff) {
      break;
    }
  }

  channel.channel_control &= ~0x11000000;
}

void dma::main() {
  auto mask = 0x00000008;

  for (auto &channel : state.channels) {
    if (state.dpcr & mask) {
      switch ((channel.channel_control >> 9) & 3) {
        case 0: dma_sync_mode_0(channel); break;
        case 1: break;
        case 2: dma_sync_mode_2(channel); break;
        case 3: break;
      }
    }

    mask = (mask << 4);
  }
}
