// Copyright 2018 psxact

#include "input/devices/digital-pad.hpp"
#include <SDL.h>

using namespace psx::input;

void devices::digital_pad_t::frame() {
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

void devices::digital_pad_t::reset() {
  step = 0;
}

void devices::digital_pad_t::send(uint8_t request, uint8_t *response) {
  printf("step(%d) request: %02x\n", step, request);

  int current_step = step;
  step++;

  switch (current_step) {
    case 0:
      if (request == 0x01) {
        *response = 0xff;
        start_ack_sequence();
      }
      break;

    case 1:
      if (request == 0x42) {
        *response = 0x41;
        start_ack_sequence();
      }
      break;

    case 2:
      *response = 0x5a;
      start_ack_sequence();
      break;

    case 3:
      *response = uint8_t(bits >> 0);
      start_ack_sequence();
      break;

    case 4:
      *response = uint8_t(bits >> 8);
      break;
  }
}
