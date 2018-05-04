#include "cpu/bios-call-decoder.hpp"


bios_call_decoder_t::bios_call_decoder_t(memory_access_t *memory)
  : memory(memory) {

  call_log = fopen("bios_call.log", "w");
  stdout_log = fopen("bios_stdout.log", "w");
}


void bios_call_decoder_t::decode_a(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(call_log, "[%08x] ", pc);

  switch (function) {
    case 0x00: {
      std::string file_name = decode_string(args[0]);
      fprintf(call_log, "FileOpen(\"%s\", %d)\n", file_name.c_str(), args[1]);
      return;
    }

    case 0x13:
      fprintf(call_log, "SaveState(0x%08x)\n", args[0]);
      return;

    case 0x17: {
      std::string a = decode_string(args[0]);
      std::string b = decode_string(args[1]);
      fprintf(call_log, "strcmp(\"%s\", \"%s\")\n",
        a.c_str(),
        b.c_str()
      );
      return;
    }

    case 0x1b: {
      std::string src = decode_string(args[0]);
      fprintf(call_log, "strlen(\"%s\")\n", src.c_str());
      return;
    }

    case 0x25:
      fprintf(call_log, "toupper('%c')\n", args[0]);
      return;

    case 0x28:
      fprintf(call_log, "bzero(0x%08x, %d)\n", args[0], args[1]);
      return;

    case 0x2a:
      fprintf(call_log, "memcpy(0x%08x, 0x%08x, %d)\n", args[0], args[1], args[2]);
      return;

    case 0x33:
      fprintf(call_log, "malloc(%d)\n", args[0]);
      return;

    case 0x34:
      fprintf(call_log, "free(0x%08x)\n", args[0]);
      return;

    case 0x39:
      fprintf(call_log, "InitHeap(0x%08x, %d)\n", args[0], args[1]);
      return;

    case 0x3c: // putc
      fputc(args[0], stdout_log);
      return;

    case 0x3e: {
      std::string text = decode_string(args[0]);
      fputs(text.c_str(), stdout_log);
      return;
    }

    case 0x3f: {
      std::string fmt = decode_string(args[0]);
      fprintf(call_log, "printf(\"%s\")\n", fmt.c_str());
      return;
    }

    case 0x44:
      fprintf(call_log, "FlushCache()\n");
      return;

    case 0x49:
      fprintf(call_log, "GPU_cw(0x%08x)\n", args[0]);
      return;

    case 0x70:
      fprintf(call_log, "_bu_init()\n");
      return;

    case 0x72:
      fprintf(call_log, "CdRemove()\n");
      return;

    case 0x78: {
      std::string timecode = decode_timecode(args[0]);
      fprintf(call_log, "CdAsyncSeekL(\"%s\")\n", timecode.c_str());
      return;
    }

    case 0x7c:
      fprintf(call_log, "CdAsyncGetStatus(0x%08x)\n", args[0]);
      return;

    case 0x7e:
      fprintf(call_log, "CdAsyncReadSector(%d, 0x%08x, %x)\n", args[0], args[1], args[2]);
      return;

    case 0x95:
      fprintf(call_log, "CdInitSubFunc()\n");
      return;

    case 0x96:
      fprintf(call_log, "AddCDROMDevice()\n");
      return;

    case 0x97:
      fprintf(call_log, "AddMemCardDevice()\n");
      return;

    case 0x99:
      fprintf(call_log, "AddDummyTtyDevice()\n");
      return;

    case 0xa2:
      fprintf(call_log, "EnqueueCdIntr()\n");
      return;

    case 0xa3:
      fprintf(call_log, "DequeueCdIntr()\n");
      return;

    case 0xa9:
      fprintf(call_log, "bu_callback_err_busy()\n");
      return;
  }

  printf("bios::a(0x%02x)\n", function);
}


void bios_call_decoder_t::decode_b(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(call_log, "[%08x] ", pc);

  switch (function) {
    case 0x00:
      fprintf(call_log, "alloc_kernel_memory(0x%08x)\n", args[0]);
      return;

    case 0x07:
      fprintf(call_log, "DeliverEvent(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x08:
      fprintf(call_log, "OpenEvent(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
        args[0],
        args[1],
        args[2],
        args[3]);
      return;

    case 0x09:
      fprintf(call_log, "CloseEvent(0x%08x)\n", args[0]);
      return;

    case 0x0b:
      fprintf(call_log, "TestEvent(0x%08x)\n", args[0]);
      return;

    case 0x0c:
      fprintf(call_log, "EnableEvent(0x%08x)\n", args[0]);
      return;

    case 0x17:
      fprintf(call_log, "ReturnFromException()\n");
      return;

    case 0x18:
      fprintf(call_log, "SetDefaultExitFromException()\n");
      return;

    case 0x19:
      fprintf(call_log, "SetCustomExitFromException(0x%08x)\n", args[0]);
      return;

    case 0x20:
      fprintf(call_log, "UnDeliverEvent(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x32: {
      std::string file_name = decode_string(args[0]);
      fprintf(call_log, "FileOpen(\"%s\", %d)\n", file_name.c_str(), args[1]);
      return;
    }

    case 0x34:
      fprintf(call_log, "FileRead(%d, 0x%08x, %d)\n", args[0], args[1], args[2]);
      return;

    case 0x35:
      fprintf(call_log, "FileWrite(%d, 0x%08x, %d)\n", args[0], args[1], args[2]);
      return;

    case 0x36:
      fprintf(call_log, "FileClose(%d)\n", args[0]);
      return;

    case 0x3d:
      fputc(args[0], stdout_log);
      return;

    case 0x3f: {
      std::string text = decode_string(args[0]);
      fputs(text.c_str(), stdout_log);
      return;
    }

    case 0x47:
      fprintf(call_log, "AddDevice(0x%08x)\n", args[0]);
      return;

    case 0x4a:
      fprintf(call_log, "InitCard(%d)\n", args[0]);
      return;

    case 0x4b:
      fprintf(call_log, "StartCard()\n");
      return;

    case 0x4f:
      fprintf(call_log, "read_card_sector(%d, %d, 0%08x)\n", args[0], args[1], args[2]);
      return;

    case 0x50:
      fprintf(call_log, "allow_new_card()\n");
      return;

    case 0x56:
      fprintf(call_log, "GetC0Table()\n");
      return;

    case 0x58:
      fprintf(call_log, "get_bu_callback_port()\n");
      return;

    case 0x5b:
      fprintf(call_log, "ChangeClearPad(%d)\n", args[0]);
      return;
  }

  printf("bios::b(0x%02x)\n", function);
}


void bios_call_decoder_t::decode_c(uint32_t pc, uint32_t function, uint32_t *args) {
  fprintf(call_log, "[%08x] ", pc);

  switch (function) {
    case 0x00:
      fprintf(call_log, "EnqueueTimerAndVblankIrqs(%d)\n", args[0]);
      return;

    case 0x01:
      fprintf(call_log, "EnqueueSyscallHandler(%d)\n", args[0]);
      return;

    case 0x02:
      fprintf(call_log, "SysEnqIntRP(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x03:
      fprintf(call_log, "SysDeqIntRP(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x07:
      fprintf(call_log, "InstallExceptionHandlers()\n");
      return;

    case 0x08:
      fprintf(call_log, "SysInitMemory(0x%08x, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x0a:
      fprintf(call_log, "ChangeClearRCnt(%d, 0x%08x)\n", args[0], args[1]);
      return;

    case 0x0c:
      fprintf(call_log, "InitDefInt(%d)\n", args[0]);
      return;

    case 0x12:
      fprintf(call_log, "InstallDevices(0x%08x)\n", args[0]);
      return;

    case 0x1c:
      fprintf(call_log, "AdjustA0Table()\n");
      return;
  }

  printf("bios::c(0x%02x)\n", function);
}


std::string bios_call_decoder_t::decode_string(uint32_t arg) {
  std::string result;
  char curr;

  do {
    curr = (char) memory->read_byte(arg & 0x1fffffff);
    arg++;

    if (curr == '\n') {
      result += "\\n";
    }
    else {
      result += curr;
    };
  }
  while (curr);

  return result;
}


std::string bios_call_decoder_t::decode_timecode(uint32_t arg) {
  uint32_t m = memory->read_byte((arg + 0) & 0x1fffffff);
  uint32_t s = memory->read_byte((arg + 1) & 0x1fffffff);
  uint32_t f = memory->read_byte((arg + 2) & 0x1fffffff);

  char result[8];
  sprintf(result, "%02d:%02d:%02d", m, s, f);

  return std::string(result);
}
