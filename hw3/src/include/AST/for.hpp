#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/CompoundStatement.hpp"
#include <string>

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            /* TODO: declaration, assignment, expression,
             *       compound statement */
            DeclNode* d, AssignmentNode* a, ConstantValueNode* con, CompoundStatementNode* com);
    ~ForNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  private:
    // TODO: declaration, assignment, expression, compound statement
    DeclNode* decl_node = NULL;
    AssignmentNode* ass_node = NULL;
    ConstantValueNode* con_node = NULL;
    CompoundStatementNode* com_node = NULL;

};

#endif
