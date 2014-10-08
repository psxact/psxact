#include "stdafx.hpp"
#include "system.hpp"

static PlayStation* playStation;

int main(int argc, char* argv[]) {
  if (argc <= 1 || argc >= 4) {
    printf("Improper usage:\n");
    printf("$ psxact <bios path> <disk path>");
    return 1;
  }

  playStation = new PlayStation();
  
  if (argc > 1 && !playStation->LoadBiosImage(argv[1])) {
    printf("Unable to load file '%s'.", argv[1]);
    return 0;
  }

  if (argc > 2 && !playStation->LoadDiskImage(argv[2])) {
    printf("Unable to load disk '%s'.", argv[2]);
    return 0;
  }

  while (true) {
    playStation->Step();
  }

  delete playStation;

  return 0;
}
