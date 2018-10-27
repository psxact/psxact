#ifndef __psxact_memory_component__
#define __psxact_memory_component__


#include <cstdint>


namespace psx {

class memory_component_t {

  const char *name;

public:

  memory_component_t(const char *name);

  virtual uint32_t io_read_byte(uint32_t address);

  virtual uint32_t io_read_half(uint32_t address);

  virtual uint32_t io_read_word(uint32_t address);

  // virtual uint32_t io_read(memory_size_t size, uint32_t address);

  virtual void io_write_byte(uint32_t address, uint32_t data);

  virtual void io_write_half(uint32_t address, uint32_t data);

  virtual void io_write_word(uint32_t address, uint32_t data);

  // virtual void io_write(memory_size_t size, uint32_t address, uint32_t data);

};

}

#endif // __psxact_memory_component__
