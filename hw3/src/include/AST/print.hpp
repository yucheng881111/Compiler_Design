#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include <string>

class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col,
              /* TODO: expression */
              ExpressionNode* exp);
    ~PrintNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression
    ExpressionNode* exp_node = NULL;
};

#endif
