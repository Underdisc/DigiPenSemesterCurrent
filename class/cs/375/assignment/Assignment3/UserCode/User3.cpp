/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/
#include "../Drivers/AstNodes.hpp"
#include "../Drivers/Driver3.hpp"

#include <iostream>
#include <stack>

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
    static Token PreviousToken(unsigned delta = 1);
    static unsigned _index;
    static std::vector<Token> * _token_stream;

    static bool Block();
    static bool Class();
    static bool Var();
    static bool Function();
    static bool Parameter();
    static std::unique_ptr<TypeNode> SpecifiedType();
    static std::unique_ptr<TypeNode> Type();
    static std::unique_ptr<TypeNode> NamedType();
    static std::unique_ptr<TypeNode> FunctionType();
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
    static std::unique_ptr<ExpressionNode> Expression1();
    static std::unique_ptr<ExpressionNode> Expression2();
    static std::unique_ptr<ExpressionNode> Expression3();
    static std::unique_ptr<ExpressionNode> Expression4();
    static std::unique_ptr<ExpressionNode> Expression5();
    static std::unique_ptr<ExpressionNode> Expression6();
    static std::unique_ptr<ExpressionNode> Expression7();
    static std::unique_ptr<MemberAccessNode> MemberAccess();
    static std::unique_ptr<CallNode> Call();
    static std::unique_ptr<CastNode> Cast();
    static std::unique_ptr<IndexNode> Index();
    
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
    throw ParsingException();
}


bool Parser::Expect(bool check)
{
    if (check)
    {
        return true;
    }
    throw ParsingException();
}

Token Parser::PreviousToken(unsigned delta)
{
    unsigned previous_index = _index - delta;
    return (*_token_stream)[previous_index];
}

#define Acc(TokenType) Accept(TokenType)
#define Exp(TokenType) Expect(TokenType)

// Recursive Decent Parser Function Declarations /////////////////////////////

#define ReturnNullNode(node_type)                       \
    std::unique_ptr<node_type> new_node(nullptr);       \
    return std::move(new_node);                  
    
// Recursive Decent Parser Function Definitions ///////////////////////////////

std::unique_ptr<IndexNode> Parser::Index()
{
    PrintRule print_rule("Index");
    if(!Acc(TokenType::OpenBracket))
    {
        ReturnNullNode(IndexNode);
    }
    std::unique_ptr<IndexNode> final_node(new IndexNode);
    final_node->mIndex = Expression();
    Exp(final_node->mIndex.get());
    Exp(TokenType::CloseBracket);
    print_rule.Accept();
    return std::move(final_node);
}

std::unique_ptr<CastNode> Parser::Cast()
{
    PrintRule print_rule("Cast");
    if (!Acc(TokenType::As))
    {
        ReturnNullNode(CastNode);
    }

    std::unique_ptr<CastNode> final_node(new CastNode);
    final_node->mType = Type();
    Exp(final_node->mType.get());
    print_rule.Accept();
    return std::move(final_node);
}

std::unique_ptr<CallNode> Parser::Call()
{
    PrintRule print_rule("Call");
    if (!Acc(TokenType::OpenParentheses))
    {
        ReturnNullNode(CallNode);
    }

    std::unique_ptr<CallNode> new_node(new CallNode);
    std::unique_ptr<ExpressionNode> node;
    node = Expression();
    if (node)
    {
        new_node->mArguments.push_back(std::move(node));
        while (Acc(TokenType::Comma))
        {
            node = Expression();
            Exp(node.get());
            new_node->mArguments.push_back(std::move(node));
        }
    }

    Exp(TokenType::CloseParentheses);
    print_rule.Accept();
    return std::move(new_node);

}

std::unique_ptr<MemberAccessNode> Parser::MemberAccess()
{
    PrintRule print_rule("MemberAccess");
    if (!(Acc(TokenType::Dot) ||
          Acc(TokenType::Arrow)))
    {
        ReturnNullNode(MemberAccessNode);
    }

    Exp(TokenType::Identifier);
    print_rule.Accept();
    
    std::unique_ptr<MemberAccessNode> new_node(new MemberAccessNode);
    new_node->mOperator = PreviousToken(2);
    new_node->mName = PreviousToken();
    return std::move(new_node);
}

std::unique_ptr<ExpressionNode> Parser::Expression7()
{
    PrintRule print_rule("Expression7");

    std::unique_ptr<ExpressionNode> final_node;
    final_node = Value();
    if (!final_node.get())
    {
        ReturnNullNode(PostExpressionNode);
    }

    std::unique_ptr<PostExpressionNode> post;
    while ((post = MemberAccess()) != nullptr ||
           (post = Call()) != nullptr ||
           (post = Cast()) != nullptr ||
           (post = Index()) != nullptr)
    {
        post->mLeft = std::move(final_node);
        final_node = std::move(post);
    }
    
    print_rule.Accept();
    return std::move(final_node);

}

std::unique_ptr<ExpressionNode> Parser::Expression6()
{
    PrintRule print_rule("Expression6");
    
    std::stack<Token> unary_stack;
    while (Acc(TokenType::Asterisk) ||
           Acc(TokenType::Ampersand) ||
           Acc(TokenType::Plus) ||
           Acc(TokenType::Minus) ||
           Acc(TokenType::LogicalNot) ||
           Acc(TokenType::Increment) ||
           Acc(TokenType::Decrement))
    {
       unary_stack.push(PreviousToken());
    }

    std::unique_ptr<ExpressionNode> right;
    right = Expression7();
    if (right)
    {
        print_rule.Accept();
    }

    while (!unary_stack.empty())
    {
        std::unique_ptr<UnaryOperatorNode> unary(new UnaryOperatorNode);
        unary->mOperator = unary_stack.top();
        unary->mRight = std::move(right);
        right = std::move(unary);
        unary_stack.pop();
    }

    return std::move(right);
}

std::unique_ptr<ExpressionNode> Parser::Expression5()
{
    PrintRule print_rule("Expression5");

    std::unique_ptr<ExpressionNode> left;
    left = Expression6();
    if (!left)
    {
        ReturnNullNode(ExpressionNode);
    }

    while (Acc(TokenType::Asterisk) ||
           Acc(TokenType::Divide) || 
           Acc(TokenType::Modulo))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression6();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Expression4()
{
    PrintRule print_rule("Expression4");

    std::unique_ptr<ExpressionNode> left;
    left = Expression5();
    if (!left)
    {
        ReturnNullNode(ExpressionNode);
    }

    while (Acc(TokenType::Plus) ||
           Acc(TokenType::Minus))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression5();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Expression3()
{
    PrintRule print_rule("Expression3");

    std::unique_ptr<ExpressionNode> left;
    left = Expression4();
    if (!left.get())
    {
        ReturnNullNode(ExpressionNode);
    }

    while (Acc(TokenType::LessThan) ||
           Acc(TokenType::GreaterThan) ||
           Acc(TokenType::LessThanOrEqualTo) ||
           Acc(TokenType::GreaterThanOrEqualTo) ||
           Acc(TokenType::Equality) ||
           Acc(TokenType::Inequality))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression4();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Expression2()
{
    PrintRule print_rule("Expression2");

    std::unique_ptr<ExpressionNode> left;
    left = Expression3();
    if (!left.get())
    {
        ReturnNullNode(ExpressionNode);
    }
    while (Acc(TokenType::LogicalAnd))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression3();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Expression1()
{
    PrintRule print_rule("Expression1");

    std::unique_ptr<ExpressionNode> left;
    left = Expression2();
    if (!left)
    {
        ReturnNullNode(ExpressionNode);
    }

    while (Acc(TokenType::LogicalOr))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression2();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Expression()
{
    PrintRule print_rule("Expression");

    std::unique_ptr<ExpressionNode> left;
    left = Expression1();
    if (!left)
    {
        ReturnNullNode(ExpressionNode);
    }

    if(Acc(TokenType::Assignment) ||
       Acc(TokenType::AssignmentPlus) ||
       Acc(TokenType::AssignmentMinus) ||
       Acc(TokenType::AssignmentMultiply) ||
       Acc(TokenType::AssignmentDivide) ||
       Acc(TokenType::AssignmentModulo))
    {
        std::unique_ptr<BinaryOperatorNode> binary(new BinaryOperatorNode);
        binary->mOperator = PreviousToken();
        std::unique_ptr<ExpressionNode> right;
        right = Expression();
        Exp(right.get());
        binary->mLeft = std::move(left);
        binary->mRight = std::move(right);
        left = std::move(binary);
    }

    print_rule.Accept();
    return std::move(left);
}

std::unique_ptr<ExpressionNode> Parser::Value()
{
    PrintRule print_rule("Value");
    std::unique_ptr<ExpressionNode> node;
    (node = Literal()) != nullptr || 
    (node = NameReference()) != nullptr || 
    (node = GroupedExpression()) != nullptr;
    if (!node)
    {
        ReturnNullNode(ExpressionNode);
    }
    print_rule.Accept();
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
    return std::move(node);
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

std::unique_ptr<TypeNode> Parser::FunctionType()
{
    PrintRule print_rule("FunctionType");
    if (!Acc(TokenType::Function))
    {
        ReturnNullNode(TypeNode);
    }

    std::unique_ptr<FunctionTypeNode> function_type(new FunctionTypeNode);
    std::unique_ptr<TypeNode> final_node = std::move(function_type);

    Exp(TokenType::Asterisk);
    if (Acc(TokenType::Ampersand))
    {
        std::unique_ptr<ReferenceTypeNode> ref_type(new ReferenceTypeNode);
        ref_type->mReferenceTo = std::move(function_type);
        final_node = std::move(ref_type);
    }

    Exp(TokenType::OpenParentheses);
 
    std::unique_ptr<TypeNode> type = Type();
    Exp(type.get());
    function_type->mParameters.push_back(std::move(type));
    while (Acc(TokenType::Comma))
    {
        type = Type();
        Exp(type.get());
        function_type->mParameters.push_back(std::move(type));
    }
    Exp(TokenType::CloseParentheses);
    function_type->mReturn = SpecifiedType();
    print_rule.Accept();

    return std::move(final_node);
}

std::unique_ptr<TypeNode> Parser::NamedType()
{
    PrintRule print_rule("NamedType");
    if (!Acc(TokenType::Identifier))
    {
        ReturnNullNode(NamedTypeNode);
    }

    std::unique_ptr<NamedTypeNode> named_type(new NamedTypeNode);
    named_type->mName = PreviousToken();
    std::unique_ptr<TypeNode> final_node = std::move(named_type);

    while (Acc(TokenType::Asterisk))
    {
        std::unique_ptr<PointerTypeNode> pointer_type(new PointerTypeNode);
        pointer_type->mPointerTo = std::move(final_node);
        final_node = std::move(pointer_type);
    }

    if(Acc(TokenType::Ampersand))
    {
        std::unique_ptr<ReferenceTypeNode> ref_type(new ReferenceTypeNode);
        ref_type->mReferenceTo = std::move(final_node);
        final_node = std::move(ref_type);
    }

    print_rule.Accept();
    return std::move(final_node);
}

std::unique_ptr<TypeNode> Parser::Type()
{
    PrintRule print_rule("Type");
    std::unique_ptr<TypeNode> final_node(new TypeNode);
    (final_node = NamedType()) != nullptr ||
    (final_node = FunctionType()) != nullptr;

    if (final_node)
    {
        print_rule.Accept();
    }
    return final_node;
}

std::unique_ptr<TypeNode> Parser::SpecifiedType()
{
    PrintRule print_rule("SpecifiedType");
    if (!Acc(TokenType::Colon))
    {
        ReturnNullNode(TypeNode);
    }

    std::unique_ptr<TypeNode> final_node;
    final_node = Type();
    Exp(final_node.get());
    print_rule.Accept();
    return std::move(final_node);
}

bool Parser::Parameter()
{
    PrintRule print_rule("Parameter");
    if (!Acc(TokenType::Identifier))
    {
        return false;
    }
    /*Exp(SpecifiedType());*/
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

    /*Exp(TokenType::Identifier);
    Exp(SpecifiedType());*/

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

#define NodePrinterVisitorHeader(node_type) \
    Visitor::VisitResult NodePrinterVisitor::Visit(node_type * node)

#define PrintNode(node_type)   \
    NodePrinter printer;       \
    printer << #node_type


NodePrinterVisitorHeader(AbstractNode)
{
    PrintNode(AbstractNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(BlockNode)
{
    PrintNode(BlockNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ClassNode)
{
    PrintNode(ClassNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}
NodePrinterVisitorHeader(StatementNode)
{
    PrintNode(StatementNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(TypeNode)
{
    PrintNode(TypeNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(PointerTypeNode)
{
    PrintNode(PointerTypeNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ReferenceTypeNode)
{
    PrintNode(ReferenceTypeNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(NamedTypeNode)
{
    PrintNode(NamedTypeNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(FunctionTypeNode)
{
    PrintNode(FunctionTypeNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(VariableNode)
{
    PrintNode(VariableNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ParameterNode)
{
    PrintNode(ParameterNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ScopeNode)
{
    PrintNode(ScopeNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(FunctionNode)
{
    PrintNode(FunctionNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(LabelNode)
{
    PrintNode(LabelNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(GotoNode)
{
    PrintNode(GotoNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ReturnNode)
{
    PrintNode(ReturnNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(BreakNode)
{
    PrintNode(BreakNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ContinueNode)
{
    PrintNode(StopNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ExpressionNode)
{
    PrintNode(ExpressionNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(IfNode)
{
    PrintNode(BreakNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(WhileNode)
{
    PrintNode(WhileNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(ForNode)
{
    PrintNode(ForNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(LiteralNode)
{
    PrintNode(LiteralNode) << "(" << node->mToken << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(NameReferenceNode)
{
    PrintNode(NameReferenceNode) << "(" << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(BinaryOperatorNode)
{
    PrintNode(BinaryOperatorNode) << "(" << node->mOperator << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(UnaryOperatorNode)
{
    PrintNode(UnaryOperatorNode) << "(" << node->mOperator << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(PostExpressionNode)
{
    node->mLeft->Walk(this);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(MemberAccessNode)
{
    PrintNode(MemberAccessNode) << "(" << node->mOperator
                                << ", " << node->mName << ")";
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(CallNode)
{
    PrintNode(CallNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(CastNode)
{
    PrintNode(CastNode);
    node->Walk(this, false);
    return Stop;
}

NodePrinterVisitorHeader(IndexNode)
{
    PrintNode(IndexNode);
    node->Walk(this, false);
    return Stop;
}


// Node Walk Implementations //////////////////////////////////////////////////

#define VisitNode                                       \
    if (visit && visitor->Visit(this) == Visitor::Stop) \
    {                                                   \
        return;                                         \
    }

#define WalkMember(member_name)            \
    this->member_name->Walk(visitor) \

#define WalkExisting(member_name)         \
    if (this->member_name)                \
    {                                     \
        this->member_name->Walk(visitor); \
    }

#define WalkLoop(member_name)                              \
    for(unsigned i = 0; i < this->member_name.size(); ++i) \
    {                                                      \
        this->member_name[i]->Walk(visitor);               \
    }


void BlockNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    AbstractNode::Walk(visitor, false);
    WalkLoop(mGlobals);
}
void ClassNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    AbstractNode::Walk(visitor, false);
    WalkLoop(mMembers);
}
void StatementNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    AbstractNode::Walk(visitor, false);
}
void TypeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    AbstractNode::Walk(visitor, false);
}
void PointerTypeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    TypeNode::Walk(visitor, false);
    WalkMember(mPointerTo);
}
void ReferenceTypeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    TypeNode::Walk(visitor, false);
    WalkMember(mReferenceTo);
}
void NamedTypeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    TypeNode::Walk(visitor, false);
}
void FunctionTypeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    TypeNode::Walk(visitor, false);
    WalkLoop(mParameters);
    WalkMember(mReturn);
}
void VariableNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkMember(mType);
    WalkExisting(mInitialValue);
}
void ParameterNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    VariableNode::Walk(visitor, false);
}
void ScopeNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkLoop(mStatements);
}
void FunctionNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    AbstractNode::Walk(visitor, false);
    WalkLoop(mParameters);
    WalkExisting(mReturnType);
    WalkMember(mScope);
}
void LabelNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
}
void GotoNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
}
void ReturnNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkExisting(mReturnValue);
}
void BreakNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
}
void ContinueNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
}
void ExpressionNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
}
void IfNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkExisting(mCondition);
    WalkMember(mScope);
    WalkExisting(mElse);

}
void WhileNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkMember(mCondition);
    WalkMember(mScope);
}
void ForNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    StatementNode::Walk(visitor, false);
    WalkExisting(mInitialVariable);
    WalkExisting(mInitialExpression);
    WalkExisting(mCondition);
    WalkExisting(mIterator);
    WalkMember(mScope);
}
void LiteralNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    ExpressionNode::Walk(visitor, false);
}
void NameReferenceNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    ExpressionNode::Walk(visitor, false);
}
void BinaryOperatorNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    ExpressionNode::Walk(visitor, false);
    WalkMember(mLeft);
    WalkMember(mRight);
}
void UnaryOperatorNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    ExpressionNode::Walk(visitor, false);
    WalkMember(mRight);
}
void PostExpressionNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    ExpressionNode::Walk(visitor, false);
    WalkMember(mLeft);
}
void MemberAccessNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    PostExpressionNode::Walk(visitor, false);
}
void CallNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    PostExpressionNode::Walk(visitor, false);
    WalkLoop(mArguments);
}
void CastNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    PostExpressionNode::Walk(visitor, false);
    WalkMember(mType);
}
void IndexNode::Walk(Visitor* visitor, bool visit)
{
    VisitNode;
    PostExpressionNode::Walk(visitor, false);
    WalkMember(mIndex);
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