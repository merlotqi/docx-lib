#pragma once

#include "node.hpp"

namespace docx {

class Run;

class Paragraph : public ContainerNode {
 public:
  Paragraph &set_alignment();
  Paragraph &set_bold(bool);
  Paragraph &set_font_size(float);
};

}  // namespace docx
