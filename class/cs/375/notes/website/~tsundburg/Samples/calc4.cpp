#include <sstream>
#include <vector>

class node
{
public:
  node() {}
  node(char token) : token(token) {}
  char token;
  std::vector<node*> children;
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
      node* op = new node(*input);
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
      node* op = new node(*input);
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

    node* op = new node(*input);
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
    
    node* node = e0(); // expect

    ++input; // expect ')'
    return node;
  }

  node* n()
  {
    char n = *input;
    if (!(n >= '0' && n <= '9'))
      return nullptr;
    ++input;

    return new node(n);
  }

  const char* input;
};

void make_graph_recursive(std::stringstream& stream, node* parent)
{
  stream << "  \"" << parent << "\" [label=\"" << parent->token << "\"]" << std::endl;

  for (node* child : parent->children)
  {
    stream << "  \"" << parent << "\" -> \"" << child << "\"" << std::endl;
    make_graph_recursive(stream, child);
  }
}

std::string make_graph(node* root)
{
  std::stringstream stream;
  stream << "digraph G" << std::endl;
  stream << "{" << std::endl;
  make_graph_recursive(stream, root);
  stream << "}";

  return stream.str();
}

int main(void)
{
  parser parse;
  parse.input = "6*4*2+2*(3+-4)";
  
  node* node = parse.e0();

  std::string graph = make_graph(node);

  return 0;
}
