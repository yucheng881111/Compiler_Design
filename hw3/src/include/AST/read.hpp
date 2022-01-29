#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include <string>

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
             /* TODO: variable reference */
            VariableReferenceNode* ref);
    ~ReadNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: variable reference
    VariableReferenceNode* ref_node = NULL;
};

#endif
