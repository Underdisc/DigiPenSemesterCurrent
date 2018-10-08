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

  bool e0(double* out)
  {
    if (!e1(out))
      return false;

    while (*input == '+' || *input == '-')
    {
      char op = *input;
      ++input;

      double rhs = 0.0;
      e1(&rhs); // expect

      if (op == '+')
        *out += rhs;
      else // '-'
        *out -= rhs;
    }

    return true;
  }

  bool e1(double* out)
  {
    if (!e2(out))
      return false;

    while (*input == '*' || *input == '/')
    {
      char op = *input;
      ++input;

      double rhs = 0.0;
      e2(&rhs); // expect
      
      if (op == '*')
        (*out) *= rhs;
      else // '/'
        (*out) /= rhs;
    }

    return true;
  }

  bool e2(double* out)
  {
    if (!(*input == '+' || *input == '-'))
      return e3(out);
    char op = *input;
    ++input;

    e2(out); // expect

    if (op == '-')
      *out = -*out;

    return true;
  }

  bool e3(double* out)
  {
    if (*input != '(')
      return n(out);
    ++input;
    
    e0(out); // expect

    ++input; // expect ')'
    return true;
  }

  bool n(double* out)
  {
    char n = *input;
    if (!(n >= '0' && n <= '9'))
      return false;
    ++input;

    *out = n - '0';
    return true;
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
  parse.input = "6+2*3";

  node* root = new node{ '+' };
  node* lhs = new node{ '5' };
  node* rhs = new node{ '3' };

  root->children.push_back(lhs);
  root->children.push_back(rhs);

  string g = graph(root);

  double result = 0.0;
  parse.e0(&result);
  return 0;
}

