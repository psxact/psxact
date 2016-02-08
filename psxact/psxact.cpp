#include "bus.hpp"
#include "cpu/cpu_core.hpp"
#include "gpu/gpu_core.hpp"
#include "spu/spu_core.hpp"

using namespace r3051;

int main() {
    gpu::core_t gpu;
    spu::core_t spu;

    bus_t bus = bus_t(gpu, spu);
    core_t core(bus);

    core.main();

    return 0;
}
