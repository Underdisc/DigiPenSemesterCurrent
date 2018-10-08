#include <vector>
#include <sstream>

using namespace std;

class node
{
public:
  char token;
  vector<node*> children;
};

class parser
{
public:

  node* e0()
  {
    node* lhs = e1();
    if (!lhs)
      return nullptr;

    while (*input == '+' || *input == '-')
    {
      node* op = new node{ *input };
      ++input;

      node* rhs = e1(); // expect

      op->children.push_back(lhs);
      op->children.push_back(rhs);

      lhs = op;
    }

    return lhs;
  }

  node* e1()
  {
    node* lhs = e2();
    if (!lhs)
      return nullptr;

    while (*input == '*' || *input == '/')
    {
      node* op = new node{ *input };
      ++input;

      node* rhs = e2(); // expect

      op->children.push_back(lhs);
      op->children.push_back(rhs);
      
      lhs = op;
    }

    return lhs;
  }

  node* e2()
  {
    if (!(*input == '+' || *input == '-'))
      return e3();
    node* op = new node{ *input };
    ++input;

    node* rhs = e2(); // expect
    op->children.push_back(rhs);

    return op;
  }

  node* e3()
  {
    if (*input != '(')
      return n();
    ++input;

    node* value = e0(); // expect

    ++input; // expect ')'
    return value;
  }

  node* n()
  {
    char n = *input;
    if (!(n >= '0' && n <= '9'))
      return nullptr;
    ++input;

    return new node{ n };
  }

  const char* input;
};

void graph_recursive(stringstream& stream, node* parent)
{
  stream << "  \"" << parent << "\" [label=\"" << parent->token << "\"]" << endl;

  for (node* child : parent->children)
  {
    stream << "  \"" << parent << "\" -> \"" << child << "\"" << endl;
    graph_recursive(stream, child);
  }
}

string graph(node* root)
{
  stringstream stream;
  stream << "digraph G" << endl;
  stream << "{" << endl;

  graph_recursive(stream, root);

  stream << "}" << endl;
  return stream.str();
}

int main(void)
{
  parser parse;
  parse.input = "6*9+4*2+-0";

  node* root = parse.e0();
  string g = graph(root);
  return 0;
}


