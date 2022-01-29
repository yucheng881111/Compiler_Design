# Compiler hw5 report

|Field|Value|
|-:|:-|
|Name|陳昱丞|
|ID|0716206|

## How much time did you spend on this project

1 day.

## Project overview

這個功課主要是在visitor進行traverse的同時，執行適當的RISC-V assembly code。assembly要做的事主要為存取適當的值做運算(stack frame、register存取操作)。而visit的順序為post-order traversal，代表一個child node的運算結束後，剩下在stack上的值就是那個child執行完的結果。child node的traverse完成後，才會回到root node。這時root就能依照child的結果來執行接下來的運算。以while node為例:

```
void CodeGenerator::visit(WhileNode &p_while) {
    int loop = label++;
    int out = label++;

    dumpInstructions("L%d:\n", loop);
    
    // visit condition child
    p_while.m_condition->accept(*this);
    
    dumpInstructions("    lw t0, 0(sp)\n");
    dumpInstructions("    addi sp, sp, 4\n");
    dumpInstructions("    beqz t0, L%d\n", out);
    
    // visit body child
    p_while.m_body->accept(*this);
    
    dumpInstructions("    j L%d\n", loop);

    dumpInstructions("L%d:\n", out);
}
```

while node有兩個child node，一個是condition、一個是body。首先先visit了condition node，會得到一個bool值(true or false)在stack上。然後將它pop出來( lw t0, 0(sp) )和false(0)做比較( beqz t0, L%d\n", out )。若兩者相等，代表condition為false，就轉跳到label out；否則就跳到label loop繼續執行迴圈( j L%d\n", loop )。

另外，因為不同的child node會有不同執行結果和要處理的操作，因此我將很多的visitChildNodes函式拆開，就如上述將m_condition和m_body改成public並分別呼叫accept。這樣會比使用一個visitChildNodes還要更容易理解與管理。

## What is the hardest you think in this project

我有做了第一個bonus的部分，也就是布林值操作。我認為比較有技巧的地方在unary的not:

```
if(op == "neg"){
    dumpInstructions("    li t1, 0\n");
    dumpInstructions("    sub t0, t1, t0 \n"); // 0 - t0 = -t0
}
else if(op == "not"){
    dumpInstructions("    li t1, 1\n");
    dumpInstructions("    sub t0, t1, t0 \n");
    /* 
    Bonus:
    true -> 1, false -> 0
    1 - 0 = 1; 0->1
    1 - 1 = 0; 1->0
    */
}
```

首先，neg部分不算在bonus部分，但其概念和後面not的作法相似。要得到一個值的相反數，只要用0去減掉原數就行了。而要得到一個布林值的相反，則要用1去減。因為假設bool b = 0，則1 - 0 = 1；若b = 1，則1 - 1 = 0。所以當b是0時得到結果為1；b是1時得到結果為0。這樣就能成功得到一個布林值的相反值。

## Feedback to T.A.s

這次的作業相較之前的作業三、四簡單不少。我認為主要是因為助教給的提示很足夠。除了影片講解post-order和stack關係講得很清楚外，大部分的RISC-V assembly code參考助教spec上的範例及網路上的資源剪剪貼貼一下就可以拼湊得出來。
