#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, ExpressionNode* exp, CompoundStatementNode* com1, CompoundStatementNode* com2)
    : AstNode{line, col}, exp_node(exp), com1_node(com1), com2_node(com2) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {
    std::printf("if statement <line: %u, col: %u>\n", 
        location.line, location.col);
}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }
    if(com1_node != NULL){
        p_visitor.visit(*com1_node);
    }
    if(com2_node != NULL){
        p_visitor.visit(*com2_node);
    }

}
