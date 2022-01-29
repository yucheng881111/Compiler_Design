#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include <string>

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           /* TODO: expression, compound statement, compound statement */
          ExpressionNode* exp, CompoundStatementNode* com1, CompoundStatementNode* com2
          );
    ~IfNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: expression, compound statement, compound statement
    ExpressionNode* exp_node = NULL;
    CompoundStatementNode* com1_node = NULL;
    CompoundStatementNode* com2_node = NULL;
};

#endif
