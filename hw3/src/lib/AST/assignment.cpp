#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, VariableReferenceNode* ref, ExpressionNode* exp)
    : AstNode{line, col}, ref_node(ref), exp_node(exp) {}

AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, VariableReferenceNode* ref, ConstantValueNode* con)
    : AstNode{line, col}, ref_node(ref), for_const(con) {
        for_const->no_type = 1;
    }

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {
    std::printf("assignment statement <line: %u, col: %u>\n",
                location.line,
                location.col);
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(ref_node != NULL){
        p_visitor.visit(*ref_node);
    }
    if(exp_node != NULL){
        p_visitor.visit(*exp_node);
    }

    if(for_const != NULL){
        p_visitor.visit(*for_const);
    }

}
