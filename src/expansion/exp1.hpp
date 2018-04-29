#ifndef __psxact_exp1__
#define __psxact_exp1__


#include "memory-component.hpp"


class exp1_t : public memory_component_t {

public:

  exp1_t();

  uint32_t io_read_byte(uint32_t address);

};


#endif // __psxact_exp1__
