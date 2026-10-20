// Smoke tests for the docx-lib public headers.
//
// Most of the API is still declaration-only (preliminary design phase), so
// these tests exercise what is currently usable: header self-containment of
// the umbrella header and the inline implementations in common/color.hpp.

#include <cstdio>
#include <docx/docx.hpp>
#include <string>

namespace {

int g_failures = 0;

#define CHECK(cond)                                               \
  do {                                                            \
    if (!(cond)) {                                                \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                               \
    }                                                             \
  } while (false)

void test_rgb_from_hex() {
  const auto rgb = docx::common::Rgb::from_hex("FF8040");
  CHECK(rgb.has_value());
  if (rgb) {
    CHECK(rgb->r == 0xFF);
    CHECK(rgb->g == 0x80);
    CHECK(rgb->b == 0x40);
    CHECK(rgb->to_hex() == "FF8040");
  }

  // Lower-case input is accepted.
  const auto lower = docx::common::Rgb::from_hex("ff8040");
  CHECK(lower.has_value());
  if (lower && rgb) {
    CHECK(lower->to_hex() == rgb->to_hex());
  }
}

void test_rgb_from_hex_rejects_invalid() {
  CHECK(!docx::common::Rgb::from_hex("").has_value());
  CHECK(!docx::common::Rgb::from_hex("FFF").has_value());      // wrong length
  CHECK(!docx::common::Rgb::from_hex("GGGGGG").has_value());   // non-hex digits
  CHECK(!docx::common::Rgb::from_hex("FF80400").has_value());  // wrong length
  CHECK(!docx::common::Rgb::from_hex("#FF8040").has_value());  // '#' not allowed
}

void test_enum_values_are_usable() {
  // Compile-time sanity checks for the enumerations the style layer relies on.
  constexpr auto alignment = docx::common::TextAlignment::Both;
  constexpr auto spacing = docx::common::LineSpacingRule::AtLeast;
  constexpr auto underline = docx::style::UnderlineType::DottedHeavy;
  constexpr auto highlight = docx::style::HighlightColor::LightGray;
  constexpr auto vert_align = docx::style::RunVerticalAlign::Subscript;
  constexpr auto border = docx::style::BorderStyle::DotDotDash;

  CHECK(static_cast<int>(alignment) >= 0);
  CHECK(static_cast<int>(spacing) >= 0);
  CHECK(static_cast<int>(underline) >= 0);
  CHECK(static_cast<int>(highlight) >= 0);
  CHECK(static_cast<int>(vert_align) >= 0);
  CHECK(static_cast<int>(border) >= 0);
}

void test_border_defaults() {
  const docx::style::Border border;
  CHECK(border.style == docx::style::BorderStyle::Nil);
  CHECK(!border.color.has_value());
  CHECK(border.size_pt == 0.5f);
}

}  // namespace

int main() {
  test_rgb_from_hex();
  test_rgb_from_hex_rejects_invalid();
  test_enum_values_are_usable();
  test_border_defaults();

  if (g_failures == 0) {
    std::printf("All tests passed\n");
    return 0;
  }
  std::printf("%d check(s) failed\n", g_failures);
  return 1;
}
