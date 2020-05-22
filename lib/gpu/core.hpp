#ifndef GPU_CORE_HPP_
#define GPU_CORE_HPP_

#include "gpu/types.hpp"
#include "memory/memory-base.hpp"
#include "util/fifo.hpp"
#include "util/wire.hpp"
#include "addressable.hpp"
#include "dma-comms.hpp"

namespace psx::gpu {

  enum class gpu_display_depth {
    bpp15,
    bpp24
  };

  enum class gpu_h_resolution {
    h256 = 256,
    h320 = 320,
    h368 = 368,
    h512 = 512,
    h640 = 640
  };

  enum class gpu_v_resolution {
    v240 = 240,
    v480 = 480
  };

  enum class gpu_field {
    even,
    odd
  };

  constexpr int GPU_GP0 = 0x1f801810;
  constexpr int GPU_GP1 = 0x1f801814;
  constexpr int GPU_READ = 0x1f801810;
  constexpr int GPU_STAT = 0x1f801814;

  class core final
      : public addressable
      , public dma_comms {

    /// Used to send IRQ(1) signals to the CPU.
    util::wire irq;

    /// Used to send HBlank signals to the timers.
    util::wire hbl;

    /// Used to send VBlank signals to the timers, and IRQ(0) signals to the CPU.
    util::wire vbl;

    int prescaler = {};
    int counter = {};

    gpu_h_resolution h_resolution = gpu_h_resolution::h256;
    gpu_v_resolution v_resolution = gpu_v_resolution::v240;
    gpu_display_depth display_depth;
    gpu_field field = gpu_field::even;
    uint32_t video_buffer[480][640];

    uint32_t status = 0x14802000;

  public:
    memory_base<mib(1)> *vram;

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

    psx::util::fifo< uint32_t, 4 > fifo = {};

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

    core(util::wire irq, util::wire hbl, util::wire vbl);
    ~core();

    // Interface for external video rendering

    uint32_t *get_video_buffer() const;
    int32_t get_width() const;
    int32_t get_height() const;

    /// Called with the number of CPU cycles that have elapsed.
    /// Returns true if a field has elapsed, false otherwise.
    bool tick(int amount);

    /// Called with the number of GPU cycles that have elapsed.
    /// Returns true if a field has elapsed, false otherwise.
    bool step(int amount);

    /// Called at the end of a field to draw the field into the video buffer.
    void render_field_240p();

    /// Called at the end of a field to draw the field into the video buffer.
    void render_field_480i();

    uint32_t get_status() const;
    gpu_h_resolution get_h_resolution() const;
    gpu_v_resolution get_v_resolution() const;
    gpu_display_depth get_display_depth() const;

    int dma_speed() override;
    bool dma_read_ready() override;
    bool dma_write_ready() override;
    uint32_t dma_read() override;
    void dma_write(uint32_t val) override;

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;

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

    void draw_point(point point, color color);
    void draw_line();
    void draw_polygon();
    void draw_rectangle();
    void draw_triangle(gp0_command command, const triangle &triangle);
    void fill_rectangle();

    // common functionality

    texture_color get_texture_color__4bpp(const texture_params &tev, const texture_coord &coord);
    texture_color get_texture_color__8bpp(const texture_params &tev, const texture_coord &coord);
    texture_color get_texture_color_15bpp(const texture_params &tev, const texture_coord &coord);
    texture_color get_texture_color(const texture_params &tev, const texture_coord &coord);

    void draw_color(gp0_command command,
        const color &color,
        const point &point,
        const texture_coord &coord,
        const texture_params &tev);
  };
}

#endif
