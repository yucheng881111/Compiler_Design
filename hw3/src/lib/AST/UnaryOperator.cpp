#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, const char *const op, ExpressionNode* exp)
    : AstNode{line, col}, oper(op), exp_node(exp) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {
    std::printf("unary operator <line: %u, col: %u> %s\n",
                location.line, location.col,
                oper.c_str());
}

const char* UnaryOperatorNode::get_un_op(){
    return oper.c_str();
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }
}
