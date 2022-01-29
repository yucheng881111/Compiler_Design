#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col, ExpressionNode* exp_n, CompoundStatementNode* com_n)
    : AstNode{line, col}, exp_node(exp_n), com_node(com_n) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {
    std::printf("while statement <line: %u, col: %u>\n",
                location.line, location.col);
}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }
    if(com_node != NULL){
        p_visitor.visit(*com_node);
    }
}
