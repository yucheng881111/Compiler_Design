#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/decl.hpp"

#include <string>
#include <vector>

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */
		const char *const name, const char *const type, CompoundStatementNode* com,
    std::vector<DeclNode*>* vec);
    ~FunctionNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getFuncNameCString() const;
    const char *getFuncTypeCString() const;
    const char *getArgList() const;

  private:
    // TODO: name, declarations, return type, compound statement
    std::string func_name;
    std::string func_type;
    std::string arg_list;
    CompoundStatementNode* compound = NULL;
    std::vector<DeclNode*>* decl_vec = NULL;
};

#endif
