#include "cop0_t.hpp"

using namespace r3051;

cop0_t::cop0_t(void)
    : registers() {
}

uint32_t cop0_t::get_register(uint32_t index) {
    return registers[index];
}

void cop0_t::set_register(uint32_t index, uint32_t data) {
    registers[index] = data;
}
