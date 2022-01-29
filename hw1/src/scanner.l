%{
#include <stdint.h>

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }

#define MAX_LINE_LENG       512

uint32_t line_num = 1;
static char buffer[MAX_LINE_LENG];

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
}


<INITIAL>0|([1-9][0-9]*) { TOKEN_STRING(integer, yytext); }
<INITIAL>0[0-7]+         { TOKEN_STRING(oct_integer, yytext); }

<INITIAL>","        { TOKEN_CHAR(','); }
<INITIAL>";"        { TOKEN_CHAR(';'); }
<INITIAL>":"        { TOKEN_CHAR(':'); }
<INITIAL>"("        { TOKEN_CHAR('('); }
<INITIAL>")"        { TOKEN_CHAR(')'); }
<INITIAL>"["        { TOKEN_CHAR('['); }
<INITIAL>"]"        { TOKEN_CHAR(']'); }
<INITIAL>"+"        { TOKEN(+);        }
<INITIAL>"-"        { TOKEN(-);        }
<INITIAL>"*"        { TOKEN(*);        }
<INITIAL>"/"        { TOKEN(/);        }
<INITIAL>"mod"      { TOKEN(mod);      }
<INITIAL>":="       { TOKEN(:=);       }
<INITIAL>"<"        { TOKEN(<);        }
<INITIAL>"<="       { TOKEN(<=);       }
<INITIAL>"<>"       { TOKEN(<>);       }
<INITIAL>">="       { TOKEN(>=);       }
<INITIAL>">"        { TOKEN(>);        }
<INITIAL>"="        { TOKEN(=);        }
<INITIAL>"and"      { TOKEN(and);      }
<INITIAL>"or"       { TOKEN(or);       } 
<INITIAL>"not"      { TOKEN(not);      }
<INITIAL>"var"      { TOKEN(KWvar);    }
<INITIAL>"def"      { TOKEN(KWdef);    }
<INITIAL>"array"    { TOKEN(KWarray);  }
<INITIAL>"of"       { TOKEN(KWof);     }
<INITIAL>"boolean"  { TOKEN(KWboolean);}
<INITIAL>"integer"  { TOKEN(KWinteger);}
<INITIAL>"real"     { TOKEN(KWreal);   }
<INITIAL>"string"   { TOKEN(KWstring); }
<INITIAL>"true"     { TOKEN(KWtrue);   }
<INITIAL>"false"    { TOKEN(KWfalse);  }
<INITIAL>"while"    { TOKEN(KWwhile);  }
<INITIAL>"do"       { TOKEN(KWdo);     }
<INITIAL>"if"       { TOKEN(KWif);     }
<INITIAL>"then"     { TOKEN(KWthen);   }
<INITIAL>"else"     { TOKEN(KWelse);   }
<INITIAL>"for"      { TOKEN(KWfor);    }
<INITIAL>"to"       { TOKEN(KWto);     }
<INITIAL>"begin"    { TOKEN(KWbegin);  }
<INITIAL>"end"      { TOKEN(KWend);    }
<INITIAL>"print"    { TOKEN(KWprint);  }
<INITIAL>"read"     { TOKEN(KWread);   }
<INITIAL>"return"   { TOKEN(KWreturn); }

<INITIAL>{isAlpha}({isAlpha}|{isDigit})*                      { TOKEN_STRING(id, yytext); }
<INITIAL>({isFloat}|([1-9][0-9]*))[eE][+-]?(0|([1-9][0-9]*))  { TOKEN_STRING(scientific, yytext); }
<INITIAL>{isFloat}                                            { TOKEN_STRING(float, yytext); }

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

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}
