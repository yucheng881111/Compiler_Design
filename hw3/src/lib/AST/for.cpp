#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col, DeclNode* d, AssignmentNode* a, ConstantValueNode* con, CompoundStatementNode* com)
    : AstNode{line, col}, decl_node(d), ass_node(a), con_node(con), com_node(com) {
        con_node->no_type = 1;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {
    std::printf("for statement <line: %u, col: %u>\n", 
        location.line, location.col);
}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(decl_node != NULL){
        p_visitor.visit(*decl_node);
    }
    if(ass_node != NULL){
        p_visitor.visit(*ass_node);
    }
    if(con_node != NULL){
        p_visitor.visit(*con_node);
    }
    if(com_node != NULL){
        p_visitor.visit(*com_node);
    }
}
