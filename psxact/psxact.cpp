#include "r3051/bus.hpp"
#include "r3051/cop0.hpp"
#include "r3051/core.hpp"

using namespace r3051;

int main() {
    cop0_t cop0 = cop0_t();
    bus_t bus = bus_t();
    core_t core(bus, cop0);

    core.main();

    return 0;
}
