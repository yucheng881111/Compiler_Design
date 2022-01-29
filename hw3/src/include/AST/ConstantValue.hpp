#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/ast.hpp"
#include <string>
#include <stdlib.h>

class ConstantValueNode : public AstNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      /* TODO: constant value */
		      const char *const con_val, UnaryOperatorNode* un, const char* s);
    ~ConstantValueNode() = default;

    void print() override;
    const char *getValCString() const;
    const char *getConstantType() const;
    bool no_type;
    
  private:
    // TODO: constant value
    std::string const_val;
    UnaryOperatorNode* unary = NULL;
    std::string constant_type;
    
};

#endif
