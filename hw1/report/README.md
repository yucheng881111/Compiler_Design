# hw1 report

|Field|Value|
|-:|:-|
|Name|陳昱丞|
|ID|0716206|

## How much time did you spend on this project

5 hours.

## Project overview

第一部分。我先在transition rules外面宣告數字、字元、浮點數及COMMENT狀態，以利後續使用。要注意的地方是float的整數部分多個0開頭是不合法的、小數部分多個0結尾也是不合法的，而0.0是合法的。
```
isDigit [0-9]
isAlpha [a-zA-Z]
isFloat (0|[1-9][0-9]*)\.(0|[0-9]*[1-9])
%Start COMMENT
```
接著進入transition rules部分。\\n為助教提供，\\t及空白格皆省略。這個部分不論是COMMENT或INITIAL狀態都須執行，因此不用在前面加<>為首的condition。而後面的部分皆需要。
```
\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}
[\t ]  { LIST; }
```
match opt狀態。state在INITIAL的時候才能parse。
```
<INITIAL>"//&S+".* { opt_src = 1; LIST; }
<INITIAL>"//&S-".* { opt_src = 0; LIST; }
<INITIAL>"//&T+".* { opt_tok = 1; LIST; }
<INITIAL>"//&T-".* { opt_tok = 0; LIST; }
```
parse註解。注意INITIAL及COMMENT間的轉換即可。
```
<INITIAL>"//".*  { LIST; }
<INITIAL>"/*"    { LIST; BEGIN COMMENT; }
<COMMENT>.       { LIST;                }
<COMMENT>"*/"    { LIST; BEGIN 0;       }
```
處理以\"\"括起來之字串。必須處理中間有偶數個引號的狀況。回傳也必須省略前後引號。
```
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
```
數字部分。0開頭接一個以上數字為八進位；單個0為十進位的0。
```
<INITIAL>0|([1-9][0-9]*) { TOKEN_STRING(integer, yytext); }
<INITIAL>0[0-7]+         { TOKEN_STRING(oct_integer, yytext); }
```
match所有能用土法煉鋼parse出來的東西。
```
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
```
parse id、float、scientific。scientific的底數部分可以是float也可以是十進位數字，+或- match 0個或1個，因此用?。指數部分為0或十進位數字。**id的部分要特別注意，它必須被放在土法煉鋼rules的下面，要不然keywords和reserved words都會被parse成id。** 這裡因為longest match不適用(字元數固定)，所以用的是first rule match。
```
<INITIAL>{isAlpha}({isAlpha}|{isDigit})*                      { TOKEN_STRING(id, yytext); }
<INITIAL>({isFloat}|([1-9][0-9]*))[eE][+-]?(0|([1-9][0-9]*))  { TOKEN_STRING(scientific, yytext); }
<INITIAL>{isFloat}                                            { TOKEN_STRING(float, yytext); }
```
最後就是error部分，由助教提供。
```
    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}
```


## What is the hardest you think in this project

我在寫作業時主要遇到了兩個瓶頸:
1. String Matching
在match string時用到的regular expression是:
```
\"([^"]|\"{2})*\"
```
token前後要先match \"，接著在中間的部分要match到除了\"外的字元或連續兩個\"，若連續兩個\"則要略過其中一個並把另一個當作string的一部分。最後回傳的string token不能包含前後兩個引號。在這個正規表達式中，要用到許多逃脫字元符號\\，會有混淆的效果，不容易寫對。再加上我一開始把 **^a** 和 **[^a]** 搞混了，前者是match**以a開頭的字串**；後者則是complement，是match**字串中除了a以外的字元**。這些因素導致我在這上面花蠻多時間的。

2. Start Condition
在處理多行註解的時候，我最初的寫法是:
```
%Start COMMENT
...
"/*"          { LIST; BEGIN COMMENT; }
<COMMENT>.    { LIST;                }
<COMMENT>"*/" { LIST; BEGIN 0;       }
```
但是這樣卻出錯了，在COMMENT state下依然parse出了id token。因為我誤會了以下兩點：一是我以為前面不加state就是符合預設的INITIAL，但其實是**全部的state都會符合**。也就是說前面不加<>就是所有condition，包括 COMMENT都會被吃進去，而這是不符合我預期的。另外我將上述code寫在id token rule的上方，希望先match COMMENT state的rule，沒想到還是parse出了id token。這是因為Ambiguous rule下會**以longest match為優先，接著才是first match**。而我以為是first match為最優先。在釐清以上兩點後才寫出了正確的code:
```
<INITIAL>"/*" { LIST; BEGIN COMMENT; }
<COMMENT>.    { LIST;                }
<COMMENT>"*/" { LIST; BEGIN 0;       }
```
而為了避免類似的ambiguity發生，我將除了換行和空白格之外的其他所有rules前面都加了INITIAL。

## Feedback to T.A.s

其實這次的作業並不難，但因為對regular expression和lex的語法結構不夠熟悉，還是花了很多時間在debug。好在助教提供的測資豐富且清楚，讓我能在短時間之內發現問題，並將大部分時間花在如何解決問題。
