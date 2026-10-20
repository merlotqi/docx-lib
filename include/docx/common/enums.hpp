#pragma once

namespace docx::common {

enum class TextAlignment {
  Left,
  Center,
  Right,
  Both,
  Distribute,
  Justify
};

enum class LineSpacingRule {
  Auto,
  Exact,
  AtLeast
};

}  // namespace docx::common
