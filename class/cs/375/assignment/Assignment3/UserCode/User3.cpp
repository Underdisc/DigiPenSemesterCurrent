/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/
#include "../Drivers/AstNodes.hpp"
#include "../Drivers/Driver3.hpp"

#include <iostream>

// Parser ///////////////////////////////////////////////////////////////////

class Parser
{
public:
    static void Initialize(std::vector<Token> & token_stream);
    static bool AllTokensCovered();
    static bool Accept(TokenType::Enum next_token);
    static bool Expect(TokenType::Enum next_token);
    static bool Expect(AbstractNode * node);
    static bool Expect(bool check);
    static Token PreviousToken();
    static unsigned _index;
    static std::vector<Token> * _token_stream;

    static bool Block();
    static bool Class();
    static bool Var();
    static bool Function();
    static bool Parameter();
    static bool SpecifiedType();
    static bool Type();
    static bool NamedType();
    static bool FunctionType();
    static bool Scope();
    static bool Statement();
    static bool DelimitedStatement();
    static bool FreeStatement();
    static bool Label();
    static bool Goto();
    static bool Return();
    static bool If();
    static bool Else();
    static bool While();
    static bool For();
    static std::unique_ptr<ExpressionNode> GroupedExpression();
    static std::unique_ptr<LiteralNode> Literal();
    static std::unique_ptr<NameReferenceNode> NameReference();
    static std::unique_ptr<ExpressionNode> Value();
    static std::unique_ptr<ExpressionNode> Expression();
    static bool Expression1();
    static bool Expression2();
    static bool Expression3();
    static bool Expression4();
    static bool Expression5();
    static bool Expression6();
    static bool Expression7();
    static bool MemberAccess();
    static bool Call();
    static bool Cast();
    static bool Index();
    
};

unsigned Parser::_index = 0;
std::vector<Token> * Parser::_token_stream = nullptr;

void Parser::Initialize(std::vector<Token> & token_stream)
{
    _index = 0;
    _token_stream = &token_stream;
}

bool Parser::AllTokensCovered()
{
    if (_index == _token_stream->size())
    {
        return true;
    }
    return false;
}

bool Parser::Accept(TokenType::Enum accepted_token)
{
    if (_index >= _token_stream->size())
    {
        return false;
    }

    Token & current_token = (*_token_stream)[_index];
    if(current_token.mTokenType == accepted_token)
    {
        PrintRule::AcceptedToken(current_token);
        ++_index;
        return true;
    }
    return false;
}

bool Parser::Expect(TokenType::Enum accepted_token)
{

    if(Accept(accepted_token))
    {
        return true;
    }
    throw ParsingException();
}

bool Parser::Expect(AbstractNode * node)
{
    if (node)
    {
        return true;
    }
    return false;
}


bool Parser::Expect(bool check)
{
    if (check)
    {
        return true;
    }
    throw ParsingException();
}

Token Parser::PreviousToken()
{
    unsigned previous_index = _index - 1;
    return (*_token_stream)[previous_index];
}

#define Acc(TokenType) Accept(TokenType)
#define Exp(TokenType) Expect(TokenType)

// Recursive Decent Parser Function Declarations /////////////////////////////

#define ReturnNullNode(node_type)                       \
    std::unique_ptr<node_type> new_node(nullptr);       \
    return std::move(new_node);                  
    
// Recursive Decent Parser Function Definitions ///////////////////////////////

bool Parser::Index()
{
    PrintRule print_rule("Index");
    if(!Acc(TokenType::OpenBracket))
    {
        return false;
    }
    Exp(Expression().get());
    Exp(TokenType::CloseBracket);
    print_rule.Accept();
    return true;
}

bool Parser::Cast()
{
    PrintRule print_rule("Cast");
    if (!Acc(TokenType::As))
    {
        return false;
    }
    Exp(Type());
    print_rule.Accept();
    return true;
}

bool Parser::Call()
{
    PrintRule print_rule("Call");
    if (!Acc(TokenType::OpenParentheses))
    {
        return false;
    }

    if (Expression())
    {
        while (Acc(TokenType::Comma))
        {
            Exp(Expression().get());
        }
    }

    Exp(TokenType::CloseParentheses);
    print_rule.Accept();
    return true;

}

bool Parser::MemberAccess()
{
    PrintRule print_rule("MemberAccess");
    if (!(Acc(TokenType::Dot) ||
          Acc(TokenType::Arrow)))
    {
        return false;
    }

    Exp(TokenType::Identifier);
    print_rule.Accept();
    return true;

}

bool Parser::Expression7()
{
    PrintRule print_rule("Expression7");
    if (!Value())
    {
        return false;
    }

    while(MemberAccess() || Call() || Cast() || Index());
    
    print_rule.Accept();
    return true;

}

bool Parser::Expression6()
{
    PrintRule print_rule("Expression6");
    
    while(Acc(TokenType::Asterisk) ||
          Acc(TokenType::Ampersand) ||
          Acc(TokenType::Plus) || 
          Acc(TokenType::Minus) || 
          Acc(TokenType::LogicalNot) || 
          Acc(TokenType::Increment) || 
          Acc(TokenType::Decrement));

    bool result = Expression7();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::Expression5()
{
    PrintRule print_rule("Expression5");
    if (!Expression6())
    {
        return false;
    }

    while (Acc(TokenType::Asterisk) ||
           Acc(TokenType::Divide) || 
           Acc(TokenType::Modulo))
    {
        Exp(Expression6());
    }

    print_rule.Accept();
    return true;
}

bool Parser::Expression4()
{
    PrintRule print_rule("Expression4");
    if (!Expression5())
    {
        return false;
    }

    while (Acc(TokenType::Plus) ||
           Acc(TokenType::Minus))
    {
        Exp(Expression5());
    }

    print_rule.Accept();
    return true;
}

bool Parser::Expression3()
{
    PrintRule print_rule("Expression3");
    if (!Expression4())
    {
        return false;
    }

    while (Acc(TokenType::LessThan) ||
           Acc(TokenType::GreaterThan) ||
           Acc(TokenType::LessThanOrEqualTo) ||
           Acc(TokenType::GreaterThanOrEqualTo) ||
           Acc(TokenType::Equality) ||
           Acc(TokenType::Inequality))
    {
        Exp(Expression4());
    }

    print_rule.Accept();
    return true;
}

bool Parser::Expression2()
{
    PrintRule print_rule("Expression2");
    if (!Expression3())
    {
        return false;
    }
    while (Acc(TokenType::LogicalAnd))
    {
        Exp(Expression3());
    }

    print_rule.Accept();
    return true;
}

bool Parser::Expression1()
{
    PrintRule print_rule("Expression1");
    if (!Expression2())
    {
        return false;
    }
    while (Acc(TokenType::LogicalOr))
    {
        Exp(Expression2());
    }

    print_rule.Accept();
    return true;
}

std::unique_ptr<ExpressionNode> Parser::Expression()
{
    PrintRule print_rule("Expression");
    if (!Expression1())
    {
        return false;
    }

    if(Acc(TokenType::Assignment) ||
       Acc(TokenType::AssignmentPlus) ||
       Acc(TokenType::AssignmentMinus) ||
       Acc(TokenType::AssignmentMultiply) ||
       Acc(TokenType::AssignmentDivide) ||
       Acc(TokenType::AssignmentModulo))
    {
        Exp(Expression().get());
    }

    print_rule.Accept();
    return true;
}

std::unique_ptr<ExpressionNode> Parser::Value()
{
    PrintRule print_rule("Value");
    std::unique_ptr<ExpressionNode> node(Literal().get() || 
                                         NameReference().get() || 
                                         GroupedExpression().get());
    if (node)
    {
        print_rule.Accept();
    }
    return std::move(node);
}

std::unique_ptr<NameReferenceNode> Parser::NameReference()
{
    PrintRule print_rule("NameReference");
    if (!Acc(TokenType::Identifier))
    {
        ReturnNullNode(NameReferenceNode)
    }

    print_rule.Accept();
    std::unique_ptr<NameReferenceNode> new_node(new NameReferenceNode);
    new_node->mName = PreviousToken();
    return std::move(new_node);
}

std::unique_ptr<LiteralNode> Parser::Literal()
{
    PrintRule print_rule("Literal");
    bool result = Acc(TokenType::True) ||
                  Acc(TokenType::False) ||
                  Acc(TokenType::Null) ||
                  Acc(TokenType::IntegerLiteral) ||
                  Acc(TokenType::FloatLiteral) ||
                  Acc(TokenType::StringLiteral) ||
                  Acc(TokenType::CharacterLiteral);
    if (result)
    {
        print_rule.Accept();
        std::unique_ptr<LiteralNode> new_node(new LiteralNode);
        new_node->mToken = Parser::PreviousToken();
        return std::move(new_node);
    }
    ReturnNullNode(LiteralNode);
}

std::unique_ptr<ExpressionNode> Parser::GroupedExpression()
{
    PrintRule print_rule("GroupedExpression");
    if (!Acc(TokenType::OpenParentheses))
    {
        ReturnNullNode(ExpressionNode);
    }
    std::unique_ptr<ExpressionNode> node = Expression();
    Exp(node.get());
    Exp(TokenType::CloseParentheses);
    print_rule.Accept();
}

bool Parser::For()
{
    PrintRule print_rule("For");
    if (!Acc(TokenType::For))
    {
        return false;
    }
    Exp(TokenType::OpenParentheses);
    Var() || Expression();
    Exp(TokenType::Semicolon);
    Expression();
    Exp(TokenType::Semicolon);
    Expression();
    Exp(TokenType::CloseParentheses);
    Exp(Scope());

    print_rule.Accept();
    return true;
}

bool Parser::While()
{
    PrintRule print_rule("While");
    if (!Acc(TokenType::While))
    {
        return false;
    }
    Exp(GroupedExpression().get());
    Exp(Scope());
    print_rule.Accept();
    return true;

}

bool Parser::Else()
{
    PrintRule print_rule("Else");
    if (!Acc(TokenType::Else))
    {
        return false;
    }
    Exp(If() || Scope());
    print_rule.Accept();
    return true;
}

bool Parser::If()
{
    PrintRule print_rule("If");
    if (!Acc(TokenType::If))
    {
        return false;
    }
    Exp(GroupedExpression().get());
    Exp(Scope());
    Else();

    print_rule.Accept();
    return true;
}

bool Parser::Return()
{
    PrintRule print_rule("Return");
    if (Acc(TokenType::Return))
    {
        Expression();
        print_rule.Accept();
        return true;
    }
    return false;
}

bool Parser::Goto()
{
    PrintRule print_rule("Goto");
    bool result = Acc(TokenType::Goto) &&
        Exp(TokenType::Identifier);
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::Label()
{
    PrintRule print_rule("Label");
    bool result = Acc(TokenType::Label) && 
                  Exp(TokenType::Identifier);
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::FreeStatement()
{
    PrintRule print_rule("FreeStatement");
    bool result = If() || While() || For();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::DelimitedStatement()
{
    PrintRule print_rule("DelimitedStatement");
    bool result = Label() || 
                  Goto() || 
                  Return() || 
                  Acc(TokenType::Break) ||
                  Acc(TokenType::Continue) ||
                  Var() ||
                  Expression();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
        
}

bool Parser::Statement()
{
    PrintRule print_rule("Statement");
    bool result =  FreeStatement() || (DelimitedStatement() && 
                                       Exp(TokenType::Semicolon));
    if(result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::Scope()
{
    PrintRule print_rule("Scope");
    if (!Acc(TokenType::OpenCurley))
    {
        return false;
    }
    while(Statement());
    Exp(TokenType::CloseCurley);
    print_rule.Accept();
    return true;
}

bool Parser::FunctionType()
{
    PrintRule print_rule("FunctionType");
    if (!Acc(TokenType::Function))
    {
        return false;
    }

    Exp(TokenType::Asterisk);
    Acc(TokenType::Ampersand);

    Exp(TokenType::OpenParentheses);
    Exp(Type());

    while(Acc(TokenType::Comma) && Exp(Type()));
    Exp(TokenType::CloseParentheses);
    SpecifiedType();
    print_rule.Accept();

    return true;
}

bool Parser::NamedType()
{
    PrintRule print_rule("NamedType");
    if (!Acc(TokenType::Identifier))
    {
        return false;
    }
    while(Acc(TokenType::Asterisk));
    Acc(TokenType::Ampersand);
    print_rule.Accept();
    return true;
}

bool Parser::Type()
{
    PrintRule print_rule("Type");
    bool result = NamedType() || FunctionType();
    
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Parser::SpecifiedType()
{
    PrintRule print_rule("SpecifiedType");
    if (!Acc(TokenType::Colon))
    {
        return false;
    }
    Exp(Type());
    print_rule.Accept();
    return true;
}

bool Parser::Parameter()
{
    PrintRule print_rule("Parameter");
    if (!Acc(TokenType::Identifier))
    {
        return false;
    }
    Exp(SpecifiedType());
    print_rule.Accept();
    return true;
}

bool Parser::Function()
{
    PrintRule print_rule("Function");
    if (!Acc(TokenType::Function))
    {
        return false;
    }

    Exp(TokenType::Identifier);
    Exp(TokenType::OpenParentheses);

    if (Parameter())
    {
        while(Acc(TokenType::Comma))
        {
            Exp(Parameter());
        }
    }

    Exp(TokenType::CloseParentheses);
    SpecifiedType();
    Exp(Scope());
    
    print_rule.Accept();
    return true;
}

bool Parser::Var()
{
    PrintRule print_rule("Var");
    if (!Acc(TokenType::Var))
    {
      return false;
    }

    Exp(TokenType::Identifier);
    Exp(SpecifiedType());

    if (Acc(TokenType::Assignment))
    {
        Exp(Expression().get());
    }

    print_rule.Accept();
    return true;
}

bool Parser::Class()
{
    PrintRule print_rule("Class");
    if(!Acc(TokenType::Class))
    {
        return false;
    }

    Exp(TokenType::Identifier);
    Exp(TokenType::OpenCurley);

    while(true)
    {
        if(Var())
        {
            Exp(TokenType::Semicolon);
        }
        else if(!Function())
        {
            break;
        }
    }

    Exp(TokenType::CloseCurley);

    print_rule.Accept();
    return true;
}

bool Parser::Block()
{
    PrintRule print_rule("Block");
    while(Class() || 
          Function() || 
          (Var() && Exp(TokenType::Semicolon)));
    print_rule.Accept();
    return false;
}

// Visitor Implementation /////////////////////////////////////////////////////

#define BaseVisitDefinition(node_type)          \
    virtual VisitResult Visit(node_type * node) \
    {                                           \
        return Continue;                        \
    }

class Visitor
{
public:
    enum VisitResult
    {
        Continue,
        Stop
    };
public:
    BaseVisitDefinition(AbstractNode)
    BaseVisitDefinition(BlockNode)
    BaseVisitDefinition(ClassNode)
    BaseVisitDefinition(StatementNode)
    BaseVisitDefinition(VariableNode)
    BaseVisitDefinition(ScopeNode)
    BaseVisitDefinition(ParameterNode)
    BaseVisitDefinition(FunctionNode)
    BaseVisitDefinition(TypeNode)
    BaseVisitDefinition(PointerTypeNode)
    BaseVisitDefinition(ReferenceTypeNode)
    BaseVisitDefinition(NamedTypeNode)
    BaseVisitDefinition(FunctionTypeNode)
    BaseVisitDefinition(LabelNode)
    BaseVisitDefinition(GotoNode)
    BaseVisitDefinition(ReturnNode)
    BaseVisitDefinition(BreakNode)
    BaseVisitDefinition(ContinueNode)
    BaseVisitDefinition(ExpressionNode)
    BaseVisitDefinition(IfNode)
    BaseVisitDefinition(WhileNode)
    BaseVisitDefinition(ForNode)
    BaseVisitDefinition(LiteralNode)
    BaseVisitDefinition(NameReferenceNode)
    BaseVisitDefinition(BinaryOperatorNode)
    BaseVisitDefinition(UnaryOperatorNode)
    BaseVisitDefinition(PostExpressionNode)
    BaseVisitDefinition(MemberAccessNode)
    BaseVisitDefinition(CallNode)
    BaseVisitDefinition(CastNode)
    BaseVisitDefinition(IndexNode)
};

#define NodePrinterVisitDefinition(node_type, token_name)              \
    Visitor::VisitResult NodePrinterVisitor::Visit(node_type * node)   \
    {                                                                  \
        NodePrinter printer;                                           \
        printer << #node_type << "(" << node->token_name.mText << ")"; \
        return Continue;                                               \
    }

#define NodePrinterVisitDefinitionTypeOnly(node_type)                \
    Visitor::VisitResult NodePrinterVisitor::Visit(node_type * node) \
    {                                                                \
        NodePrinter printer;                                         \
        printer << #node_type;                                       \
        return Continue;                                             \
    }

#define NodePrinterVisitDefinitionContinue(node_type)                \
    Visitor::VisitResult NodePrinterVisitor::Visit(node_type * node) \
    {                                                                \
        return Continue;                                             \
    }

#define NodePrinterVisitDeclarationOverride(node_type) \
    VisitResult Visit(node_type * node) override;

class NodePrinterVisitor : public Visitor
{
public:
    NodePrinterVisitDeclarationOverride(AbstractNode)
    NodePrinterVisitDeclarationOverride(BlockNode)
    NodePrinterVisitDeclarationOverride(ClassNode)
    NodePrinterVisitDeclarationOverride(StatementNode)
    NodePrinterVisitDeclarationOverride(VariableNode)
    NodePrinterVisitDeclarationOverride(ScopeNode)
    NodePrinterVisitDeclarationOverride(ParameterNode)
    NodePrinterVisitDeclarationOverride(FunctionNode)
    NodePrinterVisitDeclarationOverride(TypeNode)
    NodePrinterVisitDeclarationOverride(PointerTypeNode)
    NodePrinterVisitDeclarationOverride(ReferenceTypeNode)
    NodePrinterVisitDeclarationOverride(NamedTypeNode)
    NodePrinterVisitDeclarationOverride(FunctionTypeNode)
    NodePrinterVisitDeclarationOverride(LabelNode)
    NodePrinterVisitDeclarationOverride(GotoNode)
    NodePrinterVisitDeclarationOverride(ReturnNode)
    NodePrinterVisitDeclarationOverride(BreakNode)
    NodePrinterVisitDeclarationOverride(ContinueNode)
    NodePrinterVisitDeclarationOverride(ExpressionNode)
    NodePrinterVisitDeclarationOverride(IfNode)
    NodePrinterVisitDeclarationOverride(WhileNode)
    NodePrinterVisitDeclarationOverride(ForNode)
    NodePrinterVisitDeclarationOverride(LiteralNode)
    NodePrinterVisitDeclarationOverride(NameReferenceNode)
    NodePrinterVisitDeclarationOverride(BinaryOperatorNode)
    NodePrinterVisitDeclarationOverride(UnaryOperatorNode)
    NodePrinterVisitDeclarationOverride(PostExpressionNode)
    NodePrinterVisitDeclarationOverride(MemberAccessNode)
    NodePrinterVisitDeclarationOverride(CallNode)
    NodePrinterVisitDeclarationOverride(CastNode)
    NodePrinterVisitDeclarationOverride(IndexNode)
};

NodePrinterVisitDefinitionContinue(AbstractNode)
NodePrinterVisitDefinitionContinue(BlockNode)
NodePrinterVisitDefinition(ClassNode, mName)
NodePrinterVisitDefinitionContinue(StatementNode)
NodePrinterVisitDefinition(VariableNode, mName)
NodePrinterVisitDefinitionContinue(ScopeNode)
NodePrinterVisitDefinition(ParameterNode, mName)
NodePrinterVisitDefinition(FunctionNode, mName)
NodePrinterVisitDefinitionContinue(TypeNode)
NodePrinterVisitDefinitionTypeOnly(PointerTypeNode)
NodePrinterVisitDefinitionTypeOnly(ReferenceTypeNode)
NodePrinterVisitDefinition(NamedTypeNode, mName)
NodePrinterVisitDefinitionTypeOnly(FunctionTypeNode)
NodePrinterVisitDefinition(LabelNode, mName)
NodePrinterVisitDefinition(GotoNode, mName)
NodePrinterVisitDefinitionTypeOnly(ReturnNode)
NodePrinterVisitDefinitionTypeOnly(BreakNode)
NodePrinterVisitDefinitionTypeOnly(ContinueNode)
NodePrinterVisitDefinitionContinue(ExpressionNode)
NodePrinterVisitDefinitionTypeOnly(IfNode)
NodePrinterVisitDefinitionTypeOnly(WhileNode)
NodePrinterVisitDefinitionTypeOnly(ForNode)
NodePrinterVisitDefinition(LiteralNode, mToken)
NodePrinterVisitDefinition(NameReferenceNode, mName)
NodePrinterVisitDefinition(BinaryOperatorNode, mOperator)
NodePrinterVisitDefinition(UnaryOperatorNode, mOperator)
NodePrinterVisitDefinitionContinue(PostExpressionNode)
NodePrinterVisitDefinition(MemberAccessNode, mName)
NodePrinterVisitDefinitionTypeOnly(CallNode)
NodePrinterVisitDefinitionTypeOnly(CastNode)
NodePrinterVisitDefinitionTypeOnly(IndexNode)

// Node Walk Implementations //////////////////////////////////////////////////

void BlockNode::Walk(Visitor* visitor, bool visit)
{
}
void ClassNode::Walk(Visitor* visitor, bool visit)
{
}
void StatementNode::Walk(Visitor* visitor, bool visit)
{
}
void TypeNode::Walk(Visitor* visitor, bool visit)
{
}
void PointerTypeNode::Walk(Visitor* visitor, bool visit)
{
}
void ReferenceTypeNode::Walk(Visitor* visitor, bool visit)
{
}
void NamedTypeNode::Walk(Visitor* visitor, bool visit)
{
}
void FunctionTypeNode::Walk(Visitor* visitor, bool visit)
{
}
void VariableNode::Walk(Visitor* visitor, bool visit)
{
}
void ParameterNode::Walk(Visitor* visitor, bool visit)
{
}
void ScopeNode::Walk(Visitor* visitor, bool visit)
{
}
void FunctionNode::Walk(Visitor* visitor, bool visit)
{
}
void LabelNode::Walk(Visitor* visitor, bool visit)
{
}
void GotoNode::Walk(Visitor* visitor, bool visit)
{
}
void ReturnNode::Walk(Visitor* visitor, bool visit)
{
}
void BreakNode::Walk(Visitor* visitor, bool visit)
{
}
void ContinueNode::Walk(Visitor* visitor, bool visit)
{
}

void ExpressionNode::Walk(Visitor* visitor, bool visit)
{
}
void IfNode::Walk(Visitor* visitor, bool visit)
{
}
void WhileNode::Walk(Visitor* visitor, bool visit)
{
}
void ForNode::Walk(Visitor* visitor, bool visit)
{
}
void LiteralNode::Walk(Visitor* visitor, bool visit)
{
    if(visit && visitor->Visit(this) == Visitor::Stop) return;

}
void NameReferenceNode::Walk(Visitor* visitor, bool visit)
{
}
void BinaryOperatorNode::Walk(Visitor* visitor, bool visit)
{
}
void UnaryOperatorNode::Walk(Visitor* visitor, bool visit)
{
}
void PostExpressionNode::Walk(Visitor* visitor, bool visit)
{
}
void MemberAccessNode::Walk(Visitor* visitor, bool visit)
{
}
void CallNode::Walk(Visitor* visitor, bool visit)
{
}
void CastNode::Walk(Visitor* visitor, bool visit)
{
}
void IndexNode::Walk(Visitor* visitor, bool visit)
{
}

// Assignment Functions ///////////////////////////////////////////////////////

std::unique_ptr<ExpressionNode> ParseExpression(std::vector<Token>& tokens)
{
    Parser::Initialize(tokens);
    std::unique_ptr<ExpressionNode> expression_node;
    expression_node = Parser::Expression();
    return expression_node;
}

void PrintTree(AbstractNode* node)
{
    NodePrinterVisitor visitor;
    // The appropriate Walk function will be called depedning on node's type.
    // Once we are in the Walk function for the correct node type, we can call
    // Visit within that function and the correct visit function will be used.
    node->Walk(&visitor);
}

std::unique_ptr<BlockNode> ParseBlock(std::vector<Token>& tokens)
{
    std::unique_ptr<BlockNode> block_node;
    return block_node;
}