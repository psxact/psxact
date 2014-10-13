#include "stdafx.hpp"
#include "spu.hpp"
#include "../bus/bus.hpp"

Spu::Spu(void) {
}

Spu::~Spu(void) {
}

void Spu::AttachBus(Bus* bus) {
  this->bus = bus;
}

uint32_t Spu::Fetch(uint32_t address) {
  if (address >= 0x1f801c00 && address < 0x1f801d80) {
    uint32_t voice = (address - 0x1f801c00) / 16;
    uint32_t index = (address & 15);

    return 0;
  }

  if (address >= 0x1f801e00 && address < 0x1f802000) {
    uint32_t voice = (address - 0x1f801e00) / 4;
    uint32_t index = (address & 3);

    return 0;
  }

  switch (address) {
  // SPU Control Registers
  case 0x1f801d80: // 4  Main Volume Left/Right
  case 0x1f801d84: // 4  Reverb Output Volume Left/Right
  case 0x1f801d88: // 4  Voice 0..23 Key ON (Start Attack/Decay/Sustain) (W)
  case 0x1f801d8c: // 4  Voice 0..23 Key OFF (Start Release) (W)
  case 0x1f801d90: // 4  Voice 0..23 Channel FM (pitch lfo) mode (R/W)
  case 0x1f801d94: // 4  Voice 0..23 Channel Noise mode (R/W)
  case 0x1f801d98: // 4  Voice 0..23 Channel Reverb mode (R/W)
  case 0x1f801d9c: // 4  Voice 0..23 Channel ON/OFF (status) (R)
  case 0x1f801da0: // 2  Unknown? (R) or (W)
  case 0x1f801da2: // 2  Sound RAM Reverb Work Area Start Address
  case 0x1f801da4: // 2  Sound RAM IRQ Address
  case 0x1f801da6: // 2  Sound RAM Data Transfer Address
  case 0x1f801da8: // 2  Sound RAM Data Transfer Fifo
  case 0x1f801daa: // 2  SPU Control Register (SPUCNT)
  case 0x1f801dac: // 2  Sound RAM Data Transfer Control
  case 0x1f801dae: // 2  SPU Status Register (SPUSTAT) (R)
  case 0x1f801db0: // 4  CD Volume Left/Right
  case 0x1f801db4: // 4  Extern Volume Left/Right
  case 0x1f801db8: // 4  Current Main Volume Left/Right
  case 0x1f801dbc: // 4  Unknown? (R/W)
  // SPU Reverb Configuration Area
  case 0x1f801dc0: // 2  dAPF1  Reverb APF Offset 1
  case 0x1f801dc2: // 2  dAPF2  Reverb APF Offset 2
  case 0x1f801dc4: // 2  vIIR   Reverb Reflection Volume 1
  case 0x1f801dc6: // 2  vCOMB1 Reverb Comb Volume 1
  case 0x1f801dc8: // 2  vCOMB2 Reverb Comb Volume 2
  case 0x1f801dca: // 2  vCOMB3 Reverb Comb Volume 3
  case 0x1f801dcc: // 2  vCOMB4 Reverb Comb Volume 4
  case 0x1f801dce: // 2  vWALL  Reverb Reflection Volume 2
  case 0x1f801dd0: // 2  vAPF1  Reverb APF Volume 1
  case 0x1f801dd2: // 2  vAPF2  Reverb APF Volume 2
  case 0x1f801dd4: // 4  mSAME  Reverb Same Side Reflection Address 1 Left/Right
  case 0x1f801dd8: // 4  mCOMB1 Reverb Comb Address 1 Left/Right
  case 0x1f801ddc: // 4  mCOMB2 Reverb Comb Address 2 Left/Right
  case 0x1f801de0: // 4  dSAME  Reverb Same Side Reflection Address 2 Left/Right
  case 0x1f801de4: // 4  mDIFF  Reverb Different Side Reflection Address 1 Left/Right
  case 0x1f801de8: // 4  mCOMB3 Reverb Comb Address 3 Left/Right
  case 0x1f801dec: // 4  mCOMB4 Reverb Comb Address 4 Left/Right
  case 0x1f801df0: // 4  dDIFF  Reverb Different Side Reflection Address 2 Left/Right
  case 0x1f801df4: // 4  mAPF1  Reverb APF Address 1 Left/Right
  case 0x1f801df8: // 4  mAPF2  Reverb APF Address 2 Left/Right
  case 0x1f801dfc: // 4  vIN    Reverb Input Volume Left/Right
    return 0;
  }

  return 0;
}

void Spu::Store(uint32_t address, uint32_t data) {
  if (address >= 0x1f801c00 && address < 0x1f801d80) {
    uint32_t voice = (address - 0x1f801c00) / 16;
    uint32_t index = (address & 15);

    return;
  }

  if (address >= 0x1f801e00 && address < 0x1f802000) {
    uint32_t voice = (address - 0x1f801e00) / 4;
    uint32_t index = (address & 3);

    return;
  }

  switch (address) {
  // SPU Control Registers
  case 0x1f801d80: // 4  Main Volume Left / Right
  case 0x1f801d84: // 4  Reverb Output Volume Left / Right
  case 0x1f801d88: // 4  Voice 0..23 Key ON(Start Attack / Decay / Sustain) (W)
  case 0x1f801d8c: // 4  Voice 0..23 Key OFF(Start Release) (W)
  case 0x1f801d90: // 4  Voice 0..23 Channel FM(pitch lfo) mode(R / W)
  case 0x1f801d94: // 4  Voice 0..23 Channel Noise mode(R / W)
  case 0x1f801d98: // 4  Voice 0..23 Channel Reverb mode(R / W)
  case 0x1f801d9c: // 4  Voice 0..23 Channel ON / OFF(status) (R)
  case 0x1f801da0: // 2  Unknown ? (R)or(W)
  case 0x1f801da2: // 2  Sound RAM Reverb Work Area Start Address
  case 0x1f801da4: // 2  Sound RAM IRQ Address
  case 0x1f801da6: // 2  Sound RAM Data Transfer Address
  case 0x1f801da8: // 2  Sound RAM Data Transfer Fifo
  case 0x1f801daa: // 2  SPU Control Register(SPUCNT)
  case 0x1f801dac: // 2  Sound RAM Data Transfer Control
  case 0x1f801dae: // 2  SPU Status Register(SPUSTAT) (R)
  case 0x1f801db0: // 4  CD Volume Left / Right
  case 0x1f801db4: // 4  Extern Volume Left / Right
  case 0x1f801db8: // 4  Current Main Volume Left / Right
  case 0x1f801dbc: // 4  Unknown ? (R / W)
  // SPU Reverb Configuration Area
  case 0x1f801dc0: // 2  dAPF1  Reverb APF Offset 1
  case 0x1f801dc2: // 2  dAPF2  Reverb APF Offset 2
  case 0x1f801dc4: // 2  vIIR   Reverb Reflection Volume 1
  case 0x1f801dc6: // 2  vCOMB1 Reverb Comb Volume 1
  case 0x1f801dc8: // 2  vCOMB2 Reverb Comb Volume 2
  case 0x1f801dca: // 2  vCOMB3 Reverb Comb Volume 3
  case 0x1f801dcc: // 2  vCOMB4 Reverb Comb Volume 4
  case 0x1f801dce: // 2  vWALL  Reverb Reflection Volume 2
  case 0x1f801dd0: // 2  vAPF1  Reverb APF Volume 1
  case 0x1f801dd2: // 2  vAPF2  Reverb APF Volume 2
  case 0x1f801dd4: // 4  mSAME  Reverb Same Side Reflection Address 1 Left / Right
  case 0x1f801dd8: // 4  mCOMB1 Reverb Comb Address 1 Left / Right
  case 0x1f801ddc: // 4  mCOMB2 Reverb Comb Address 2 Left / Right
  case 0x1f801de0: // 4  dSAME  Reverb Same Side Reflection Address 2 Left / Right
  case 0x1f801de4: // 4  mDIFF  Reverb Different Side Reflection Address 1 Left / Right
  case 0x1f801de8: // 4  mCOMB3 Reverb Comb Address 3 Left / Right
  case 0x1f801dec: // 4  mCOMB4 Reverb Comb Address 4 Left / Right
  case 0x1f801df0: // 4  dDIFF  Reverb Different Side Reflection Address 2 Left / Right
  case 0x1f801df4: // 4  mAPF1  Reverb APF Address 1 Left / Right
  case 0x1f801df8: // 4  mAPF2  Reverb APF Address 2 Left / Right
  case 0x1f801dfc: // 4  vIN    Reverb Input Volume Left / Right
    return;
  }
}
