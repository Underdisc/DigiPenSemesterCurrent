/*****************************************************************************/
/*!
\file knapsack_brute_force_minchange.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Knapsack Brute Force
\date 11/12/2017
\brief
  Contains the implementation of the Grey Code generator and the brute force
  knapsack solution.
*/
/*****************************************************************************/

#include "GreyCode.h"
//#include <iostream>

//-----// GreyCode //-----//

// The position to change is just the exponent from the highest power
// of 2 that is a factor of next_iteration.
// Evidence: Create a grey code table on a piece of paper. Number each grey
// code from 0 to s - 1. Draw a line from the element previously changed to
// the element changed for the next grey code.
/**\
0 1
 /
1 1
 \
1 0
\**/
// The pattern should be clear on a s = 4 table.
int GreyCode::FindPositionToChange(unsigned long long next_iteration)
{
  int current_exp = 0;
  unsigned long long current_power = 1;
  while(next_iteration % current_power == 0){
    ++current_exp;
    current_power = current_power << 1;
  }
  // The position of the value that will change
  return current_exp - 1;
}

GreyCode::GreyCode(int s) : _numPermutations(1),
  _currentIteration(0), _currentGreyCode(s, false)
{
  // finding number of permutations
  for(int i = 0; i < s; ++i)
    _numPermutations = _numPermutations << 1;
}

// Finds the next Grey Code.
// ret value: more codes, is add, position
std::pair< bool, std::pair< bool, int > > GreyCode::Next()
{
  // find the position to change
  unsigned long long next_iteration = _currentIteration + 1;
  int position = FindPositionToChange(next_iteration);
  // only add if the current grey code value at position is false
  bool add = !_currentGreyCode[position];
  // change position in current grey code
  _currentGreyCode[position] = !_currentGreyCode[position];
  _currentIteration = next_iteration;
  // check to see if we are on the last iteration
  bool last;
  if (next_iteration == _numPermutations - 1)
    last = true;
  else
    last = false;
  return std::make_pair(!last, std::make_pair(add, position));
}

// Return: The current grey code that the grey code generator is on
const std::vector<bool> & GreyCode::GetCurrentGreyCode()
{
  return _currentGreyCode;
}
