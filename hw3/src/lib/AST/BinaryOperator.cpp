#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, const char *const op, ExpressionNode* exp1, ExpressionNode* exp2)
    : AstNode{line, col}, oper(op), left_exp(exp1), right_exp(exp2){}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {
    std::printf("binary operator <line: %u, col: %u> %s\n",
                location.line, location.col,
                oper.c_str());
}

const char* BinaryOperatorNode::get_bin_op(){
    return oper.c_str();
} 

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(left_exp != NULL){
        p_visitor.visit(*left_exp);
    }
    if(right_exp != NULL){
        p_visitor.visit(*right_exp);
    }
 }
