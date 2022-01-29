#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col, const char *const name, std::vector<ExpressionNode*>* vec)
    : AstNode{line, col}, var_name(name), exp_node_vec(vec) {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {
    std::printf("variable reference <line: %u, col: %u> %s\n",
                location.line,
                location.col,
                var_name.c_str());
}
const char* VariableReferenceNode::get_varrefName(){
    return var_name.c_str();
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_node_vec != NULL){
        for(auto v : *exp_node_vec){
            p_visitor.visit(*v);
        }
    }
}
