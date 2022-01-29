#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, char* id_list, const char *const type)
    : AstNode{line, col}, var_name(id_list), var_type(type){}

VariableNode::VariableNode(const uint32_t line, const uint32_t col, char* id_list, ConstantValueNode* con)
    : AstNode{line, col}, var_name(id_list), con_val(con){}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {
    /*
    std::printf("variable <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                var_name.c_str(), var_type.c_str());
                */
}

void VariableNode::setType(std::string s){
    var_type = s;
}

const char *VariableNode::getVarNamesCString() const{
    return var_name.c_str();
}

const char *VariableNode::getVarTypeCString() const{
    return var_type.c_str();
}


void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
     // TODO
    return ;
 }
