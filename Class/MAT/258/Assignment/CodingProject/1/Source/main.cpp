#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <vector>

void problem1();
void problem2();
void problem3();

int main()
{
  bool running = true;
  while(running){
    unsigned problem = 0;
    std::cout << "Input problem number: ";
    std::cin >> problem;
    std::cout << "---------------------" << std::endl;
    switch (problem) {
    case 1:
      problem1();
      break;
    case 2:
      problem2();
      break;
    case 3:
      problem3();
      break;
    default:
      std::cout << std::endl << "Not an existing problem";
    }
    std::cout << std::endl << "---------------------" << std::endl;
    char choice = 0;
    std::cout << "Run another problem [y] [n]: ";
    std::cin >> choice;
    switch(choice){
    case 'y':
      break;
    case 'n':
      running = false;
      break;
    default:
      std::cout << "Not an option. Exiting.";
      running = false;
      break;
    }
  }
}

// PROBLEM 1 CODE /////////////////////////////////////////////////////////////

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
  std::cout << "Probability of sum: " << std::setprecision(10) << p_of_sum;
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

// will generate values in the range of [start, end)
unsigned unsigned_random_range(unsigned start, unsigned end)
{
  unsigned elements_in_range = end - start;
  unsigned random_value = rand();
  random_value = random_value % elements_in_range;
  random_value = random_value + start;
  return random_value;
}

bool monte_hall(unsigned n, unsigned k)
{
  std::vector<unsigned> unopened_doors;
  unopened_doors.resize(n);
  // starting at one to ignore the player selected door
  for(unsigned i = 0; i < n; ++i)
    unopened_doors[i] = i;
  // index of door with car
  unsigned c = unsigned_random_range(0, n);
  // the player is at door 0
  // openging doors
  for(unsigned i = 0; i < k; ++i)
  {
    bool cannot_open = true;
    unsigned index;
    while(cannot_open)
    {
      // ignore player door
      index = unsigned_random_range(1, unopened_doors.size());
      // make sure it is not a car
      bool not_car = unopened_doors[index] != c;
      cannot_open = !(not_car);
    }
    // we now have an openable index
    std::vector<unsigned>::iterator it = unopened_doors.begin() + index;
    unopened_doors.erase(it);
  }
  // randomly assign the player to a new door
  unsigned p_index = unsigned_random_range(1, unopened_doors.size());
  if(unopened_doors[p_index] == c)
    return true;
  return false;
}

void problem2()
{
  // initialize rand
  srand(time(NULL));
  std::cout << "< Problem 2 >" << std::endl;
  // number of doors
  unsigned n;
  // number of doors to open
  unsigned k;
  // gather inputs
  std::cout << "Input number of doors (n): ";
  std::cin >> n;
  std::cout << "Input number of doors to open (k): ";
  std::cin >> k;
  // perform 100 trials
  unsigned num_successes = 0;
  for(unsigned i = 0; i < 100; ++i){
    if(monte_hall(n,k))
      ++num_successes;
  }
  float prob_of_win_on_switch = (float)num_successes / 100.0f;
  // printing results
  std::cout << std::endl << "~ Results ~" << std::endl;
  std::cout << "Number of trials: " << 100 << std::endl;
  std::cout << "Probability of success on switch: " << std::setprecision(10)
    << prob_of_win_on_switch;
}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

// returns a value in the range of [0, 1] - floating pint
float uniform_random(void)
{
  unsigned random = rand();
  return ((float)random / (float)RAND_MAX);
}

bool gamblers_ruin(unsigned a, unsigned b, float p)
{
  // wait till we reach b or 0
  while(a != b && a != 0){
    float random = uniform_random();
    if(random < p)
      // success
      ++a;
    else
      // fail
      --a;
  }
  if(a == b)
    return true;
  return false;
}

void problem3()
{
  // initialize rand
  srand(time(NULL));
  std::cout << "< Problem 3 >" << std::endl;
  // starting dollar amount
  unsigned a;
  // ending dollar amount
  unsigned b;
  // probability of winning
  float p;
  // gather inputs
  std::cout << "Input starting dollar amount (a): ";
  std::cin >> a;
  std::cout << "Input ending dollar amount (b): ";
  std::cin >> b;
  std::cout << "Input probability of winning (p): ";
  std::cin >> p;
  // perform 100 trials
  unsigned num_successes = 0;
  for(int i = 0; i < 100; ++i){
    if(gamblers_ruin(a, b, p))
      ++num_successes;
  }
  // print results
  std::cout << std::endl << "~ Results ~" << std::endl << "Trials performed: "
    << 100 << std::endl << "Number of successes: " << num_successes;
}
