// Copyright 2018 psxact

#include "input/devices/digital-pad.hpp"
#include <SDL.h>

using namespace psx::input::devices;

void digital_pad_t::frame() {
  int numkeys;
  
  if (const uint8_t *keys = SDL_GetKeyboardState(&numkeys)) {
    bits =
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
  }
  else {
    bits = 0;
  }

  bits ^= 0xffff;
}

int digital_pad_t::send(int data) {
  if (!dtr) {
    return 1;
  }

  int tx_bit = (tx_buffer >> bit) & 1;
  tx_buffer |= (1 << bit);

  rx_buffer &= ~(1 << bit);
  rx_buffer |= data << bit;

  bit++;

  if (bit == 8) {
    bit = 0;

    switch (step) {
      case 0:
        if (rx_buffer == 0x01) {
          start_dsr_pulse();
          tx_buffer = 0x41;
          step++;
        }
        break;

      case 1:
        if (rx_buffer == 0x42) {
          start_dsr_pulse();
          tx_buffer = 0x5a;
          step++;
        }
        break;

      case 2:
        start_dsr_pulse();
        tx_buffer = uint8_t(bits >> 0);
        step++;
        break;

      case 3:
        start_dsr_pulse();
        tx_buffer = uint8_t(bits >> 8);
        step++;
        break;
    }
  }

  return tx_bit;
}

void digital_pad_t::set_dtr(bool next_dtr) {
  if (!dtr && next_dtr) {
    step = 0;
    bit = 0;
    rx_buffer = 0;
    tx_buffer = 0xff;
  }

  dtr = next_dtr;
}
