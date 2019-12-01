// Copyright 2018 psxact

#include "input/devices/not-connected.hpp"

using namespace psx::input::devices;

const not_connected_t not_connected_t::instance = not_connected_t();

not_connected_t::not_connected_t() {
}

void not_connected_t::frame() {
}

void not_connected_t::reset() {
}

void not_connected_t::send(uint8_t, uint8_t *response) {
  *response = 0xff;
}
