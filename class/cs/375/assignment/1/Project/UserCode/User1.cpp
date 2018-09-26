/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/

#include <vector>

#include "../Drivers/Driver1.hpp"

class Range
{
public:
    void Init(char start, char end);
    bool Within(char c);
private:
    char _start;
    char _end;
};

void Range::Init(char start, char end)
{
    _start = start;
    _end = end;
}

bool Range::Within(char c)
{
    return (_start <= c && _end >= c);
}



class DfaState
{
public:
    void Init(int acceptingToken);
    void DeleteAllChildren();
    void AddEdge(DfaState * to, char c);
    void AddDefault(DfaState * to);
    DfaState * FindNextState(char c);
    

    int _acceptingToken;

private:
    struct Edge
    {
        bool Qualify(char c);
        DfaState * _next;
        std::vector<char> _single_chars;
        std::vector<Range> _ranges;
    };

    std::vector<Edge> _edges;
    DfaState * _default;
};

bool DfaState::Edge::Qualify(char c)
{
    // Test to see if the character is equivalent to any of the single 
    // characters for this edge.
    for(char compare_c : _single_chars)
    {
        if(compare_c == c)
        {
            return true;
        }
    }

    // Test to see if the character is within any of the ranges used on this
    // edge.
    for(const Range & range : _ranges)
    {
        if(range.Within(c))
        {
            return true;
        }
    }
    
    // If the character was not found within the single characters or ranges,
    // the character is not qualified for passing over the edge.
    return false;
}


void DfaState::Init(int acceptingToken)
{
    _acceptingToken = acceptingToken;
    _default = nullptr;
}

void DfaState::AddEdge(DfaState * to, char c)
{
    Edge new_edge;
    new_edge._next = to;
    new_edge._token = c;
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
        if(c == edge._token)
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

// Assignment Implementation

DfaState* AddState(int acceptingToken)
{
    DfaState * new_state = new DfaState;
    new_state->Init(acceptingToken);
    return new_state;
}

void AddEdge(DfaState* from, DfaState* to, char c)
{
    from->AddEdge(to, c);
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
    root->DeleteAllChildren();
    delete root;
}

void ReadLanguageToken(DfaState* startingState, const char* stream, Token& outToken)
{
}

DfaState* CreateLanguageDfa()
{
    return nullptr;
}
