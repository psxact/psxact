#ifndef __psxact_exp2__
#define __psxact_exp2__


#include "memory-component.hpp"


namespace psx {

class exp2_t : public memory_component_t {

public:

  exp2_t();

  void io_write_byte(uint32_t address, uint32_t data);

};

}

#endif // __psxact_exp2__
