#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode*>* tmp, const char* type, ConstantValueNode* con)
    : AstNode{line, col}, var_node(tmp), del_type(type), con_val(con){
        if(del_type != ""){
            for(auto v : *var_node){
                v->setType(del_type);
                type_list += (del_type + ", ");
            }
        }else{
            del_type = con_val->getConstantType();
            for(auto v : *var_node){
                v->setType(del_type);
            }
        }
    }

DeclNode::DeclNode(const uint32_t line, const uint32_t col,VariableNode* tmp)
    : AstNode{line, col}, for_var(tmp) {}

const char* DeclNode::getDelType(){
    return type_list.c_str();
}
// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {
    std::printf("declaration <line: %u, col: %u>\n", 
        location.line, location.col);
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(for_var != NULL){
        p_visitor.visit(*for_var);
    }

    if(var_node != NULL){
        for(auto v : *var_node){
            p_visitor.visit(*v);
            if(con_val != NULL){
                p_visitor.visit(*con_val);
            }
            
        }
    }
    

}
