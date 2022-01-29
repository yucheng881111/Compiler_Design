#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include <string>
#include <vector>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 /* TODO: variable name, type, constant value */
		char* id_list, const char *const type);

    VariableNode(const uint32_t line, const uint32_t col,
                 /* TODO: variable name, type, constant value */
    char* id_list, ConstantValueNode* con);

    ~VariableNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void setType(std::string s);
    const char *getVarNamesCString() const;
    const char *getVarTypeCString() const;

  private:
    // TODO: variable name, type, constant value
    std::string var_name;
    std::string var_type;
    ConstantValueNode* con_val = NULL;
};

#endif
