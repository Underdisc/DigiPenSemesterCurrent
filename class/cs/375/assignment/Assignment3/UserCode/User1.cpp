/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/

#include <iostream>
#include <unordered_map>
#include <vector>

#include "../Drivers/Driver1.hpp"

class Range
{
public:
    Range(char start, char end);
    bool Within(char c) const;
private:
    char _start;
    char _end;
};

Range::Range(char start, char end)
{
    _start = start;
    _end = end;
}

bool Range::Within(char c) const
{
    return (_start <= c && _end >= c);
}

class Qualifier
{
public:
    void AddWhitelistChar(char c);
    void AddWhitelistRange(char start, char end);
    void AddWhitelistRange(Range range);
    void Clear();
    bool Qualify(char c) const;
    bool QualifySpecific(char c) const;
private:
    std::vector<char> _whitelist_chars;
    std::vector<Range> _whitelist_ranges;
};

void Qualifier::AddWhitelistChar(char c)
{
    _whitelist_chars.push_back(c);
}

void Qualifier::AddWhitelistRange(char start, char end)
{
    _whitelist_ranges.push_back(Range(start, end));
}

void Qualifier::AddWhitelistRange(Range range)
{
    _whitelist_ranges.push_back(range);
}

void Qualifier::Clear()
{
    _whitelist_chars.clear();
    _whitelist_ranges.clear();
}

bool Qualifier::Qualify(char c) const
{
    // Test to see if the character is equivalent to any single characters.
    for(char compare_c : _whitelist_chars)
    {
        if(compare_c == c)
        {
            return true;
        }
    }

    // Test to see if the character is within any of the ranges.
    for(const Range & range : _whitelist_ranges)
    {
        if(range.Within(c))
        {
            return true;
        }
    }

    // The character did not qualify for this qualifier.
    return false;
}

bool Qualifier::QualifySpecific(char c) const
{
    if(_whitelist_chars.size() != 1 || _whitelist_ranges.size() != 0)
    {
        return false;
    }

    if(_whitelist_chars[0] == c)
    {
        return true;
    }
    return false;
}

class DfaState
{
public:
    void Init(int acceptingToken);
    void DeleteAllChildren();
    void AddEdge(DfaState * to, const Qualifier & q);
    void AddDefault(DfaState * to);
    void MakeAccepting(int accepting_token);
    DfaState * FindNextState(char c);
    DfaState * FindNextStateSpecific(char c);

    int _accepting_token;

private:
    struct Edge
    {
        DfaState * _next;
        Qualifier _qualifier;
        bool _infinite_recursion;
    };
    bool FindState(DfaState * state);

    std::vector<Edge> _edges;
    DfaState * _default;
    bool _default_infinite_recursion;
    int _parents;
};


void DfaState::Init(int acceptingToken)
{
    _accepting_token = acceptingToken;
    _default = nullptr;
    _default_infinite_recursion = false;
    _parents = 0;
}

void DfaState::DeleteAllChildren()
{
    for(const Edge & edge : _edges)
    {
        if(edge._infinite_recursion)
        {
            continue;
        }
        edge._next->DeleteAllChildren();
        --edge._next->_parents;
        if(edge._next->_parents <= 0)
        {
            delete edge._next;
        }
    }
    _edges.clear();

    if(_default && !_default_infinite_recursion)
    {
        _default->DeleteAllChildren();
        --_default->_parents;
        if(_default->_parents <= 0)
        {
            delete _default;
        }
    }
}

void DfaState::AddEdge(DfaState * to, const Qualifier & q)
{
    bool state_found = to->FindState(this);

    // Create a new edge.
    Edge new_edge;
    new_edge._next = to;
    new_edge._qualifier = q;
    new_edge._infinite_recursion = state_found;
    _edges.push_back(new_edge);

    // Increment the number of parents the child state has.
    if(!state_found)
    {
        ++to->_parents;
    }
}

void DfaState::AddDefault(DfaState * to)
{
    bool state_found = to->FindState(this);

    _default = to;
    _default_infinite_recursion = state_found;

    // Increment the number of parents the child state has.
    if(!state_found)
    {
        ++to->_parents;
    }
}

void DfaState::MakeAccepting(int accepting_token)
{
    _accepting_token = accepting_token;
}

DfaState * DfaState::FindNextState(char c)
{
    // Find the edge that uses the given token and return the state that the
    // edge leads to.
    for(const Edge & edge : _edges)
    {
        if(edge._qualifier.Qualify(c))
        {
            return edge._next;
        }
    }

    // If we hit a null terminator, there is no state to transition to.
    if(c == 0)
    {
        return nullptr;
    }

    // If an edge was not found and the default edge exist, we return the
    // default edge.
    if(_default)
    {
        return _default;
    }
    return nullptr;
}

DfaState * DfaState::FindNextStateSpecific(char c)
{
    //
    for(const Edge & edge : _edges)
    {
        
        if(edge._qualifier.QualifySpecific(c))
        {
            return edge._next;
        }
    }
    return nullptr;
}

bool DfaState::FindState(DfaState * state)
{
    if(this == state)
    {
        return true;
    }

    for(const Edge & edge : _edges)
    {
        if(edge._infinite_recursion)
        {
            continue;
        }
        if(edge._next->FindState(state))
        {
            return true;
        }
    }
    return false;
}

// Assignment Implementation

DfaState* AddState(int acceptingToken)
{
    DfaState * new_state = new DfaState;
    new_state->Init(acceptingToken);
    return new_state;
}

void AddEdge(DfaState* from, DfaState* to, char c)
{
    Qualifier q;
    q.AddWhitelistChar(c);
    from->AddEdge(to, q);
}

void AddEdge(DfaState * from, DfaState * to, const Qualifier & q)
{
    from->AddEdge(to, q);
}

void AddDefaultEdge(DfaState* from, DfaState* to)
{
    from->AddDefault(to);
}

void ReadToken(DfaState* starting_state, const char* stream, Token& out_token)
{
    char current_char = *stream;
    int last_accept_index = 0;
    int current_index = 0;
    DfaState * last_accept_state = nullptr;
    DfaState * current_state = starting_state->FindNextState(current_char);

    // Progress until we find a character that does not correspond to an edge
    // of the state we are currently on.
    while(current_state)
    {
        if(current_state->_accepting_token)
        {
            last_accept_index = current_index;
            last_accept_state = current_state;
        }

        ++current_index;
        current_char = stream[current_index];
        current_state = current_state->FindNextState(current_char);
    }

    if(last_accept_state)
    {
        out_token.mText = stream;
        out_token.mLength = last_accept_index + 1;
        out_token.mTokenType = last_accept_state->_accepting_token;
    }
    else
    {
        out_token.mText = stream;
        out_token.mLength = current_index;
        out_token.mTokenType = 0;
    }
}

void DeleteStateAndChildren(DfaState* root)
{
    root->DeleteAllChildren();
    delete root;
}

typedef std::unordered_map<std::string, int> keyword_map;

class Language
{
public:
    static void Init();
    static void Delete();
    static void AddLanguageDfa();
    static void AddSymbolsAndKeywords();
    static void AddSymbolDfa(const char * value, int type);

    static DfaState * _root;
    static keyword_map _keyword_tokens;
};

DfaState * Language::_root = nullptr;
keyword_map Language::_keyword_tokens = keyword_map(); 

void Language::Init()
{
    AddLanguageDfa();
    AddSymbolsAndKeywords();
}

void Language::AddLanguageDfa()
{
    _root = AddState(0);

    // Creating states and edges for identifiers.
    {
        DfaState * id0 = AddState(TokenType::Identifier);
        Qualifier q;
        q.AddWhitelistRange('a', 'z');
        q.AddWhitelistRange('A', 'Z');
        q.AddWhitelistChar('_');
        AddEdge(_root, id0, q);
        q.AddWhitelistRange('0', '9');
        AddEdge(id0, id0, q);
    }

    // We create a single state for the beginning of integer and float literals
    // since they start with the same range of values.
    DfaState * nl0 = AddState(TokenType::IntegerLiteral);
    
    // Create the edges for integer literals.
    {
        Qualifier q;
        q.AddWhitelistRange('0', '9');
        AddEdge(_root, nl0, q);
        AddEdge(nl0, nl0, q);
    }

    // Create the states and edges for float literals.
    {
        DfaState * fl0 = AddState(0);
        DfaState * fl1 = AddState(TokenType::FloatLiteral);
        DfaState * fl2 = AddState(TokenType::FloatLiteral);
        DfaState * fl3 = AddState(0);
        DfaState * fl4 = AddState(0);
        DfaState * fl5 = AddState(TokenType::FloatLiteral);

        Qualifier q;
        q.AddWhitelistChar('.');
        AddEdge(nl0, fl0, q);

        q.Clear();
        q.AddWhitelistRange('0', '9');
        AddEdge(fl0, fl1, q);
        AddEdge(fl1, fl1, q);

        q.Clear();
        q.AddWhitelistChar('f');
        AddEdge(fl1, fl2, q);

        q.Clear();
        q.AddWhitelistChar('e');
        AddEdge(fl1, fl3, q);

        q.Clear();
        q.AddWhitelistChar('-');
        q.AddWhitelistChar('+');
        AddEdge(fl3, fl4, q);

        q.Clear();
        q.AddWhitelistRange('0', '9');
        AddEdge(fl3, fl5, q);
        AddEdge(fl4, fl5, q);
        AddEdge(fl5, fl5, q);

        q.Clear();
        q.AddWhitelistChar('f');
        AddEdge(fl5, fl2, q);
    }

    // Create the states and edges for string literals.
    {
        DfaState * sl0 = AddState(0);
        DfaState * sl1 = AddState(0);
        DfaState * sl2 = AddState(TokenType::StringLiteral);

        Qualifier q;
        q.AddWhitelistChar('"');
        AddEdge(_root, sl0, q);

        q.Clear();
        q.AddWhitelistChar('\\');
        AddEdge(sl0, sl1, q);

        q.Clear();
        q.AddWhitelistChar('r');
        q.AddWhitelistChar('n');
        q.AddWhitelistChar('t');
        q.AddWhitelistChar('"');
        AddEdge(sl1, sl0, q);

        q.Clear();
        q.AddWhitelistChar('"');
        AddEdge(sl0, sl2, q);

        AddDefaultEdge(sl0, sl0);
    }

    // Create the states and edges for character literals.
    {
        DfaState * cl0 = AddState(0);
        DfaState * cl1 = AddState(0);
        DfaState * cl2 = AddState(TokenType::CharacterLiteral);

        Qualifier q;
        q.AddWhitelistChar('\'');
        AddEdge(_root, cl0, q);

        q.Clear();
        q.AddWhitelistChar('\\');
        AddEdge(cl0, cl1, q);

        q.Clear();
        q.AddWhitelistChar('r');
        q.AddWhitelistChar('n');
        q.AddWhitelistChar('t');
        q.AddWhitelistChar('\'');
        AddEdge(cl1, cl0, q);

        q.Clear();
        q.AddWhitelistChar('\'');
        AddEdge(cl0, cl2, q);

        AddDefaultEdge(cl0, cl0);
    }

    // Create the states and edges for whitepspace.
    {
        DfaState * w0 = AddState(TokenType::Whitespace);

        Qualifier q;
        q.AddWhitelistChar(' ');
        q.AddWhitelistChar('\r');
        q.AddWhitelistChar('\n');
        q.AddWhitelistChar('\t');
        AddEdge(_root, w0, q);
        AddEdge(w0, w0, q);
    }
    
    // Create a state that is shared by the start of both single and multi line
    // comments.
    DfaState * c0 = AddState(0);
    
    {
        Qualifier q;
        q.AddWhitelistChar('/'); 
        AddEdge(_root, c0, q);
    }

    // Create the states and edges for single line comments.
    {
        DfaState * slc1 = AddState(0);
        DfaState * slc2 = AddState(TokenType::SingleLineComment);

        Qualifier q;
        q.AddWhitelistChar('/');
        AddEdge(c0, slc1, q);

        q.Clear();
        q.AddWhitelistChar('\r');
        q.AddWhitelistChar('\n');
        q.AddWhitelistChar('\0');
        AddEdge(slc1, slc2, q);

        AddDefaultEdge(slc1, slc1);
    }

    // Create the states and edges for multiline comments.
    {
        DfaState * mlc1 = AddState(0);
        DfaState * mlc2 = AddState(0);
        DfaState * mlc3 = AddState(TokenType::MultiLineComment);

        Qualifier q;
        q.AddWhitelistChar('*');
        AddEdge(c0, mlc1, q);

        q.Clear();
        q.AddWhitelistChar('*');
        AddEdge(mlc1, mlc2, q);

        q.Clear();
        q.AddWhitelistChar('/');
        AddEdge(mlc2, mlc3, q);

        AddDefaultEdge(mlc1, mlc1);
        AddDefaultEdge(mlc2, mlc1);
    }
}

void Language::AddSymbolsAndKeywords()
{
    // Get an array for the values of all of the tokens.
    const char * token_values[] =
    {
        #define TOKEN(Name, Value) Value,
        #include "../Drivers/Tokens.inl"
        #undef TOKEN 
    };

    // Create the states and edges need for all of the different symbols.
    for(int i = TokenType::SymbolStart + 1; i < TokenType::KeywordStart; ++i)
    {
        AddSymbolDfa(token_values[i], i);
    }

    // Add all of the keywords to the keyword map.
    for(int i = TokenType::KeywordStart + 1; i <= TokenType::Yield; ++i)
    {
        std::string keyword(token_values[i]);
        _keyword_tokens.insert({keyword, i});
    }
}

void Language::AddSymbolDfa(const char * symbol, int type)
{
    const char * value = symbol;
    DfaState * current_state = _root;
    while(*value != '\0')
    {
        DfaState * next_state = nullptr;
        if(current_state)
        {
            next_state = current_state->FindNextStateSpecific(*value);
        }

        if(!next_state)
        {
            DfaState * new_state = AddState(0);
            Qualifier q;
            q.AddWhitelistChar(*value);
            current_state->AddEdge(new_state, q);
            current_state = new_state;
        }
        else
        {
            current_state = next_state;
        }
        ++value;
    }
    current_state->MakeAccepting(type);
}

DfaState * CreateLanguageDfa()
{
    Language::Init();
    return Language::_root;
}

void ReadLanguageToken(DfaState* starting_state,
                       const char* stream,
                       Token& out_token)
{
    ReadToken(starting_state, stream, out_token);
    if(out_token.mTokenType == TokenType::Identifier)
    {
        std::string identifier(stream, out_token.mLength);
        keyword_map::iterator it;
        it = Language::_keyword_tokens.find(identifier);
        if(it != Language::_keyword_tokens.end())
        {
            out_token.mTokenType = it->second;
        }
    }
    return;
}

