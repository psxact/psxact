#include "r3051/bus.hpp"
#include "r3051/core.hpp"

using namespace r3051;

int main() {
    bus_t bus = bus_t();
    core_t core(bus);

    core.main();

    return 0;
}
