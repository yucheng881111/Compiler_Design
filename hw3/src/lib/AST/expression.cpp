#include "AST/expression.hpp"

ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col, UnaryOperatorNode* un) : AstNode{line, col}, un_op(un) {}
ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col, BinaryOperatorNode* bin) : AstNode{line, col}, bin_op(bin) {}
ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col, ConstantValueNode* c) : AstNode{line, col}, con(c) {}
ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var) : AstNode{line, col}, var_ref(var) {}
ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col, FunctionInvocationNode* func) : AstNode{line, col}, func_inv(func) {}

void ExpressionNode::visitChildNodes(AstNodeVisitor &p_visitor){
    if(un_op != NULL){
        p_visitor.visit(*un_op);
    }
    if(bin_op != NULL){
        p_visitor.visit(*bin_op);
    }
    if(con != NULL){
        p_visitor.visit(*con);
    }
    if(var_ref != NULL){
        p_visitor.visit(*var_ref);
    }
    if(func_inv != NULL){
        p_visitor.visit(*func_inv);
    }

}

void ExpressionNode::print(){
    
}

