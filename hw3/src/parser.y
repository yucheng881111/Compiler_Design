%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"

#include "AST/AstDumper.hpp"

#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <bits/stdc++.h>

using namespace std;

extern "C" int yylex();
extern "C" int yyparse();

#define YYLTYPE yyltype



typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

//static AstNode *root;
static ProgramNode *root;
static AstDumper AstDumper_root;

//extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

%}

%code requires {
    #include <vector>
    using namespace std;

    class AstNode;
    class AssignmentNode;
    class BinaryOperatorNode;
    class CompoundStatementNode;
    class ConstantValueNode;
    class DeclNode;
    class ExpressionNode;
    class ForNode;
    class FunctionNode;
    class FunctionInvocationNode;
    class IfNode;
    class PrintNode;
    class ProgramNode;
    class ReadNode;
    class ReturnNode;
    class UnaryOperatorNode;
    class VariableNode;
    class VariableReferenceNode;
    class WhileNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;

    AstNode *node;
    int int_val;
    double double_val;
    char* str;

    AssignmentNode* ass_node;
    BinaryOperatorNode* bin_node;
    CompoundStatementNode* compound_node;
    ConstantValueNode* constval_node;
    DeclNode* decl_node;
    ExpressionNode* exp_node;
    ForNode* for_node;
    FunctionNode* func_node;
    FunctionInvocationNode* func_inv_node;
    IfNode* if_node;
    PrintNode* print_node;
    ProgramNode* program_node;
    ReadNode* read_node;
    ReturnNode* return_node;
    UnaryOperatorNode* un_node;
    VariableNode* var_node;
    VariableReferenceNode* var_ref_node;
    WhileNode* while_node;
   
    vector<char*>* id_list; 
    vector<DeclNode*>* decl_list;
    vector<FunctionNode*>* func_list;
    vector<VariableNode*>* var_list;
    vector<CompoundStatementNode*>* stat_list;
    vector<ExpressionNode*>* exp_list;
}

%locations

%type <identifier> ProgramName FunctionName
%type <program_node> Program
%type <compound_node> CompoundStatement Statement Simple ElseOrNot
%type <decl_node> Declaration FormalArg
%type <decl_list> DeclarationList Declarations FormalArgList FormalArgs
%type <func_node> FunctionDeclaration FunctionDefinition Function
%type <func_list> FunctionList Functions
%type <id_list> ArrDecl
%type <var_list> IdList
%type <stat_list> StatementList Statements
%type <while_node> While
%type <for_node> For
%type <return_node> Return
%type <func_inv_node> FunctionInvocation FunctionCall
%type <exp_node> Expression
%type <exp_list> ArrRefList ArrRefs ExpressionList Expressions
%type <var_ref_node> VariableReference
%type <read_node> READ
%type <if_node> Condition
%type <print_node> PRINT
%type <constval_node> LiteralConstant
%type <str> ArrType ScalarType Type ReturnType StringAndBoolean IntegerAndReal 
%type <un_node> NegOrNot

    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY 
%token <str> BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token <str> FALSE TRUE
%token PRINT READ

    /* Identifier */
%token <identifier> ID

    /* Literal */
%token <str> INT_LITERAL
%token <str> REAL_LITERAL
%token <str> STRING_LITERAL

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
	
        $$ = new ProgramNode(@1.first_line, @1.first_column,
                               $1, $5, $3, $4);
	root = $$;
        //free($1);
    }
;

ProgramName:
    ID { $$ = $1; }
;

DeclarationList:
    Epsilon { $$ = NULL; }
    |
    Declarations { $$ = $1; }
;

Declarations:
    Declaration { $$ = new vector<DeclNode*>(); $$->push_back($1); }
    |
    Declarations Declaration { $1->push_back($2); $$ = $1; }
;

FunctionList:
    Epsilon { $$ = NULL; }
    |
    Functions { $$ = $1; }
;

Functions:
    Function { $$ = new vector<FunctionNode*>(); $$->push_back($1); }
    |
    Functions Function { $1->push_back($2); $$ = $1; }
;

Function:
    FunctionDeclaration { $$ = $1; }
    |
    FunctionDefinition { $$ = $1; }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON { $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, NULL, $3); }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END { $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $5, $6, $3); }
;

FunctionName:
    ID { $$ = $1; }
;

FormalArgList:
    Epsilon { $$ = NULL; }
    |
    FormalArgs { $$ = $1; }
;

FormalArgs:
    FormalArg { $$ = new vector<DeclNode*>(); $$->push_back($1); }
    |
    FormalArgs SEMICOLON FormalArg { $1->push_back($3); $$ = $1; }
;

FormalArg:
    IdList COLON Type { 
	
	$$ = new DeclNode(@1.first_line, @1.first_column, $1, $3, NULL); }
;

IdList:
    ID { VariableNode* tmp_node = new VariableNode(@1.first_line, @1.first_column, $1, "");
	 $$ = new vector<VariableNode*>(); $$->push_back(tmp_node); }
    |
    IdList COMMA ID { VariableNode* tmp_node = new VariableNode(@3.first_line, @3.first_column, $3, "");
		      $1->push_back(tmp_node); 
		      $$ = $1; }
;

ReturnType:
    COLON ScalarType { $$ = $2; }
    |
    Epsilon { $$ = strdup("void"); }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON { $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4, NULL); }
    |
    VAR IdList COLON LiteralConstant SEMICOLON { $$ = new DeclNode(@1.first_line, @1.first_column, $2, "", $4); }
;

Type:
    ScalarType { $$ = $1; }
    |
    ArrType { $$ = $1; }
;

ScalarType:
    INTEGER { $$ = $1; }
    |
    REAL { $$ = $1; }
    |
    STRING { $$ = $1; }
    |
    BOOLEAN { $$ = $1; }
;

ArrType:
    ArrDecl ScalarType {
	
	string s = $2;
	s += " ";
	for(auto v : *($1)){
	    string t = v;
	    s += ("[" + t + "]");
	}
	
	$$ = (char*)s.c_str(); }
;

ArrDecl:
    ARRAY INT_LITERAL OF { $$ = new vector<char*>(); $$->push_back($2); }
    |
    ArrDecl ARRAY INT_LITERAL OF { $1->push_back($3); $$ = $1; }
;

LiteralConstant:
    NegOrNot INT_LITERAL { 	if($1 == NULL){
					$$ = new ConstantValueNode(@2.first_line, @2.first_column, $2, $1, strdup("int"));
				}else{
					$$ = new ConstantValueNode(@1.first_line, @1.first_column, $2, $1, strdup("int"));
				}
			 }
    |
    NegOrNot REAL_LITERAL { 
				if($1 == NULL){
                                        $$ = new ConstantValueNode(@2.first_line, @2.first_column, $2, $1, strdup("real"));
                                }else{
                                        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $2, $1, strdup("real"));
                                } 
			  }
    |
    StringAndBoolean { $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1, NULL, strdup("sb")); }
;

NegOrNot:
    Epsilon { $$ = NULL; }
    |
    MINUS %prec UNARY_MINUS { $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, strdup("neg"), NULL); }
;

StringAndBoolean:
    STRING_LITERAL { $$ = $1; }
    |
    TRUE { $$ = $1; }
    |
    FALSE { $$ = $1; }
;

IntegerAndReal:
    INT_LITERAL { $$ = $1; }
    |
    REAL_LITERAL { $$ = $1; }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement { $$ = $1; }
    |
    Simple { $$ = $1; }
    |
    Condition { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $1); }
    |
    While { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $1); }
    |
    For { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $1); }
    |
    Return { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $1); }
    |
    FunctionCall { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $1); }
;

StatementList:
    Epsilon { $$ = NULL; }
    |
    Statements { $$ = $1; }
;

Statements:
    Statement { $$ = new vector<CompoundStatementNode*>(); $$->push_back($1); }
    |
    Statements Statement { $1->push_back($2); $$ = $1; }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3); }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON { 
	AssignmentNode* tmp = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
	$$ = new CompoundStatementNode(@2.first_line, @2.first_column, tmp); 
	}
    |
    PRINT Expression SEMICOLON { 
	PrintNode* tmp = new PrintNode(@1.first_line, @1.first_column, $2);
	$$ = new CompoundStatementNode(@1.first_line, @1.first_column, tmp); 
	}
    |
    READ VariableReference SEMICOLON { 
	ReadNode* tmp = new ReadNode(@1.first_line, @1.first_column, $2);
	$$ = new CompoundStatementNode(@1.first_line, @1.first_column, tmp);
	}
;

VariableReference:
    ID ArrRefList { $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2); }
;

ArrRefList:
    Epsilon { $$ = NULL; }
    |
    ArrRefs { $$ = $1; }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET { $$ = new vector<ExpressionNode*>(); $$->push_back($2); }
    |
    ArrRefs L_BRACKET Expression R_BRACKET { $1->push_back($3); $$ = $1; }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot 
    END IF { $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5); }
;

ElseOrNot:
    ELSE
    CompoundStatement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO { $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4); }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
	ConstantValueNode* con_tmp1 = new ConstantValueNode(@4.first_line, @4.first_column, $4, NULL, strdup("int"));
	ConstantValueNode* con_tmp2 = new ConstantValueNode(@6.first_line, @6.first_column, $6, NULL, strdup("int"));
	VariableNode* var_tmp = new VariableNode(@2.first_line, @2.first_column, $2, strdup("integer"));
	DeclNode* decl_tmp = new DeclNode(@2.first_line, @2.first_column, var_tmp);
	VariableReferenceNode* varref_tmp = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
	AssignmentNode* ass_tmp = new AssignmentNode(@3.first_line, @3.first_column, varref_tmp, con_tmp1);
	$$ = new ForNode(@1.first_line, @1.first_column, decl_tmp, ass_tmp, con_tmp2, $8); }
;

Return:
    RETURN Expression SEMICOLON { $$ = new ReturnNode(@1.first_line, @1.first_column, $2); }
;

FunctionCall:
    FunctionInvocation SEMICOLON { $$ = $1; }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS { $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3); }
;

ExpressionList:
    Epsilon { $$ = NULL; }
    |
    Expressions { $$ = $1; }
;

Expressions:
    Expression { $$ = new vector<ExpressionNode*>(); $$->push_back($1); }
    |
    Expressions COMMA Expression { $1->push_back($3); $$ = $1; }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS { $$ = $2; }
    |
    MINUS Expression %prec UNARY_MINUS { UnaryOperatorNode* tmp = new UnaryOperatorNode(@1.first_line, @1.first_column, strdup("neg"), $2); 
					$$ = new ExpressionNode(@1.first_line, @1.first_column, tmp);}
    |
    Expression MULTIPLY Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("*"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression DIVIDE Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("/"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression MOD Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("mod"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression PLUS Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("+"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression MINUS Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("-"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression LESS Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("<"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression LESS_OR_EQUAL Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("<="), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression GREATER Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup(">"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression GREATER_OR_EQUAL Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup(">="), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression EQUAL Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("="), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression NOT_EQUAL Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("<>"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    NOT Expression { UnaryOperatorNode* tmp = new UnaryOperatorNode(@1.first_line, @1.first_column, strdup("not"), $2); 
					$$ = new ExpressionNode(@1.first_line, @1.first_column, tmp);}
    |
    Expression AND Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("and"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    Expression OR Expression { BinaryOperatorNode* tmp = new BinaryOperatorNode(@2.first_line, @2.first_column, strdup("or"), $1, $3); 
					$$ = new ExpressionNode(@2.first_line, @2.first_column, tmp);}
    |
    IntegerAndReal { ConstantValueNode* tmp = new ConstantValueNode(@1.first_line, @1.first_column, $1, NULL, strdup("")); 
					$$ = new ExpressionNode(@1.first_line, @1.first_column, tmp);}
    |
    StringAndBoolean { ConstantValueNode* tmp = new ConstantValueNode(@1.first_line, @1.first_column, $1, NULL, strdup("")); 
					$$ = new ExpressionNode(@1.first_line, @1.first_column, tmp);}
    |
    VariableReference { $$ = new ExpressionNode(@1.first_line, @1.first_column, $1); }
    |
    FunctionInvocation { $$ = new ExpressionNode(@1.first_line, @1.first_column, $1); }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        root->accept(AstDumper_root);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
