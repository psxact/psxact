#include "util/wire.hpp"

#include <cstdio>

using namespace psx::util;

void wire::recv_on(std::function<void()> receiver) {
  on_receivers.push_back(receiver);
}

void wire::recv_off(std::function<void()> receiver) {
  off_receivers.push_back(receiver);
}

void wire::recv_rise(std::function<void()> receiver) {
  rise_receivers.push_back(receiver);
}

void wire::recv_fall(std::function<void()> receiver) {
  fall_receivers.push_back(receiver);
}

void wire::operator()(wire_state s) {
  if (s == wire_state::on) {
    for (auto &receiver : on_receivers) {
      receiver();
    }

    if (state == wire_state::off) {
      for (auto &receiver : rise_receivers) {
        receiver();
      }
    }
  }

  if (s == wire_state::off) {
    for (auto &receiver : off_receivers) {
      receiver();
    }

    if (state == wire_state::on) {
      for (auto &receiver : fall_receivers) {
        receiver();
      }
    }
  }

  state = s;
}
