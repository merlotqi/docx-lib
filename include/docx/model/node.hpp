#pragma once

#include <memory>
#include <vector>

namespace docx {

class NodeVisitor;

class Node {
 public:
  virtual ~Node() = default;
  virtual void accept(NodeVisitor &visitor) = 0;

  Node *parent() const;
};

class ContainerNode : public Node {
 public:
  void append_child(std::unique_ptr<Node> child);
  const std::vector<std::unique_ptr<Node>> &children() const;

 private:
  Node *parent_ = nullptr;
  std::vector<std::unique_ptr<Node>> children_;
};

}  // namespace docx
