#pragma once

#include "stdafx.hpp"

class Util {
public:
  static bool LoadFile(const char* fileName, uint8_t** buffer) {
    FILE* file;
    size_t size;

    if (fopen_s(&file, fileName, "rb")) {
      return false;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    *buffer = new uint8_t[size];
    if (!*buffer) {
      return false;
    }

    if (!fread(*buffer, size, sizeof(uint8_t), file)) {
      fclose(file);
      delete *buffer;
      return false;
    }

    fclose(file);
    return true;
  }

  template<int BITS>
  inline static uint32_t Sign(uint32_t value) {
    enum {
      SIGN = 1U << (BITS - 1),
      MASK = (1U << BITS) - 1
    };

    return ((value & MASK) ^ SIGN) - SIGN;
  }

  template<int BITS>
  inline static uint32_t Mask(uint32_t value) {
    enum {
      SIGN = 0U << (BITS - 1),
      MASK = (1U << BITS) - 1
    };

    return ((value & MASK) ^ SIGN) - SIGN;
  }
};
