#include "utility.hpp"
#include <stdio.h>
#include <stdarg.h>

namespace utility {
    void debug(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    void read_all_bytes(const char *filename, uint8_t *result, int skip, int take) {
        if (FILE* file = fopen(filename, "rb+")) {
            fseek(file, skip, SEEK_SET);
            fread(result, 1, take, file);
            fclose(file);
        }
    }

    uint32_t read_byte(uint8_t *buffer, uint32_t address) {
        return buffer[address];
    }

    uint32_t read_half(uint8_t *buffer, uint32_t address) {
        return
            (read_byte(buffer, address &~1) << 0) |
            (read_byte(buffer, address | 1) << 8);
    }

    uint32_t read_word(uint8_t *buffer, uint32_t address) {
        return
            (read_half(buffer, address &~2) << 0) |
            (read_half(buffer, address | 2) << 16);
    }

    void write_byte(uint8_t *buffer, uint32_t address, uint32_t data) {
        buffer[address] = data & 0xff;
    }

    void write_half(uint8_t *buffer, uint32_t address, uint32_t data) {
        write_byte(buffer, address &~1, data);
        write_byte(buffer, address | 1, data >> 8);
    }

    void write_word(uint8_t *buffer, uint32_t address, uint32_t data) {
        write_half(buffer, address &~2, data);
        write_half(buffer, address | 2, data >> 16);
    }
}
