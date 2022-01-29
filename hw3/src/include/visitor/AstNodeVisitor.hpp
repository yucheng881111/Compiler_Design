#ifndef VISITOR_AST_NODE_VISITOR_H
#define VISITOR_AST_NODE_VISITOR_H

// Forward declaration of AST nodes
class ProgramNode;
class DeclNode;
class VariableNode;
class ConstantValueNode;
class FunctionNode;
class CompoundStatementNode;
class PrintNode;
class BinaryOperatorNode;
class UnaryOperatorNode;
class FunctionInvocationNode;
class VariableReferenceNode;
class AssignmentNode;
class ReadNode;
class IfNode;
class WhileNode;
class ForNode;
class ReturnNode;
class ExpressionNode;

class AstNodeVisitor {
  public:
    virtual ~AstNodeVisitor() = 0;

    virtual void visit(ProgramNode &p_program) = 0;
    virtual void visit(DeclNode &p_decl) = 0;
    virtual void visit(VariableNode &p_variable) = 0;
    virtual void visit(ConstantValueNode &p_constant_value) = 0;
    virtual void visit(FunctionNode &p_function) = 0;
    virtual void visit(CompoundStatementNode &p_compound_statement) = 0;
    virtual void visit(PrintNode &p_print) = 0;
    virtual void visit(BinaryOperatorNode &p_bin_op) = 0;
    virtual void visit(UnaryOperatorNode &p_un_op) = 0;
    virtual void visit(FunctionInvocationNode &p_func_invocation) = 0;
    virtual void visit(VariableReferenceNode &p_variable_ref) = 0;
    virtual void visit(AssignmentNode &p_assignment) = 0;
    virtual void visit(ReadNode &p_read) = 0;
    virtual void visit(IfNode &p_if) = 0;
    virtual void visit(WhileNode &p_while) = 0;
    virtual void visit(ForNode &p_for) = 0;
    virtual void visit(ReturnNode &p_return) = 0;
    virtual void visit(ExpressionNode &p_expression) = 0;
};

#endif
