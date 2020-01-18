#ifndef INTERRUPT_ACCESS_HPP_
#define INTERRUPT_ACCESS_HPP_

namespace psx {

enum class interrupt_type_t {
  VBLANK = 0x001,
  GPU    = 0x002,
  CDROM  = 0x004,
  DMA    = 0x008,
  TMR0   = 0x010,
  TMR1   = 0x020,
  TMR2   = 0x040,
  INPUT  = 0x080,
  SIO    = 0x100,
  SPU    = 0x200,
  PIO    = 0x400,
};

class interrupt_access_t {
 public:
  virtual void send(interrupt_type_t flag) = 0;
};

}  // namespace psx

#endif  // INTERRUPT_ACCESS_HPP_
