#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"
#include <string>
#include <vector>

class FunctionInvocationNode : public AstNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           /* TODO: function name, expressions */
		          const char *const name, std::vector<ExpressionNode*>* vec);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char* get_func_name();

  private:
    // TODO: function name, expressions
    std::string func_name;
    std::vector<ExpressionNode*>* exp_vec = NULL;
};

#endif
