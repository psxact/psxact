#ifndef __PSXACT_FIFO_HPP__
#define __PSXACT_FIFO_HPP__

template<typename T, int bits>
class fifo_t {
private:
  enum {
    MASK = (1 << (bits + 1)) - 1,
    MASK_LSB = (1 << bits) - 1,
    MASK_MSB = (1 << bits),
    SIZE = (1 << bits)
  };

  T buffer[SIZE];
  unsigned rd_ptr;
  unsigned wr_ptr;

public:
  void clear() {
    rd_ptr = 0;
    wr_ptr = 0;
  }

  const T &read() {
    T &value = buffer[rd_ptr & MASK_LSB];
    rd_ptr = (rd_ptr + 1) & MASK;

    return value;
  }

  void write(const T &value) {
    buffer[wr_ptr & MASK_LSB] = value;
    wr_ptr = (wr_ptr + 1) & MASK;
  }

  bool is_empty() const {
    return rd_ptr == wr_ptr;
  }

  bool is_full() const {
    return rd_ptr == (wr_ptr ^ MASK_MSB);
  }

  bool has_data() const {
    return !is_empty();
  }

  bool has_room() const {
    return !is_full();
  }
};

#endif // __PSXACT_FIFO_HPP__
