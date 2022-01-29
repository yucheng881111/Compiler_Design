#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

extern bool opt_dmp;
extern bool global_error;

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    SymbolTable* new_table = new SymbolTable(TABLE_KIND_PROGRAM);
    symbol_tables.pushTable(new_table);
    symbol_tables.addSymbol(p_program.getNameCString(), ENTRY_KIND_PROGRAM, PType::PrimitiveTypeEnum::kVoidType, "", 0);
    p_program.visitChildNodes(*this);
    symbol_tables.popTable(opt_dmp);
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    bool e = 0;

    // check redeclare
    if(symbol_tables.get_entry_kind(p_variable.getNameCString(), 0) != ENTRY_KIND_NOT_FOUND || symbol_tables.get_for_entry_kind(p_variable.getNameCString())){
        print_error(p_variable.getLocation().line, p_variable.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_variable.getNameCString());
        print_error_code(p_variable.getLocation().line, p_variable.getLocation().col);
        e = 1;
        global_error = 1;
    }

    bool e2 = 0;
    // check index validation
    std::vector<uint64_t> idx = p_variable.getType().get_vec_idx();
    for(int i = 0; i < idx.size(); i++){
        if(idx[i] <= 0){
            print_error(p_variable.getLocation().line, p_variable.getLocation().col);
            fprintf(stderr, "'%s' declared as an array with an index that is not greater than 0\n", p_variable.getNameCString());
            print_error_code(p_variable.getLocation().line, p_variable.getLocation().col);
            e2 = 1;
            global_error = 1;
        }
    }

    int kind;
    if(p_variable.is_const()){
        kind = ENTRY_KIND_CONSTANT;
    }else if(symbol_tables.get_top_table_kind() == TABLE_KIND_FUNCTION){
        kind = ENTRY_KIND_PARAMETER;
    }else if(symbol_tables.get_top_table_kind() == TABLE_KIND_FOR){
        kind = ENTRY_KIND_LOOP_VAR;
    }else{
        kind = ENTRY_KIND_VARIABLE;
    }

    if(!e){
        if(e2){
            symbol_tables.addSymbol(p_variable.getNameCString(), kind, p_variable.getType(), p_variable.getconstantCString(), 1);
        }else{
            symbol_tables.addSymbol(p_variable.getNameCString(), kind, p_variable.getType(), p_variable.getconstantCString(), 0);
        }
    }
    
    p_variable.visitChildNodes(*this);

}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_constant_value.visitChildNodes(*this);
    p_constant_value.set_type(p_constant_value.getTypeSharedPtr());
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    bool e = 0;

    if(symbol_tables.get_entry_kind(p_function.getNameCString(), 0) != ENTRY_KIND_NOT_FOUND){
        print_error(p_function.getLocation().line, p_function.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_function.getNameCString());
        print_error_code(p_function.getLocation().line, p_function.getLocation().col);
        e = 1;
        global_error = 1;
    }

    functions.addFunctionEntry(p_function.getNameCString(), p_function.getParaType(), p_function.getReturnType());
    functions.func_check++;

    if(!e){
        symbol_tables.addSymbol(p_function.getNameCString(), ENTRY_KIND_FUNCTION, p_function.getReturnType(), p_function.getParaTypeString(), 0);
    }
    SymbolTable* new_table = new SymbolTable(TABLE_KIND_FUNCTION);
    symbol_tables.pushTable(new_table);
    p_function.visitChildNodes(*this);
    symbol_tables.popTable(opt_dmp);
    functions.func_check--;
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    int k = symbol_tables.get_top_table_kind();

    if(k != TABLE_KIND_FUNCTION){
        SymbolTable* new_table = new SymbolTable(TABLE_KIND_COM);
        symbol_tables.pushTable(new_table);
    }
    
    symbol_tables.change_state(TABLE_KIND_COM);
    p_compound_statement.visitChildNodes(*this);
    
    if(k != TABLE_KIND_FUNCTION){
        symbol_tables.popTable(opt_dmp);
    }

}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);

    if(!p_print.get_print_error()){
        PType t = p_print.get_print_type();
        if(t.get_vec_idx().size() != 0 || t.getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType){
            print_error(p_print.m_target->getLocation().line, p_print.m_target->getLocation().col);
            fprintf(stderr, "expression of print statement must be scalar type\n");
            print_error_code(p_print.m_target->getLocation().line, p_print.m_target->getLocation().col);
            global_error = 1;
        }
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    bool e = 0;

    p_bin_op.visitChildNodes(*this);
    std::string op = p_bin_op.getOpCString();

    PType left_node = p_bin_op.get_left_type();
    PType right_node = p_bin_op.get_right_type();

    if(op == "+" || op =="-" || op == "*" || op == "/"){
        if((left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
         || (right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
         || left_node.get_vec_idx().size() > 0 || right_node.get_vec_idx().size() > 0){
            // except string plus
            if(op != "+" || left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType || right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType
            || left_node.get_vec_idx().size() > 0 || right_node.get_vec_idx().size() > 0){
                print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left_node.getPTypeCString(), right_node.getPTypeCString());
                print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                e = 1;
                p_bin_op.set_error();
                PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kStringType));
                p_bin_op.set_type(temp);
            // string plus string
            }else{
                PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kStringType));
                p_bin_op.set_type(temp);
            }
        // real plus real
        }else if(left_node.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType || right_node.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_bin_op.set_type(temp);
        // int plus int
        }else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        }
    }else if(op == "mod"){
        if(left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType || right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
        || left_node.get_vec_idx().size() > 0 || right_node.get_vec_idx().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left_node.getPTypeCString(), right_node.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            e = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        // int mod int
        }else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        }
    }else if(op == "and" || op == "or" || op == "not"){
        if(left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType || right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
        || left_node.get_vec_idx().size() > 0 || right_node.get_vec_idx().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left_node.getPTypeCString(), right_node.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            e = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        // bool and bool
        }else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
    }else if(op == "<" || op == "<=" || op == "=" || op == ">=" || op == ">" || op == "<>"){
        if((left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && left_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
        || (right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && right_node.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
        || left_node.get_vec_idx().size() > 0 || right_node.get_vec_idx().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left_node.getPTypeCString(), right_node.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            e = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        // (int or real) >= (int or real)
        }else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
    }

    if(e){
        global_error = 1;
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    bool e = 0;

    p_un_op.visitChildNodes(*this);
    std::string op = p_un_op.getOpCString();
    PType operand = p_un_op.get_operand_type();
    if(op == "neg"){
        // -int
        if(operand.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType && operand.get_vec_idx().size() == 0){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_un_op.set_type(temp);
        // -real
        }else if(operand.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType && operand.get_vec_idx().size() == 0){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.set_type(temp);
        }else{
            print_error(p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), operand.getPTypeCString());
            print_error_code(p_un_op.getLocation().line, p_un_op.getLocation().col);
            e = 1;
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.set_type(temp);
        }
    }else if(op == "not"){
        // !bool
        if(operand.getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType && operand.get_vec_idx().size() == 0){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.set_type(temp);
        }else{
            print_error(p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), operand.getPTypeCString());
            print_error_code(p_un_op.getLocation().line, p_un_op.getLocation().col);
            e = 1;
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.set_type(temp);
        }
    }

    if(e){
        global_error = 1;
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_func_invocation.visitChildNodes(*this);
    int k = symbol_tables.get_entry_kind(p_func_invocation.getNameCString(), 1); // find in global

    bool e = 0;

    if(k == ENTRY_KIND_NOT_FOUND){
        print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_func_invocation.getNameCString());
        print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        p_func_invocation.set_error();
        e = 1;
        global_error = 1;
    }else if(k != ENTRY_KIND_FUNCTION){
        print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "call of non-function symbol '%s'\n", p_func_invocation.getNameCString());
        print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        p_func_invocation.set_error();
        e = 1;
        global_error = 1;
    }else{
        std::vector<PType> paras = p_func_invocation.get_vec_para_type();
        Function func_entry = functions.find_entry(p_func_invocation.getNameCString());
        // #parameters not match
        if(paras.size() != func_entry.parameters.size()){
            print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            fprintf(stderr, "too few/much arguments provided for function '%s'\n", p_func_invocation.getNameCString());
            print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            p_func_invocation.set_error();
            e = 1;
            global_error = 1;
        // parameters type not match
        }else{
            for(int i = 0; i < paras.size(); ++i){
                if(paras[i].getPTypeString() != func_entry.parameters[i].getPTypeString()){
                    print_error(p_func_invocation.m_args[i]->getLocation().line, p_func_invocation.m_args[i]->getLocation().col);
                    fprintf(stderr, "incompatible type passing '%s' to parameter of type '%s'\n", paras[i].getPTypeCString(), func_entry.parameters[i].getPTypeCString());
                    print_error_code(p_func_invocation.m_args[i]->getLocation().line, p_func_invocation.m_args[i]->getLocation().col);
                    p_func_invocation.set_error();
                    e = 1;
                    global_error = 1;
                    break;
                }
            }
        }

    }

    if(!e && !symbol_tables.get_symbol_entry(p_func_invocation.getNameCString()).error){
        PType* temp = new PType(symbol_tables.get_symbol_entry(p_func_invocation.getNameCString()).type);
        std::vector<uint64_t> new_dim;
        temp->setDimensions(new_dim);
        PTypeSharedPtr tmp = (PTypeSharedPtr)temp;
        p_func_invocation.set_type(tmp);
    }

}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_variable_ref.visitChildNodes(*this);

    bool e = 0;

    if(p_variable_ref.error_already_occur()){
        p_variable_ref.set_error();
        return ;
    }

    int k = symbol_tables.get_entry_kind(p_variable_ref.getNameCString(), 1); // get from global
    if(k == ENTRY_KIND_NOT_FOUND){
        print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_variable_ref.getNameCString());
        print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.set_error();
        e = 1;
        global_error = 1;

    }else if(k == ENTRY_KIND_PROGRAM || k == ENTRY_KIND_FUNCTION){
        print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of non-variable symbol '%s'\n", p_variable_ref.getNameCString());
        print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.set_error();
        e = 1;
        global_error = 1;
    }else if(symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).error){
        p_variable_ref.set_error();
    }

    if(!e && !symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).error){
        //fprintf(stderr, "%s\n", symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).name.c_str());
        
        std::vector<uint64_t> old_dim = symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).type.get_vec_idx();
        std::vector<uint64_t> new_dim;
        
        for(int i = p_variable_ref.get_idx_size(); i < old_dim.size(); i++){
            new_dim.push_back(old_dim[i]);
        }

        PType* temp = new PType(symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).type);
        //fprintf(stderr, "%s\n", temp->getPTypeCString());
        temp->setDimensions(new_dim);
        PTypeSharedPtr tmp = (PTypeSharedPtr)temp;
        //fprintf(stderr, "%s\n", tmp->getPTypeCString());
        p_variable_ref.set_type(tmp);
        
        std::vector<PType::PrimitiveTypeEnum> variable_ref_idx = p_variable_ref.get_vec_idxType();
        for(int i = 0; i < variable_ref_idx.size(); i++){
            if(variable_ref_idx[i] != PType::PrimitiveTypeEnum::kIntegerType){
                print_error(p_variable_ref.getLocation().line, p_variable_ref.m_indices[i]->getLocation().col);
                fprintf(stderr, "index of array reference must be an integer\n");
                print_error_code(p_variable_ref.getLocation().line, p_variable_ref.m_indices[i]->getLocation().col);
                p_variable_ref.set_error();
                e = 1;
                global_error = 1;
            }
        }

        int idx = symbol_tables.get_symbol_entry(p_variable_ref.getNameCString()).type.get_vec_idx().size();
        if(p_variable_ref.get_idx_size() > idx){
            print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            fprintf(stderr, "there is an over array subscript on '%s'\n", p_variable_ref.getNameCString());
            print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            p_variable_ref.set_error();
            e = 1;
            global_error = 1;
        }
    }

}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    bool e = 0;
    
    p_assignment.visitChildNodes(*this);

    PType t_left;
    PType t_right;
    
    // left node
    if(!p_assignment.m_lvalue->get_error()){
        t_left = p_assignment.m_lvalue->get_type();
        int k = symbol_tables.get_entry_kind(p_assignment.m_lvalue->getNameCString(), 1); // get from global
        if(t_left.get_vec_idx().size() != 0){
            print_error(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            print_error_code(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            global_error = 1;
            e = 1;
        }else if(k == ENTRY_KIND_CONSTANT){
            print_error(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "cannot assign to variable '%s' which is a constant\n", p_assignment.m_lvalue->getNameCString());
            print_error_code(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            global_error = 1;
            e = 1;
        }else if(k == ENTRY_KIND_LOOP_VAR && symbol_tables.get_top_table_kind() != TABLE_KIND_FOR){
            print_error(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "the value of loop variable cannot be modified inside the loop body\n");
            print_error_code(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            global_error = 1;
            e = 1;
        }
    }
    if(e){
        return ;
    }

    // right node
    if(!p_assignment.m_expr->get_error()){
        t_right = p_assignment.m_expr->get_type();
        if(t_right.get_vec_idx().size() != 0){
            print_error(p_assignment.m_expr->getLocation().line, p_assignment.m_expr->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            print_error_code(p_assignment.m_expr->getLocation().line, p_assignment.m_expr->getLocation().col);
            global_error = 1;
            e = 1;
        }else{
            // type not match (except real := int)
            if(t_left.getPrimitiveType() != PType::PrimitiveTypeEnum::kVoidType
            && !(t_left.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType && t_right.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType) 
            && (t_left.getPTypeString() != t_right.getPTypeString())){
                print_error(p_assignment.getLocation().line, p_assignment.getLocation().col);
                fprintf(stderr, "assigning to '%s' from incompatible type '%s'\n", t_left.getPTypeCString(), t_right.getPTypeCString());
                print_error_code(p_assignment.getLocation().line, p_assignment.getLocation().col);
                global_error = 1;
                e = 1;
            }
        }
    }

}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_read.visitChildNodes(*this);

    if(!p_read.get_read_error()){
        PType t = p_read.get_read_type();
        if(t.get_vec_idx().size() != 0){
            print_error(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            fprintf(stderr, "variable reference of read statement must be scalar type\n");
            print_error_code(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            global_error = 1;
        }else{
            int k = symbol_tables.get_entry_kind(p_read.m_target->getNameCString(), 1); // find in global
            if(k == ENTRY_KIND_LOOP_VAR || k == ENTRY_KIND_CONSTANT){
                print_error(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
                fprintf(stderr, "variable reference of read statement cannot be a constant or loop variable\n");
                print_error_code(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
                global_error = 1;
            }
        }
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_if.visitChildNodes(*this);

    if(!p_if.m_condition->get_error()){
        PType t = p_if.m_condition->get_type();
        if(t.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType || t.get_vec_idx().size() != 0){
            print_error(p_if.m_condition->getLocation().line, p_if.m_condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            print_error_code(p_if.m_condition->getLocation().line, p_if.m_condition->getLocation().col);
            global_error = 1;
        }
    }
    
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // same as IfNode
    p_while.visitChildNodes(*this);
    if(!p_while.m_condition->get_error()){
        PType t = p_while.m_condition->get_type();
        if(t.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType || t.get_vec_idx().size() != 0){
            print_error(p_while.m_condition->getLocation().line, p_while.m_condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            print_error_code(p_while.m_condition->getLocation().line, p_while.m_condition->getLocation().col);
            global_error = 1;
        }
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    SymbolTable* new_table = new SymbolTable(TABLE_KIND_FOR);
    symbol_tables.pushTable(new_table);
    p_for.visitChildNodes(*this);
    //int start_var = 10;
    int start_var = p_for.initial_val; // fix parser to get the start value
    int end_var = atoi(p_for.m_end_condition->getConstantValueCString());

    if(start_var > end_var){
        print_error(p_for.getLocation().line, p_for.getLocation().col);
        fprintf(stderr, "the lower bound and upper bound of iteration count must be in the incremental order\n");
        print_error_code(p_for.getLocation().line, p_for.getLocation().col);
        global_error = 1;
    }

    symbol_tables.popTable(opt_dmp);
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    
    p_return.visitChildNodes(*this);

    if(functions.func_check == -1){
        print_error(p_return.getLocation().line, p_return.getLocation().col);
        fprintf(stderr, "program/procedure should not return a value\n");
        print_error_code(p_return.getLocation().line, p_return.getLocation().col);
        global_error = 1;
        
    }else{
        Function func = functions.find_curr_entry();
        //fprintf(stderr, "%s\n", func.return_type.getPTypeCString());
        if((p_return.m_ret_val->get_type().getPTypeString() != func.return_type.getPTypeString())){
            //fprintf(stderr, "%s\n", func.name.c_str());
            print_error(p_return.m_ret_val->getLocation().line, p_return.m_ret_val->getLocation().col);
            fprintf(stderr, "return '%s' from a function with return type '%s'\n", p_return.m_ret_val->get_type().getPTypeCString(), func.return_type.getPTypeCString());
            print_error_code(p_return.m_ret_val->getLocation().line, p_return.m_ret_val->getLocation().col);
            global_error = 1;
        }
    }
}
