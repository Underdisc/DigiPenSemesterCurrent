/*****************************************************************************/
/*!
\file knapsack_brute_force_minchange..h
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Knapsack Brute Force
\date 11/15/2017
\brief
  Contains the interface for a Grey Code generator and a brute force knapsack
  implementation.
*/
/*****************************************************************************/

#ifndef KNAPSACK_BRUTE_FORCE_MINCHANGE
#define KNAPSACK_BRUTE_FORCE_MINCHANGE

#include <utility>
#include <vector>
#include <bitset>
#include "definitions.h"

class GreyCode {
  private:
    unsigned long long _numPermutations;
    unsigned long long _currentIteration;
    std::vector<bool> _currentGreyCode;
    int FindPositionToChange(unsigned long long next_iteration);
  public:
    GreyCode( int s );
    std::pair< bool, std::pair< bool, int > > Next();
    const std::vector<bool> & GetCurrentGreyCode();
};

// brute-force knapsack
std::vector<bool> knapsack_brute_force(std::vector<Item> const& items,
  Weight const& W);

#endif //!KNAPSACK_BRUTE_FORCE_MINCHANGE
