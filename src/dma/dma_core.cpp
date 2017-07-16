#include "dma_core.hpp"
#include "../bus.hpp"
#include "../gpu/gpu_core.hpp"
#include "../state.hpp"

static void update_irq_active_flag(dma_state_t *state) {
  auto forced = ((state->dicr >> 15) & 1) != 0;
  auto master = ((state->dicr >> 23) & 1) != 0;
  auto signal = ((state->dicr >> 16) & (state->dicr >> 24) & 0x7f) != 0;
  auto active = forced || (master && signal);

  if (active) {
    if (!(state->dicr & 0x80000000)) {
      bus::irq(3);
    }

    state->dicr |= 0x80000000;
  }
  else {
    state->dicr &= ~0x80000000;
  }
}

static uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}

static uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}

uint32_t dma::io_read(dma_state_t *state, int width, uint32_t address) {
  if (utility::log_dma) {
    printf("dma::io_read(%d, 0x%08x)\n", width, address);
  }

  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
    case 0: return state->dpcr;
    case 1: return state->dicr;
    case 2: return 0x7ffac68b;
    case 3: return 0x00fffff7;
    }
  }
  else {
    switch (get_register_index(address)) {
    case 0: return state->channels[channel].address;
    case 1: return state->channels[channel].counter;
    case 2: return state->channels[channel].control;
    }
  }

  return 0;
}

void dma::io_write(dma_state_t *state, int width, uint32_t address, uint32_t data) {
  if (utility::log_dma) {
    printf("dma::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
    case 0: state->dpcr = data; break;

    case 1:
      state->dicr &= 0xff000000;
      state->dicr |= (data & 0x00ff803f);
      state->dicr &= ~(data & 0x7f000000);
      update_irq_active_flag(state);
      break;

    case 2: break;
    case 3: break;
    }
  }
  else {
    switch (get_register_index(address)) {
    case 0: state->channels[channel].address = data & 0x00ffffff; break;
    case 1: state->channels[channel].counter = data & 0xffffffff; break;
    case 2: state->channels[channel].control = data & 0x71770703; break;
    }
  }

  dma::main(state);
}

void dma::main(dma_state_t *state) {
  if (state->dpcr & 0x08000000) { run_channel(state, 6); }
  if (state->dpcr & 0x00800000) { run_channel(state, 5); }
  if (state->dpcr & 0x00080000) { run_channel(state, 4); }
  if (state->dpcr & 0x00008000) { run_channel(state, 3); }
  if (state->dpcr & 0x00000800) { run_channel(state, 2); }
  if (state->dpcr & 0x00000080) { run_channel(state, 1); }
  if (state->dpcr & 0x00000008) { run_channel(state, 0); }
}

static void run_channel_0(dma_state_t *state) {
  state->channels[0].control &= ~0x01000000;

  dma::irq_channel(state, 0);
}

static void run_channel_1(dma_state_t *state) {
  state->channels[1].control &= ~0x01000000;

  dma::irq_channel(state, 1);
}

static void run_channel_2_data_read(dma_state_t *state) {
  auto address = state->channels[2].address;
  auto bs = (state->channels[2].counter >>  0) & 0xffff;
  auto ba = (state->channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (unsigned a = 0; a < ba; a++) {
    for (unsigned s = 0; s < bs; s++) {
      unsigned data = bus::read(bus::BUS_WIDTH_WORD, 0x1f801810);
      bus::write(bus::BUS_WIDTH_WORD, address, data);
      address += 4;
    }
  }

  state->channels[2].control &= ~0x01000000;

  dma::irq_channel(state, 2);
}

static void run_channel_2_data_write(dma_state_t *state) {
  auto address = state->channels[2].address;
  auto bs = (state->channels[2].counter >>  0) & 0xffff;
  auto ba = (state->channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (unsigned a = 0; a < ba; a++) {
    for (unsigned s = 0; s < bs; s++) {
      unsigned data = bus::read(bus::BUS_WIDTH_WORD, address);
      bus::write(bus::BUS_WIDTH_WORD, 0x1f801810, data);
      address += 4;
    }
  }

  state->channels[2].control &= ~0x01000000;

  dma::irq_channel(state, 2);
}

static void run_channel_2_list(dma_state_t *state) {
  auto address = state->channels[2].address;

  while (address != 0xffffff) {
    auto value = bus::read(bus::BUS_WIDTH_WORD, address);
    address += 4;

    auto count = value >> 24;

    for (unsigned index = 0; index < count; index++) {
      unsigned data = bus::read(bus::BUS_WIDTH_WORD, address);
      bus::write(bus::BUS_WIDTH_WORD, 0x1f801810, data);
      address += 4;
    }

    address = value & 0xffffff;
  }

  state->channels[2].control &= ~0x01000000;

  dma::irq_channel(state, 2);
}

static void run_channel_3(dma_state_t *state) {
  auto address = state->channels[3].address;
  auto counter = state->channels[3].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (unsigned i = 0; i < counter; i++) {
    auto data = bus::read(bus::BUS_WIDTH_WORD, 0x1f801800);
    bus::write(bus::BUS_WIDTH_WORD, address, data);

    address += 4;
  }

  state->channels[3].control &= ~0x01000000;

  dma::irq_channel(state, 3);
}

static void run_channel_4_write(dma_state_t *state) {
  state->channels[4].control &= ~0x01000000;

  dma::irq_channel(state, 4);
}

static void run_channel_6(dma_state_t *state) {
  auto address = state->channels[6].address;
  auto counter = state->channels[6].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (unsigned i = 1; i < counter; i++) {
    bus::write(bus::BUS_WIDTH_WORD, address, address - 4);
    address -= 4;
  }

  bus::write(bus::BUS_WIDTH_WORD, address, 0x00ffffff);

  state->channels[6].control &= ~0x01000000;

  dma::irq_channel(state, 6);
}

void dma::run_channel(dma_state_t *state, int n) {
  if (n == 0) {
    switch (state->channels[0].control) {
    case 0x01000201: return run_channel_0(state);
    }
  }

  if (n == 1) {
    switch (state->channels[1].control) {
    case 0x01000200: return run_channel_1(state);
    }
  }

  if (n == 2) {
    switch (state->channels[2].control) {
    case 0x01000200: return run_channel_2_data_read(state);
    case 0x01000201: return run_channel_2_data_write(state);
    case 0x01000401: return run_channel_2_list(state);
    }
  }

  if (n == 3) {
    switch (state->channels[3].control) {
    case 0x11000000: return run_channel_3(state);
    }
  }

  if (n == 4) {
    switch (state->channels[4].control) {
    case 0x01000201: return run_channel_4_write(state);
    }
  }

  if (n == 6) {
    switch (state->channels[6].control) {
    case 0x11000002: return run_channel_6(state);
    }
  }
}

void dma::irq_channel(dma_state_t *state, int n) {
  int flag = 1 << (n + 24);
  int mask = 1 << (n + 16);

  if (state->dicr & mask) {
    state->dicr |= flag;
  }

  update_irq_active_flag(state);
}
