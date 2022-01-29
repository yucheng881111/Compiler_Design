# Compiler hw3 report

|Field|Value|
|-:|:-|
|Name|陳昱丞|
|ID|0716206|

## How much time did you spend on this project

5 days.

## Project overview

### parser - 宣告型別
要做出AST，首先要先修改parser。先定義後面CFG中要用到的型別。型別需要宣告在%union裡面，需include的檔案要放在%code requires裡面。之後，用%type來定義CFG中non-terminal的型別。舉例來說，假設我要定義FunctionDeclaration、FunctionDefinition、Function這三個non-termianl是屬於FunctionNode* 型態，而FunctionList、Functions則是屬於vector<FunctionNode*>* 型態，那麼寫法應如下:
```
%{
#include "AST/function.hpp"
%}

%code requires {
    #include <vector>
    using namespace std;
    class FunctionNode;
}

%union {
    FunctionNode* func_node;
    vector<FunctionNode*>* func_list;
}

%type <func_node> FunctionDeclaration FunctionDefinition Function
%type <func_list> FunctionList Functions

%%
// CFG
%%
```

剩下的non-terminal就以此類推，寫出所有non-terminal的型別。

### scanner
在parser端宣告完所有型別後，scanner必須在適當的terminal處加上:
```
yylval.identifier = strdup(yytext);
```
以identifier為例:
```
/* Identifier */
[a-zA-Z][a-zA-Z0-9]* {
    TOKEN_STRING(id, yytext);
    yylval.identifier = strndup(yytext, MAX_ID_LENG); // add
    return ID;
}
```
而對應的yylval.identifier就是在parser中的:
```
%union {
    char *identifier;
}

%token <identifier> ID
```
所有需要被存取的terminal都以此類推。這樣寫完後他們在CFG中都能被讀取成char* 型態。而我將int、float等都讀成char* 型態是為了方便接下來的讀取。可以在CFG吃到string值後，再轉成自己想要的型態。

### parser - AST
最後要建出一整棵AST，要修改parser中的CFG部分。以Program為例:
```
Program:
    ProgramName SEMICOLON
    DeclarationList FunctionList CompoundStatement
    END {
        $$ = new ProgramNode(@1.first_line, @1.first_column,
                               $1, $5, $3, $4);
    }
;
```
上述code中，\$$代表CFG左邊的Program，而他的型別是ProgramNode* 。因此\$$ = new ProgramNode()是正確的賦值；而\$1是CFG右邊的第一個，也就是DeclarationList。\$2是第二個，以此類推。呼叫了ProgramNode的建構子，就需要看到program.hpp和program.cpp檔，以下合稱program檔。在program檔中:
```
ProgramNode(const uint32_t line, const uint32_t col,const char *const p_name,
                CompoundStatementNode* com, 
                std::vector<DeclNode*>* dec,
                std::vector<FunctionNode*>* vec)
: AstNode{line, col}, name(p_name), compound(com), decl_vec(dec), func_vec(vec) {}
                
```
可以看到因為Program可以derive出ProgramName、DeclarationList、FunctionList、CompoundStatement這四個元素，因此將這幾個元素設成ProgramNode的private member，再利用建構式賦值給他。之後會在visit的時候對它們進行存取。

### parser - Visit
承上，在visit的時候我們會呼叫ProgramNode的visitChildNodes，而在visitChildNodes中會分別visit Program的child nodes，也就是DeclarationList、FunctionList、CompoundStatement。而因為ProgramName只是一個普通的member資訊，並不是一個nodes，因此只需要將他print出來，而不用進行visit。以下是ProgramNode的visitChildNodes:
```
void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor){

    if(decl_vec != NULL){
        for(auto v : *decl_vec){
            p_visitor.visit(*v);
        }
    }

    if(func_vec != NULL){
        for(auto v : *func_vec){
            p_visitor.visit(*v);
        }
    }

    if(compound != NULL){
        p_visitor.visit(*compound);
    }
    
}
```
接著，child nodes又會再去visit自己的child nodes，一直呼叫到底層並return。如此便完成了visit的遞迴呼叫。這樣我們就只要在parser的main函式裡呼叫root node的visit，就可以visit完整棵AST tree。

## What is the hardest you think in this project
我覺得最難的部分在一開始要了解它整個運作機制。因為這個寫法包含了許多繼承、覆寫(override)的概念，並且如何用AstDumper來visit整棵tree。而一旦了解了運作機制後，剩下的東西就都大同小異。前面的四到五筆測資花了我很多時間，因為大部分的non-terminal都還沒寫完整。而後面的幾筆就進行得很快，因為只需要呼叫寫好的non-terminal物件就自動完成了。

## Feedback to T.A.s
我覺得這個作業和前兩次比，可以說是相當的難。因為它結合許多物件導向的寫法。但一旦寫出前面後，後面就能用差不多的方法實作出來，可以說是「頭過身就過」。另外，我一開始寫的時候不知道#include<vector>和using namespace std除了寫在開頭的地方外，還要寫在%code requires裡面，導致編譯一直不會過。還好在我發了issue之後，助教很快就回覆我，並幫我解決了問題。感謝助教有效率的回覆讓我不用在這個點卡很久。
