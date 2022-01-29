# Compiler hw4 report

|Field|Value|
|-:|:-|
|Name|陳昱丞|
|ID|0716206|

## How much time did you spend on this project

4 days.

## Project overview
### Symbol table 架構
要建好一張symbol table，首先要先把symbol table要用到的API先建立起來。symbol table總共分為3層: 第一層是最小單位的table entry、第二層是symbol table、第三層是存了多張symbol table的symbol tables。架構大致如下:
```
class SymbolEntry{
    // code there
public:
    std::string name;
    int kind;
    int level;
    PType type;
    std::string attribute;
};

class SymbolTable{
public:
    // code there
private:
    std::vector<SymbolEntry> entries;
};

class SymbolTables{
public:
    // code there
private:
    std::vector<SymbolTable*> tables;
};
```
用vector儲存資料是因為vector可以支援類似stack的操作，像是back()、push_back()、pop_back()，又可以進行隨機尋訪，比起stack更方便於查找。接下來就是把分別把push entry函式、push table函式、find entry函式實作出來即可。

### Visit 架構
Visit的架構就是助教提到的那5點:
1. Push a new symbol table if this node forms a scope.
2. Insert the symbol into current symbol table if this node is related to declaration (ProgramNode, VariableNode, FunctionNode).
3. Travere child nodes of this node.
4. Perform semantic analyses of this node.
5. Pop the symbol table pushed at the 1st step.

以ForNode為例，遇到ForNode需要push一張新的symbol table，但不需要insert table entry。然後語意分析的部分需要檢查start value有沒有大於end value，有的話即報錯。因此要做的上述的1、3、4、5點。實作如下:
```
// 1: push new symbol table
SymbolTable* new_table = new SymbolTable(TABLE_KIND_FORLOOP);
symbol_tables.pushTable(new_table);

// 3: visit child node
p_for.visitChildNodes(*this);

// 4: semantic analyses
int start_var = ...
int end_var = ...
if(start_var > end_var){
    ...
}

// 5: Pop the symbol table
symbol_tables.popTable(opt_dmp);
```

每個node都依照此邏輯架構下去實作即可完成。

### scanner 修改部分
1. get source code (to print error)
2. check dump symbol table or not

### parser 修改部分
1. add error detection

在parser宣告bool global_error = 0; 若semantic analyses時出錯，即將該值改為1。最後若該值為1，則不要print "There is no syntactic error and semantic error!" 這行。

2. add ForNode initial value

因為AssignmentNode的child是一個VariableReferenceNode和一個ExpressionNode，而ExpressionNode是無法直接取得constant value的值的。為求方便，我直接將$4傳入ForNode中:
```
For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        ...
        $$ = new ForNode(@1.first_line, @1.first_column,
        var_decl, assignment, constant_value_node, $8, $4); // add $4: initial_value
    }
```

## What is the hardest you think in this project
在這個作業中遇到了3個讓我比較有印象的問題:
1. unique pointer問題

我對unique pointer不太熟，甚至是因為這個作業才知道有這個東西，因此操作起來有點不順。很多class的private member是unique pointer這個型態，而因為visit要用到，我將它們放在public的member function裡面回傳，就是一個很普通的物件導向操作方式，但這樣卻常會編譯不過。為了避免麻煩，我將會出問題的private member都改成了public member。

2. variable reference問題

我認為variable reference有一個細節需要特別注意。在最後一筆case的地方會遇到這樣一個問題:
![](https://i.imgur.com/JMZ1RXL.png)
在前面bin_op的地方已經報錯，後面的index not integer卻沒有成功skip掉。這是因為bin_op在這個case的架構下屬於variable reference的下面一層，因此雖bin_op已set error，variable reference卻並不知道。因此在原本的semantic analyses之前，要先檢查下層的expression是否已經發生error了。實作如下:

```
class VariableReferenceNode final : public ExpressionNode {
public:
...
    bool error_already_occur(){
        for(int i = 0; i < m_indices.size(); i++){
            if(m_indices[i]->get_error()){
                return 1;
            }
        }
        return 0;
    }
};

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);

    if(p_variable_ref.error_already_occur()){
        p_variable_ref.set_error();
        return ;
    }

    // semantic analyses
}
```

3. check return in program/procedure or not

為了能夠更方便的查找function的parameters和return type，我另外定義了一個function class專門存放function的資訊。而要知道當前的return type是否符合所屬function的return type，只需要存取最近push的function資訊即可。但若是在最外層program的地方非法return，那也會存取到最後push的那個function，這明顯是不對的。而我想到一個方法可以巧妙運用遞迴性質判斷是否在program的地方return。在visit function node的地方加上以下操作:
```
void SemanticAnalyzer::visit(FunctionNode &p_function) {
    ...
    functions.func_check++; // add
    p_function.visitChildNodes(*this);
    ...
    functions.func_check--; // add
}
```
另外在function class的地方:
```
class Functions{
public:
    std::vector<Function> vec_func;
    int func_check = -1; // add
    ...
};
```
多維持了這個func_check常數，可以得知在visit function時，func_check為0，若visit結束，func_check為-1。若在visit結束後，也就是func_check為-1時，發生了return，就表示這個return是非法的。如此一來就可以判斷是否在program的地方return了。

## Feedback to T.A.s
這次功課的難度和份量都和上次差不多，而且又卡到期末考，情況可以說是比上次還要艱鉅。因此我認為助教可以再多給一點提示，例如在AST的部分多寫幾個get member function方便semantic analyses時呼叫、操作，或者甚至只要多標幾個TODO，都可以大幅減輕我們的loading。要不然我們在處理analyses的同時，還要想怎麼存取我們要用到的資料，要多set什麼、get什麼東西。若這個部分有TODO提示會清楚許多。
