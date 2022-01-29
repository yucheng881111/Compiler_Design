#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include <string>

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              /* TODO: expression, compound statement */
              ExpressionNode* exp_n, CompoundStatementNode* com_n);
    ~WhileNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression, compound statement
    ExpressionNode* exp_node = NULL;
    CompoundStatementNode* com_node = NULL;
};

#endif
