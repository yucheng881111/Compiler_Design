# Compiler hw2 report

|Field|Value|
|-:|:-|
|Name|陳昱丞|
|ID|0716206|

## How much time did you spend on this project

7 hours.

## Project overview

### Scanner
將作業一的scanner.l直接複製過來，並做以下修改:
1. 把main函式去掉
2. 加上return token
3. 加標頭檔 #include "parser.h"
4. 將static char buffer[ ]的static刪掉

### Parser
我認為要寫出這個parser有幾個關鍵的重點:
1. 了解non-terminal上下層結構的關係。例如program可以derive出variables，variables又可以derive出...直到最下層的terminal。
2. 注意哪些東西可以derive出epsilon。可以derive出epsilon的規則通常都是zero or more something。直接套用助教提供的提示:
```
argseq : /* empty */ | argseq1 ;
argseq1 : arg | argseq1 ',' arg ;
```
3. 注意什麼時候要derive出分號(';')。若是沒注意很有可能上層下層同時derive出分號，在reduce的過程中就會出現兩個分號而導致出錯。還有END後面不用分號、function_call在statement時要加分號在expression時不用等等細節，都需要多加留意。

parser的架構首先是declaration section宣告部分，在scanner.l裡return的token，都先宣告於%token。有left-associative性質的operator也要宣告於%left。例如:
```
%token ID
%token AND
%left AND
```
接著CFG的部分依照助教寫的spec，依序為:
Program、Function、Variable、Constant、Statements、Compound、Simple、Conditional、While、For、Return、Function Call、Expressions。大部分簡單改一下助教spec上寫的form都能運行。我的寫法大致由上層到底層。最上層是
```
ProgramName: ID ';' program END
	    ;
```
接著就再由program往下
```
program: variables functions compounds
       ;
```
然後variables、functions、compounds都繼續往下分支rules，直到遇到最底層的terminal。

## What is the hardest you think in this project
我有卡住比較久的地方大致是以下兩點:
1. function可以derive出多個function_declaration或function_definition。我一開始寫的CFG如下:
```
function: function function_declaration
        | function function_definition
	 ;
```
但是這樣function會一直recursion下去。因此必須修改成以下版本:
```
function: function function_declaration
        | function_declaration
        | function function_definition
	| function_definition
	;
```
2. 接著是function_call的地方。function_call會被用在兩處。一個是statement，一個是expression。用在statement時後面要加分號，用在expression時不用。因此function_call自己不能derive出分號，必須要是在statement的地方derive。如下:
```
statement: compounds
         | simple
         | conditional
         | while
         | for
         | return
         | function_call ';'
         ;
```

## Feedback to T.A.s
我覺得這次助教給的spec相當佛心，大部分的form都已經寫得相當完整，只要照著打就能打出大致架構。而且寫出來的CFG也不太需要考慮conflict或ambiguous等問題。最重要的是有給測資答案讓我們對，讓我debug起來方便許多。

