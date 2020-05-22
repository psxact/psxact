#include "cdrom/core.hpp"

#include "util/bcd.hpp"
#include "args.hpp"
#include "timing.hpp"

using namespace psx::cdrom;
using namespace psx::util;

constexpr int int2_get_id_timing = 19'000;
constexpr int int2_init_timing = 900'000;
constexpr int int2_pause_timing = 1'800; // TODO: This can take much longer?
constexpr int int2_read_toc_timing = 16'000'000;
constexpr int int2_seek_l_timing = 1'800;

core::core(wire irq, xa_adpcm_decoder &xa_adpcm, const char *game_file_name)
    : addressable("cdc", args::log_cdrom)
    , irq(irq)
    , xa_adpcm(xa_adpcm) {

  if (game_file_name) {
    disc_file.emplace(fopen(game_file_name, "rb"));
  } else {
    disc_file.reset();
  }
}

void core::tick(int amount) {
  int1_timer = std::max(0, int1_timer - amount);
  int2_timer = std::max(0, int2_timer - amount);

  timer -= amount;

  while (timer < 0) {
    timer += 3000;

    // Do nothing if there is an un-acked IRQ.
    if (irq_flag != 0) {
      continue;
    }

    // Process a pending INT1.
    if (int1 && int1_timer == 0) {
      (*this.*int1)();
      int1_timer = get_read_time();
      continue;
    }

    // Process a pending INT2.
    if (int2 && int2_timer == 0) {
      (*this.*int2)();
      int2 = nullptr;
      continue;
    }

    // Process a pending command.
    if (command.has_value()) {
      auto cmd = command.value();
      command.reset();
      response.clear();

      switch (cmd) {
        case 0x01: {
          log("Processing 'GetStat' command.");

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x02: {
          log("Processing 'SetLoc' command.");

          seek_timecode.minute = bcd::to_dec(get_parameter());
          seek_timecode.second = bcd::to_dec(get_parameter());
          seek_timecode.sector = bcd::to_dec(get_parameter());
          seek_pending = true;

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x03: {
          log("Processing 'Play' command.");

          parameter.clear();

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x1b:
        case 0x06: {
          log("Processing 'ReadN' command.");

          drive_state = cdrom_drive_state::reading;

          put_response(get_drive_status());
          put_irq_flag(3);

          int1 = &core::int1_read_n;
          int1_timer = get_read_time();

          if (seek_pending) {
            seek_pending = false;
            read_timecode = seek_timecode;
            int1_timer += get_seek_time();
          }
          break;
        }

        case 0x09: {
          log("Processing 'Pause' command.");

          put_response(get_drive_status());
          put_irq_flag(3);

          int1 = nullptr;
          int1_timer = 0;

          int2 = &core::int2_pause;
          int2_timer = int2_pause_timing;

          log("Processing complete, delaying second response for %d cycles.", int2_timer);
          break;
        }

        case 0x0a: {
          log("Processing 'Init' command.");

          put_response(get_drive_status());
          put_irq_flag(3);

          int1 = nullptr;
          int1_timer = 0;

          int2 = &core::int2_init;
          int2_timer = int2_init_timing;

          log("Processing complete, delaying second response for %d cycles.", int2_timer);
          break;
        }

        case 0x0b: {
          log("Processing 'Mute' command.");

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x0c: {
          log("Processing 'Un-mute' command.");

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x0d: {
          log("Processing 'SetFilter' command.");

          filter.put_file(get_parameter());
          filter.put_channel(get_parameter());

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x0e: {
          log("Processing 'SetMode' command.");

          mode = cdrom_mode { get_parameter() };

          put_response(get_drive_status());
          put_irq_flag(3);
          break;
        }

        case 0x10: {
          log("Processing 'GetLocL' command.");

          put_response(sector.get_minute());
          put_response(sector.get_second());
          put_response(sector.get_second());
          put_response(sector.get_mode());
          put_response(sector.get_xa_file());
          put_response(sector.get_xa_channel());
          put_response(sector.get_xa_sub_mode());
          put_response(sector.get_xa_coding_info());
          put_irq_flag(3);
          break;
        }

        case 0x11: {
          log("Processing 'GetLocP' command.");

          put_response(0x01); // Track
          put_response(0x01); // Index
          put_response(sector.get_minute()); // Track-relative
          put_response(sector.get_second());
          put_response(sector.get_sector());
          put_response(sector.get_minute()); // Absolute
          put_response(sector.get_second());
          put_response(sector.get_sector());
          put_irq_flag(3);
          break;
        }

        case 0x13: {
          log("Processing 'GetTN' command.");

          put_response(get_drive_status());
          put_response(0x01);
          put_response(0x01);
          put_irq_flag(3);
          break;
        }

        case 0x14: {
          log("Processing 'GetTD' command.");

          auto track = get_parameter();

          log("Track=%d", track);

          put_response(get_drive_status());
          put_response(0x00);
          put_response(0x00);
          put_irq_flag(3);
          break;
        }

        case 0x15:
        case 0x16: {
          log("Processing 'SeekL' command.");

          drive_state = cdrom_drive_state::seeking;

          put_response(get_drive_status());
          put_irq_flag(3);

          // Cancel any on-going read/play operations.
          int1 = nullptr;
          int1_timer = 0;

          int2 = &core::int2_seek_l;
          int2_timer = get_seek_time() + int2_seek_l_timing;

          log("Processing complete, delaying second response for %d cycles.", int2_timer);
          break;
        }

        case 0x19: {
          log("Processing 'Test' command.");

          auto sub_cmd = get_parameter();
          switch (sub_cmd) {
            case 0x04:
              put_response(get_drive_status());
              put_irq_flag(3);
              break;

            case 0x05:
              parameter.clear();
              put_response(0);
              put_response(0);
              put_irq_flag(3);
              break;

            case 0x20:
              put_response(0x98);
              put_response(0x06);
              put_response(0x10);
              put_response(0xc3);
              put_irq_flag(3);
              break;

            default:
              log("Unhandled sub-command: %02x", sub_cmd);
              assert(0);
              break;
          }
          break;
        }

        case 0x1a: {
          log("Processing 'GetID' command.");

          drive_state = cdrom_drive_state::reading;

          put_response(get_drive_status());
          put_irq_flag(3);

          int1 = nullptr;
          int1_timer = 0;

          int2 = &core::int2_get_id;
          int2_timer = int2_get_id_timing;

          log("Processing complete, delaying second response for %d cycles.", int2_timer);
          break;
        }

        case 0x1e: {
          log("Processing 'ReadTOC' command.");

          drive_state = cdrom_drive_state::reading;

          put_response(get_drive_status());
          put_irq_flag(3);

          int1 = nullptr;
          int1_timer = 0;

          int2 = &core::int2_read_toc;
          int2_timer = int2_read_toc_timing;

          log("Processing complete, delaying second response for %d cycles.", int2_timer);
          break;
        }

        default:
          log("Unhandled command: %02x", cmd);
          assert(0);
          break;
      }

      assert(parameter.is_empty());
    }
  }
}

int core::get_read_time() const {
  if (mode.double_speed()) {
    return CPU_FREQ / 150;
  } else {
    return CPU_FREQ / 75;
  }
}

int core::get_seek_time() const {
  int minute_diff = std::abs(seek_timecode.minute - read_timecode.minute);
  int second_diff = std::abs(seek_timecode.second - read_timecode.second);
  int sector_diff = std::abs(seek_timecode.sector - read_timecode.sector);

  // For now, we'll assume 100 cycles per sector to seek.
  // I've just pulled this number out of thin air, because why not?

  return 100 * ((minute_diff * 60) + (second_diff * 75) + sector_diff);
}

uint8_t core::get_data() {
  assert(sector_read_active && "Sector read while not active.");

  if (sector_read_active) {
    assert(sector_read_cursor < sector_read_length);

    uint8_t result = sector.get(sector_read_cursor + sector_read_offset);
    sector_read_cursor++;

    if (sector_read_cursor == sector_read_length) {
      sector_read_active = false;
    }

    return result;
  }

  return sector.get((sector_read_cursor + sector_read_offset) & ~7);
}

uint8_t core::get_drive_status() {
  uint8_t response = 0;

  switch (drive_state) {
    case cdrom_drive_state::idle: break;
    case cdrom_drive_state::reading: response |= (1 << 5); break;
    case cdrom_drive_state::seeking: response |= (1 << 6); break;
    case cdrom_drive_state::playing: response |= (1 << 7); break;
  }

  // 0  Error         Invalid Command/parameters (followed by Error Byte)
  // 1  Spindle Motor (0=Motor off, or in spin-up phase, 1=Motor on)
  // 2  SeekError     (0=Okay, 1=Seek error)     (followed by Error Byte)
  // 3  IdError       (0=Okay, 1=GetID denied) (also set when Setmode.Bit4=1)
  // 4  ShellOpen     Once shell open (0=Closed, 1=Is/was Open)

  response |= (1 << 1);

  log("Get drive status: %02x", response);

  return response;
}

uint8_t core::get_irq_flag() const {
  return 0xe0 | irq_flag;
}

uint8_t core::get_irq_mask() const {
  return 0xe0 | irq_mask;
}

uint8_t core::get_parameter() {
  auto data = parameter.read();

  log("Get parameter: %02x", data);

  return data;
}

uint8_t core::get_response() {
  auto data = response.read();

  log("Get response: %02x", data);

  return data;
}

uint8_t core::get_status() {
  auto bit2 = 0; // 2   ADPBUSY XA-ADPCM fifo empty  (0=Empty) ;set when playing XA-ADPCM sound
  auto bit3 = parameter.is_empty();
  auto bit4 = !parameter.is_full();
  auto bit5 = !response.is_empty();
  auto bit6 = sector_read_cursor < sector_read_length;
  auto bit7 = 0; // 7   BUSYSTS Command/parameter transmission busy  (1=Busy)

  auto stat = index
    | (bit2 << 2)
    | (bit3 << 3)
    | (bit4 << 4)
    | (bit5 << 5)
    | (bit6 << 6)
    | (bit7 << 7);

  log("Get status: %02x", stat);

  return stat;
}

void core::ack_irq_flag(uint8_t val) {
  log("Ack IRQ flag: %02x", val);

  if (val & 0x40) {
    parameter.clear();
  }

  put_irq_flag(irq_flag & ~(val & 31));
}

void core::put_command(uint8_t val) {
  log("Put command: %02x", val);

  if (command.has_value()) {
    assert(0 && "Command written before previous command was started.");
  }

  command.emplace(val);
}

void core::put_host_control(uint8_t val) {
  log("Put host control: %02x", val);

  if (sector_read_active == 0 && (val & 0x80)) {
    sector_read_cursor = 0;
  }

  sector_read_active = !!(val & 0x80);
}

void core::put_irq_flag(uint8_t val) {
  log("Put IRQ flag: %02x", val);

  irq_flag = val & 31;

  if (irq_flag != 0 && (irq_flag & irq_mask) == irq_flag) {
    log("IRQ wire: 1");
    irq(wire_state::on);
  } else {
    log("IRQ wire: 0");
    irq(wire_state::off);
  }
}

void core::put_irq_mask(uint8_t val) {
  log("Put IRQ mask: %02x", val);

  irq_mask = val & 31;
}

void core::put_parameter(uint8_t val) {
  log("Put parameter: %02x", val);

  parameter.write(val);
}

void core::put_response(uint8_t val) {
  log("Put response: %02x", val);

  response.write(val);
}

bool core::try_deliver_sector_as_adpcm() {
  if (sector.get_mode() != 2) {
    return false;
  }

  if (!mode.send_xa_adpcm_to_spu()) {
    return false;
  }

  if (mode.filter_xa_adpcm() && !filter.match(sector)) {
    return false;
  }

  if ((sector.get_xa_sub_mode() & 0x44) != 0x44) {
    return false;
  }

  xa_adpcm.decode(sector);

  return true;
}

bool core::try_deliver_sector_as_data() {
  auto type = sector.get_type();
  if (type == cdrom_sector_type::mode2_form1 || type == cdrom_sector_type::mode2_form2) {
    if (mode.filter_xa_adpcm() && (sector.get_xa_sub_mode() & 0x44) == 0x44) {
      return false;
    }
  }

  if (mode.read_whole_sector()) {
    log("Full sector");
    sector_read_offset = 12;
    sector_read_length = CDROM_SECTOR_SIZE - 12;
  } else {
    switch (type) {
      case cdrom_sector_type::unknown:
      case cdrom_sector_type::mode0:
        assert(0 && "Mode 0");
        break;

      case cdrom_sector_type::mode1:
        log("Mode 1 sector");
        sector_read_offset = 16;
        sector_read_length = 2048;
        break;

      case cdrom_sector_type::mode2_form1:
        log("Mode 2 form 1 sector");
        sector_read_offset = 24;
        sector_read_length = 2048;
        break;

      case cdrom_sector_type::mode2_form2:
        assert(0 && "Mode 2 Form 2");
        break;
    }
  }

  return true;
}

void core::int1_read_n() {
  assert(disc_file.has_value() && "Reading non-existant disc.");

  sector.fill_from(*disc_file, read_timecode);

  read_timecode.sector++;
  if (read_timecode.sector == 75) {
    read_timecode.sector = 0;
    read_timecode.second++;
    if (read_timecode.second == 60) {
      read_timecode.second = 0;
      read_timecode.minute++;
      // TODO: Overflow minute?
    }
  }

  if (try_deliver_sector_as_adpcm()) {
    log("Delivered sector (%02d:%02d:%02d) as CD-XA ADPCM.",
      read_timecode.minute,
      read_timecode.second,
      read_timecode.sector);

    // TODO: Send INT1?
    return;
  }

  if (try_deliver_sector_as_data()) {
    log("Delivered sector (%02d:%02d:%02d) as data.",
      read_timecode.minute,
      read_timecode.second,
      read_timecode.sector);

    put_response(get_drive_status());
    put_irq_flag(1);
    return;
  }

  // Sectors silently drop out here if they can't be delivered.
}

void core::int2_get_id() {
  log("Delivering 'GetID' second response.");

  drive_state = cdrom_drive_state::idle;

  if (disc_file.has_value()) {
    // INT2(02h,00h, 20h,00h, 53h,43h,45h,4xh)
    put_response(0x02);
    put_response(0x00);
    put_response(0x20);
    put_response(0x00);
    put_response('S');
    put_response('C');
    put_response('E');
    put_response('A');
    put_irq_flag(2);
  } else {
    // INT5(08h,40h, 00h,00h, 00h,00h,00h,00h)
    put_response(0x08);
    put_response(0x40);
    put_response(0x00);
    put_response(0x00);
    put_response(0x00);
    put_response(0x00);
    put_response(0x00);
    put_response(0x00);
    put_irq_flag(5);
  }
}

void core::int2_init() {
  log("Delivering 'Init' second response.");

  drive_state = cdrom_drive_state::idle;
  mode = cdrom_mode { 0 };

  put_response(get_drive_status());
  put_irq_flag(2);
}

void core::int2_pause() {
  log("Delivering 'Pause' second response.");

  drive_state = cdrom_drive_state::idle;

  put_response(get_drive_status());
  put_irq_flag(2);
}

void core::int2_read_toc() {
  log("Delivering 'ReadTOC' second response.");

  drive_state = cdrom_drive_state::idle;

  put_response(get_drive_status());
  put_irq_flag(2);
}

void core::int2_seek_l() {
  log("Delivering 'SeekL' second response.");

  drive_state = cdrom_drive_state::idle;

  read_timecode = seek_timecode;
  seek_pending = false;

  put_response(get_drive_status());
  put_irq_flag(2);
}

int core::dma_speed() {
  return 40;
}

bool core::dma_read_ready() {
  return true;
}

bool core::dma_write_ready() {
  return true;
}

uint32_t core::dma_read() {
  uint8_t b0 = get_data();
  uint8_t b1 = get_data();
  uint8_t b2 = get_data();
  uint8_t b3 = get_data();

  if (!sector_read_active) {
    log("DMA complete.");
  }

  return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

void core::dma_write(uint32_t) {
}

static void add_cpu_time(psx::address_width width) {
  switch (width) {
    case psx::address_width::byte: return psx::timing::add_cpu_time(9);
    case psx::address_width::half: return psx::timing::add_cpu_time(15);
    case psx::address_width::word: return psx::timing::add_cpu_time(27);
  }
}

uint32_t core::io_read(address_width width, uint32_t address) {
  add_cpu_time(width);

  if (width == address_width::byte) {
    if (address == 0x1f801800) {
      return get_status();
    }

    if (address == 0x1f801801) {
      return get_response();
    }

    if (address == 0x1f801802) {
      return get_data();
    }

    if (address == 0x1f801803 && (index & 1) == 1) {
      return get_irq_flag();
    }

    if (address == 0x1f801803 && (index & 1) == 0) {
      return get_irq_mask();
    }
  }

  log("index=%d", index);

  return addressable::io_read(width, address);
}

void core::io_write(address_width width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  if (width == address_width::byte) {
    if (address == 0x1f801800) {
      index = data & 3;
      return;
    } else {
      // I'm going to make my life easier by creating a single identifier for
      // each port, created from the index and I/O address.

      switch ((index * 4) | (address & 3)) {
        case 0x1: return put_command(data);
        case 0x2: return put_parameter(data);
        case 0x3: return put_host_control(data);

        //  5 - Sound Map Data Out
        case 0x6: return put_irq_mask(data);
        case 0x7: return ack_irq_flag(data);

        //  9 - Sound Map Coding Info
        case 0xa: return; // 10 - Audio Volume for Left-CD-Out to Left-SPU-Input
        case 0xb: return; // 11 - Audio Volume for Left-CD-Out to Right-SPU-Input

        case 0xd: return; // 13 - Audio Volume for Right-CD-Out to Right-SPU-Input
        case 0xe: return; // 14 - Audio Volume for Right-CD-Out to Left-SPU-Input
        case 0xf: return; // 15 - Audio Volume Apply Changes
      }
    }
  }

  log("index=%d", index);

  return addressable::io_write(width, address, data);
}
