#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, ExpressionNode* exp)
    : AstNode{line, col}, exp_node(exp) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {
    std::printf("print statement <line: %u, col: %u>\n",
                location.line, location.col);
}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }
 }
