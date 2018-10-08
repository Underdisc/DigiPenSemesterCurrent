#include <sstream>
#include <iostream>
#include <unordered_map>
#include <cctype>
#include <vector>

using namespace std;

enum class token_type
{
  invalid,
  identifier,
  number,

  plus,
  minus,
  multiply,
  divide
};

class token
{
public:
  size_t start;
  string text;
  token_type type = token_type::invalid;
};

void tokenize(const string& input, std::vector<token>& tokens_out)
{
  istringstream stream(input);
  size_t position = 0;

  unordered_map<string, token_type> keywords;
  keywords["+"] = token_type::plus;
  keywords["-"] = token_type::minus;
  keywords["*"] = token_type::multiply;
  keywords["/"] = token_type::divide;

  while (stream)
  {
    token token;
    token.start = position;
    stream >> token.text;
    position += token.text.size();

    if (token.text.empty())
      break;

    char front = token.text.front();

    auto found = keywords.find(token.text);
    if (found != keywords.end())
      token.type = found->second;
    else if (isalpha(front))
      token.type = token_type::identifier;
    else if (isdigit(front))
      token.type = token_type::number;

    cout << "Token: '" << token.text << "' of type " << (int)token.type << endl;

    if (token.type != token_type::invalid)
      tokens_out.push_back(token);
  }
}

// E0 = E1 (('+'|'-') E1)*
// E1 = NN (('*'|'/') NN)*

class parser
{
public:
  bool accept(token_type type, token** token_out = nullptr)
  {
    if (index >= tokens.size())
      return false;

    if (tokens[index].type == type)
    {
      if (token_out)
        *token_out = &tokens[index];
      ++index;
      return true;
    }
    return false;
  }

  bool expect(bool result)
  {
    if (!result)
      cerr << "Syntax error at token " << index << " with text " << tokens[index].text;
    return result;
  }

  bool expect(token_type type, token** token_out = nullptr)
  {
    return expect(accept(type, token_out));
  }

  bool e0(double* out)
  {
    if (!e1(out))
      return false;

    token* op = nullptr;
    while (accept(token_type::plus, &op) || accept(token_type::minus, &op))
    {
      double rhs = 0.0;
      expect(e1(&rhs));

      if (op->type == token_type::plus)
        (*out) += rhs;
      else // minus
        (*out) -= rhs;
    }

    return true;
  }

  bool e1(double* out)
  {
    token* number = nullptr;
    if (!accept(token_type::number, &number))
      return false;

    *out = strtod(number->text.c_str(), nullptr);

    token* op = nullptr;
    while (accept(token_type::multiply, &op) || accept(token_type::divide, &op))
    {
      expect(token_type::number, &number);
      double rhs = strtod(number->text.c_str(), nullptr);
      
      if (op->type == token_type::multiply)
        (*out) *= rhs;
      else // divide
        (*out) /= rhs;
    }

    return true;
  }

  std::vector<token> tokens;
  size_t index;
};

void parse()
{
}

int main(void)
{
  string input = "8 + 9 * 5 + 3";

  parser parse;
  tokenize(input, parse.tokens);
  double result = 0.0;
  parse.e0(&result);

  getchar();
  return 0;
}
