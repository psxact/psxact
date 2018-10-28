#include "cpu/bios/decoder.hpp"

#include "utility.hpp"


using namespace psx::cpu::bios;

decoder_t::decoder_t(memory_access_t *memory)
  : memory(memory) {

  log = fopen("bios_call.log", "w");
}


void decoder_t::decode_a(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(log, "[%08x] ", pc);

  switch (function) {
    case 0x00: {
      std::string file_name = decode_string(args[0]);
      fprintf(log, "FileOpen(\"%s\", %d)\n", file_name.c_str(), args[1]);
      return;
    }

    case 0x13:
      fprintf(log, "SaveState(0x%08x)\n", args[0]);
      return;

    case 0x17: {
      std::string a = decode_string(args[0]);
      std::string b = decode_string(args[1]);
      fprintf(log, "strcmp(\"%s\", \"%s\")\n",
        a.c_str(),
        b.c_str()
      );
      return;
    }

    case 0x18: {
      std::string a = decode_string(args[0]);
      std::string b = decode_string(args[2]);
      fprintf(log, "strncmp(\"%s\", \"%s\", %d)\n",
        a.c_str(),
        b.c_str(),
        args[2]
      );
      return;
    }

    case 0x1b: {
      std::string src = decode_string(args[0]);
      fprintf(log, "strlen(\"%s\")\n", src.c_str());
      return;
    }

    case 0x25:
      fprintf(log, "toupper('%c')\n", args[0]);
      return;

    case 0x28:
      fprintf(log, "bzero(0x%08x, %d)\n", args[0], args[1]);
      return;

    case 0x2a:
      fprintf(log, "memcpy(0x%08x, 0x%08x, %d)\n", args[0], args[1], args[2]);
      return;

    case 0x33:
      fprintf(log, "malloc(%d)\n", args[0]);
      return;

    case 0x34:
      fprintf(log, "free(0x%08x)\n", args[0]);
      return;

    case 0x39:
      fprintf(log, "InitHeap(0x%08x, %d)\n", args[0], args[1]);
      return;

    case 0x3c:
      fprintf(log, "putc(\'%c\')\n", args[0]);
      return;

    case 0x3e: {
      std::string text = decode_string(args[0]);
      fprintf(log, "puts(\"%s\")\n", text.c_str());
      return;
    }

    case 0x3f: {
      std::string fmt = decode_string(args[0]);
      fprintf(log, "printf(\"%s\")\n", fmt.c_str());
      return;
    }

    case 0x44:
      fprintf(log, "FlushCache()\n");
      return;

    case 0x49:
      fprintf(log, "GPU_cw(0x%08x)\n", args[0]);
      return;

    case 0x70:
      fprintf(log, "_bu_init()\n");
      return;

    case 0x72:
      fprintf(log, "CdRemove()\n");
      return;

    case 0x78: {
      std::string timecode = decode_timecode(args[0]);
      fprintf(log, "CdAsyncSeekL(\"%s\")\n", timecode.c_str());
      return;
    }

    case 0x7c:
      fprintf(log, "CdAsyncGetStatus(0x%08x)\n", args[0]);
      return;

    case 0x7e:
      fprintf(log, "CdAsyncReadSector(%d, 0x%08x, 0x%x)\n", args[0], args[1], args[2]);
      return;

    case 0x95:
      fprintf(log, "CdInitSubFunc()\n");
      return;

    case 0x96:
      fprintf(log, "AddCDROMDevice()\n");
      return;

    case 0x97:
      fprintf(log, "AddMemCardDevice()\n");
      return;

    case 0x99:
      fprintf(log, "AddDummyTtyDevice()\n");
      return;

    case 0xa1:
      fprintf(log, "SystemErrorBootOrDiskFailure('%c', %d)\n", args[0], args[1]);
      return;

    case 0xa2:
      fprintf(log, "EnqueueCdIntr()\n");
      return;

    case 0xa3:
      fprintf(log, "DequeueCdIntr()\n");
      return;

    case 0xa9:
      fprintf(log, "bu_callback_err_busy()\n");
      return;
  }

  printf("bios::a(0x%02x)\n", function);
}


void decoder_t::decode_b(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(log, "[%08x] ", pc);

  switch (function) {
    case 0x00:
      fprintf(log, "alloc_kernel_memory(0x%08x)\n", args[0]);
      return;

    case 0x07:
      fprintf(log, "DeliverEvent(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x08:
      fprintf(log, "OpenEvent(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
        args[0],
        args[1],
        args[2],
        args[3]);
      return;

    case 0x09:
      fprintf(log, "CloseEvent(0x%08x)\n", args[0]);
      return;

    case 0x0b:
      fprintf(log, "TestEvent(0x%08x)\n", args[0]);
      return;

    case 0x0c:
      fprintf(log, "EnableEvent(0x%08x)\n", args[0]);
      return;

    case 0x13:
      fprintf(log, "StartPad()\n");
      return;

    case 0x17:
      fprintf(log, "ReturnFromException()\n");
      return;

    case 0x18:
      fprintf(log, "SetDefaultExitFromException()\n");
      return;

    case 0x19:
      fprintf(log, "SetCustomExitFromException(0x%08x)\n", args[0]);
      return;

    case 0x20:
      fprintf(log, "UnDeliverEvent(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x32: {
      std::string file_name = decode_string(args[0]);
      fprintf(log, "FileOpen(\"%s\", %d)\n", file_name.c_str(), args[1]);
      return;
    }

    case 0x34:
      fprintf(log, "FileRead(%d, 0x%08x, %d)\n", args[0], args[1], args[2]);
      return;

    case 0x35:
      if (args[0] == 1) {
        std::string str = decode_string(args[1], args[2]);
        fprintf(log, "printf(\"%s\")\n", str.c_str());
      }
      else {
        fprintf(log, "FileWrite(%d, 0x%08x, %d)\n", args[0], args[1], args[2]);
      }
      return;

    case 0x36:
      fprintf(log, "FileClose(%d)\n", args[0]);
      return;

    case 0x3d:
      fprintf(log, "putc(\'%c\')\n", args[0]);
      return;

    case 0x3f: {
      std::string text = decode_string(args[0]);
      fprintf(log, "puts(\"%s\")\n", text.c_str());
      return;
    }

    case 0x47:
      fprintf(log, "AddDevice(0x%08x)\n", args[0]);
      return;

    case 0x4a:
      fprintf(log, "InitCard(%d)\n", args[0]);
      return;

    case 0x4b:
      fprintf(log, "StartCard()\n");
      return;

    case 0x4f:
      fprintf(log, "read_card_sector(%d, %d, 0%08x)\n", args[0], args[1], args[2]);
      return;

    case 0x50:
      fprintf(log, "allow_new_card()\n");
      return;

    case 0x56:
      fprintf(log, "GetC0Table()\n");
      return;

    case 0x57:
      fprintf(log, "GetB0Table()\n");
      return;

    case 0x58:
      fprintf(log, "get_bu_callback_port()\n");
      return;

    case 0x5b:
      fprintf(log, "ChangeClearPad(%d)\n", args[0]);
      return;
  }

  printf("bios::b(0x%02x)\n", function);
}


void decoder_t::decode_c(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(log, "[%08x] ", pc);

  switch (function) {
    case 0x00:
      fprintf(log, "EnqueueTimerAndVblankIrqs(%d)\n", args[0]);
      return;

    case 0x01:
      fprintf(log, "EnqueueSyscallHandler(%d)\n", args[0]);
      return;

    case 0x02:
      fprintf(log, "SysEnqIntRP(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x03:
      fprintf(log, "SysDeqIntRP(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x07:
      fprintf(log, "InstallExceptionHandlers()\n");
      return;

    case 0x08:
      fprintf(log, "SysInitMemory(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x0a:
      fprintf(log, "ChangeClearRCnt(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x0c:
      fprintf(log, "InitDefInt(%d)\n", args[0]);
      return;

    case 0x12:
      fprintf(log, "InstallDevices(0x%08x)\n", args[0]);
      return;

    case 0x1c:
      fprintf(log, "AdjustA0Table()\n");
      return;
  }

  printf("bios::c(0x%02x)\n", function);
}


std::string decoder_t::decode_string(uint32_t arg) {
  std::string result;
  char curr;

  do {
    curr = (char) memory->read_byte(arg & 0x1fffffff);
    arg++;

    if (curr == '\0') {
      result += "\\0";
    }
    else if (curr == '\\') {
      result += "\\\\";
    }
    else if (curr == '\n') {
      result += "\\n";
    }
    else if (curr == '\r') {
      result += "\\r";
    }
    else if (curr == '\t') {
      result += "\\t";
    }
    else {
      result += curr;
    }
  }
  while (curr);

  return result;
}


std::string decoder_t::decode_string(uint32_t arg, uint32_t size) {
  std::string result;
  char curr;

  for (uint32_t i = 0; i < size; i++) {
    curr = (char) memory->read_byte(arg & 0x1fffffff);
    arg++;

    if (curr == '\0') {
      result += "\\0";
    }
    else if (curr == '\\') {
      result += "\\\\";
    }
    else if (curr == '\n') {
      result += "\\n";
    }
    else if (curr == '\r') {
      result += "\\r";
    }
    else if (curr == '\t') {
      result += "\\t";
    }
    else {
      result += curr;
    }
  }

  return result;
}


std::string decoder_t::decode_timecode(uint32_t arg) {
  uint32_t m = memory->read_byte((arg + 0) & 0x1fffffff);
  uint32_t s = memory->read_byte((arg + 1) & 0x1fffffff);
  uint32_t f = memory->read_byte((arg + 2) & 0x1fffffff);

  char result[8];
  sprintf(result, "%02d:%02d:%02d",
    utility::bcd_to_dec(m),
    utility::bcd_to_dec(s),
    utility::bcd_to_dec(f)
  );

  return std::string(result);
}
