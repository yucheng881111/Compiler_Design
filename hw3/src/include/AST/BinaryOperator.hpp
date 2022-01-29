#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"

#include <memory>
#include <string>

class BinaryOperatorNode : public AstNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       /* TODO: operator, expressions */
		      const char *const op, ExpressionNode* exp1, ExpressionNode* exp2);
    ~BinaryOperatorNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char* get_bin_op(); 

    
  private:
    // TODO: operator, expressions
    std::string oper;
    ExpressionNode* left_exp = NULL;
    ExpressionNode* right_exp = NULL;
};

#endif
