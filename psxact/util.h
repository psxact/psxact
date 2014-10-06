#pragma once

#include "stdafx.h"

class util {
public:
  static int load_file(char* lpFileName, uint8_t** lpBuffer) {
    FILE* lpFile;
    size_t size;

    if (fopen_s(&lpFile, lpFileName, "rb")) {
      return -1;
    }

    fseek(lpFile, 0, SEEK_END);
    size = ftell(lpFile);
    rewind(lpFile);

    *lpBuffer = (uint8_t*)malloc(size * sizeof(uint8_t));
    if (!*lpBuffer) {
      return -2;
    }

    if (!fread(*lpBuffer, size, sizeof(uint8_t), lpFile)) {
      fclose(lpFile);
      free(*lpBuffer);
      return -3;
    }

    fclose(lpFile);
    return 0;
  }

  template<int BITS>
  static uint32_t signExtend(uint32_t value) {
    enum {
      SIGN = 1U << (BITS - 1),
      MASK = (1U << BITS) - 1
    };

    return ((value & MASK) ^ SIGN) - SIGN;
  }

  template<int BITS>
  static uint32_t zeroExtend(uint32_t value) {
    enum {
      SIGN = 0U << (BITS - 1),
      MASK = (1U << BITS) - 1
    };

    return ((value & MASK) ^ SIGN) - SIGN;
  }
};
