#ifndef __psxact_exp1__
#define __psxact_exp1__


#include "memory-component.hpp"


namespace psx {
namespace exp {

class expansion1_t : public memory_component_t {

public:

  expansion1_t();

  uint32_t io_read_byte(uint32_t address);

};

}
}

#endif // __psxact_exp1__
