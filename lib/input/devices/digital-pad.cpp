#include "input/devices/digital-pad.hpp"

using namespace psx::input;
using namespace psx::input::devices;

void digital_pad_t::latch(const host_device_t &device) {
  bits =
    (int(device.select)               <<  0) |
    (0                                <<  1) | // 1   L3/Joy-button ; analog mode only
    (0                                <<  2) | // 2   R3/Joy-button ; analog mode only
    (int(device.start)                <<  3) |
    (int(device.dpad_up)              <<  4) |
    (int(device.dpad_right)           <<  5) |
    (int(device.dpad_down)            <<  6) |
    (int(device.dpad_left)            <<  7) |
    (int(device.left_back_shoulder)   <<  8) |
    (int(device.right_back_shoulder)  <<  9) |
    (int(device.left_front_shoulder)  << 10) |
    (int(device.right_front_shoulder) << 11) |
    (int(device.button_3)             << 12) |
    (int(device.button_2)             << 13) |
    (int(device.button_0)             << 14) |
    (int(device.button_1)             << 15);

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
