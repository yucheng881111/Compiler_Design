#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/VariableReference.hpp"
#include "AST/ConstantValue.hpp"

#include <string>

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   /* TODO: variable reference, expression */
                  VariableReferenceNode* ref, ExpressionNode* exp);
    
    AssignmentNode(const uint32_t line, const uint32_t col,
                  VariableReferenceNode* ref, ConstantValueNode* con);
    
    ~AssignmentNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: variable reference, expression
    VariableReferenceNode* ref_node = NULL;
    ExpressionNode* exp_node = NULL;
    ConstantValueNode* for_const = NULL;
};

#endif
