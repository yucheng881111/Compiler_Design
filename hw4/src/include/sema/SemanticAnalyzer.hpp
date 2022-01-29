#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include <vector>
#include <string>
#include <stdlib.h>
#include "AST/PType.hpp"

#define ENTRY_KIND_NOT_FOUND 0
#define ENTRY_KIND_PROGRAM 1
#define ENTRY_KIND_FUNCTION 2
#define ENTRY_KIND_PARAMETER 3
#define ENTRY_KIND_VARIABLE 4
#define ENTRY_KIND_LOOP_VAR 5
#define ENTRY_KIND_CONSTANT 6

#define TABLE_KIND_PROGRAM 1
#define TABLE_KIND_FUNCTION 2
#define TABLE_KIND_FOR 3
#define TABLE_KIND_COM 4

extern char code[512][512];

class SymbolEntry{
public:
    //SymbolEntry(std::string n, int k, int l, PType t, std::string a) : name(n), kind(k), level(l), type(t), attribute(a){}
    SymbolEntry(std::string n, int k, int l, PType t, std::string a, bool e) : name(n), kind(k), level(l), type(t), attribute(a), error(e){}
    bool error = 0;
public:
    std::string name;
    int kind;
    int level;
    PType type;
    std::string attribute;
};

class SymbolTable{
public:
    SymbolTable(int k) : kind(k){}
    void addSymbolEntry(std::string n, int k, int l, PType t, std::string a, bool e){
      SymbolEntry E(n, k, l, t, a, e);
      entries.push_back(E);
    }

    int get_entry_kind(std::string name){
      for(int i = 0; i < entries.size(); i++){
        if(name == entries[i].name){
          return entries[i].kind;
        }
      }
      return ENTRY_KIND_NOT_FOUND;
    }

    int get_table_kind(){
      return kind; //1:program, 2:function, 3:forloop, 0:others
    }

    SymbolEntry get_symbol_entry(std::string name){
      for(int i = 0; i < entries.size(); i++){
        if(name == entries[i].name){
          return entries[i];
        }
      }
    }

    void change_state(int n){
      kind = n;
    }

    void print_table(){
      const char* kind_str[]= {"", "program", "function", "parameter", "variable", "loop_var", "constant"};
      dumpDemarcation('=');
      printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
      dumpDemarcation('-');
      for(int i = 0; i < entries.size(); i++){
        printf("%-33s", entries[i].name.c_str());
        printf("%-11s", kind_str[entries[i].kind]);
        printf("%d", entries[i].level);
        if(entries[i].level == 0){
          printf("%-10s", "(global)");
        }
        else{
          printf("%-10s", "(local)");
        }
          
        printf("%-17s", entries[i].type.getPTypeCString());
        printf("%-11s", entries[i].attribute.c_str());
        puts("");
      }
      dumpDemarcation('-');
    }

    void dumpDemarcation(const char chr){
      for(size_t i = 0; i < 110; ++i){
        printf("%c", chr);
      }
      puts("");
    }

private:
    std::vector<SymbolEntry> entries;
    int kind; //1:program, 2:function, 3:forloop, 0:others
    //int dummy;
};

class SymbolTables{
public:
    void addSymbol(std::string n, int k, PType t, std::string a, bool e){
      tables.back()->addSymbolEntry(n, k, tables.size()-1, t, a, e);
    }

    int get_entry_kind(std::string name, bool is_global){
      if(is_global){
        for(int i = tables.size() - 1; i >= 0; i--){
          if(tables[i]->get_entry_kind(name) != ENTRY_KIND_NOT_FOUND){
            return tables[i]->get_entry_kind(name);
          }
        }
      }else{
        return tables.back()->get_entry_kind(name);
      }
      return ENTRY_KIND_NOT_FOUND;
    }

    bool get_for_entry_kind(std::string name){
      for(int i = tables.size() - 1; i >= 0; i--){
        if(tables[i]->get_table_kind() == TABLE_KIND_FOR && tables[i]->get_entry_kind(name) != ENTRY_KIND_NOT_FOUND){
          return true;
        }
      }
      return false;
    }

    int get_top_table_kind(){
      return tables.back()->get_table_kind();
    }

    SymbolEntry get_symbol_entry(std::string name){
      for(int i = tables.size() - 1; i >= 0; i--){
        if(tables[i]->get_entry_kind(name) != ENTRY_KIND_NOT_FOUND){
          return tables[i]->get_symbol_entry(name);
        }
      }
    }

    void change_state(int n){
      tables.back()->change_state(n);
    }

    void pushTable(SymbolTable *new_table){
      tables.push_back(new_table);
    }

    void popTable(bool is_print){
      SymbolTable* last_table = tables.back();
      if(is_print){
        last_table->print_table();
      }      
      tables.pop_back();
      free(last_table);
    }

private:
    std::vector<SymbolTable*> tables;
};

class Function{
public:
  std::string name;
  std::vector<PType> parameters;
  PType return_type;
  Function(std::string n, std::vector<PType> p, PType r) : name(n), parameters(p), return_type(r) {}

};

class Functions{
public:
  std::vector<Function> vec_func;
  int func_check = -1;
  void addFunctionEntry(std::string n, std::vector<PType> p, PType r){
    Function f(n, p, r);
    vec_func.push_back(f);
  }

  Function find_entry(std::string n){
    for(int i = 0; i < vec_func.size(); ++i){
      if(n == vec_func[i].name){
        return vec_func[i];
      }
    }
  }

  Function find_curr_entry(){
    return vec_func.back();
  }

};

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    SymbolTables symbol_tables;
    Functions functions;

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

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

    void print_error(const uint32_t line, const uint32_t col){
      fprintf(stderr, "<Error> Found in line %d, column %d: ", line, col);
    }

    void print_error_code(const uint32_t line, const uint32_t col){
      fprintf(stderr, "    %s\n    ", code[line]);
      for(int i = 0; i < col - 1; i++){
        fprintf(stderr, " ");
      }
      fprintf(stderr, "^\n");
    }
};

#endif
