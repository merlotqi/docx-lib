#pragma once

#include <optional>

#include "../common/color.hpp"

namespace docx::style {

enum class BorderStyle {
  Nil,
  None,
  Single,
  Thick,
  Double,
  Dotted,
  Dashed,
  DotDash,
  DotDotDash,
  Triple,
  ThinThickSmallGap,
  ThickThinSmallGap,
};

struct Border {
  BorderStyle style = BorderStyle::Nil;
  float size_pt{0.5f};
  std::optional<common::Rgb> color;
};

}  // namespace docx::style
