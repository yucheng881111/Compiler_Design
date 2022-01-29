#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

CodeGenerator::CodeGenerator(const std::string source_file_name,
                             const std::string save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    const std::string &real_path =
        (save_path == "") ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    std::string output_file_path(
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
    //m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    m_output_file = fopen(output_file_path.c_str(), "w");
    //assert(m_output_file.get() && "Failed to open output file");
    assert(m_output_file != NULL && "Failed to open output file");
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char*const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ".section    .text\n"
        "    .align 2\n";
    // clang-format on
    dumpInstructions(riscv_assembly_file_prologue, m_source_file_path.c_str());
    
    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_program.getSymbolTable());

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    dumpInstructions("\n.text\n");
    dumpInstructions("  .align  2\n");
    dumpInstructions("  .global main\n");
    dumpInstructions("  .type   main, @function\n");
    dumpInstructions("\nmain:\n");

    function_prologue();
    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    function_epilogue("main");

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    // global
    if(!is_local){
        // global variable
        if(m_symbol_manager_ptr->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kVariableKind){
            dumpInstructions("\n.comm %s, 4, 4\n", p_variable.getNameCString());
        }
        // global constant
        else if(m_symbol_manager_ptr->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kConstantKind){
            dumpInstructions("\n.section    .rodata \n");
            dumpInstructions("    .align 2\n");
            dumpInstructions("    .globl %s\n", p_variable.getNameCString());
            dumpInstructions("    .type %s, @object\n", p_variable.getNameCString());
            dumpInstructions("%s:\n", p_variable.getNameCString());
            dumpInstructions("    .word %s \n", p_variable.getConstantPtr()->getConstantValueCString());
        }
    }
    // local
    else{
        SymbolEntry *decl = (SymbolEntry*)m_symbol_manager_ptr->lookup(p_variable.getName());
        // local variable
        if(decl->getKind() == SymbolEntry::KindEnum::kVariableKind){
            offset -= 4;
            decl->offset = offset;
            dumpInstructions("#Declare local const variable:%s, offset: %d\n", decl->getNameCString(), decl->offset);
        }
        // local constant
        else if(decl->getKind() == SymbolEntry::KindEnum::kConstantKind){
            offset -= 4;
            decl->offset = offset;
            dumpInstructions("#Declare local const variable:%s, offset: %d\n", decl->getNameCString(), decl->offset);
            dumpInstructions("    addi t0, s0, %d\n", offset);
            dumpInstructions("    addi sp, sp, -4\n");
            dumpInstructions("    sw t0, 0(sp)\n");
            dumpInstructions("    li t0, %s\n", p_variable.getConstantPtr()->getConstantValueCString());
            dumpInstructions("    addi sp, sp, -4\n");
            dumpInstructions("    sw t0, 0(sp)\n");
            dumpInstructions("    lw t0, 0(sp)\n");
            dumpInstructions("    addi sp, sp, 4\n");
            dumpInstructions("    lw t1, 0(sp)\n");
            dumpInstructions("    addi sp, sp, 4\n");
            dumpInstructions("    sw t0, 0(t1)\n");
        }
        // parameter
        else if(decl->getKind() == SymbolEntry::KindEnum::kParameterKind){
            offset -= 4;
            decl->offset = offset;
        }
        // loop var
        else if(decl->getKind() == SymbolEntry::KindEnum::kLoopVarKind){
            offset -= 4;
            decl->offset = offset;
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    if(p_constant_value.getTypePtr()->isInteger()){
        dumpInstructions("    li t0, %s\n", p_constant_value.getConstantValueCString());
        dumpInstructions("    addi sp, sp, -4\n");
        dumpInstructions("    sw t0, 0(sp)\n");
    }else if(p_constant_value.getTypePtr()->isBool()){
        if(p_constant_value.getConstantValueCString() == "true"){
            dumpInstructions("    li t0, 1\n");
        }
        else{
            dumpInstructions("    li t0, 0\n");
        }
        dumpInstructions("    addi sp, sp, -4\n");
        dumpInstructions("    sw t0, 0(sp)\n");
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());
    is_local = 1;
    dumpInstructions("\n.text\n");
    dumpInstructions("  .align  2\n");
    dumpInstructions("  .global %s\n", p_function.getNameCString());
    dumpInstructions("  .type   %s, @function\n", p_function.getNameCString());
    dumpInstructions("\n%s:\n", p_function.getNameCString());
    function_prologue();
    dumpInstructions("#Parameter num: %d\n", p_function.getParametersNum_t());

    // visit parameter child node
    p_function.visitChildNodes(*this);

    if(p_function.getParametersNum_t() <= 8){
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *parameter = (SymbolEntry *) m_symbol_manager_ptr->lookup(name);
            dumpInstructions("    sw a%d, %d(s0)\n", i, parameter->offset);
        }
    }else{
        int big_size = 4 * p_function.getParametersNum_t();
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *parameter = (SymbolEntry *) m_symbol_manager_ptr->lookup(name);
            dumpInstructions("    lw t0, %d(s0)\n", big_size - 4);
            dumpInstructions("    sw t0, %d(s0)\n", parameter->offset);
            big_size -= 4;
        }
    }

    p_function.visitBodyChildNodes(*this);
    function_epilogue(p_function.getNameCString());
    is_local = 0;

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_compound_statement.getSymbolTable());

    is_local = 1;

    p_compound_statement.visitChildNodes(*this);

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(
        p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    dumpInstructions("#Print start\n");

    p_print.visitChildNodes(*this);
    
    dumpInstructions("    lw a0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    jal ra, printInt\n");
    dumpInstructions("#Print end\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    std::string op = p_bin_op.getOpCString();
    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    lw t1, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");

    if(op == "+"){
        dumpInstructions("    add t0, t1, t0 \n");
    }else if(op == "-"){
        dumpInstructions("    sub t0, t1, t0 \n");
    }else if(op == "*"){
        dumpInstructions("    mul t0, t1, t0 \n");
    }else if(op == "/"){
        dumpInstructions("    div t0, t1, t0 \n");
    }else if(op == "mod"){
        dumpInstructions("    rem t0, t1, t0 \n");
    }else if(op == "<"){
        int out = label++;
        int True = label++;
        dumpInstructions("    blt t1, t0, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == "<="){
        int out = label++;
        int True = label++;
        dumpInstructions("    ble t1, t0, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == "="){
        int out = label++;
        int True = label++;
        dumpInstructions("    beq t0, t1, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == ">="){
        int out = label++;
        int True = label++;
        dumpInstructions("    bge t1, t0, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == ">"){
        int out = label++;
        int True = label++;
        dumpInstructions("    bgt t1, t0, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == "<>"){
        int out = label++;
        int True = label++;
        dumpInstructions("    bne t0, t1, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("L%d:\n", out);
    }else if(op == "and"){
        int out = label++;
        int False = label++;
        dumpInstructions("    beqz t1, L%d\n", False);
        dumpInstructions("    beqz t0, L%d\n", False);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        dumpInstructions("    j L%d\n", out);
        //label_false:
        dumpInstructions("L%d:\n", False);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        //label_out:
        dumpInstructions("L%d:\n", out);
    }
    else if(op == "or"){
        int out = label++;
        int True = label++;
        dumpInstructions("    bneqz t1, L%d\n", True);
        dumpInstructions("    bneqz t0, L%d\n", True);
        dumpInstructions("    li t0, 0\n"); // Bonus: treat false as 0
        dumpInstructions("    j L%d\n", out);
        //label_true:
        dumpInstructions("L%d:\n", True);
        dumpInstructions("    li t0, 1\n"); // Bonus: treat true as 1
        //label_out:
        dumpInstructions("L%d:\n", out);
    }
    dumpInstructions("    addi sp, sp, -4\n");
    dumpInstructions("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string op = p_un_op.getOpCString();
    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    if(op == "neg"){
        dumpInstructions("    li t1, 0\n");
        dumpInstructions("    sub t0, t1, t0 \n"); // 0 - t0 = -t0
    }
    else if(op == "not"){
        dumpInstructions("    li t1, 1\n");
        dumpInstructions("    sub t0, t1, t0 \n");
        /* 
        Bonus:
        true -> 1, false -> 0
        1 - 0 = 1; 0->1
        1 - 1 = 0; 1->0
        */
    }
    dumpInstructions("    addi sp, sp, -4\n");
    dumpInstructions("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    dumpInstructions("#Function call start\n");
    p_func_invocation.visitChildNodes(*this);
    int arg_num = p_func_invocation.get_args_size();
    int big_size = 0;

    if(arg_num <= 8){
        for(int i = arg_num - 1; i >= 0; i--){
            dumpInstructions("    lw a%d, 0(sp)\n", i);
            dumpInstructions("    addi sp, sp, 4\n");
        }
    }else{
        big_size = arg_num * 4;
    }

    dumpInstructions("    jal ra, %s\n", p_func_invocation.getNameCString());

    if(big_size > 0) {
        dumpInstructions("    addi sp, sp, %d\n", big_size);
    }

    dumpInstructions("    mv t0, a0\n");
    dumpInstructions("    addi sp, sp, -4\n");
    dumpInstructions("    sw t0, 0(sp)\n");
    dumpInstructions("#Function call end\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);

    SymbolEntry* var = (SymbolEntry*)m_symbol_manager_ptr->lookup(p_variable_ref.getName());
    dumpInstructions("#Variable reference:%s, offset: %d\n", var->getNameCString(), var->offset);
    if(is_read){
        // global
        if(var->getLevel() == 0){
            dumpInstructions("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstructions("    addi sp, sp, -4\n");
            dumpInstructions("    sw t0, 0(sp)\n");
        }
        // local
        else{
            dumpInstructions("    addi t0, s0, %d\n", var->offset);
            dumpInstructions("    addi sp, sp, -4\n", p_variable_ref.getNameCString());
            dumpInstructions("    sw t0, 0(sp)\n");
        }
    }else{
        // global
        if(var->getLevel() == 0){
            dumpInstructions("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstructions("    lw t1, 0(t0)\n");
            dumpInstructions("    mv t0, t1\n");
            dumpInstructions("    addi sp, sp, -4\n");
            dumpInstructions("    sw t0, 0(sp)\n");
        }
        // local
        else{
            dumpInstructions("    lw t0, %d(s0)\n", var->offset);
            dumpInstructions("    addi sp, sp, -4 \n");
            dumpInstructions("    sw t0, 0(sp)\n");
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    dumpInstructions("#Assignment start\n");
    is_read = 1;
    p_assignment.m_lvalue->accept(*this);
    is_read = 0;
    p_assignment.m_expr->accept(*this);

    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    lw t1, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    sw t0, 0(t1)\n");
    dumpInstructions("#Assignment end\n");
}

void CodeGenerator::visit(ReadNode &p_read) {
    dumpInstructions("#Read start\n");
    is_read = 1;
    p_read.visitChildNodes(*this);
    is_read = 0;
    dumpInstructions("    jal ra, readInt\n");
    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    sw a0, 0(t0)\n");
    dumpInstructions("#Read end\n");
}

void CodeGenerator::visit(IfNode &p_if) {
    int If = label++;
    int Else = label++;
    int out = label++;

    p_if.m_condition->accept(*this);

    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    beqz t0, L%d\n", Else);

    dumpInstructions("#Label for if\n");
    dumpInstructions("L%d:\n", If);

    p_if.m_body->accept(*this);
    dumpInstructions("    j L%d\n", out);

    dumpInstructions("#Label for else\n");
    dumpInstructions("L%d:\n", Else);
    if (p_if.m_else_body) {
        p_if.m_else_body->accept(*this);
    }
    dumpInstructions("#Label for out\n");
    dumpInstructions("L%d:\n", out);
}

void CodeGenerator::visit(WhileNode &p_while) {
    int loop = label++;
    int out = label++;

    dumpInstructions("L%d:\n", loop);

    p_while.m_condition->accept(*this);

    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    beqz t0, L%d\n", out);

    p_while.m_body->accept(*this);
    
    dumpInstructions("    j L%d\n", loop);

    dumpInstructions("L%d:\n", out);
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_for.getSymbolTable());

    //p_for.visitChildNodes(*this);
    SymbolEntry* loop_var = (SymbolEntry*)m_symbol_manager_ptr->lookup(p_for.m_loop_var_decl->get_for_var_Cstr());
    dumpInstructions("#For start\n");
    dumpInstructions("#For_var name: %s\n", loop_var->getNameCString());

    int loop = label++;
    int out = label++;

    p_for.m_loop_var_decl->accept(*this);
    p_for.m_init_stmt->accept(*this);

    dumpInstructions("L%d:\n", loop);

    p_for.m_end_condition->accept(*this);

    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");

    dumpInstructions("    lw t1, %d(s0)\n", loop_var->offset);
    dumpInstructions("    bge t1, t0, L%d\n", out);

    p_for.m_body->accept(*this);

    dumpInstructions("    lw t1, %d(s0)\n", loop_var->offset);
    dumpInstructions("    addi t1, t1, 1\n");
    dumpInstructions("    sw t1, %d(s0)\n", loop_var->offset);
    dumpInstructions("    j L%d\n", loop);

    dumpInstructions("#For end\n");
    dumpInstructions("L%d:\n", out);

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);

    dumpInstructions("#Return start\n");
    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    mv a0, t0\n");
    dumpInstructions("#Return end\n");
}
