/*****************************************************************************/
/*!
\file perm-jt.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Johnson Trotter Permutations
\date 12/12/2017
\brief
  Contains the implementation for the Johnson Trotter permutation generator.
*/
/*****************************************************************************/
#include "perm-jt.h"

PermJohnsonTrotter::PermJohnsonTrotter(int size) :
_currentPermutation(), _currentDirections()
{
  for(int i = 1; i <= size; ++i){
    _currentPermutation.push_back(i);
    _currentDirections.push_back(-1);
  }
}

#define NO_MOBILE -1

// Generates the next permutation
bool PermJohnsonTrotter::Next()
{
  int largest_mobile = NO_MOBILE;
  int largest_mobile_index = NO_MOBILE;
  // check if first element is largest mobile
  if(_currentDirections[0] == 1 &&
    _currentPermutation[0] > _currentPermutation[1]){
    largest_mobile = _currentPermutation[0];
    largest_mobile_index = 0;
  }
  // check if last element is largest mobile
  int last = _currentPermutation.size() - 1;
  if(_currentDirections[last] == -1 &&
    _currentPermutation[last] > _currentPermutation[last - 1] &&
    _currentPermutation[last] > largest_mobile){
    largest_mobile = _currentPermutation[last];
    largest_mobile_index = last;
  }
  // find the largest mobile element for the remainder of the permutation
  for(int i = 1; i < last; ++i){
    int current_element = _currentPermutation[i];
    int pointed_element = _currentPermutation[i + _currentDirections[i]];
    if(current_element > pointed_element && current_element > largest_mobile){
      largest_mobile = current_element;
      largest_mobile_index = i;
    }
  }
  // no mobile elements
  // all permutations have been visited
  if(largest_mobile == NO_MOBILE)
    return false;
  // swapping the largest mobile element with the element it is pointing to
  int to_swap = largest_mobile_index + _currentDirections[largest_mobile_index];
  Swap(_currentPermutation, largest_mobile_index, to_swap);
  Swap(_currentDirections, largest_mobile_index, to_swap);
  // swap the directions of all elements larger than the largest mobile element
  for(unsigned int i = 0; i < _currentPermutation.size(); ++i){
    if(_currentPermutation[i] > largest_mobile)
      _currentDirections[i] *= -1;
  }
  return true;
}

std::vector<int> const & PermJohnsonTrotter::Get() const
{
  return _currentPermutation;
}

inline void PermJohnsonTrotter::Swap(std::vector<int> & vec, int i, int j)
{
  int temp = vec[i];
  vec[i] = vec[j];
  vec[j] = temp;
}
