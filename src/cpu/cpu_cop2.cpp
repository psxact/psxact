#include "cpu_core.hpp"
#include "cpu_cop2.hpp"

//
// Porting
//

enum matrix_type_t {
  Rotation = 0,
  Light = 1,
  Color = 2,
  Invalid,
};

namespace Matrix {
  matrix_type_t from_command(uint32_t command) {
    switch ((command >> 17) & 3) {
    case  0: return matrix_type_t::Rotation;
    case  1: return matrix_type_t::Light;
    case  2: return matrix_type_t::Color;
    default: return matrix_type_t::Invalid;
    }
  }
}

enum control_vector_type_t {
  Translation = 0,
  BackgroundColor = 1,
  FarColor = 2,
  Zero = 3,
};

namespace ControlVector {
  control_vector_type_t from_command(uint32_t command) {
    switch ((command >> 13) & 3) {
    case  0: return control_vector_type_t::Translation;
    case  1: return control_vector_type_t::BackgroundColor;
    case  2: return control_vector_type_t::FarColor;
    default: return control_vector_type_t::Zero;
    }
  }
}

struct command_config_t {
  uint8_t shift;
  bool clamp_negative;
  matrix_type_t matrix;
  uint8_t vector_mul;
  control_vector_type_t vector_add;
};

namespace CommandConfig {
  command_config_t from_command(uint32_t command) {
    auto shift = (command & (1 << 19)) ? 12 : 0;
    auto clamp_negative = (command & (1 << 10)) != 0;
    auto vector_index = (command >> 15 ) & 3;

    return command_config_t {
      uint8_t(shift),
      clamp_negative,
      Matrix::from_command(command),
      uint8_t(vector_index),
      ControlVector::from_command(command),
    };
  }
}

void command(cop2_state_t *self, uint32_t command);

uint32_t control(cop2_state_t *self, uint32_t reg);

void set_control(cop2_state_t *self, uint32_t reg, uint32_t val);

uint32_t data(cop2_state_t *self, uint32_t reg);

void set_data(cop2_state_t *self, uint32_t reg, uint32_t val);

void cmd_rtps(cop2_state_t *self, command_config_t config);

void cmd_nclip(cop2_state_t *self);

void cmd_op(cop2_state_t *self, command_config_t config);

void cmd_dpcs(cop2_state_t *self, command_config_t config);

void cmd_dcpl(cop2_state_t *self, command_config_t config);

void cmd_dpct(cop2_state_t *self, command_config_t config);

void do_dpc(cop2_state_t *self, command_config_t config);

void cmd_intpl(cop2_state_t *self, command_config_t config);

void cmd_mvmva(cop2_state_t *self, command_config_t config);

void cmd_ncds(cop2_state_t *self, command_config_t config);

void cmd_ncdt(cop2_state_t *self, command_config_t config);

void cmd_nccs(cop2_state_t *self, command_config_t config);

void cmd_cc(cop2_state_t *self, command_config_t config);

void cmd_ncs(cop2_state_t *self, command_config_t config);

void cmd_nct(cop2_state_t *self, command_config_t config);

void cmd_sqr(cop2_state_t *self, command_config_t config);

void cmd_avsz3(cop2_state_t *self);

void cmd_avsz4(cop2_state_t *self);

void cmd_rtpt(cop2_state_t *self, command_config_t config);

void cmd_gpf(cop2_state_t *self, command_config_t config);

void cmd_gpl(cop2_state_t *self, command_config_t config);

void cmd_ncct(cop2_state_t *self, command_config_t config);

void do_ncc(cop2_state_t *self, command_config_t config, uint8_t vector_index);

void do_nc(cop2_state_t *self, command_config_t config, uint8_t vector_index);

void do_ncd(cop2_state_t *self, command_config_t config, uint8_t vector_index);

void multiply_matrix_by_vector(cop2_state_t *self, command_config_t config, matrix_type_t matrix, uint8_t vector_index, control_vector_type_t control_vector);

void mac_to_ir(cop2_state_t *self, command_config_t config);

uint8_t mac_to_color(cop2_state_t *self, int32_t mac, uint8_t which);

void mac_to_rgb_fifo(cop2_state_t *self);

uint32_t do_rtp(cop2_state_t *self, command_config_t config, unsigned vector_index);

void depth_queuing(cop2_state_t *self, uint32_t projection_factor);

void set_flag(cop2_state_t *self, uint8_t bit);

int64_t i64_to_i44(cop2_state_t *self, uint8_t flag, int64_t val);

int16_t i32_to_i16_saturate(cop2_state_t *self, command_config_t config, uint8_t flag, int32_t val);

int16_t i32_to_i11_saturate(cop2_state_t *self, uint8_t flag, int32_t val);

void check_mac_overflow(cop2_state_t *self, int64_t val);

uint16_t i64_to_otz(cop2_state_t *self, int64_t average);



namespace divider {
// Unsigned Newtown-Raphson look up table
  static uint8_t UNR_TABLE[0x101] = {
      0xff, 0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf3, 0xf1,
      0xef, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe3,
      0xe1, 0xdf, 0xdd, 0xdc, 0xda, 0xd8, 0xd6, 0xd5,
      0xd3, 0xd1, 0xd0, 0xce, 0xcd, 0xcb, 0xc9, 0xc8,
      0xc6, 0xc5, 0xc3, 0xc1, 0xc0, 0xbe, 0xbd, 0xbb,
      0xba, 0xb8, 0xb7, 0xb5, 0xb4, 0xb2, 0xb1, 0xb0,
      0xae, 0xad, 0xab, 0xaa, 0xa9, 0xa7, 0xa6, 0xa4,
      0xa3, 0xa2, 0xa0, 0x9f, 0x9e, 0x9c, 0x9b, 0x9a,
      0x99, 0x97, 0x96, 0x95, 0x94, 0x92, 0x91, 0x90,
      0x8f, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x87, 0x86,
      0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7d,
      0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x75, 0x74,
      0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d, 0x6c,
      0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64,
      0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5d, 0x5d,
      0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55,
      0x54, 0x53, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e,
      0x4d, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x48,
      0x47, 0x46, 0x45, 0x44, 0x43, 0x43, 0x42, 0x41,
      0x40, 0x3f, 0x3f, 0x3e, 0x3d, 0x3c, 0x3c, 0x3b,
      0x3a, 0x39, 0x39, 0x38, 0x37, 0x36, 0x36, 0x35,
      0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2f,
      0x2e, 0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2a, 0x2a,
      0x29, 0x28, 0x28, 0x27, 0x26, 0x26, 0x25, 0x24,
      0x24, 0x23, 0x22, 0x22, 0x21, 0x20, 0x20, 0x1f,
      0x1e, 0x1e, 0x1d, 0x1d, 0x1c, 0x1b, 0x1b, 0x1a,
      0x19, 0x19, 0x18, 0x18, 0x17, 0x16, 0x16, 0x15,
      0x15, 0x14, 0x14, 0x13, 0x12, 0x12, 0x11, 0x11,
      0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c,
      0x0c, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x08, 0x08,
      0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04,
      0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00,
      0x00
  };

  uint32_t reciprocal(uint16_t d) {
    auto index = ((d & 0x7fff) + 0x40) >> 7;
    auto factor = int32_t(UNR_TABLE[index]) + 0x101;
    auto d1 = int32_t(d | 0x8000);
    auto tmp = ((d1 * -factor) + 0x80) >> 8;
    auto r = ((factor * (0x20000 + tmp)) + 0x80) >> 8;

    return uint32_t(r);
  }

  int leading_zeros(uint16_t value) {
    int n, m;

    m = 1 << 15;

    while (!(value & m) && n < 16) {
      m = m >> 1;
      n = n + 1;
    }

    return n;
  }

  uint32_t divide(uint16_t numerator, uint16_t divisor) {
    auto shift = leading_zeros(divisor);

    auto n = uint64_t(numerator) << shift;
    auto d = divisor << shift;

    auto r = uint64_t(reciprocal(d));

    auto res = (n * r + 0x8000) >> 16;

    if (res <= 0x1ffff) {
      return uint32_t(res);
    } else {
      return 0x1ffff;
    }
  }
}



void command(cop2_state_t *self, uint32_t command) {
  auto opcode = command & 0x3f;
  auto config = CommandConfig::from_command(command);

  // Clear flags prior to command execution
  self->flags = 0;

  switch (opcode) {
  case 0x01: return cmd_rtps(self, config);
  case 0x06: return cmd_nclip(self);
  case 0x0c: return cmd_op(self, config);
  case 0x10: return cmd_dpcs(self, config);
  case 0x11: return cmd_intpl(self, config);
  case 0x12: return cmd_mvmva(self, config);
  case 0x13: return cmd_ncds(self, config);
  case 0x16: return cmd_ncdt(self, config);
  case 0x1b: return cmd_nccs(self, config);
  case 0x1c: return cmd_cc(self, config);
  case 0x1e: return cmd_ncs(self, config);
  case 0x20: return cmd_nct(self, config);
  case 0x28: return cmd_sqr(self, config);
  case 0x29: return cmd_dcpl(self, config);
  case 0x2a: return cmd_dpct(self, config);
  case 0x2d: return cmd_avsz3(self);
  case 0x2e: return cmd_avsz4(self);
  case 0x30: return cmd_rtpt(self, config);
  case 0x3d: return cmd_gpf(self, config);
  case 0x3e: return cmd_gpl(self, config);
  case 0x3f: return cmd_ncct(self, config);
  default:
    printf("Unhandled GTE opcode %07x\n", opcode);
    break;
  }

  // Update the flags MSB: OR together bits [30:23] + [18:13]
  auto msb = (self->flags & 0x7f87e000) != 0;
  self->flags |= (msb << 31);
}

uint32_t control(cop2_state_t *self, uint32_t reg) {
  switch (reg) {
    case 0: {
      auto &matrix = self->matrices[matrix_type_t::Rotation];

      auto v0 = uint32_t(uint16_t(matrix[0][0]));
      auto v1 = uint32_t(uint16_t(matrix[0][1]));

      return v0 | (v1 << 16);
    }
    case 1: {
      auto &matrix = self->matrices[matrix_type_t::Rotation];

      auto v0 = uint32_t(uint16_t(matrix[0][2]));
      auto v1 = uint32_t(uint16_t(matrix[1][0]));

      return v0 | (v1 << 16);
    }
    case 2: {
      auto &matrix = self->matrices[matrix_type_t::Rotation];

      auto v0 = uint32_t(uint16_t(matrix[1][1]));
      auto v1 = uint32_t(uint16_t(matrix[1][2]));

      return v0 | (v1 << 16);
    }
    case 3: {
      auto &matrix = self->matrices[matrix_type_t::Rotation];

      auto v0 = uint32_t(uint16_t(matrix[2][0]));
      auto v1 = uint32_t(uint16_t(matrix[2][1]));

      return v0 | (v1 << 16);
    }
    case 4: {
      auto &matrix = self->matrices[matrix_type_t::Rotation];

      return uint32_t(matrix[2][2]);
    }
    case 5:
    case 6:
    case 7: {
      auto &vector = self->control_vectors[control_vector_type_t::Translation];

      return uint32_t(vector[reg - 5]);
    }
    case 8: {
      auto &matrix = self->matrices[matrix_type_t::Light];

      auto v0 = uint32_t(uint16_t(matrix[0][0]));
      auto v1 = uint32_t(uint16_t(matrix[0][1]));

      return v0 | (v1 << 16);
    }
    case 9: {
      auto &matrix = self->matrices[matrix_type_t::Light];

      auto v0 = uint32_t(uint16_t(matrix[0][2]));
      auto v1 = uint32_t(uint16_t(matrix[1][0]));

      return v0 | (v1 << 16);
    }
    case 10: {
      auto &matrix = self->matrices[matrix_type_t::Light];

      auto v0 = uint32_t(uint16_t(matrix[1][1]));
      auto v1 = uint32_t(uint16_t(matrix[1][2]));

      return v0 | (v1 << 16);
    }
    case 11: {
      auto &matrix = self->matrices[matrix_type_t::Light];

      auto v0 = uint32_t(uint16_t(matrix[2][0]));
      auto v1 = uint32_t(uint16_t(matrix[2][1]));

      return v0 | (v1 << 16);
    }
    case 12: {
      auto &matrix = self->matrices[matrix_type_t::Light];

      return uint32_t(matrix[2][2]);
    }
    case 13:
    case 14:
    case 15: {
      auto &vector = self->control_vectors[control_vector_type_t ::BackgroundColor];

      return uint32_t(vector[reg - 13]);
    }
    case 16: {
      auto &matrix = self->matrices[matrix_type_t::Color];

      auto v0 = uint32_t(uint16_t(matrix[0][0]));
      auto v1 = uint32_t(uint16_t(matrix[0][1]));

      return v0 | (v1 << 16);
    }
    case 17: {
      auto &matrix = self->matrices[matrix_type_t::Color];

      auto v0 = uint32_t(uint16_t(matrix[0][2]));
      auto v1 = uint32_t(uint16_t(matrix[1][0]));

      return v0 | (v1 << 16);
    }
    case 18: {
      auto &matrix = self->matrices[matrix_type_t::Color];

      auto v0 = uint32_t(uint16_t(matrix[1][1]));
      auto v1 = uint32_t(uint16_t(matrix[1][2]));

      return v0 | (v1 << 16);
    }
    case 19: {
      auto &matrix = self->matrices[matrix_type_t::Color];

      auto v0 = uint32_t(uint16_t(matrix[2][0]));
      auto v1 = uint32_t(uint16_t(matrix[2][1]));

      return v0 | (v1 << 16);
    }
    case 20: {
      auto &matrix = self->matrices[matrix_type_t::Color];

      return uint32_t(matrix[2][2]);
    }
  case 21:
  case 22:
  case 23: {
      auto &vector = self->control_vectors[control_vector_type_t::FarColor];

      return uint32_t(vector[reg - 21]);
    }
  case 24: return uint32_t(self->ofx);
  case 25: return uint32_t(self->ofy);
    // H reads back as a signed value even though unsigned)
  case 26: return uint32_t(int16_t(self->h));
  case 27: return uint32_t(self->dqa);
  case 28: return uint32_t(self->dqb);
  case 29: return uint32_t(self->zsf3);
  case 30: return uint32_t(self->zsf4);
  case 31: return self->flags;
  default:
    printf("Unhandled GTE control register %d\n", reg);
    break;
}
}

void set_control(cop2_state_t *self, uint32_t reg, uint32_t val) {
  switch (reg) {
  case 0: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Rotation];

    matrix[0][0] = v0;
    matrix[0][1] = v1;
    break;
  }
  case 1: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Rotation];

    matrix[0][2] = v0;
    matrix[1][0] = v1;
    break;
  }
  case 2: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Rotation];

    matrix[1][1] = v0;
    matrix[1][2] = v1;
    break;
  }
  case 3: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Rotation];

    matrix[2][0] = v0;
    matrix[2][1] = v1;
    break;
  }
  case 4: {
    auto &matrix = self->matrices[matrix_type_t::Rotation];

    matrix[2][2] = int16_t(val);
    break;
  }
  case 5: case 6: case 7: {
    auto &vector = self->control_vectors[control_vector_type_t::Translation];

    vector[reg - 5] = int32_t(val);
    break;
  }
  case 8: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Light];

    matrix[0][0] = v0;
    matrix[0][1] = v1;
    break;
  }
  case 9: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Light];

    matrix[0][2] = v0;
    matrix[1][0] = v1;
    break;
  }
  case 10: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Light];

    matrix[1][1] = v0;
    matrix[1][2] = v1;
    break;
  }
  case 11: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Light];

    matrix[2][0] = v0;
    matrix[2][1] = v1;
    break;
  }
  case 12: {
    auto &matrix = self->matrices[matrix_type_t::Light];

    matrix[2][2] = int16_t(val);
    break;
  }
  case 13: case 14: case 15: {
    auto &vector = self->control_vectors[control_vector_type_t::BackgroundColor];

    vector[reg - 13] = int32_t(val);
    break;
  }
  case 16: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Color];

    matrix[0][0] = v0;
    matrix[0][1] = v1;
    break;
  }
  case 17: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Color];

    matrix[0][2] = v0;
    matrix[1][0] = v1;
    break;
  }
  case 18: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Color];

    matrix[1][1] = v0;
    matrix[1][2] = v1;
    break;
  }
  case 19: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    auto &matrix = self->matrices[matrix_type_t::Color];

    matrix[2][0] = v0;
    matrix[2][1] = v1;
    break;
  }
  case 20: {
    auto &matrix = self->matrices[matrix_type_t::Color];

    matrix[2][2] = int16_t(val);
    break;
  }
  case 21: case 22: case 23: {
    auto &vector = self->control_vectors[control_vector_type_t::FarColor];

    vector[reg - 21] = int32_t(val);
    break;
  }
  case 24: self->ofx = int32_t(val); break;
  case 25: self->ofy = int32_t(val); break;
  case 26: self->h = uint16_t(val); break;
  case 27: self->dqa = int16_t(val); break;
  case 28: self->dqb = int32_t(val); break;
  case 29: self->zsf3 = int16_t(val); break;
  case 30: self->zsf4 = int16_t(val); break;
  case 31: {
    self->flags = val & 0x7ffff00;

    auto msb = (val & 0x7f87e000) != 0;

    self->flags |= uint32_t(msb) << 31;
    break;
  }
  default:
    printf("Unhandled GTE control register %d %x\n", reg, val);
    break;
  }
}

uint32_t data(cop2_state_t *self, uint32_t reg) {
  auto rgbx_to_u32 = []( cop2_state_t::color_t rgbx ) -> uint32_t {
    auto r = uint32_t(rgbx.r);
    auto g = uint32_t(rgbx.g);
    auto b = uint32_t(rgbx.b);
    auto x = uint32_t(rgbx.c);

    return r | (g << 8) | (b << 16) | (x << 24);
  };

  auto xy_to_u32 = []( cop2_state_t::point_t xy ) -> uint32_t {
    auto x = uint16_t(xy.x);
    auto y = uint16_t(xy.y);

    return uint32_t(x) | (uint32_t(y) << 16);
  };

  switch (reg) {
  case 0: {
    auto v0 = uint32_t(uint16_t(self->v[0][0]));
    auto v1 = uint32_t(uint16_t(self->v[0][1]));

    return v0 | (v1 << 16);
  }
  case 1: return uint32_t(self->v[0][2]);
  case 2: {
    auto v0 = uint32_t(uint16_t(self->v[1][0]));
    auto v1 = uint32_t(uint16_t(self->v[1][1]));

    return v0 | (v1 << 16);
  }
  case 3: return uint32_t(self->v[1][2]);
  case 4: {
    auto v0 = uint32_t(uint16_t(self->v[2][0]));
    auto v1 = uint32_t(uint16_t(self->v[2][1]));

    return v0 | (v1 << 16);
  }
  case 5: return uint32_t(self->v[2][2]);
  case 6: return uint32_t(rgbx_to_u32(self->rgb));
  case 7: return uint32_t(self->otz);
  case 8: return uint32_t(self->ir[0]);
  case 9: return uint32_t(self->ir[1]);
  case 10: return uint32_t(self->ir[2]);
  case 11: return uint32_t(self->ir[3]);
  case 12: return uint32_t(xy_to_u32(self->xy_fifo[0]));
  case 13: return uint32_t(xy_to_u32(self->xy_fifo[1]));
  case 14: return uint32_t(xy_to_u32(self->xy_fifo[2]));
  case 15: return uint32_t(xy_to_u32(self->xy_fifo[3]));
  case 16: return uint32_t(self->z_fifo[0]);
  case 17: return uint32_t(self->z_fifo[1]);
  case 18: return uint32_t(self->z_fifo[2]);
  case 19: return uint32_t(self->z_fifo[3]);
  case 20: return uint32_t(rgbx_to_u32(self->rgb_fifo[0]));
  case 21: return uint32_t(rgbx_to_u32(self->rgb_fifo[1]));
  case 22: return uint32_t(rgbx_to_u32(self->rgb_fifo[2]));
  case 23: return uint32_t(self->reg_23);
  case 24: return uint32_t(self->mac[0]);
  case 25: return uint32_t(self->mac[1]);
  case 26: return uint32_t(self->mac[2]);
  case 27: return uint32_t(self->mac[3]);
  case 28:
  case 29: {
      auto saturate = [](int16_t v) -> uint32_t {
        if (v < 0x00) { return 0x00; }
        if (v > 0x1f) { return 0x1f; }
        return uint32_t(v);
      };

      auto a = saturate(self->ir[1] >> 7);
      auto b = saturate(self->ir[2] >> 7);
      auto c = saturate(self->ir[3] >> 7);

      return a | (b << 5) | (c << 10);
    }
  case 30: return uint32_t(self->lzcs);
  case 31: return uint32_t(self->lzcr);
  default:
    printf("Unhandled GTE data register %d\n", reg);
    break;
  }
}

void set_data(cop2_state_t *self, uint32_t reg, uint32_t val) {

  auto val_to_rgbx = [val]() -> cop2_state_t::color_t {
    auto r = uint8_t(val);
    auto g = uint8_t(val >> 8);
    auto b = uint8_t(val >> 16);
    auto x = uint8_t(val >> 24);

    return {x, b, g, r};
  };

  auto val_to_xy = [val]() -> cop2_state_t::point_t {
    auto x = int16_t(val);
    auto y = int16_t(val >> 16);

    return {x, y};
  };

  switch (reg) {
  case 0: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    self->v[0][0] = v0;
    self->v[0][1] = v1;
    break;
  }
  case 1: self->v[0][2] = int16_t(val); break;
  case 2: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    self->v[1][0] = v0;
    self->v[1][1] = v1;
    break;
  }
  case 3: self->v[1][2] = int16_t(val); break;
  case 4: {
    auto v0 = int16_t(val);
    auto v1 = int16_t(val >> 16);

    self->v[2][0] = v0;
    self->v[2][1] = v1;
    break;
  }
  case 5: self->v[2][2] = int16_t(val); break;
  case 6: self->rgb = val_to_rgbx(); break;
  case 7: self->otz = uint16_t(val); break;
  case 8: self->ir[0] = int16_t(val); break;
  case 9: self->ir[1] = int16_t(val); break;
  case 10: self->ir[2] = int16_t(val); break;
  case 11: self->ir[3] = int16_t(val); break;
  case 12: self->xy_fifo[0] = val_to_xy(); break;
  case 13: self->xy_fifo[1] = val_to_xy(); break;
  case 14: {
    auto xy = val_to_xy();
    self->xy_fifo[2] = xy;
    self->xy_fifo[3] = xy;
    break;
  }
  case 15: {
    self->xy_fifo[3] = val_to_xy();
    self->xy_fifo[0] = self->xy_fifo[1];
    self->xy_fifo[1] = self->xy_fifo[2];
    self->xy_fifo[2] = self->xy_fifo[3];
    break;
  }
  case 16: self->z_fifo[0] = uint16_t(val); break;
  case 17: self->z_fifo[1] = uint16_t(val); break;
  case 18: self->z_fifo[2] = uint16_t(val); break;
  case 19: self->z_fifo[3] = uint16_t(val); break;
  case 20: self->rgb_fifo[0] = val_to_rgbx(); break;
  case 21: self->rgb_fifo[1] = val_to_rgbx(); break;
  case 22: self->rgb_fifo[2] = val_to_rgbx(); break;
  case 23: self->reg_23 = val; break;
  case 24: self->mac[0] = int32_t(val); break;
  case 25: self->mac[1] = int32_t(val); break;
  case 26: self->mac[2] = int32_t(val); break;
  case 27: self->mac[3] = int32_t(val); break;
  case 28: {
    auto to_ir = [](uint32_t v) -> int16_t {
      return int16_t((v & 0x1f) << 7);
    };

    self->ir[0] = to_ir(val);
    self->ir[1] = to_ir(val >> 5);
    self->ir[2] = to_ir(val >> 10);
  }
  case 29: break;
  case 30: {
    self->lzcs = val;

    // If val is negative we count the leading ones,
    // otherwise we count the leading zeroes.
    auto tmp = ((val >> 31) & 1) ? ~val : val;

    self->lzcr = uint8_t(divider::leading_zeros(tmp));
  }
  case 31:
    // printf("Write to read-only GTE data register 31\n");
    break;

  default:
    // unreachable();
    break;
  }
}

void cmd_rtps(cop2_state_t *self, command_config_t config) {
  // Transform vector 0
  auto projection_factor = do_rtp(self, config, 0);

  depth_queuing(self, projection_factor);
}

void cmd_nclip(cop2_state_t *self) {
  auto x0 = int32_t(self->xy_fifo[0].x);
  auto y0 = int32_t(self->xy_fifo[0].y);
  auto x1 = int32_t(self->xy_fifo[1].x);
  auto y1 = int32_t(self->xy_fifo[1].y);
  auto x2 = int32_t(self->xy_fifo[2].x);
  auto y2 = int32_t(self->xy_fifo[2].y);

  // Convert to 32bits

  auto a = x0 * (y1 - y2);
  auto b = x1 * (y2 - y0);
  auto c = x2 * (y0 - y1);

  // Compute the sum in 64bits to detect overflows
  auto sum = int64_t(a) + int64_t(b) + int64_t(c);

  check_mac_overflow(self, sum);

  self->mac[0] = int32_t(sum);
}

void cmd_op(cop2_state_t *self, command_config_t config) {
  auto rm = matrix_type_t::Rotation;

  auto ir1 = int32_t(self->ir[1]);
  auto ir2 = int32_t(self->ir[2]);
  auto ir3 = int32_t(self->ir[3]);

  auto r0 = int32_t(self->matrices[rm][0][0]);
  auto r1 = int32_t(self->matrices[rm][1][1]);
  auto r2 = int32_t(self->matrices[rm][2][2]);

  auto shift = config.shift;

  self->mac[1] = (r1 * ir3 - r2 * ir2) >> shift;
  self->mac[2] = (r2 * ir1 - r0 * ir3) >> shift;
  self->mac[3] = (r0 * ir2 - r1 * ir1) >> shift;

  mac_to_ir(self, config);
}

void cmd_dpcs(cop2_state_t *self, command_config_t config) {
  auto r = self->rgb.r;
  auto g = self->rgb.g;
  auto b = self->rgb.b;

  uint8_t col[3] = {r, g, b};

  for (auto i = 0; i < 3; i++) {
    auto fc = int64_t(self->control_vectors[control_vector_type_t::FarColor][i]) << 12;
    auto col1 = int64_t(col[i]) << (4 + 12);

    auto sub = fc - col1;

    auto tmp = int32_t(i64_to_i44(self, i, sub) >> config.shift);

    auto ir0 = int64_t(self->ir[0]);

    auto sat = int64_t(i32_to_i16_saturate(self, CommandConfig::from_command(0), i, tmp));

    auto res = i64_to_i44(self, i, col1 + ir0 * sat);

    self->mac[i + 1] = int32_t(res >> config.shift);
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_dcpl(cop2_state_t *self, command_config_t config) {
  auto r = self->rgb.r;
  auto g = self->rgb.g;
  auto b = self->rgb.b;

  uint8_t col[3] = {r, g, b};

  for (int i = 0; i < 3; i++) {
    auto fc = int64_t(self->control_vectors[control_vector_type_t::FarColor][i]) << 12;
    auto ir = int32_t(self->ir[i + 1]);
    auto col1 = int32_t(col[i]) << 4;

    auto shading = int64_t(col1 * ir);

    auto tmp = fc - shading;
    auto tmp1 = int32_t(i64_to_i44(self, i, tmp) >> config.shift);

    auto ir0 = int64_t(self->ir[0]);
    auto res = int64_t(i32_to_i16_saturate(self, CommandConfig::from_command(0), i, tmp1));
    auto res1 = i64_to_i44(self, i, shading + ir0 * res);

    self->mac[i + 1] = int32_t(res1 >> config.shift);
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_dpct(cop2_state_t *self, command_config_t config) {
  // Each call uses the oldest entry in the RGB FIFO and pushes
  // the result at the top so the three calls will process and
  // replace the entire contents of the FIFO
  do_dpc(self, config);
  do_dpc(self, config);
  do_dpc(self, config);
}

void do_dpc(cop2_state_t *self, command_config_t config) {
  auto r = self->rgb_fifo[0].r;
  auto g = self->rgb_fifo[0].g;
  auto b = self->rgb_fifo[0].b;

  uint8_t col[3] = {r, g, b};

  for (int i = 0; i < 3; i++) {
    auto fc = int64_t(self->control_vectors[control_vector_type_t::FarColor][i]) << 12;
    auto col1 = int64_t(col[i]) << (4 + 12);

    auto sub = fc - col1;

    auto tmp = int32_t(i64_to_i44(self, i, sub) >> config.shift);

    auto ir0 = int64_t(self->ir[0]);

    auto sat = int64_t(i32_to_i16_saturate(self, CommandConfig::from_command(0), i, tmp));

    auto res = i64_to_i44(self, i, col1 + ir0 * sat);

    self->mac[i + 1] = int32_t(res >> config.shift);
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_intpl(cop2_state_t *self, command_config_t config) {
  // XXX this is very similar to the loop in DPCS above, we
  // could probably factor that.
  for (int i = 0; i < 3; i++) {
    auto fc = int64_t(self->control_vectors[control_vector_type_t::FarColor][i]) << 12;
    auto ir = int64_t(self->ir[i + 1]) << 12;

    auto sub = fc - ir;

    auto tmp = int32_t(i64_to_i44(self, i, sub) >> config.shift);

    auto ir0 = int64_t(self->ir[0]);

    auto sat = int64_t(i32_to_i16_saturate(self, CommandConfig::from_command(0), i, tmp));

    auto res = i64_to_i44(self, i, ir + ir0 * sat);

    self->mac[i + 1] = int32_t(res >> config.shift);
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_mvmva(cop2_state_t *self, command_config_t config) {
  // The fourth vector holds IR values
  self->v[3][0] = self->ir[1];
  self->v[3][1] = self->ir[2];
  self->v[3][2] = self->ir[3];

  multiply_matrix_by_vector(self, config, config.matrix, config.vector_mul, config.vector_add);
}

void cmd_ncds(cop2_state_t *self, command_config_t config) {
  do_ncd(self, config, 0);
}

void cmd_ncdt(cop2_state_t *self, command_config_t config) {
  do_ncd(self, config, 0);
  do_ncd(self, config, 1);
  do_ncd(self, config, 2);
}

void cmd_nccs(cop2_state_t *self, command_config_t config) {
  do_ncc(self, config, 0);
}

void cmd_cc(cop2_state_t *self, command_config_t config) {
  self->v[3][0] = self->ir[1];
  self->v[3][1] = self->ir[2];
  self->v[3][2] = self->ir[3];

  multiply_matrix_by_vector(self, config, matrix_type_t::Color, 3, control_vector_type_t::BackgroundColor);

  uint8_t col[3] = {self->rgb.r, self->rgb.g, self->rgb.b};

  for (int i = 0; i < 3; i++) {
    auto ir = int32_t(self->ir[i + 1]);
    auto col1 = int32_t(col[i]) << 4;

    self->mac[i + 1] = (ir * col1) >> config.shift;
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_ncs(cop2_state_t *self, command_config_t config) {
  do_nc(self, config, 0);
}

void cmd_nct(cop2_state_t *self, command_config_t config) {
  do_nc(self, config, 0);
  do_nc(self, config, 1);
  do_nc(self, config, 2);
}

void cmd_sqr(cop2_state_t *self, command_config_t config) {
  for (int i = 1; i < 4; i++) {
    auto ir = int32_t(self->ir[i]);

    self->mac[i] = (ir * ir) >> config.shift;
  }

  mac_to_ir(self, config);
}

void cmd_avsz3(cop2_state_t *self) {
  auto z1 = uint32_t(self->z_fifo[1]);
  auto z2 = uint32_t(self->z_fifo[2]);
  auto z3 = uint32_t(self->z_fifo[3]);

  auto sum = z1 + z2 + z3;

  // The average factor should generally be set to 1/3 of the
  // ordering table size. So for instance for a table of 1024
  // entries it should be set at 341 to use the full table
  // granularity.
  auto zsf3 = int64_t(self->zsf3);

  auto average = zsf3 * int64_t(sum);

  check_mac_overflow(self, average);

  self->mac[0] = int32_t(average);
  self->otz = i64_to_otz(self, average);
}

void cmd_avsz4(cop2_state_t *self) {
  auto z0 = uint32_t(self->z_fifo[0]);
  auto z1 = uint32_t(self->z_fifo[1]);
  auto z2 = uint32_t(self->z_fifo[2]);
  auto z3 = uint32_t(self->z_fifo[3]);

  auto sum = z0 + z1 + z2 + z3;

  // The average factor should generally be set to 1/4 of the
  // ordering table size. So for instance for a table of 1024
  // entries it should be set at 256 to use the full table
  // granularity.
  auto zsf4 = int64_t(self->zsf4);

  auto average = zsf4 * int64_t(sum);

  check_mac_overflow(self, average);

  self->mac[0] = int32_t(average);
  self->otz = i64_to_otz(self, average);
}

void cmd_rtpt(cop2_state_t *self, command_config_t config) {

  // Transform the three vectors at once
  do_rtp(self, config, 0);
  do_rtp(self, config, 1);
  // We do depth queuing on the last vector
  auto projection_factor = do_rtp(self, config, 2);

  depth_queuing(self, projection_factor);
}

void cmd_gpf(cop2_state_t *self, command_config_t config) {
  auto ir0 = int32_t(self->ir[0]);

  for (int i = 1; i < 4; i++) {
    auto ir = int32_t(self->ir[i]);

    self->mac[i] = (ir * ir0) >> config.shift;
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_gpl(cop2_state_t *self, command_config_t config) {
  auto ir0 = int32_t(self->ir[0]);

  auto shift = config.shift;

  for (int i = 1; i < 4; i++) {
    auto ir = int32_t(self->ir[i]);

    auto ir_prod = int64_t(ir * ir0);

    auto mac = int64_t(self->mac[i]) << shift;

    auto sum = i64_to_i44(self, (i - 1), mac + ir_prod);

    self->mac[i] = int32_t(sum >> shift);
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void cmd_ncct(cop2_state_t *self, command_config_t config) {
  // Transform the three vectors at once
  do_ncc(self, config, 0);
  do_ncc(self, config, 1);
  do_ncc(self, config, 2);
}

void do_ncc(cop2_state_t *self, command_config_t config, uint8_t vector_index) {
  multiply_matrix_by_vector(self, config, matrix_type_t::Light, vector_index, control_vector_type_t::Zero);

  // Use the 4th vector to store the intermediate values
  self->v[3][0] = self->ir[1];
  self->v[3][1] = self->ir[2];
  self->v[3][2] = self->ir[3];

  multiply_matrix_by_vector(self, config, matrix_type_t::Color, 3, control_vector_type_t::BackgroundColor);


  uint8_t col[3] = {self->rgb.r, self->rgb.g, self->rgb.b};

  for (int i = 0; i < 3; i++) {
    auto col1 = int32_t(col[i]) << 4;
    auto ir = int32_t(self->ir[i + 1]);

    self->mac[i + 1] = (col1 * ir) >> config.shift;
  }

  mac_to_ir(self, config);
  mac_to_rgb_fifo(self);
}

void do_nc(cop2_state_t *self, command_config_t config, uint8_t vector_index) {
  multiply_matrix_by_vector(self, config, matrix_type_t::Light, vector_index, control_vector_type_t::Zero);

  self->v[3][0] = self->ir[1];
  self->v[3][1] = self->ir[2];
  self->v[3][2] = self->ir[3];

  multiply_matrix_by_vector(self, config, matrix_type_t::Color, 3, control_vector_type_t::BackgroundColor);

  mac_to_rgb_fifo(self);
}

void do_ncd(cop2_state_t *self, command_config_t config, uint8_t vector_index) {

  multiply_matrix_by_vector(self, config, matrix_type_t::Light, vector_index, control_vector_type_t::Zero);

  // Use the custom 4th vector to store the intermediate
  // values. This vector does not exist in the real hardware (at
  // least not in the registers), it's just a hack to make the
  // code simpler.
  self->v[3][0] = self->ir[1];
  self->v[3][1] = self->ir[2];
  self->v[3][2] = self->ir[3];

  multiply_matrix_by_vector(self, config, matrix_type_t::Color, 3, control_vector_type_t::BackgroundColor);

  cmd_dcpl(self, config);
}

void multiply_matrix_by_vector(cop2_state_t *self, command_config_t config, matrix_type_t matrix, uint8_t vector_index, control_vector_type_t control_vector) {

  auto vector_index1 = unsigned(vector_index);

  if (matrix == matrix_type_t::Invalid) {
    // This results in a pointless calculation. Mednafen's
    // code has the details, for now I think we can safely
    // ignore it.
    printf("GTE multiplication with invalid matrix\n");
  }

  if (control_vector == control_vector_type_t::FarColor) {
    // Multiplication with this vector is buggy and needs
    // special handling. Again, Mednafen's code has the
    // details.
    printf("GTE multiplication with far color vector\n");
  }

  auto mat = unsigned(matrix);
  auto crv = unsigned(control_vector);

  // Iterate over the matrix rows
  for (int r = 0; r < 3; r++) {
    // First the control_vector is added to the result
    auto res = int64_t(self->control_vectors[crv][r]) << 12;

    // Iterate over the matrix columns
    for (int c = 0; c < 3; c++) {
      auto v = int32_t(self->v[vector_index1][c]);
      auto m = int32_t(self->matrices[mat][r][c]);

      auto product = v * m;

      // The operation is done using 44bit signed
      // arithmetics.
      res = i64_to_i44(self, r, res + int64_t(product));
    }

    // Store the result in the accumulator
    self->mac[r + 1] = int32_t(res >> config.shift);
  }

  mac_to_ir(self, config);
}

void mac_to_ir(cop2_state_t *self, command_config_t config) {
  self->ir[1] = i32_to_i16_saturate(self, config, 0, self->mac[1]);
  self->ir[2] = i32_to_i16_saturate(self, config, 1, self->mac[2]);
  self->ir[3] = i32_to_i16_saturate(self, config, 2, self->mac[3]);
}

uint8_t mac_to_color(cop2_state_t *self, int32_t mac, uint8_t which) {
  auto c = mac >> 4;

  if (c < 0) {
    set_flag(self, 21 - which);
    return 0;
  } else if (c > 0xff) {
    set_flag(self, 21 - which);
    return 0xff;
  } else {
    return uint8_t(c);
  }
}

void mac_to_rgb_fifo(cop2_state_t *self) {
  auto mac1 = self->mac[1];
  auto mac2 = self->mac[2];
  auto mac3 = self->mac[3];

  auto r = mac_to_color(self, mac1, 0);
  auto g = mac_to_color(self, mac2, 1);
  auto b = mac_to_color(self, mac3, 2);

  self->rgb_fifo[0] = self->rgb_fifo[1];
  self->rgb_fifo[1] = self->rgb_fifo[2];
  self->rgb_fifo[2].r = r;
  self->rgb_fifo[2].g = g;
  self->rgb_fifo[2].b = b;
  self->rgb_fifo[2].c = self->rgb.c;
}

uint32_t do_rtp(cop2_state_t *self, command_config_t config, unsigned vector_index) {
  // The computed Z coordinate with unconditional 12bit shift
  // applied
  auto z_shifted = 0;

  // Step 1: we compute "tr + vector * rm" and store the 32 bit
  // result in MAC 0, 1 and 2.
  auto rm = matrix_type_t::Rotation;
  auto tr = control_vector_type_t::Translation;

  // Iterate over the matrix rows
  for (int r = 0; r < 3; r++) {
    // Start with the translation. Convert translation vector
    // component from i32 to i64 with 12 fractional bits
    auto res = int64_t(self->control_vectors[tr][r]) << 12;

    // Iterate over the rotation matrix columns
    for (int c = 0; c < 3; c++) {
      auto v = int32_t(self->v[vector_index][c]);
      auto m = int32_t(self->matrices[rm][r][c]);

      auto rot = v * m;

      // The operation is done using 44bit signed
      // arithmetics.
      res = i64_to_i44(self, c, res + int64_t(rot));
    }

    // Store the result in the accumulator
    self->mac[r + 1] = int32_t(res >> config.shift);

    // The last result will be Z, we can overwrite it each
    // time and the last one will be the good one.
    z_shifted = int32_t(res >> 12);
  }

  // Step 2: we take the 32bit camera coordinates in MAC and
  // convert them to 16bit values in the IR vector, saturating
  // them in case of an overflow.

  // 16bit clamped x coordinate
  self->ir[1] = i32_to_i16_saturate(self, config, 0, self->mac[1]);
  // 16bit clamped y coordinate
  self->ir[2] = i32_to_i16_saturate(self, config, 1, self->mac[2]);
  // 16bit clamped z coordinate
  //
  // Weird behaviour of the clamping/overflow checking, taken
  // from mednafen's source code: apparently the value of IR3 is
  // a clamped value taken from MAC like the two others but the
  // overflow flag is computed against the `z_shifted` value
  // instead. So if `config.shift` is not set it's possible to
  // have a clamped value but no oveflow flag set.
  //
  // Start by testing the overflow
  if (z_shifted > 32767 || z_shifted < (-32768)) {
    set_flag(self, 22);
  }

  // Then clamp the value normally
  auto min2 = config.clamp_negative ? 0 : -32768;
  auto val = self->mac[3];

  if (val < min2) {
    self->ir[3] = int16_t(min2);
  } else if (val > 32767) {
    self->ir[3] = 32767;
  } else {
    self->ir[3] = int16_t(val);
  }

  // Step 3: convert the shifted Z value to a 16bit unsigned
  // integer (with saturation) and push it onto the Z_FIFO

  uint16_t z_saturated;
  if (z_shifted < 0) {
    set_flag(self, 18);
    z_saturated = 0;
  } else if (z_shifted > 65535) {
    set_flag(self, 18);
    z_saturated = 65535;
  } else {
    z_saturated = uint16_t(z_shifted);
  }

  // Push `z_saturated` onto the Z_FIFO
  self->z_fifo[0] = self->z_fifo[1];
  self->z_fifo[1] = self->z_fifo[2];
  self->z_fifo[2] = self->z_fifo[3];
  self->z_fifo[3] = z_saturated;

  // Step 3: perspective projection against the screen plane

  // Compute the projection factor by dividing projection plane
  // distance by the Z coordinate

  // Projection factor: 1.16 unsigned
  int projection_factor;
  if (z_saturated > (self->h / 2)) {
    // GTE-specific division algorithm for dist /
    // z. Returns a saturated 17bit value.
    projection_factor = divider::divide(self->h, z_saturated);
  } else {
    // If the Z coordinate is smaller than or equal to
    // half the projection plane distance we clip it
    set_flag(self, 17);

    projection_factor = 0x1ffff;
  }

  // Work in 64bits to detect overflows
  auto factor = int64_t(projection_factor);
  auto x = int64_t(self->ir[1]);
  auto y = int64_t(self->ir[2]);
  auto ofx = int64_t(self->ofx);
  auto ofy = int64_t(self->ofy);

  // Project X and Y onto the plane
  auto screen_x = x * factor + ofx;
  auto screen_y = y * factor + ofy;

  check_mac_overflow(self, screen_x);
  check_mac_overflow(self, screen_y);

  auto screen_x1 = int32_t(screen_x >> 16);
  auto screen_y1 = int32_t(screen_y >> 16);

  // Push onto the XY FIFO
  self->xy_fifo[3].x = i32_to_i11_saturate(self, 0, screen_x1);
  self->xy_fifo[3].y = i32_to_i11_saturate(self, 1, screen_y1);
  self->xy_fifo[0] = self->xy_fifo[1];
  self->xy_fifo[1] = self->xy_fifo[2];
  self->xy_fifo[2] = self->xy_fifo[3];

  // return projection factor
  return projection_factor;
}

void depth_queuing(cop2_state_t *self, uint32_t projection_factor) {
  // Work in 64bits to detect overflows
  auto factor = int64_t(projection_factor);
  auto dqa = int64_t(self->dqa);
  auto dqb = int64_t(self->dqb);

  auto depth = dqb + dqa * factor;

  check_mac_overflow(self, depth);

  self->mac[0] = int32_t(depth);

  // Compute 16bit IR value
  auto depth1 = depth >> 12;

  if (depth1 < 0) {
    set_flag(self, 12);
    self->ir[0] = 0;
  } else if (depth1 > 4096) {
    set_flag(self, 12);
    self->ir[0] = 4096;
  } else {
    self->ir[0] = int16_t(depth1);
  }
}

void set_flag(cop2_state_t *self, uint8_t bit) {
  self->flags |= 1 << bit;
}

int64_t i64_to_i44(cop2_state_t *self, uint8_t flag, int64_t val) {
  if (val > 0x7ffffffffffLL) {
    set_flag(self, 30 - flag);
  } else if (val < -0x80000000000LL) {
    set_flag(self, 27 - flag);
  }

  return (val << (64 - 44)) >> (64 - 44);
}

int16_t i32_to_i16_saturate(cop2_state_t *self, command_config_t config, uint8_t flag, int32_t val) {
  int16_t min = config.clamp_negative ? 0 : (-32768);
  int16_t max = 32767;

  if (val > max) {
    set_flag(self, 24 - flag);

    // Clamp to max
    return max;
  } else if (val < min) {
    set_flag(self, 24 - flag);

    // Clamp to min
    return min;
  } else {
    return int16_t(val);
  }
}

int16_t i32_to_i11_saturate(cop2_state_t *self, uint8_t flag, int32_t val) {
  if (val < -0x400) {
    set_flag(self, 14 - flag);
    return (-0x400);
  } else if (val > 0x3ff) {
    set_flag(self, 14 - flag);
    return 0x3ff;
  } else {
    return int16_t(val);
  }
}

void check_mac_overflow(cop2_state_t *self, int64_t val) {
  if (val < -0x80000000) {
    set_flag(self, 15);
  } else if (val > 0x7fffffff) {
    set_flag(self, 16);
  }
}

uint16_t i64_to_otz(cop2_state_t *self, int64_t average) {
  auto value = average >> 12;

  if (value < 0) {
    set_flag(self, 18);
    return 0;
  } else if (value > 0xffff) {
    set_flag(self, 18);
    return 0xffff;
  } else {
    return uint16_t(value);
  }
}

//

void cop2::run(cop2_state_t *state, uint32_t n) {
  command(state, n);
}

uint32_t cop2::read_ccr(cop2_state_t *state, uint32_t n) {
  return control(state, n);
}

void cop2::write_ccr(cop2_state_t *state, uint32_t n, uint32_t value) {
  set_control(state, n, value);
}

uint32_t cop2::read_gpr(cop2_state_t *state, uint32_t n) {
  return data(state, n);
}

void cop2::write_gpr(cop2_state_t *state, uint32_t n, uint32_t value) {
  set_data(state, n, value);
}
