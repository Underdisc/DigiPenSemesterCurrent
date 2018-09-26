/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/

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
    void AddChar(char c);
    void AddRange(char start, char end);
    void AddRange(Range range);
    void Clear();
    bool Qualify(char c) const;
private:
    std::vector<char> _chars;
    std::vector<Range> _ranges;
};

void Qualifier::AddChar(char c)
{
    _chars.push_back(c);
}

void Qualifier::AddRange(char start, char end)
{
    _ranges.push_back(Range(start, end));
}

void Qualifier::AddRange(Range range)
{
    _ranges.push_back(range);
}

void Qualifier::Clear()
{
    _chars.clear();
    _ranges.clear();
}

bool Qualifier::Qualify(char c) const
{
    // Test to see if the character is equivalent to any single characters.
    for(char compare_c : _chars)
    {
        if(compare_c == c)
        {
            return true;
        }
    }

    // Test to see if the character is within any of the ranges.
    for(const Range & range : _ranges)
    {
        if(range.Within(c))
        {
            return true;
        }
    }

    // The character did not qualify for this qualifier.
    return false;
}


class DfaState
{
public:
    void Init(int acceptingToken);
    void DeleteAllChildren();
    void AddEdge(DfaState * to, const Qualifier & q);
    void AddDefault(DfaState * to);
    DfaState * FindNextState(char c);
    

    int _acceptingToken;

private:
    struct Edge
    {
        DfaState * _next;
        Qualifier _qualifier;
    };
    Edge * FindEdge(DfaState * to);

    std::vector<Edge> _edges;
    DfaState * _default;
};


void DfaState::Init(int acceptingToken)
{
    _acceptingToken = acceptingToken;
    _default = nullptr;
}

void DfaState::DeleteAllChildren()
{
    for(const Edge & edge : _edges)
    {
        if(edge._next != this)
        {
            edge._next->DeleteAllChildren();
            delete edge._next;
        }
    }
    _edges.clear();

    if(_default && _default != this)
    {
        _default->DeleteAllChildren();
        delete _default;
    }
}

void DfaState::AddEdge(DfaState * to, const Qualifier & q)
{
    // Create a new edge.
    Edge new_edge;
    new_edge._next = to;
    new_edge._qualifier = q;
    _edges.push_back(new_edge);
}

void DfaState::AddDefault(DfaState * to)
{
    _default = to;
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

DfaState::Edge * DfaState::FindEdge(DfaState * to)
{
    // Find the edge that goes to the given state and return a nullptr if it
    // does not exist.
    for(Edge & edge : _edges)
    {
        if(edge._next == to)
        {
            return &edge;
        }
    }
    return nullptr;
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
    q.AddChar(c);
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
        if(current_state->_acceptingToken)
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
        out_token.mTokenType = last_accept_state->_acceptingToken;
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
    //root->DeleteAllChildren();
    //delete root;
}

void ReadLanguageToken(DfaState* startingState,
                       const char* stream,
                       Token& outToken)
{

}

DfaState* CreateLanguageDfa()
{
    DfaState * root = AddState(0);

    // Creating states and edges for identifiers.
    {
        DfaState * id0 = AddState(TokenType::Identifier);
        Qualifier q;
        q.AddRange('a', 'z');
        q.AddRange('A', 'Z');
        q.AddChar('_');
        AddEdge(root, id0, q);
        q.AddRange('0', '9');
        AddEdge(id0, id0, q);
    }

    // We create a single state for the beginning of integer and float literals
    // since they start with the same range of values.
    DfaState * nl0 = AddState(TokenType::IntegerLiteral);
    
    // Create the edges for integer literals.
    {
        Qualifier q;
        q.AddRange('0', '9');
        AddEdge(root, nl0, q);
        AddEdge(nl0, nl0, q);
    }

    // Create the states and edges for float literals.
    {
        DfaState * fl0 AddState(0);
        DfaState * fl1 AddState(TokenType::FloatLiteral);
        DfaState * fl2 AddState(TokenType::FloatLiteral);
        DfaState * fl3 AddState(0);
        DfaState * fl4 AddState(0);
        DfaState * fl5 AddState(TokenType::FloatLiteral);

        Qualifier q;
        q.AddChar('.');
        AddEdge(nl0, fl0, q);

        q.Clear();
        q.AddRange('0', '9');
        AddEdge(fl0, fl1, q);
        AddEdge(fl1, fl1, q);

        q.Clear()
        q.AddChar('f');
        AddEdge(fl1, fl2, q);

        q.Clear();
        q.AddChar('e');
        AddEdge(f11, fl3, q);

        q.Clear();
        q.AddChar('-');
        q.AddChar('+');
        AddEdge(fl3, fl4, q);

        q.Clear();
        q.AddRange('0', '9');
        AddEdge(fl3, fl5, q);
        AddEdge(fl4, fl5, q);
        AddEdge(fl5, fl5, q);

        q.Clear()
        q.AddChar('f');
        AddEdge(fl5, fl2, q);
    }

    // Create the states and edges from string literals
    {

    }



    return root;
}
