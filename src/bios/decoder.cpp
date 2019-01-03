// Copyright 2018 psxact

#include "bios/decoder.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include "utility.hpp"

using psx::bios::decoder_t;

decoder_t::decoder_t(memory_access_t *memory)
  : memory(memory) {
  log = fopen("bios-call.log", "w");
}

static std::string char_to_string(char c) {
  switch (c) {
  case '\0':
    return R"(\0)";

  case '\\':
    return R"(\\)";

  case '\n':
    return R"(\n)";

  case '\r':
    return R"(\r)";

  case '\t':
    return R"(\t)";

  default:
    return std::string(1, c);
  }
}

#define log_call(fmt, ...) \
  fprintf(log, "[%08x] " fmt "\n", pc, ## __VA_ARGS__);

void decoder_t::decode_a(uint32_t pc, uint32_t function, uint32_t *args) {
  switch (function) {
    case 0x00: {
      std::string file_name = decode_string(args[0]);
      log_call("FileOpen(%s, %d)", file_name.c_str(), args[1]);
      break;
    }

    case 0x13:
      log_call("SaveState(0x%08x)", args[0]);
      break;

    case 0x17: {
      std::string a = decode_string(args[0]);
      std::string b = decode_string(args[1]);
      log_call("strcmp(%s, %s)",
        a.c_str(),
        b.c_str());
      break;
    }

    case 0x18: {
      std::string a = decode_string(args[0]);
      std::string b = decode_string(args[1]);
      log_call("strncmp(%s, %s, %d)",
        a.c_str(),
        b.c_str(),
        args[2]);
      break;
    }

    case 0x1b: {
      std::string src = decode_string(args[0]);
      log_call("strlen(%s)", src.c_str());
      break;
    }

    case 0x25:
      log_call("toupper('%s')", char_to_string(args[0]).c_str());
      break;

    case 0x28:
      log_call("bzero(0x%08x, %d)", args[0], args[1]);
      break;

    case 0x2a:
      log_call("memcpy(0x%08x, 0x%08x, %d)", args[0], args[1], args[2]);
      break;

    case 0x2b:
      log_call("memset(0x%08x, %d, %d)", args[0], args[1], args[2]);
      break;

    case 0x33:
      log_call("malloc(%d)", args[0]);
      break;

    case 0x34:
      log_call("free(0x%08x)", args[0]);
      break;

    case 0x39:
      log_call("InitHeap(0x%08x, %d)", args[0], args[1]);
      break;

    case 0x3c:
      // log_call("putc(\'%s\')", char_to_string(args[0]).c_str());
      break;

    case 0x3e: {
      // std::string text = decode_string(args[0]);
      // log_call("puts(%s)", text.c_str());
      break;
    }

    case 0x3f: {
      // std::string fmt = decode_string(args[0]);
      // log_call("printf(%s)", fmt.c_str());
      break;
    }

    case 0x44:
      log_call("FlushCache()");
      break;

    case 0x49:
      log_call("GPU_cw(0x%08x)", args[0]);
      break;

    case 0x70:
      log_call("_bu_init()");
      break;

    case 0x72:
      log_call("CdRemove()");
      break;

    case 0x78: {
      std::string timecode = decode_timecode(args[0]);
      log_call("CdAsyncSeekL(%s)", timecode.c_str());
      break;
    }

    case 0x7c:
      log_call("CdAsyncGetStatus(0x%08x)", args[0]);
      break;

    case 0x7e:
      log_call("CdAsyncReadSector(%d, 0x%08x, 0x%x)", args[0], args[1], args[2]);
      break;

    case 0x95:
      log_call("CdInitSubFunc()");
      break;

    case 0x96:
      log_call("AddCDROMDevice()");
      break;

    case 0x97:
      log_call("AddMemCardDevice()");
      break;

    case 0x99:
      log_call("AddDummyTtyDevice()");
      break;

    case 0xa1:
      log_call("SystemErrorBootOrDiskFailure('%s', %d)",
        char_to_string(args[0]).c_str(),
        args[1]);
      break;

    case 0xa2:
      log_call("EnqueueCdIntr()");
      break;

    case 0xa3:
      log_call("DequeueCdIntr()");
      break;

    case 0xa9:
      log_call("bu_callback_err_busy()");
      break;

    default:
      log_call("bios::a(0x%02x)", function);
      break;
  }
}

void decoder_t::decode_b(uint32_t pc, uint32_t function, uint32_t *args) {
  switch (function) {
    case 0x00:
      log_call("alloc_kernel_memory(0x%08x)", args[0]);
      break;

    case 0x07:
      log_call("DeliverEvent(0x%08x, 0x%08x)", args[0], args[1]);
      break;

    case 0x08:
      log_call("OpenEvent(0x%08x, 0x%08x, 0x%08x, 0x%08x)",
        args[0],
        args[1],
        args[2],
        args[3]);
      break;

    case 0x09:
      log_call("CloseEvent(0x%08x)", args[0]);
      break;

    case 0x0b:
      // log_call("TestEvent(0x%08x)", args[0]);
      break;

    case 0x0c:
      log_call("EnableEvent(0x%08x)", args[0]);
      break;

    case 0x0e:
      log_call("OpenThread(pc=0x%08x, sp=0x%08x, gp=0x%08x)",
        args[0],
        args[1],
        args[2]);
      break;

    case 0x10:
      log_call("ChangeThread(0x%08x)", args[0]);
      break;

    case 0x13:
      log_call("StartPad()");
      break;

    case 0x15:
      log_call("OutdatedPadInitAndStart(%d, %d, %d)",
        args[0],
        args[1],
        args[2]);
      break;

    case 0x16:
      log_call("OutdatedPadGetButtons()");
      break;

    case 0x17:
      log_call("ReturnFromException()");
      break;

    case 0x18:
      log_call("SetDefaultExitFromException()");
      break;

    case 0x19:
      log_call("SetCustomExitFromException(0x%08x)", args[0]);
      break;

    case 0x20:
      log_call("UnDeliverEvent(0x%08x, 0x%08x)", args[0], args[1]);
      break;

    case 0x32: {
      std::string file_name = decode_string(args[0]);
      log_call("FileOpen(%s, %d)", file_name.c_str(), args[1]);
      break;
    }

    case 0x34:
      log_call("FileRead(%d, 0x%08x, %d)", args[0], args[1], args[2]);
      break;

    case 0x35:
      log_call("FileWrite(%d, 0x%08x, %d)", args[0], args[1], args[2]);
      break;

    case 0x36:
      log_call("FileClose(%d)", args[0]);
      break;

    case 0x3d:
      // log_call("putc('%s')", char_to_string(args[0]).c_str());
      break;

    case 0x3f: {
      // std::string text = decode_string(args[0]);
      // log_call("puts(%s)", text.c_str());
      break;
    }

    case 0x47:
      log_call("AddDevice(0x%08x)", args[0]);
      break;

    case 0x4a:
      log_call("InitCard(%d)", args[0]);
      break;

    case 0x4b:
      log_call("StartCard()");
      break;

    case 0x4f:
      log_call("read_card_sector(%d, %d, 0%08x)", args[0], args[1], args[2]);
      break;

    case 0x50:
      log_call("allow_new_card()");
      break;

    case 0x56:
      log_call("GetC0Table()");
      break;

    case 0x57:
      log_call("GetB0Table()");
      break;

    case 0x58:
      log_call("get_bu_callback_port()");
      break;

    case 0x5b:
      log_call("ChangeClearPad(%d)", args[0]);
      break;

    default:
      log_call("bios::b(0x%02x)", function);
      break;
  }
}

void decoder_t::decode_c(uint32_t pc, uint32_t function, uint32_t *args) {
  switch (function) {
    case 0x00:
      log_call("EnqueueTimerAndVblankIrqs(%d)", args[0]);
      break;

    case 0x01:
      log_call("EnqueueSyscallHandler(%d)", args[0]);
      break;

    case 0x02:
      log_call("SysEnqIntRP(%d, 0x%08x)", args[0], args[1]);
      break;

    case 0x03:
      log_call("SysDeqIntRP(%d, 0x%08x)", args[0], args[1]);
      break;

    case 0x07:
      log_call("InstallExceptionHandlers()");
      break;

    case 0x08:
      log_call("SysInitMemory(0x%08x, 0x%08x)", args[0], args[1]);
      break;

    case 0x0a:
      log_call("ChangeClearRCnt(%d, 0x%08x)", args[0], args[1]);
      break;

    case 0x0c:
      log_call("InitDefInt(%d)", args[0]);
      break;

    case 0x12:
      log_call("InstallDevices(0x%08x)", args[0]);
      break;

    case 0x1c:
      log_call("AdjustA0Table()");
      break;

    default:
      log_call("bios::c(0x%02x)", function);
      break;
  }
}

uint8_t decoder_t::read_byte(uint32_t address) {
  return memory->read_byte(address & 0x1fffffff);
}

std::string decoder_t::decode_string(uint32_t arg) {
  std::stringstream s;

  s << '"';

  while (1) {
    char curr = static_cast<char>(read_byte(arg));
    arg++;

    if (curr == 0) {
      break;
    }

    s << char_to_string(curr);
  }

  s << '"';

  return s.str();
}

std::string decoder_t::decode_timecode(uint32_t arg) {
  uint8_t m = read_byte(arg);
  uint8_t s = read_byte(arg + 1);
  uint8_t f = read_byte(arg + 2);

  std::stringstream o;

  o << std::setbase(16)
    << std::setfill('0')
    << std::setw(2)
    << '"'
    << m << ':' << s << ':' << f
    << '"';

  return o.str();
}
