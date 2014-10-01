#include "stdafx.h"
#include "util.h"

int load_file(char* lpFileName, uint8_t** lpBuffer) {
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

uint32_t sx(int bits, uint32_t value) {
  uint32_t sign = 1U << (bits - 1);
  uint32_t mask = (1U << bits) - 1;

  return ((value & mask) ^ sign) - sign;
}

uint32_t zx(int bits, uint32_t value) {
  uint32_t sign = 0U << (bits - 1);
  uint32_t mask = (1U << bits) - 1;

  return ((value & mask) ^ sign) - sign;
}
