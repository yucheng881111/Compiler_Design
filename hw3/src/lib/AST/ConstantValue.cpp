#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, const char *const con_val, UnaryOperatorNode* un, const char* s)
    : AstNode{line, col}, const_val(con_val), unary(un), constant_type(s) {
        if(unary != NULL){
            const_val = "-" + const_val;
        }
        if(constant_type == "real"){
            float f = atof(const_val.c_str());
            const_val = std::to_string(f);
        }
        if(constant_type == "sb" && const_val != "true" && const_val != "false"){
            std::string temp = "";
            for(int i = 1; i < (int)const_val.size() - 1; ++i){
                temp += const_val[i];
            }
            const_val = temp;
        }
        if(constant_type == ""){
            if(const_val[0] == '"'){
                // string
                std::string temp = "";
                for(int i = 1; i < (int)const_val.size() - 1; ++i){
                    temp += const_val[i];
                }
                const_val = temp;
            }else{
                // float
                bool is_float = 0;
                for(int i = 0; i < (int)const_val.size(); ++i){
                    if(const_val[i] == '.'){
                        is_float = 1;
                        break;
                    }
                }
                if(is_float){
                    float f = atof(const_val.c_str());
                    const_val = std::to_string(f);
                }
            }

            no_type = 1;
        }else{
            no_type = 0;
        }
    }

const char *ConstantValueNode::getValCString() const{ return const_val.c_str(); }
const char *ConstantValueNode::getConstantType() const{
                        std::string str = "";
                        if(constant_type == "sb"){
                            if(const_val == "true" || const_val == "false"){
                                str = "boolean";
                            }else{
                                str = "string";
                            }
                        }else if(constant_type == "int"){
                            str = "integer";
                        }else{
                            str = "real";
                        }
                        
                        return str.c_str(); }

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {
    std::printf("constant <line: %u, col: %u> %s\n",
                location.line,
                location.col,
                const_val.c_str());
}
