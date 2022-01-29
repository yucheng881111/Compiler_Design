#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col, std::vector<DeclNode*>* vec, std::vector<CompoundStatementNode*>* vec2)
    : AstNode{line, col}, decl_vec(vec), stat_vec(vec2){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, PrintNode* prt)
    : AstNode{line, col}, print_node(prt){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, AssignmentNode* ass)
    : AstNode{line, col}, assign_node(ass){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, ReadNode* read)
    : AstNode{line, col}, read_node(read){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, IfNode* if_n)
    : AstNode{line, col}, if_node(if_n){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, WhileNode* while_n)
    : AstNode{line, col}, while_node(while_n){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, ForNode* for_n)
    : AstNode{line, col}, for_node(for_n){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, ReturnNode* ret_n)
    : AstNode{line, col}, ret_node(ret_n){}

CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, FunctionInvocationNode* func_n)
    : AstNode{line, col}, func_inv_node(func_n){}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {
    std::printf("compound statement <line: %u, col: %u>\n",
                location.line,
                location.col);
}

PrintNode* CompoundStatementNode::get_printNode(){
    return print_node;
}

AssignmentNode* CompoundStatementNode::get_assNode(){
    return assign_node;
}

ReadNode* CompoundStatementNode::get_readNode(){
    return read_node;
}

IfNode* CompoundStatementNode::get_ifNode(){
    return if_node;
}

WhileNode* CompoundStatementNode::get_whileNode(){
    return while_node;
}

ForNode* CompoundStatementNode::get_forNode(){
    return for_node;
}

ReturnNode* CompoundStatementNode::get_retNode(){
    return ret_node;
}

FunctionInvocationNode* CompoundStatementNode::get_funcInvNode(){
    return func_inv_node;
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(func_inv_node != NULL){
        p_visitor.visit(*func_inv_node);
    }

    if(ret_node != NULL){
        p_visitor.visit(*ret_node);
    }

    if(for_node != NULL){
        p_visitor.visit(*for_node);
    }

    if(while_node != NULL){
        p_visitor.visit(*while_node);
    }

    if(if_node != NULL){
        p_visitor.visit(*if_node);
    }

    if(print_node != NULL){
        p_visitor.visit(*print_node);
    }

    if(assign_node != NULL){
        p_visitor.visit(*assign_node);
    }

    if(read_node != NULL){
        p_visitor.visit(*read_node);
    }
    

    if(decl_vec != NULL){
        for(auto v : *decl_vec){
            p_visitor.visit(*v);
        }
    }
    if(stat_vec != NULL){
        for(auto v : *stat_vec){
            p_visitor.visit(*v);
        }
    }
 }
