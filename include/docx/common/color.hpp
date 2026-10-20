#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace docx::common {

struct Rgb {
  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};

  static std::optional<Rgb> from_hex(const std::string& hex_str) {
    if (hex_str.length() != 6) return std::nullopt;
    try {
      uint32_t val = std::stoul(hex_str, nullptr, 16);
      return Rgb{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                 static_cast<uint8_t>(val & 0xFF)};
    } catch (...) {
      return std::nullopt;
    }
  }

  std::string to_hex() const {
    char buf[7];
    snprintf(buf, sizeof(buf), "%02X%02X%02X", r, g, b);
    return std::string(buf);
  }
};

}  // namespace docx::common
