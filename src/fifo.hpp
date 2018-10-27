#ifndef __psxact_fifo__
#define __psxact_fifo__


namespace psx {

template<typename T, int bits>
class fifo_t {

  static const int32_t mask     = (1 << (bits + 1)) - 1;
  static const int32_t mask_lsb = (1 << bits) - 1;
  static const int32_t mask_msb = (1 << bits);
  static const int32_t size     = (1 << bits);

  T buffer[size];
  uint32_t rd_ptr;
  uint32_t wr_ptr;

public:

  void clear() {
    rd_ptr = 0;
    wr_ptr = 0;
  }

  const T &read() {
    T &value = buffer[rd_ptr & mask_lsb];
    rd_ptr = (rd_ptr + 1) & mask;

    return value;
  }

  void write(const T &value) {
    buffer[wr_ptr & mask_lsb] = value;
    wr_ptr = (wr_ptr + 1) & mask;
  }

  bool is_empty() const {
    return rd_ptr == wr_ptr;
  }

  bool is_full() const {
    return rd_ptr == (wr_ptr ^ mask_msb);
  }

  bool has_data() const {
    return !is_empty();
  }

  bool has_room() const {
    return !is_full();
  }

};

}

#endif // __psxact_fifo__
