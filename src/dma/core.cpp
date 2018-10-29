// Copyright 2018 psxact

#include "dma/core.hpp"

#include "utility.hpp"


using psx::dma::core_t;

core_t::core_t(interrupt_access_t *irq, memory_access_t *memory)
  : memory_component_t("dma")
  , irq(irq)
  , memory(memory) {
}


static uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}


static uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}


uint32_t core_t::io_read_word(uint32_t address) {
  uint32_t channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: return dpcr;
      case 1: return dicr;
      case 2: return 0x7ffac68b;
      case 3: return 0x00fffff7;
    }
  } else {
    switch (get_register_index(address)) {
      case 0: return channels[channel].address;
      case 1: return channels[channel].counter;
      case 2: return channels[channel].control;
    }
  }

  return 0;
}


void core_t::io_write_word(uint32_t address, uint32_t data) {
  uint32_t channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: dpcr = data; break;

      case 1:
        dicr &= 0xff000000;
        dicr |=  (data & 0x00ff803f);
        dicr &= ~(data & 0x7f000000);
        update_irq_active_flag();
        break;

      case 2: break;
      case 3: break;
    }
  } else {
    switch (get_register_index(address)) {
      case 0: channels[channel].address = data & 0x00ffffff; break;
      case 1: channels[channel].counter = data & 0xffffffff; break;
      case 2: channels[channel].control = data & 0x71770703; break;
    }
  }

  main();
}


void core_t::main() {
  if (dpcr & 0x08000000) { run_channel(6); }
  if (dpcr & 0x00800000) { run_channel(5); }
  if (dpcr & 0x00080000) { run_channel(4); }
  if (dpcr & 0x00008000) { run_channel(3); }
  if (dpcr & 0x00000800) { run_channel(2); }
  if (dpcr & 0x00000080) { run_channel(1); }
  if (dpcr & 0x00000008) { run_channel(0); }
}


void core_t::run_channel_0() {
  channels[0].control &= ~0x01000000;

  irq_channel(0);
}


void core_t::run_channel_1() {
  channels[1].control &= ~0x01000000;

  irq_channel(1);
}


void core_t::run_channel_2_data_read() {
  uint32_t address = channels[2].address;
  uint32_t bs = (channels[2].counter >> 0) & 0xffff;
  uint32_t ba = (channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (uint32_t a = 0; a < ba; a++) {
    for (uint32_t s = 0; s < bs; s++) {
      uint32_t data = memory->read_word(0x1f801810);
      memory->write_word(address, data);
      address += 4;
    }
  }

  channels[2].control &= ~0x01000000;

  irq_channel(2);
}


void core_t::run_channel_2_data_write() {
  uint32_t address = channels[2].address;
  uint32_t bs = (channels[2].counter >>  0) & 0xffff;
  uint32_t ba = (channels[2].counter >> 16) & 0xffff;

  bs = bs ? bs : 0x10000;
  ba = ba ? ba : 0x10000;

  for (uint32_t a = 0; a < ba; a++) {
    for (uint32_t s = 0; s < bs; s++) {
      uint32_t data = memory->read_word(address);
      memory->write_word(0x1f801810, data);
      address += 4;
    }
  }

  channels[2].control &= ~0x01000000;

  irq_channel(2);
}


void core_t::run_channel_2_list() {
  uint32_t address = channels[2].address & 0x1ffffc;

  while (1) {
    uint32_t header = memory->read_word(address);
    uint32_t length = header >> 24;

    for (uint32_t i = 0; i < length; i++) {
      address = (address + 4) & 0x1ffffc;

      uint32_t data = memory->read_word(address);
      memory->write_word(0x1f801810, data);
    }

    if (header & 0x800000) {
      break;
    }

    address = header & 0x1ffffc;
  }

  channels[2].control &= ~0x01000000;

  irq_channel(2);
}


void core_t::run_channel_3() {
  uint32_t address = channels[3].address;
  uint32_t counter = channels[3].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (uint32_t i = 0; i < counter; i++) {
    uint32_t data = memory->read_word(0x1f801800);
    memory->write_word(address, data);

    address += 4;
  }

  channels[3].control &= ~0x01000000;

  irq_channel(3);
}


void core_t::run_channel_4_write() {
  uint32_t address = channels[4].address;
  uint32_t counter = channels[4].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (uint32_t i = 0; i < counter; i++) {
    uint32_t data = memory->read_word(address);
    memory->write_word(0x1f801da8, data);

    address += 4;
  }

  channels[4].control &= ~0x01000000;

  irq_channel(4);
}


void core_t::run_channel_6() {
  uint32_t address = channels[6].address;
  uint32_t counter = channels[6].counter & 0xffff;

  counter = counter ? counter : 0x10000;

  for (uint32_t i = 1; i < counter; i++) {
    memory->write_word(address, address - 4);
    address -= 4;
  }

  memory->write_word(address, 0x00ffffff);

  channels[6].control &= ~0x01000000;

  irq_channel(6);
}


void core_t::run_channel(int32_t n) {
  if (n == 0) {
    switch (channels[0].control) {
      case 0x00000000: return;
      case 0x01000201: return run_channel_0();
    }
  }

  if (n == 1) {
    switch (channels[1].control) {
      case 0x00000000: return;
      case 0x01000200: return run_channel_1();
    }
  }

  if (n == 2) {
    switch (channels[2].control) {
      case 0x01000200: return run_channel_2_data_read();
      case 0x00000201: return;
      case 0x01000201: return run_channel_2_data_write();
      case 0x00000401: return;
      case 0x01000401: return run_channel_2_list();
    }
  }

  if (n == 3) {
    switch (channels[3].control) {
      case 0x00000000: return;
      case 0x10000000: return;
      case 0x11000000: return run_channel_3();
    }
  }

  if (n == 4) {
    switch (channels[4].control) {
      case 0x00000000: return;
      case 0x00000201: return;
      case 0x01000201: return run_channel_4_write();
    }
  }

  if (n == 6) {
    switch (channels[6].control) {
      case 0x00000000: return;
      case 0x10000002: return;
      case 0x11000002: return run_channel_6();
    }
  }

  printf("[DMA] Unhandled DMA %d control value: 0x%08x\n", n, channels[n].control);
}


void core_t::irq_channel(int32_t n) {
  uint32_t flag = 1 << (n + 24);
  uint32_t mask = 1 << (n + 16);

  if (dicr & mask) {
    dicr |= flag;
  }

  update_irq_active_flag();
}


void core_t::update_irq_active_flag() {
  bool forced = ((dicr >> 15) & 1) != 0;
  bool master = ((dicr >> 23) & 1) != 0;
  bool signal = ((dicr >> 16) & (dicr >> 24) & 0x7f) != 0;
  bool active = forced || (master && signal);

  if (active) {
    if (!(dicr & 0x80000000)) {
      irq->send(interrupt_type_t::DMA);
    }

    dicr |= 0x80000000;
  } else {
    dicr &= ~0x80000000;
  }
}
