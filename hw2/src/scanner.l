%{
#include <stdint.h>

#include "parser.h"

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }

#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *buffer_ptr = buffer;

static void concatenateString(const char *yytext_ptr);

%}

isDigit [0-9]
isAlpha [a-zA-Z]
isFloat (0|[1-9][0-9]*)\.(0|[0-9]*[1-9])
%Start COMMENT

%%
\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}
[\t ]  { LIST; }

<INITIAL>"//&S+".* { opt_src = 1; LIST; }
<INITIAL>"//&S-".* { opt_src = 0; LIST; }
<INITIAL>"//&T+".* { opt_tok = 1; LIST; }
<INITIAL>"//&T-".* { opt_tok = 0; LIST; }

<INITIAL>"//".*        { LIST; }
<INITIAL>"/*"          { LIST; BEGIN COMMENT; }
<COMMENT>.    { LIST;                }
<COMMENT>"*/" { LIST; BEGIN 0;       }


<INITIAL>\"([^"]|\"{2})*\" { 
	int len = yyleng;
	char s[len];
	int j = 0;
	for(int i = 1; i < len; ++i){
		if(yytext[i] == '\"' && yytext[i+1] == '\"'){
			i++;
		}
		s[j] = yytext[i];
		j++;
	}
	j--;
	s[j] = '\0';
	TOKEN_STRING(string, s);
	return STRING;
}


<INITIAL>0|([1-9][0-9]*) { TOKEN_STRING(integer, yytext); return INT;}
<INITIAL>0[0-7]+         { TOKEN_STRING(oct_integer, yytext); return OCT_INT;}

<INITIAL>","        { TOKEN_CHAR(','); return ',';}
<INITIAL>";"        { TOKEN_CHAR(';'); return ';';}
<INITIAL>":"        { TOKEN_CHAR(':'); return ':';}
<INITIAL>"("        { TOKEN_CHAR('('); return '(';}
<INITIAL>")"        { TOKEN_CHAR(')'); return ')';}
<INITIAL>"["        { TOKEN_CHAR('['); return '[';}
<INITIAL>"]"        { TOKEN_CHAR(']'); return ']';}
<INITIAL>"+"        { TOKEN(+);        return '+';}
<INITIAL>"-"        { TOKEN(-);        return '-';}
<INITIAL>"*"        { TOKEN(*);        return '*';}
<INITIAL>"/"        { TOKEN(/);        return '/';}
<INITIAL>"mod"      { TOKEN(mod);      return MOD;}
<INITIAL>":="       { TOKEN(:=);       return ASSIGN;}
<INITIAL>"<"        { TOKEN(<);        return LT;}
<INITIAL>"<="       { TOKEN(<=);       return LE;}
<INITIAL>"<>"       { TOKEN(<>);       return NE;}
<INITIAL>">="       { TOKEN(>=);       return GE;}
<INITIAL>">"        { TOKEN(>);        return GT;}
<INITIAL>"="        { TOKEN(=);        return EQ;}
<INITIAL>"and"      { TOKEN(and);      return AND;}
<INITIAL>"or"       { TOKEN(or);       return OR;} 
<INITIAL>"not"      { TOKEN(not);      return NOT;}
<INITIAL>"var"      { TOKEN(KWvar);    return VAR;}
<INITIAL>"def"      { TOKEN(KWdef);    return DEF;}
<INITIAL>"array"    { TOKEN(KWarray);  return ARRAY;}
<INITIAL>"of"       { TOKEN(KWof);     return OF;}
<INITIAL>"boolean"  { TOKEN(KWboolean);return TYPE;}
<INITIAL>"integer"  { TOKEN(KWinteger);return TYPE;}
<INITIAL>"real"     { TOKEN(KWreal);   return TYPE;}
<INITIAL>"string"   { TOKEN(KWstring); return TYPE;}
<INITIAL>"true"     { TOKEN(KWtrue);   return TRUE;}
<INITIAL>"false"    { TOKEN(KWfalse);  return FALSE;}
<INITIAL>"while"    { TOKEN(KWwhile);  return WHILE;}
<INITIAL>"do"       { TOKEN(KWdo);     return DO;}
<INITIAL>"if"       { TOKEN(KWif);     return IF;}
<INITIAL>"then"     { TOKEN(KWthen);   return THEN;}
<INITIAL>"else"     { TOKEN(KWelse);   return ELSE;}
<INITIAL>"for"      { TOKEN(KWfor);    return FOR;}
<INITIAL>"to"       { TOKEN(KWto);     return TO;}
<INITIAL>"begin"    { TOKEN(KWbegin);  return BEG;}
<INITIAL>"end"      { TOKEN(KWend);    return END;}
<INITIAL>"print"    { TOKEN(KWprint);  return PRINT;}
<INITIAL>"read"     { TOKEN(KWread);   return READ;}
<INITIAL>"return"   { TOKEN(KWreturn); return RETURN;}

<INITIAL>{isAlpha}({isAlpha}|{isDigit})*                      { TOKEN_STRING(id, yytext); return ID;}
<INITIAL>({isFloat}|([1-9][0-9]*))[eE][+-]?(0|([1-9][0-9]*))  { TOKEN_STRING(scientific, yytext); return SCIENTIFIC;}
<INITIAL>{isFloat}                                            { TOKEN_STRING(float, yytext); return FLOAT;}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void concatenateString(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *buffer_ptr = *yytext_ptr;
        ++buffer_ptr;
        ++yytext_ptr;
    }
    *buffer_ptr = '\0';
}
