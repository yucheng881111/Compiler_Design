#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col, const char *const name, std::vector<ExpressionNode*>* vec)
    : AstNode{line, col}, func_name(name), exp_vec(vec) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {
    std::printf("function invocation <line: %u, col: %u> %s\n",
                location.line,
                location.col,
                func_name.c_str());
}

const char* FunctionInvocationNode::get_func_name(){
    return func_name.c_str();
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(exp_vec != NULL){
        for(auto v : *exp_vec){
            p_visitor.visit(*v);
        }
    }
}
