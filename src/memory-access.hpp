#ifndef __psxact_memory_access__
#define __psxact_memory_access__


#include <cstdint>
#include "memory-size.hpp"


class memory_access_t {

public:

  virtual uint32_t read(memory_size_t size, uint32_t address) = 0;

  virtual void write(memory_size_t size, uint32_t address, uint32_t data) = 0;

};


#endif // __psxact_memory_access__
