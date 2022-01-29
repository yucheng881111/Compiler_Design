#ifndef CODEGEN_CODE_GENERATOR_H
#define CODEGEN_CODE_GENERATOR_H

#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <bits/stdc++.h>

class CodeGenerator final : public AstNodeVisitor {
  private:
    const SymbolManager *m_symbol_manager_ptr;
    std::string m_source_file_path;
    FILE* m_output_file;

  public:
    int offset = 0;
    bool is_local = 0;
    int label = 0;
    bool is_read = 0;
    ~CodeGenerator() = default;
    CodeGenerator(const std::string source_file_name,
                  const std::string save_path,
                  const SymbolManager *const p_symbol_manager);

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    void dumpInstructions(const char *format, ...){
      va_list args;
      va_start(args, format);
      vfprintf(m_output_file, format, args);
      va_end(args);
    }

    void function_prologue(){
      offset = 0;
      offset -= 8;
      dumpInstructions("    addi sp, sp, -128\n");   //move stack pointer to lower address to allocate a new stack
      dumpInstructions("    sw ra, 124(sp)\n");      //save return address of the caller function in the current stack
      dumpInstructions("    sw s0, 120(sp)\n");      //save frame pointer of the last stack in the current stack
      dumpInstructions("    addi s0, sp, 128\n");    //move frame pointer to the bottom of the current stack
    }

    void function_epilogue(const char* name){
      dumpInstructions("    lw ra, 124(sp)\n");     //load return address saved in the current stack
      dumpInstructions("    lw s0, 120(sp)\n");     //move frame pointer back to the bottom of the last stack
      dumpInstructions("    addi sp, sp, 128\n");   //move stack pointer back to the top of the last stack
      dumpInstructions("    jr ra\n");              //jump back to the caller function
      dumpInstructions("    .size %s, .-%s\n", name, name);
      offset += 8;
    }
    
};

#endif
