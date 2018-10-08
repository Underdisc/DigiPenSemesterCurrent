#include <ctype.h>

// E0 = E1 (('+'|'-') E1)*
// E1 = NN (('*'|'/') NN)*

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
        (*out) += rhs;
      else // '-'
        (*out) -= rhs;
    }

    return true;
  }

  bool e1(double* out)
  {
    char n = *input;
    if (!isdigit(n))
      return false;
    ++input;

    *out = n - '0';

    while (*input == '*' || *input == '/')
    {
      char op = *input;
      ++input;

      char n = *input; // expect number
      ++input;

      double rhs = n - '0';

      if (op == '*')
        (*out) *= rhs;
      else // '/'
        (*out) /= rhs;
    }

    return true;
  }

  const char* input;
};

int main(void)
{
  parser parse;
  parse.input = "8+9*5+3+4*9/2";
  double result = 0.0;
  parse.e0(&result);

  return 0;
}
