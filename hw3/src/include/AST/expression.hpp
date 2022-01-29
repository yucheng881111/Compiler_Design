#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/VariableReference.hpp"
#include "AST/FunctionInvocation.hpp"
#include <string>

class ExpressionNode : public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;
    void visitChildNodes(AstNodeVisitor &p_visitor);

    ExpressionNode(const uint32_t line, const uint32_t col, UnaryOperatorNode* un);
    ExpressionNode(const uint32_t line, const uint32_t col, BinaryOperatorNode* bin);
    ExpressionNode(const uint32_t line, const uint32_t col, ConstantValueNode* c);
    ExpressionNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var);
    ExpressionNode(const uint32_t line, const uint32_t col, FunctionInvocationNode* func);

    void print() override;

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment

  private:
    UnaryOperatorNode* un_op = NULL;
    BinaryOperatorNode* bin_op = NULL;
    ConstantValueNode* con = NULL;
    VariableReferenceNode* var_ref = NULL;
    FunctionInvocationNode* func_inv = NULL;
};

#endif
