%{
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token ID
%token STRING INT OCT_INT SCIENTIFIC FLOAT
%token MOD ASSIGN LT LE NE GE GT EQ AND OR NOT VAR DEF ARRAY OF TYPE
%token TRUE FALSE
%token WHILE DO IF THEN ELSE FOR TO BEG END PRINT READ RETURN

%left AND OR NOT 
%left LT LE NE GE GT EQ
%left '+' '-' 
%left '*' '/' MOD

%%

ProgramName: ID ';' program END
	   ;

program: variables functions compounds
       ;

functions: function
	 |
	 ;

function: function function_declaration
        | function_declaration
        | function function_definition
	| function_definition
	;

function_declaration: ID '(' arguments ')' ':' TYPE ';'
		    | ID '(' arguments ')' ';'
		    ;

function_definition: ID '(' arguments ')' ':' TYPE compounds END
                   | ID '(' arguments ')' compounds END
                   ;

arguments: identifier_list ':' TYPE ';' arguments
	 | identifier_list ':' TYPE
	 | 
	 ;

identifier_list: identifier_list ',' ID
	       | ID
	       ;

variables: variable1
	 |
	 ;

variable1: variable1 variable
	 | variable
	 ;

variable: VAR identifier_list ':' TYPE ';'
	| VAR identifier_list ':' array_type ';'
	| VAR identifier_list ':' literal_constant ';'
        ;

array_type: ARRAY INT OF array_type
	  | TYPE
	  ;

literal_constant: number
                | STRING
                | TRUE
                | FALSE
                ;

number: INT
      | OCT_INT
      | SCIENTIFIC
      | FLOAT
      ;

statements: statement1
	  |
	  ;

statement1: statement1 statement
	  | statement
          ;

statement: compounds
         | simple
         | conditional
         | while
         | for
         | return
         | function_call ';'
         ;

compounds: BEG variables statements END
	 ;

simple: variable_reference ASSIGN expression ';'
      | PRINT expression ';'
      | READ variable_reference ';'
      ;

variable_reference: array_reference
                  | ID
                  ;

array_reference: ID '[' expressions ']'
	       | ID '[' ']'
	       ;

conditional: IF expression THEN statements ELSE statements END IF
	   | IF expression THEN statements END IF

while: WHILE expression DO statements END DO
     ;

for: FOR ID ASSIGN INT TO INT DO statements END DO
   ;

return: RETURN expression ';'
      ;

function_call: ID '(' expressions ')'
	     | ID '(' ')'
	     ;

expressions: expressions ',' expression
           | expression
           ;

expression: '-' expression %prec '*'
	  | expression '*' expression
          | expression '/' expression
          | expression MOD expression
	  | expression '+' expression
          | expression '-' expression
          | expression LT expression
          | expression LE expression
          | expression NE expression
          | expression GE expression
          | expression GT expression
          | expression EQ expression
          | expression AND expression
          | expression OR  expression
          | NOT expression
          | '(' expression ')' %prec '*'
          | ID
          | function_call
          | array_reference
          | literal_constant
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
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
