#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/PType.hpp"

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col)
        : AstNode{line, col} {}

    void set_type(PTypeSharedPtr t) { type = t; }
    PType get_type() { return *type; }
    void set_error() { error = 1; }
    bool get_error() { return error; }

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
    PTypeSharedPtr type = nullptr;
    bool error = 0;
};

#endif
