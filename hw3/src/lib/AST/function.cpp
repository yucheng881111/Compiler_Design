#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, const char *const name, const char *const type, CompoundStatementNode* com, std::vector<DeclNode*>* vec)
    : AstNode{line, col}, func_name(name), func_type(type), compound(com), decl_vec(vec){
        std::string s = "(";
    
        if(decl_vec != NULL){
            for(auto v : *decl_vec){
                std::string tmp = v->getDelType();
                s += tmp;
            }
            s.pop_back();
            s.pop_back();
        }
        
        s += ")";
        arg_list = s;
    }

const char *FunctionNode::getFuncNameCString() const{ return func_name.c_str(); }
const char *FunctionNode::getFuncTypeCString() const{ return func_type.c_str(); }
const char *FunctionNode::getArgList() const{
    return arg_list.c_str();
}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {
    std::printf("function declaration <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                func_name.c_str(), func_type.c_str());
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
     // TODO
    if(decl_vec != NULL){
        for(auto v : *decl_vec){
            p_visitor.visit(*v);
        }
    }


    if(compound != NULL){
        p_visitor.visit(*compound);
    }
    
 }
