#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/decl.hpp"

#include <string>
#include <vector>


class ProgramNode final : public AstNode {
  private:
    std::string name;
    // TODO: return type, declarations, functions, compound statement
    CompoundStatementNode* compound = NULL;
    DeclNode* decl = NULL;
    std::vector<FunctionNode*>* func_vec = NULL;
    std::vector<DeclNode*>* decl_vec = NULL;

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *const p_name,
                /* TODO: return type, declarations, functions,
                 *       compound statement */
                CompoundStatementNode* com, std::vector<DeclNode*>* dec,
                std::vector<FunctionNode*>* vec);

    // visitor pattern version: const char *getNameCString() const;
    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char *getNameCString() const;
    void accept(AstNodeVisitor &v);
};

#endif
