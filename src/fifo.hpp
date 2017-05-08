#ifndef __PSXACT_FIFO_HPP__
#define __PSXACT_FIFO_HPP__

template<typename T, int bits>
class fifo_t {
private:
  enum {
    MASK = (1 << bits) - 1,
    MASK_LSB = (1 << (bits - 1)) - 1,
    MASK_MSB = (1 << (bits - 1)),
    SIZE = (1 << bits)
  };

  T buffer[SIZE];
  unsigned rd_ptr;
  unsigned wr_ptr;

  unsigned get_rd_lsb() const {
    return rd_ptr & MASK_LSB;
  }

  unsigned get_wr_lsb() const {
    return wr_ptr & MASK_LSB;
  }

  unsigned get_rd_msb() const {
    return rd_ptr & MASK_MSB;
  }

  unsigned get_wr_msb() const {
    return wr_ptr & MASK_MSB;
  }

public:
  const T &read() {
    T &value = buffer[rd_ptr];
    rd_ptr = (rd_ptr + 1) & MASK;

    return value;
  }

  void write(const T &value) {
    buffer[wr_ptr] = value;
    wr_ptr = (wr_ptr + 1) & MASK;
  }

  bool is_empty() const {
    return
      (get_rd_lsb() == get_wr_lsb()) &&
      (get_rd_msb() == get_wr_msb());
  }

  bool is_full() const {
    return
      (get_rd_lsb() == get_wr_lsb()) &&
      (get_rd_msb() != get_wr_msb());
  }
};

#endif // __PSXACT_FIFO_HPP__
