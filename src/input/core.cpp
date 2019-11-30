// Copyright 2018 psxact

#include "input/core.hpp"

#include <SDL.h>

#include "utility.hpp"

using psx::input::core_t;

core_t::core_t(interrupt_access_t *irq, bool log_enabled)
  : memory_component_t("input", log_enabled)
  , devices()
  , irq(irq) {
  baud_factor = 1;
  baud_reload = 0x0088;
  baud_counter = baud_reload * baud_factor;
}

void core_t::frame() {
  int numkeys;
  
  if (const uint8_t *keys = SDL_GetKeyboardState(&numkeys)) {
    devices[0].value =
      ( keys[SDL_SCANCODE_RSHIFT] <<  0 ) | // 0   Select Button    (0=Pressed, 1=Released)
      ( 0                         <<  1 ) | // 1   L3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
      ( 0                         <<  2 ) | // 2   R3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
      ( keys[SDL_SCANCODE_RETURN] <<  3 ) | // 3   Start Button     (0=Pressed, 1=Released)
      ( keys[SDL_SCANCODE_UP]     <<  4 ) | // 4   Joypad Up        (0=Pressed, 1=Released)
      ( keys[SDL_SCANCODE_RIGHT]  <<  5 ) | // 5   Joypad Right     (0=Pressed, 1=Released)
      ( keys[SDL_SCANCODE_DOWN]   <<  6 ) | // 6   Joypad Down      (0=Pressed, 1=Released)
      ( keys[SDL_SCANCODE_LEFT]   <<  7 ) | // 7   Joypad Left      (0=Pressed, 1=Released)
      ( keys[SDL_SCANCODE_1]      <<  8 ) | // 8   L2 Button        (0=Pressed, 1=Released) (Lower-left shoulder)
      ( keys[SDL_SCANCODE_3]      <<  9 ) | // 9   R2 Button        (0=Pressed, 1=Released) (Lower-right shoulder)
      ( keys[SDL_SCANCODE_Q]      << 10 ) | // 10  L1 Button        (0=Pressed, 1=Released) (Upper-left shoulder)
      ( keys[SDL_SCANCODE_E]      << 11 ) | // 11  R1 Button        (0=Pressed, 1=Released) (Upper-right shoulder)
      ( keys[SDL_SCANCODE_W]      << 12 ) | // 12  /\ Button        (0=Pressed, 1=Released) (Triangle, upper button)
      ( keys[SDL_SCANCODE_D]      << 13 ) | // 13  () Button        (0=Pressed, 1=Released) (Circle, right button)
      ( keys[SDL_SCANCODE_X]      << 14 ) | // 14  >< Button        (0=Pressed, 1=Released) (Cross, lower button)
      ( keys[SDL_SCANCODE_A]      << 15 ) ; // 15  [] Button        (0=Pressed, 1=Released) (Square, left button)

    devices[1].value = 0;
  }
  else {
    devices[0].value = 0;
    devices[1].value = 0;
  }

  devices[0].value ^= 0xffff;
  devices[1].value ^= 0xffff;
}

void core_t::tick(int amount) {
  baud_counter -= amount;

  while (baud_counter <= 0) {
    baud_counter += baud_reload * baud_factor;
    baud_elapses++;

    if (baud_elapses == 8) {
      baud_elapses = 0;

      if (tx_data_pending) {
        tx_data_pending = 0;
        log("sending '%02x' to slot %d", tx_data, slot_select);

        device_t &device = devices[slot_select];

        switch (device.index) {
          case 0: write_rx(0xff, tx_data == 0x01); break;
          case 1: write_rx(0x41, tx_data == 0x42); break;
          case 2: write_rx(0x5a, true); break;
          case 3: write_rx(device.value >> 0, true); break;
          case 4: write_rx(device.value >> 8, false); break;
        }

        device.index++;
      }
    }
  }

  if (ack_interrupt_cycles) {
    ack_interrupt_cycles -= amount;

    if (ack_interrupt_cycles <= 0) {
      ack_interrupt_cycles = 0;

      if (ack_interrupt_enable && ack) {
        log("sending ack interrupt");

        irq->send(interrupt_type_t::INPUT);
      }
    }
  }
}

uint8_t core_t::io_read_byte(uint32_t address) {
  switch (address) {
    case 0x1f801040:
      uint8_t data = rx.has_data()
        ? rx.read()
        : 0xff;

      log("1040: returning '%02x'", data);

      return data;
  }

  return memory_component_t::io_read_byte(address);
}

uint16_t core_t::io_read_half(uint32_t address) {
  switch (address) {
    case 0x1f801044: {
      uint16_t data =
        (1 << 0) | //   0     TX Ready Flag 1   (1=Ready/Started)
        (rx.has_data() << 1) |
        (1 << 2) | //   2     TX Ready Flag 2   (1=Ready/Finished)
        (0 << 3) | //   3     RX Parity Error   (0=No, 1=Error; Wrong Parity, when enabled)  (sticky)
        (ack << 7) |
        (interrupt << 9) |
        (baud_counter << 11);

      log("1044: returning '%04x'", data);

      return data;
    }

    case 0x1f80104a: {
      uint16_t data =
        (tx_enable << 0) |
        (slot_output << 1) |
        (rx_enable << 2) |
        (rx_interrupt_mode << 8) |
        (tx_interrupt_enable << 10) |
        (rx_interrupt_enable << 11) |
        (ack_interrupt_enable << 12) |
        (slot_select << 13);

      log("104a: returning '%04x'", data);

      return data;
    }
  }

  return memory_component_t::io_read_half(address);
}

uint32_t core_t::io_read_word(uint32_t address) {
  return memory_component_t::io_read_word(address);
}

void core_t::io_write_byte(uint32_t address, uint8_t data) {
  switch (address) {
    case 0x1f801040:
      tx_data = data & 0xff;
      tx_data_pending = 1;
      baud_elapses = 0;
      return;
  }

  return memory_component_t::io_write_byte(address, data);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  switch (address) {
    case 0x1f801048:
      // 1F801048h JOY_MODE (R/W) (usually 000Dh, ie. 8bit, no parity, MUL1)
      // 
      //   0-1   Baudrate Reload Factor (1=MUL1, 2=MUL16, 3=MUL64) (or 0=MUL1, too)
      //   2-3   Character Length       (0=5bits, 1=6bits, 2=7bits, 3=8bits)
      //   4     Parity Enable          (0=No, 1=Enable)
      //   5     Parity Type            (0=Even, 1=Odd) (seems to be vice-versa...?)
      //   6-7   Unknown (always zero)
      //   8     CLK Output Polarity    (0=Normal:High=Idle, 1=Inverse:Low=Idle)
      //   9-15  Unknown (always zero)

      if (data != 0x000d) {
        log("non-standard value for JOY_MODE: %04x", data);
      }
      return;

    case 0x1f80104a:
      if (data & 0x40) {
        log("resetting input");
      }
      else {
        if (data & 0x10) {
          interrupt = 0;
        }

        tx_enable = (data >> 0) & 1;
        slot_output = (data >> 1) & 1;
        rx_enable = (data >> 2) & 1;
        rx_interrupt_mode = (data >> 8) & 3;
        tx_interrupt_enable = (data >> 10) & 1;
        rx_interrupt_enable = (data >> 11) & 1;
        ack_interrupt_enable = (data >> 12) & 1;
        slot_select = (data >> 13) & 1;

        if (devices[slot_select].output < slot_output) {
          log("resetting slot %d", slot_select);
          devices[slot_select].index = 0;
        }

        devices[slot_select].output = slot_output;
      }
      return;

    case 0x1f80104e:
      baud_reload = data & 0xffff;
      return;
  }

  return memory_component_t::io_write_half(address, data);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  return memory_component_t::io_write_word(address, data);
}

void core_t::write_rx(uint8_t data, bool ack) {
  rx.write(data);

  if (rx_interrupt_enable) {
    log("sending rx interrupt");

    irq->send(interrupt_type_t::INPUT);
  }

  this->ack = ack;
  this->ack_interrupt_cycles = 100;
}
