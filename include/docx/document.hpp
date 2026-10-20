#pragma once

#include <filesystem>

namespace docx {

class Document {
 public:
  explicit Document();
  bool open(const std::filesystem::path& filepath);
};

}  // namespace docx
