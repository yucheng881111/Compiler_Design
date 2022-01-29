#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, CompoundStatementNode* com, std::vector<DeclNode*>* dec, std::vector<FunctionNode*>* vec)
    : AstNode{line, col}, name(p_name), compound(com), decl_vec(dec), func_vec(vec) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }
const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::accept(AstNodeVisitor &v){
    v.visit(*this);
}


void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor){
    // basic case 1 pass
    // std::printf("  ");
    // compound->print();

    if(decl_vec != NULL){
        for(auto v : *decl_vec){
            p_visitor.visit(*v);
        }
    }

    if(func_vec != NULL){
        for(auto v : *func_vec){
            p_visitor.visit(*v);
        }
    }

    if(compound != NULL){
        p_visitor.visit(*compound);
    }
    
}

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}


// void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
//     /* TODO
//      *
//      * for (auto &decl : var_decls) {
//      *     decl->accept(p_visitor);
//      * }
//      *
//      * // functions
//      *
//      * body->accept(p_visitor);
//      */
// }
