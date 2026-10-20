#pragma once

namespace docx::style {

enum class RunVerticalAlign {
  Baseline,
  Superscript,
  Subscript
};

enum class UnderlineType {
  None,
  Single,
  Double,
  Thick,
  Dotted,
  DottedHeavy,
  Dash,
  DashedHeavy,
  Wave
};

enum class HighlightColor {
  None,
  Black,
  Blue,
  Cyan,
  Green,
  Magenta,
  Red,
  Yellow,
  White,
  DarkBlue,
  DarkCyan,
  DarkGreen,
  DarkMagenta,
  DarkRed,
  DarkYellow,
  DarkGray,
  LightGray
};

}  // namespace docx::style
