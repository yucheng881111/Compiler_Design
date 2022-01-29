#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"
#include <string>

class UnaryOperatorNode : public AstNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      /* TODO: operator, expression */
		     const char *const op, ExpressionNode* exp);
    ~UnaryOperatorNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char* get_un_op();

  private:
    // TODO: operator, expression
    std::string oper;
    ExpressionNode* exp_node = NULL;
};

#endif
