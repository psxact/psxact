#ifndef __psxact_bios_call_decoder__
#define __psxact_bios_call_decoder__


#include <cstdio>
#include <string>
#include "memory-access.hpp"


namespace psx {

class bios_call_decoder_t {
private:

  memory_access_t *memory;

  FILE *log;

public:

  bios_call_decoder_t(memory_access_t *memory);

  void decode_a(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_b(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_c(uint32_t pc, uint32_t function, uint32_t *args);

private:

  std::string decode_string(uint32_t arg);

  std::string decode_string(uint32_t arg, uint32_t size);

  std::string decode_timecode(uint32_t arg);
};

}

#endif // __psxact_bios_call_decoder__
