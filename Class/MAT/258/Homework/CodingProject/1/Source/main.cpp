#include <iostream>
#include <iomanip>
#include <vector>

void problem1();
void problem2();

int main()
{
  problem1();
  problem2();
}

void problem1()
{
  std::cout << "< Problem 1 >" << std::endl;
  // number of sides of all dice
  unsigned k;
  // number of dice
  unsigned n;
  // desired sum
  unsigned s;
  // input values
  std::cout << "Input number of sides (k): ";
  std::cin >> k;
  std::cout << "Input number of dice (n): ";
  std::cin >> n;
  std::cout << "Input desired sum (s): ";
  std::cin >> s;

  // prepping dice
  std::vector<unsigned> dice;
  dice.resize(n);
  for(unsigned & die_value : dice)
    die_value = 1;
  // identifies whether any combinations remain
  bool more_combinations = true;
  // number of tuples found
  unsigned num_tuples = 0;
  while(more_combinations){
    // seeing if the combination results in a tuple
    // that adds to the correct sum
    unsigned sum = 0;
    for(const unsigned & die_value : dice)
      sum += die_value;
    if(sum == s)
      ++num_tuples;
    // moving on to next combination
    unsigned i = 0;
    while(dice[i] == k && i < (n-1)){
      dice[i] = 1;
      ++i;
    }
    if(i == (n - 1) && dice[i] == k)
      more_combinations = false;
    else
      ++dice[i];
  }
  // output results
  std::cout << std::endl << "~ Results ~" << std::endl;
  std::cout << "Number of tuples: " << num_tuples
    << std::endl;
  float num_possible_combos = 1.0f;
  for(unsigned i = 0; i < n; ++i)
    num_possible_combos *= static_cast<float>(k);
  float p_of_sum = static_cast<float>(num_tuples) / num_possible_combos;
  std::cout << "Probability of sum: " << std::setprecision(10) << p_of_sum
    << std::endl;
}

void problem2()
{
  std::cout << "< Problem 2 >" << std::endl;

}
