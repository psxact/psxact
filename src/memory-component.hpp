#ifndef __psxact_memory_component__
#define __psxact_memory_component__


#include <cstdint>
#include <string>
#include "memory-size.hpp"


class memory_component_t {

  const char *name;

public:

  memory_component_t(const char *name);

  virtual uint32_t io_read(memory_size_t size, uint32_t address);

  virtual void io_write(memory_size_t size, uint32_t address, uint32_t data);

};


#endif // __psxact_memory_component__
