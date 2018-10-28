/******************************************************************\
 * Author: 
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/
#include "../Drivers/Driver2.hpp"

class Acceptor
{
public:
    static void Initialize(std::vector<Token> & token_stream);
    static bool AllTokensCovered();
    static bool Accept(TokenType::Enum next_token);
    static bool Expect(TokenType::Enum next_token);
    static bool Expect(bool check);
    static unsigned _index;
    static std::vector<Token> * _token_stream;
};

unsigned Acceptor::_index = 0;
std::vector<Token> * Acceptor::_token_stream = nullptr;

void Acceptor::Initialize(std::vector<Token> & token_stream)
{
    _index = 0;
    _token_stream = &token_stream;
}

bool Acceptor::AllTokensCovered()
{
    if (_index == _token_stream->size())
    {
        return true;
    }
    return false;
}

bool Acceptor::Accept(TokenType::Enum accepted_token)
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

bool Acceptor::Expect(TokenType::Enum accepted_token)
{

    if(Accept(accepted_token))
    {
        return true;
    }
    throw ParsingException();
}

bool Acceptor::Expect(bool check)
{
    if (check)
    {
        return true;
    }
    throw ParsingException();
}

bool Block();
bool Class();
bool Var();
bool Function();
bool Parameter();
bool SpecifiedType();
bool Type();
bool NamedType();
bool FunctionType();
bool Scope();
bool Statement();
bool DelimitedStatement();
bool FreeStatement();
bool Label();
bool Goto();
bool Return();
bool If();
bool Else();
bool While();
bool For();
bool GroupedExpression();
bool Literal();
bool NameReference();
bool Value();
bool Expression();
bool Expression1();
bool Expression2();
bool Expression3();
bool Expression4();
bool Expression5();
bool Expression6();
bool Expression7();
bool MemberAccess();
bool Call();
bool Cast();
bool Index();

bool Index()
{
    PrintRule print_rule("Index");
    if(!Acceptor::Accept(TokenType::OpenBracket))
    {
        return false;
    }
    Acceptor::Expect(Expression());
    Acceptor::Expect(TokenType::CloseBracket);
    print_rule.Accept();
    return true;
}

bool Cast()
{
    PrintRule print_rule("Cast");
    if (!Acceptor::Accept(TokenType::As))
    {
        return false;
    }
    Acceptor::Expect(Type());
    print_rule.Accept();
    return true;
}

bool Call()
{
    PrintRule print_rule("Call");
    if (!Acceptor::Accept(TokenType::OpenParentheses))
    {
        return false;
    }

    if (Expression())
    {
        while (Acceptor::Accept(TokenType::Comma))
        {
            Acceptor::Expect(Expression());
        }
    }

    Acceptor::Expect(TokenType::CloseParentheses);
    print_rule.Accept();
    return true;

}

bool MemberAccess()
{
    PrintRule print_rule("MemberAccess");
    if (!(Acceptor::Accept(TokenType::Dot) ||
          Acceptor::Accept(TokenType::Arrow)))
    {
        return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    print_rule.Accept();
    return true;

}

bool Expression7()
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

bool Expression6()
{
    PrintRule print_rule("Expression6");
    
    while(Acceptor::Accept(TokenType::Asterisk) ||
          Acceptor::Accept(TokenType::Ampersand) ||
          Acceptor::Accept(TokenType::Plus) || 
          Acceptor::Accept(TokenType::Minus) || 
          Acceptor::Accept(TokenType::LogicalNot) || 
          Acceptor::Accept(TokenType::Increment) || 
          Acceptor::Accept(TokenType::Decrement));

    bool result = Expression7();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Expression5()
{
    PrintRule print_rule("Expression5");
    if (!Expression6())
    {
        return false;
    }

    while (Acceptor::Accept(TokenType::Asterisk) ||
           Acceptor::Accept(TokenType::Divide) || 
           Acceptor::Accept(TokenType::Modulo))
    {
        Acceptor::Expect(Expression6());
    }

    print_rule.Accept();
    return true;
}

bool Expression4()
{
    PrintRule print_rule("Expression4");
    if (!Expression5())
    {
        return false;
    }

    while (Acceptor::Accept(TokenType::Plus) ||
           Acceptor::Accept(TokenType::Minus))
    {
        Acceptor::Expect(Expression5());
    }

    print_rule.Accept();
    return true;
}

bool Expression3()
{
    PrintRule print_rule("Expression3");
    if (!Expression4())
    {
        return false;
    }

    while (Acceptor::Accept(TokenType::LessThan) ||
           Acceptor::Accept(TokenType::GreaterThan) ||
           Acceptor::Accept(TokenType::LessThanOrEqualTo) ||
           Acceptor::Accept(TokenType::GreaterThanOrEqualTo) ||
           Acceptor::Accept(TokenType::Equality) ||
           Acceptor::Accept(TokenType::Inequality))
    {
        Acceptor::Expect(Expression4());
    }

    print_rule.Accept();
    return true;
}

bool Expression2()
{
    PrintRule print_rule("Expression2");
    if (!Expression3())
    {
        return false;
    }
    while (Acceptor::Accept(TokenType::LogicalAnd))
    {
        Acceptor::Expect(Expression3());
    }

    print_rule.Accept();
    return true;
}

bool Expression1()
{
    PrintRule print_rule("Expression1");
    if (!Expression2())
    {
        return false;
    }
    while (Acceptor::Accept(TokenType::LogicalOr))
    {
        Acceptor::Expect(Expression2());
    }

    print_rule.Accept();
    return true;
}

bool Expression()
{
    PrintRule print_rule("Expression");
    if (!Expression1())
    {
        return false;
    }
    if(Acceptor::Accept(TokenType::Assignment) ||
       Acceptor::Accept(TokenType::AssignmentPlus) ||
       Acceptor::Accept(TokenType::AssignmentMinus) ||
       Acceptor::Accept(TokenType::AssignmentMultiply) ||
       Acceptor::Accept(TokenType::AssignmentDivide) ||
       Acceptor::Accept(TokenType::AssignmentModulo))
    {
        Acceptor::Expect(Expression());
    }

    print_rule.Accept();
    return true;
}

bool Value()
{
    PrintRule print_rule("Value");
    bool result = Literal() || NameReference() || GroupedExpression();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool NameReference()
{
    PrintRule print_rule("NameReference");
    if (!Acceptor::Accept(TokenType::Identifier))
    {
        return false;
    }
    print_rule.Accept();
    return true;
}

bool Literal()
{
    PrintRule print_rule("Literal");
    bool result = Acceptor::Accept(TokenType::True) ||
                  Acceptor::Accept(TokenType::False) ||
                  Acceptor::Accept(TokenType::Null) ||
                  Acceptor::Accept(TokenType::IntegerLiteral) ||
                  Acceptor::Accept(TokenType::FloatLiteral) ||
                  Acceptor::Accept(TokenType::StringLiteral) ||
                  Acceptor::Accept(TokenType::CharacterLiteral);
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool GroupedExpression()
{
    PrintRule print_rule("GroupedExpression");
    if (!Acceptor::Accept(TokenType::OpenParentheses))
    {
        return false;
    }
    Acceptor::Expect(Expression());
    Acceptor::Expect(TokenType::CloseParentheses);
    print_rule.Accept();
    return true;
}

bool For()
{
    PrintRule print_rule("For");
    if (!Acceptor::Accept(TokenType::For))
    {
        return false;
    }
    Acceptor::Expect(TokenType::OpenParentheses);
    Var() || Expression();
    Acceptor::Expect(TokenType::Semicolon);
    Expression();
    Acceptor::Expect(TokenType::Semicolon);
    Expression();
    Acceptor::Expect(TokenType::CloseParentheses);
    Acceptor::Expect(Scope());

    print_rule.Accept();
    return true;
}

bool While()
{
    PrintRule print_rule("While");
    if (!Acceptor::Accept(TokenType::While))
    {
        return false;
    }
    Acceptor::Expect(GroupedExpression());
    Acceptor::Expect(Scope());
    print_rule.Accept();
    return true;

}

bool Else()
{
    PrintRule print_rule("Else");
    if (!Acceptor::Accept(TokenType::Else))
    {
        return false;
    }
    Acceptor::Expect(If() || Scope());
    print_rule.Accept();
    return true;
}

bool If()
{
    PrintRule print_rule("If");
    if (!Acceptor::Accept(TokenType::If))
    {
        return false;
    }
    Acceptor::Expect(GroupedExpression());
    Acceptor::Expect(Scope());
    Else();

    print_rule.Accept();
    return true;
}

bool Return()
{
    PrintRule print_rule("Return");
    if (Acceptor::Accept(TokenType::Return))
    {
        Expression();
        print_rule.Accept();
        return true;
    }
    return false;
}

bool Goto()
{
    PrintRule print_rule("Goto");
    bool result = Acceptor::Accept(TokenType::Goto) &&
        Acceptor::Expect(TokenType::Identifier);
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Label()
{
    PrintRule print_rule("Label");
    bool result = Acceptor::Accept(TokenType::Label) && 
                  Acceptor::Expect(TokenType::Identifier);
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool FreeStatement()
{
    PrintRule print_rule("FreeStatement");
    bool result = If() || While() || For();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool DelimitedStatement()
{
    PrintRule print_rule("DelimitedStatement");
    bool result = Label() || 
                  Goto() || 
                  Return() || 
                  Acceptor::Accept(TokenType::Break) ||
                  Acceptor::Accept(TokenType::Continue) ||
                  Var() ||
                  Expression();
    if (result)
    {
        print_rule.Accept();
    }
    return result;
        
}

bool Statement()
{
    PrintRule print_rule("Statement");
    bool result =  FreeStatement() || (DelimitedStatement() && 
                                       Acceptor::Expect(TokenType::Semicolon));
    if(result)
    {
        print_rule.Accept();
    }
    return result;
}

bool Scope()
{
    PrintRule print_rule("Scope");
    if (!Acceptor::Accept(TokenType::OpenCurley))
    {
        return false;
    }
    while(Statement());
    Acceptor::Expect(TokenType::CloseCurley);
    print_rule.Accept();
    return true;
}

bool FunctionType()
{
    PrintRule print_rule("FunctionType");
    if (!Acceptor::Accept(TokenType::Function))
    {
        return false;
    }

    Acceptor::Expect(TokenType::Asterisk);
    Acceptor::Accept(TokenType::Ampersand);

    Acceptor::Expect(TokenType::OpenParentheses);
    Acceptor::Expect(Type());

    while(Acceptor::Accept(TokenType::Comma) && Acceptor::Expect(Type()));
    Acceptor::Expect(TokenType::CloseParentheses);
    SpecifiedType();
    print_rule.Accept();

    return true;
}

bool NamedType()
{
    PrintRule print_rule("NamedType");
    if (!Acceptor::Accept(TokenType::Identifier))
    {
        return false;
    }
    while(Acceptor::Accept(TokenType::Asterisk));
    Acceptor::Accept(TokenType::Ampersand);
    print_rule.Accept();
    return true;
}

bool Type()
{
    PrintRule print_rule("Type");
    bool result = NamedType() || FunctionType();
    
    if (result)
    {
        print_rule.Accept();
    }
    return result;
}

bool SpecifiedType()
{
    PrintRule print_rule("SpecifiedType");
    if (!Acceptor::Accept(TokenType::Colon))
    {
        return false;
    }
    Acceptor::Expect(Type());
    print_rule.Accept();
    return true;
}

bool Parameter()
{
    PrintRule print_rule("Parameter");
    if (!Acceptor::Accept(TokenType::Identifier))
    {
        return false;
    }
    Acceptor::Expect(SpecifiedType());
    print_rule.Accept();
    return true;
}

bool Function()
{
    PrintRule print_rule("Function");
    if (!Acceptor::Accept(TokenType::Function))
    {
        return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    Acceptor::Expect(TokenType::OpenParentheses);

    if (Parameter())
    {
        while(Acceptor::Accept(TokenType::Comma))
        {
            Acceptor::Expect(Parameter());
        }
    }

    Acceptor::Expect(TokenType::CloseParentheses);
    SpecifiedType();
    Acceptor::Expect(Scope());
    
    print_rule.Accept();
    return true;
}

bool Var()
{
    PrintRule print_rule("Var");
    if (!Acceptor::Accept(TokenType::Var))
    {
      return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    Acceptor::Expect(SpecifiedType());

    if (Acceptor::Accept(TokenType::Assignment))
    {
        Acceptor::Expect(Expression());
    }

    print_rule.Accept();
    return true;
}

bool Class()
{
    PrintRule print_rule("Class");
    if(!Acceptor::Accept(TokenType::Class))
    {
        return false;
    }

    Acceptor::Expect(TokenType::Identifier);
    Acceptor::Expect(TokenType::OpenCurley);

    while(true)
    {
        if(Var())
        {
            Acceptor::Expect(TokenType::Semicolon);
        }
        else if(!Function())
        {
            break;
        }
    }

    Acceptor::Expect(TokenType::CloseCurley);

    print_rule.Accept();
    return true;
}


bool Block()
{
    PrintRule print_rule("Block");
    while(Class() || 
          Function() || 
          (Var() && Acceptor::Expect(TokenType::Semicolon)));
    print_rule.Accept();
    return false;
}

void RemoveWhitespaceAndComments(std::vector<Token>& tokens)
{
    // Create new vector for all non whitespace and non comment tokens.
    std::vector<Token> stripped;
    stripped.reserve(tokens.size());

    // Add acceptable tokens to the stripped vector.
    for(const Token & token : tokens)
    {
        if(token.mTokenType != TokenType::Whitespace && 
           token.mTokenType != TokenType::SingleLineComment &&
           token.mTokenType != TokenType::MultiLineComment)
        {
            stripped.push_back(token);
        }
    }

    stripped.shrink_to_fit();
    tokens = stripped;
}

void Recognize(std::vector<Token>& tokens)
{
    Acceptor::Initialize(tokens);
    Block();
    Acceptor::Expect(Acceptor::AllTokensCovered());
}
