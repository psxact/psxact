#include "stdafx.h"
#include "system.h"

extern uint8_t* bios;
extern uint8_t* disk;

int main(int argc, char* argv[]) {
  if (argc <= 1 || argc >= 4) {
    printf("Improper usage:\n");
    printf("$ psxact <bios path> <disk path>");
    return 1;
  }

  if (argc > 1 && load_file(argv[1], &bios)) {
    printf("Unable to load file '%s'.", argv[1]);
    return 0;
  }

  if (argc > 2 && load_file(argv[2], &disk)) {
    printf("Unable to load disk '%s'.", argv[2]);
    return 0;
  }

  psx_init();
  
  while (true) {
    psx_step();
  }

  psx_kill();

  return 0;
}
