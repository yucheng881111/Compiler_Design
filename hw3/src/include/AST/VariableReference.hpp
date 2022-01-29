#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"
#include <string>
#include <vector>

class VariableReferenceNode : public AstNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          /* TODO: name */
		         const char *const name, std::vector<ExpressionNode*>* vec);
    // array reference
    // VariableReferenceNode(const uint32_t line, const uint32_t col
    //                       /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;

    const char* get_varrefName();

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: variable name, expressions
    std::string var_name;
    std::vector<ExpressionNode*>* exp_node_vec = NULL;
};

#endif
