#ifndef INTERRUPTIBLE_HPP_
#define INTERRUPTIBLE_HPP_

namespace psx {

  enum class interrupt_type {
    vblank = 0x001,
    gpu    = 0x002,
    cdrom  = 0x004,
    dma    = 0x008,
    timer0 = 0x010,
    timer1 = 0x020,
    timer2 = 0x040,
    input  = 0x080,
    sio    = 0x100,
    spu    = 0x200,
    pio    = 0x400,
  };

  class interruptible {
  public:
    virtual ~interruptible() {}

    virtual void interrupt(interrupt_type flag) = 0;
  };
}

#endif
