/******************************************************************\
 * Author: Connor Deakin
 * Copyright 2015, DigiPen Institute of Technology
\******************************************************************/

#include <vector>

#include "../Drivers/Driver1.hpp"


class DfaState
{
public:
    void Init(int acceptingToken);
    void AddEdge(DfaState * to, char c);
    void AddDefault(DfaState * to);
    DfaState * FindNextSatate(char c);

    int _acceptingToken;

private:
    struct Edge
    {
        DfaState * _next;
        char _token;
    };

    std::vector<Edge> _edges;
    DfaState * _default;
};


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
    for(const Edge & edge : _edges)
    {
        if(c == edge._token)
        {
            return edge._next;
        }
    }
    return nullprt;
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

void ReadToken(DfaState* startingState, const char* stream, Token& outToken)
{
    D
}

void DeleteStateAndChildren(DfaState* root)
{
}

void ReadLanguageToken(DfaState* startingState, const char* stream, Token& outToken)
{
}

DfaState* CreateLanguageDfa()
{
    return nullptr;
}
