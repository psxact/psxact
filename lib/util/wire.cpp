#include "util/wire.hpp"

#include <cstdio>

using namespace psx::util;

void wire_t::recv_on(std::function<void()> receiver) {
  on_receivers.push_back(receiver);
}

void wire_t::recv_off(std::function<void()> receiver) {
  off_receivers.push_back(receiver);
}

void wire_t::recv_rise(std::function<void()> receiver) {
  rise_receivers.push_back(receiver);
}

void wire_t::recv_fall(std::function<void()> receiver) {
  fall_receivers.push_back(receiver);
}

void wire_t::operator()(wire_state_t s) {
  if (s == wire_state_t::on) {
    for (auto &receiver : on_receivers) {
      receiver();
    }

    if (state == wire_state_t::off) {
      for (auto &receiver : rise_receivers) {
        receiver();
      }
    }
  }

  if (s == wire_state_t::off) {
    for (auto &receiver : off_receivers) {
      receiver();
    }

    if (state == wire_state_t::on) {
      for (auto &receiver : fall_receivers) {
        receiver();
      }
    }
  }

  state = s;
}
