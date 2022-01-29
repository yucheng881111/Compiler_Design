#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/print.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/FunctionInvocation.hpp"
#include <string>
#include <vector>

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          /* TODO: declarations, statements */
                          std::vector<DeclNode*>* vec, std::vector<CompoundStatementNode*>* vec2);
    CompoundStatementNode(const uint32_t line, const uint32_t col, PrintNode* prt);
    CompoundStatementNode(const uint32_t line, const uint32_t col, AssignmentNode* ass);
    CompoundStatementNode(const uint32_t line, const uint32_t col, ReadNode* read);
    CompoundStatementNode(const uint32_t line, const uint32_t col, IfNode* if_n);
    CompoundStatementNode(const uint32_t line, const uint32_t col, WhileNode* while_n);
    CompoundStatementNode(const uint32_t line, const uint32_t col, ForNode* for_n);
    CompoundStatementNode(const uint32_t line, const uint32_t col, ReturnNode* ret_n);
    CompoundStatementNode(const uint32_t line, const uint32_t col, FunctionInvocationNode* func_n);


    ~CompoundStatementNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    PrintNode* get_printNode();
    AssignmentNode* get_assNode();
    ReadNode* get_readNode();
    IfNode* get_ifNode();
    WhileNode* get_whileNode();
    ForNode* get_forNode();
    ReturnNode* get_retNode();
    FunctionInvocationNode* get_funcInvNode();

  private:
    // TODO: declarations, statements
    std::vector<DeclNode*>* decl_vec = NULL;
    std::vector<CompoundStatementNode*>* stat_vec = NULL;
    PrintNode* print_node = NULL;
    AssignmentNode* assign_node = NULL;
    ReadNode* read_node = NULL;
    IfNode* if_node = NULL;
    WhileNode* while_node = NULL;
    ForNode* for_node = NULL;
    ReturnNode* ret_node = NULL;
    FunctionInvocationNode* func_inv_node = NULL;
};

#endif
