#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace set_bits {
// clang-format off
template <typename T>
inline std::int32_t set_bit_at(std::size_t const position) {
    return 1 << ((sizeof(T) * position) - 1);
}

inline void set_high_bit_as_0(std::int32_t& value) {
    value &= ~set_bit_at<std::int32_t>(8);
}

inline void set_high_bit_as_1(std::int32_t& value) {
    value |= set_bit_at<std::int32_t>(8);
}

inline void set_low_bit_as_0(std::int32_t& value) {
    value &= ~set_bit_at<char>(1);
}

inline void set_low_bit_as_1(std::int32_t& value) {
    value |= set_bit_at<char>(1);
}

static std::int32_t message_type = 0x0;
// clang-format on

// assumes little endian
void print_bits(std::size_t const size, void const* const ptr) {

    std::uint8_t* b = (std::uint8_t*)ptr;
    std::uint8_t  byte;

    for (std::int32_t i = size - 1; i >= 0; --i) {
        for (std::int32_t j = 7; j >= 0; --j) {
            byte = (b[i] >> j) & 1;
            std::printf("%u", byte);
        }
    }
    std::puts("");
}

} // namespace set_bits
