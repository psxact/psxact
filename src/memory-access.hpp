#ifndef __psxact_memory_access__
#define __psxact_memory_access__


#include <cstdint>


enum class bus_width_t {

  byte,
  half,
  word

};


class memory_access_t {

public:

  virtual uint32_t read(bus_width_t width, uint32_t address) = 0;

  virtual void write(bus_width_t width, uint32_t address, uint32_t data) = 0;

};


#endif // __psxact_memory_access__
