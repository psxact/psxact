#include "dma/core.hpp"

#include <cassert>
#include "args.hpp"

using namespace psx;
using namespace psx::dma;

static const uint32_t REG_BIT0[32] = {
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma0
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma1
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma2
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma3
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma4
  0xff000000, 0x00000000, 0x8e88f8fc, 0x8e88f8fc, // dma5
  0xff000000, 0x00000000, 0xaeffffff, 0xaeffffff, // dma6
  0x00000000, 0x00007fc0, 0x00000000, 0x00000000, // ctrl
};

static const uint32_t REG_BIT1[32] = {
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma0
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma1
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma2
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma3
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma4
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // dma5
  0x00000000, 0x00000000, 0x00000002, 0x00000002, // dma6
  0x00000000, 0x00000000, 0x00000000, 0x00000000, // ctrl
};

core_t::core_t(interruptible_t &irq, addressable_t &memory)
  : addressable_t("dma", args::log_dma)
  , irq(irq)
  , memory(memory) {
}

void core_t::attach(int n, dma_comms_t *comms) {
  channels[n].comms = comms;
}

uint8_t core_t::channel_priority(int n) const {
  return (pcr >> (4 * n)) & 15;
}

int core_t::tick() {
  // Check if any channels are enabled
  if (priority_len == 0) {
    return 0;
  }

  uint32_t all_controls =
    channels[0].control |
    channels[1].control |
    channels[2].control |
    channels[3].control |
    channels[4].control |
    channels[5].control |
    channels[6].control;

  // Check if any channels are active
  if ((all_controls & 0x11000000) == 0) {
    return 0;
  }

  int amount = 0;

  for (int n = 0; n < priority_len; n++) {
    amount += tick_channel(priority_lut[n]);
  }

  log("all transfers completed in %d cycles", amount);

  return amount;
}

int core_t::tick_channel(int n) {
  log("attempting to run channel %d");

  if ((channels[n].control & (1 << 28)) == 0 &&
      (channels[n].control & (1 << 24)) == 0) {
    return 0;
  }

  log("running channel %d. madr=%08x, bcr=%08x, chcr=%08x", n,
    channels[n].address,
    channels[n].counter,
    channels[n].control);

  channels[n].control &= ~(1 << 28);

  // Chopping isn't supported, we'll die if a game uses it.
  assert((channels[n].control & (1 << 8)) == 0);

  if (n == 6) {
    return tick_sync_mode_otc();
  }

  switch ((channels[n].control >> 9) & 3) {
    case  0: return tick_sync_mode_0(n); // SyncMode(0): Run entire transfer
    case  1: return tick_sync_mode_1(n); // SyncMode(1): Wait for DRQ
    case  2: return tick_sync_mode_2(n); // SyncMode(2): Linked-list
    default:
      assert(0 && "channel tried to use an undefined sync mode.");
      return 0;
  }
}

int core_t::tick_sync_mode_otc() {
  auto &channel = channels[6];
  auto address = channel.address;

  uint16_t bc = channel.counter & 0xffff;

  auto time = bc;

  while (--bc) {
    memory.io_write_word(address, address - 4);
    address -= 4;
  }

  memory.io_write_word(address, 0x00ffffff);

  channel.control &= ~(1 << 24);
  irq_channel(6);

  return time;
}

int core_t::tick_sync_mode_0(int n) {
  auto address = channels[n].address;
  auto address_step = (channels[n].control & 2) ? (-4) : 4;

  uint16_t bc = channels[n].counter & 0xffff;

  auto time = channels[n].comms->dma_speed() * bc;

  if (channels[n].control & 1) {
    do {
      uint32_t word = memory.io_read_word(address);
      address += address_step;

      channels[n].comms->dma_write(word);
    } while (--bc);
  } else {
    do {
      uint32_t word = channels[n].comms->dma_read();

      memory.io_write_word(address, word);
      address += address_step;
    } while (--bc);
  }

  irq_channel(n);

  return time;
}

int core_t::tick_sync_mode_1(int n) {
  auto address = channels[n].address;
  auto address_step = (channels[n].control & 2) ? (-4) : 4;

  uint16_t bs = (channels[n].counter >>  0) & 0xffff;
  uint16_t ba = (channels[n].counter >> 16) & 0xffff;

  auto time = channels[n].comms->dma_speed() * (bs * ba);

  if (channels[n].control & 1) {
    do {
      uint16_t bc = bs;

      do {
        uint32_t word = memory.io_read_word(address);
        address += address_step;

        channels[n].comms->dma_write(word);
      } while (--bc);
    } while (--ba);
  } else {
    do {
      uint16_t bc = bs;

      do {
        uint32_t word = channels[n].comms->dma_read();

        memory.io_write_word(address, word);
        address += address_step;
      } while (--bc);
    } while (--ba);
  }

  channels[n].address = address;
  channels[n].counter = (ba << 16) | bs;
  irq_channel(n);

  return time;
}

int core_t::tick_sync_mode_2(int n) {
  int time = 0;

  do {
    uint32_t header = memory.io_read_word(channels[n].address);
    uint32_t length = (header >> 24) & 0xff;

    time += channels[n].comms->dma_speed() * (length + 1);

    while (length--) {
      channels[n].address += 4;

      uint32_t data = memory.io_read_word(channels[n].address);
      channels[n].comms->dma_write(data);
    }

    channels[n].address = header & 0xffffff;
  } while (channels[n].address != 0xffffff);

  irq_channel(n);

  return time;
}

void core_t::irq_channel(int32_t n) {
  channels[n].control &= ~(1 << 24);

  uint32_t flag = 1 << (n + 24);
  uint32_t mask = 1 << (n + 16);

  if (icr & mask) {
    icr |= flag;
  }

  update_irq_active_flag();
}

void core_t::update_irq_active_flag() {
  bool forced = ((icr >> 15) & 1) != 0;
  bool master = ((icr >> 23) & 1) != 0;
  bool signal = ((icr >> 16) & (icr >> 24) & 0x7f) != 0;
  bool active = forced || (master && signal);

  if (active) {
    if ((icr & 0x80000000) == 0) {
      log("sending interrupt");
      irq.interrupt(interrupt_type_t::dma);
    }

    icr |= 0x80000000;
  }
  else {
    icr &= ~0x80000000;
  }
}

void core_t::put_pcr(uint32_t val) {
  if (pcr != val) {
    log("re-computing priority table. old=%08x, new=%08x", pcr, val);

    pcr = val;

    priority_len = 0;

    for (int priority = 0; priority < 8; priority++) {
      for (int channel = 6; channel >= 0; channel--) {
        if (channel_priority(channel) == (8 | priority)) {
          priority_lut[priority_len] = channel;
          priority_len++;

          log("enabled channel %d with priority %d", channel, priority & 7);
        }
      }
    }
  }
}

void core_t::put_icr(uint32_t val) {
  icr &= 0xff000000;
  icr |=  (val & 0x00ff803f);
  icr &= ~(val & 0x7f000000);
  update_irq_active_flag();
}

uint8_t core_t::io_read_byte(uint32_t address) {
  auto shift = 8 * (address & 3);
  auto value = uint8_t(io_read_word(address) >> shift);
  log("io_read_byte(0x%08x) = 0x%02x", address, value);

  return value;
}

uint16_t core_t::io_read_half(uint32_t address) {
  auto shift = 8 * (address & 2);
  auto value = uint16_t(io_read_word(address) >> shift);
  log("io_read_half(0x%08x) = 0x%04x", address, value);

  return value;
}

uint32_t core_t::io_read_word(uint32_t address) {
  uint32_t bit0 = REG_BIT0[(address / 4) & 31];
  uint32_t bit1 = REG_BIT1[(address / 4) & 31];
  auto value = (get32(address) & ~bit0) | bit1;
  log("io_read_word(0x%08x) = 0x%08x", address, value);

  return value;
}

static uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}

static uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}

uint32_t core_t::get32(uint32_t address) {
  uint32_t channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case  0: return pcr;
      case  1: return icr;
      case  2: return 0x6ffac611; // These values are junk, just what my
      default: return 0x00ffffff; // console happened to give last time.
    }
  }
  else {
    switch (get_register_index(address)) {
      case  0: return channels[channel].address;
      case  1: return channels[channel].counter;
      case  2:
      default: return channels[channel].control;
    }
  }
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  log("io_write_word(0x%08x, 0x%08x)", address, data);

  uint32_t channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: put_pcr(data); break;
      case 1: put_icr(data); break;
      case 2: break;
      case 3: break;
    }
  }
  else {
    switch (get_register_index(address)) {
      case 0: channels[channel].address = data & 0xffffff; break;
      case 1: channels[channel].counter = data; break;
      case 2: channels[channel].control = data; break;
    }
  }
}
