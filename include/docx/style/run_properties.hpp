#pragma once

#include <string>

#include "run_types.hpp"

namespace docx::style {

class RunProperties {
 public:
  explicit RunProperties() = default;

  RunProperties& set_ascii_font(const std::string& font_name);
  RunProperties& set_east_asia_font(const std::string& font_name);
  RunProperties& set_size(float size_pt);

  RunProperties& set_bold(bool enable = true);
  RunProperties& set_italic(bool enable = true);
  RunProperties& set_strike(bool enable = true);
  RunProperties& set_underline(UnderlineType type = UnderlineType::Single);
};

}  // namespace docx::style
