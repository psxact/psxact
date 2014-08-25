// psxact.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "r3051/r3051.h"

#define E_FATAL -1
#define E_OK     0
#define E_USAGE +1

int _tmain(int argc, _TCHAR* argv[]) {
  if (argc < 3) {
    printf("improper usage:\n");
    printf("$ psxact <bios image> <disk image>\n");
    return E_USAGE;
  }

  return E_OK;
}
