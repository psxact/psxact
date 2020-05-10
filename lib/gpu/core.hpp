#ifndef GPU_CORE_HPP_
#define GPU_CORE_HPP_

#include "gpu/types.hpp"
#include "util/fifo.hpp"
#include "util/wire.hpp"
#include "addressable.hpp"
#include "dma-comms.hpp"
#include "memory.hpp"

namespace psx::gpu {

enum class gpu_display_depth_t {
  bpp15,
  bpp24
};

enum class gpu_h_resolution_t {
  h256 = 256,
  h320 = 320,
  h368 = 368,
  h512 = 512,
  h640 = 640
};

enum class gpu_v_resolution_t {
  v240 = 240,
  v480 = 480
};

enum class gpu_field_t {
  even,
  odd
};

constexpr int GPU_GP0 = 0x1f801810;
constexpr int GPU_GP1 = 0x1f801814;
constexpr int GPU_READ = 0x1f801810;
constexpr int GPU_STAT = 0x1f801814;

class core_t final
    : public addressable_t
    , public dma_comms_t {

  /// Used to send IRQ(1) signals to the CPU.
  util::wire_t irq;

  /// Used to send HBlank signals to the timers.
  util::wire_t hbl;

  /// Used to send VBlank signals to the timers, and IRQ(0) signals to the CPU.
  util::wire_t vbl;

  int prescaler = {};
  int counter = {};

  gpu_h_resolution_t h_resolution = gpu_h_resolution_t::h256;
  gpu_v_resolution_t v_resolution = gpu_v_resolution_t::v240;
  gpu_display_depth_t display_depth;
  gpu_field_t field = gpu_field_t::even;
  uint32_t video_buffer[480][640];

  uint32_t status = 0x14802000;

 public:
  memory_t< mib(1) > *vram;

  uint32_t data_latch = {};
  uint32_t texture_window_mask_x = {};
  uint32_t texture_window_mask_y = {};
  uint32_t texture_window_offset_x = {};
  uint32_t texture_window_offset_y = {};
  int32_t drawing_area_x1 = {};
  int32_t drawing_area_y1 = {};
  int32_t drawing_area_x2 = {};
  int32_t drawing_area_y2 = {};
  int32_t x_offset = {};
  int32_t y_offset = {};
  uint32_t display_area_x = {};
  uint32_t display_area_y = {};
  uint32_t display_area_x1 = {};
  uint32_t display_area_y1 = {};
  uint32_t display_area_x2 = {};
  uint32_t display_area_y2 = {};
  bool textured_rectangle_x_flip = {};
  bool textured_rectangle_y_flip = {};

  psx::util::fifo_t< uint32_t, 4 > fifo = {};

  struct {
    struct {
      int32_t x = {};
      int32_t y = {};
      int32_t w = {};
      int32_t h = {};
    } reg = {};

    struct {
      bool active = {};
      int32_t x = {};
      int32_t y = {};
    } run = {};
  } cpu_to_gpu_transfer = {};

  struct {
    struct {
      int32_t x = {};
      int32_t y = {};
      int32_t w = {};
      int32_t h = {};
    } reg = {};

    struct {
      bool active = {};
      int32_t x = {};
      int32_t y = {};
    } run = {};
  } gpu_to_cpu_transfer = {};

  core_t(util::wire_t irq, util::wire_t hbl, util::wire_t vbl);
  ~core_t();

  // Interface for external video rendering

  uint32_t *get_video_buffer() const;
  int32_t get_width() const;
  int32_t get_height() const;

  /// Called with the number of CPU cycles that have elapsed.
  /// Returns true if a field has elapsed, false otherwise.
  bool run(int amount);

  /// Called with the number of GPU cycles that have elapsed.
  /// Returns true if a field has elapsed, false otherwise.
  bool tick(int amount);

  /// Called at the end of a field to draw the field into the video buffer.
  void render_field_240p();

  /// Called at the end of a field to draw the field into the video buffer.
  void render_field_480i();

  uint32_t get_status() const;
  gpu_h_resolution_t get_h_resolution() const;
  gpu_v_resolution_t get_v_resolution() const;
  gpu_display_depth_t get_display_depth() const;

  int dma_speed() override;
  bool dma_read_ready() override;
  bool dma_write_ready() override;
  uint32_t dma_read() override;
  void dma_write(uint32_t val) override;

  uint32_t io_read(address_width_t width, uint32_t address) override;
  void io_write(address_width_t width, uint32_t address, uint32_t data) override;

  uint32_t data();
  uint32_t stat();

  void gp0(uint32_t data);
  void gp1(uint32_t data);

  void run_command();

  uint32_t vram_address(int x, int y);
  uint8_t vram_read8(int x, int y);
  uint16_t vram_read(int x, int y);
  void vram_write(int x, int y, uint16_t data);

  uint16_t vram_transfer_read();
  void vram_transfer_write(uint16_t data);

  void copy_vram_to_vram();
  void copy_wram_to_vram();
  void copy_vram_to_wram();

  void draw_point(point_t point, color_t color);
  void draw_line();
  void draw_polygon();
  void draw_rectangle();
  void draw_triangle(gp0_command_t command, const triangle_t &triangle);
  void fill_rectangle();

  // common functionality

  texture_color_t get_texture_color__4bpp(const tev_t &tev, const texture_coord_t &coord);
  texture_color_t get_texture_color__8bpp(const tev_t &tev, const texture_coord_t &coord);
  texture_color_t get_texture_color_15bpp(const tev_t &tev, const texture_coord_t &coord);
  texture_color_t get_texture_color(const tev_t &tev, const texture_coord_t &coord);

  void draw_color(gp0_command_t command,
      const color_t &color,
      const point_t &point,
      const texture_coord_t &coord,
      const tev_t &tev);
};

}  // namespace psx::gpu

#endif  // GPU_CORE_HPP_
