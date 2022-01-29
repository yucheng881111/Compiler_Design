#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, ExpressionNode* exp)
    : AstNode{line, col}, exp_node(exp) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {
    std::printf("return statement <line: %u, col: %u>\n",
                location.line, location.col);
}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }
}
