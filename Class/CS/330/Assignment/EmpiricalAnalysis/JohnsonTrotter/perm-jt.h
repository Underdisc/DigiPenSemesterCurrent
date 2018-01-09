/*****************************************************************************/
/*!
\file perm-jt.h
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Johnson Trotter Permutations
\date 12/12/2017
\brief
  Contains the interface for the Johnson Trotter permutation generator.
*/
/*****************************************************************************/

#ifndef PERM_JOHNSON_TROTTER_H
#define PERM_JOHNSON_TROTTER_H
#include <vector>

class PermJohnsonTrotter
{
public:
  PermJohnsonTrotter(int size);
  bool Next();
  std::vector<int> const& Get() const;
private:
  void Swap(std::vector<int> & vec, int i, int j);
  // values in the current permutation
  std::vector<int> _currentPermutation;
  // directions of the elements in the permutation
  std::vector<int> _currentDirections;
};
#endif
