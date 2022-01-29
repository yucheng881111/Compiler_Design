#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/variable.hpp"
#include <string>
#include <vector>

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             /* TODO: identifiers, type */
            std::vector<VariableNode*>* tmp, const char* type, ConstantValueNode* con);

    DeclNode(const uint32_t line, const uint32_t col,
             /* TODO: identifiers, type */
            VariableNode* tmp);

    ~DeclNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    const char* getDelType();

  private:
    // TODO: variables
    std::vector<VariableNode*>* var_node = NULL;
    std::string del_type;
    ConstantValueNode* con_val = NULL;
    std::string type_list = "";
    VariableNode* for_var = NULL;
};

#endif
