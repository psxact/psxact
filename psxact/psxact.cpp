#include "r3051/bus_t.hpp"
#include "r3051/cop0_t.hpp"
#include "r3051/core_t.hpp"

using namespace r3051;

int main() {
    cop0_t cop0 = cop0_t();
    bus_t bus(cop0);
    core_t core(bus, cop0);

    core.main();

    return 0;
}
