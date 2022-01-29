#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, VariableReferenceNode* ref)
    : AstNode{line, col}, ref_node(ref) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {
    std::printf("read statement <line: %u, col: %u>\n",
                location.line, location.col);
}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(ref_node != NULL){
        p_visitor.visit(*ref_node);
    }

}
