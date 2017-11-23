/*****************************************************************************/
/*!
\file knapsack-dp.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: CS 330 Assignment 5
\date 22/11/2017
\brief Contains the iteratic and recursive dynamic programming implementation
  of the knapsack problem.
*/
/*****************************************************************************/

#include "knapsack-dp.h"
#include <iostream>
#include <numeric>

// Item Constructors
////////////////////////////////////////////////////////////
Item::Item( int const& weight, int const& value )
  : weight(weight), value(value)
{}
Item::Item( Item const& original )
  : weight(original.weight), value(original.value)
{}

// Item Printers
////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, Item const& item) {
  os << "(" << item.weight << " , " << item.value << ") ";
  return os;
}
std::istream& operator>> (std::istream& os, Item & item) {
  os >> item.weight >> item.value;
  return os;
}

/*!
\brief Gets the max of two ints.
*/
int max(int a, int b)
{
  if(a > b)
    return a;
  return b;
}

/*!
\brief Fills a vector with the indices of the items added to the knapsack.
\param table The dynamic table generated.
\param items The items that could be in the knapsack.
\param W The total weight that the knapsack can hold.

\par Description
  if the value at [w][i - 1] is smaller than the value at [w][i],
  item i was added to the bag
  -> go to w = w - item.weight;
  -> go to i = i - 1;
  if it is not smaller
  item was not added
  -> do not pass go, do not collect 200$
  -> go to w = w
  -> got to i = i -1
*/
inline void fillBag(const std::vector<std::vector<int> > & table,
  const std::vector<Item> & items, const int & W,
  std::vector<int> * bag)
{
  int i = items.size();
  int w = W;
  int value = table[w][i];
  // All items have been added once we
  // arrive at 0
  while(value != 0){
    int prev_value = table[w][i - 1];
    if(prev_value < value){
      // item i - 1 was added
      bag->push_back(i - 1);
      const Item & item = items[i - 1];
      w = w - item.weight;
      i = i - 1;
    }
    else{
      // item i - 1 was not added
      i = i - 1;
    }
    value = table[w][i];
  }
}

/*****************************************************************************/
/*!
\brief
  Contains the interavtive dynamic programming solution of the knapsack
  problem.

\param items
  The items that can potentially be placed in the knapsack.
\param W
  The total weight that the knapsack can hold.

\return A vector containing the indicies of which items were added to the
  knapsack.
*/
/*****************************************************************************/
std::vector<int> knapsackDP( std::vector<Item> const& items, int const& W )
{
  int num_items = items.size();
  // You don't need to sort the items upon further inspection
  // Resizing table to proper size
  std::vector<std::vector<int> > table;
  table.resize(W + 1);
  for(unsigned r = 0; r < table.size(); ++r){
    std::vector<int> & row = table[r];
    row.resize(items.size() + 1);
  }
  // columns are for items
  // weights are for rows
  for(unsigned i = 0; i <= items.size(); ++i){
    for(int w = 0; w <= W; ++w){
      // initialize first row and column to zero
      if(i == 0 || w == 0){
        table[w][i] = 0;
        continue;
      }
      const Item & item = items[i - 1];
      // The case where the item is too heavy
      if(w - item.weight < 0){
        table[w][i] = table[w][i - 1];
      }
      // It is possible to include this item?
      // is it worth it?
      else{
        const int & weight = item.weight;
        const int & value = item.value;
        table[w][i] = max(table[w][i - 1], value + table[w - weight][i - 1]);
      }
    }
  }

  //print final table - for debugging?
  //do not delete this code
  if ( num_items + W < 50 ) { //print only if table is not too big
    std::cout << "   ";
    for ( int n=0; n<=num_items; ++n)
      std::cout << n << "     ";
    std::cout << "  items\n        ";
    for ( int n=0; n<num_items; ++n)
      std::cout << items[n].weight << "," << items[n].value<<"   ";
    std::cout << "\n   ";
    for ( int n=0; n<=num_items; ++n)
      std::cout << "------";
    std::cout << std::endl;
    for ( int w=0; w<=W; ++w) {
      std::cout << w << "| ";
      for ( int n=0; n<=num_items; ++n) {
        std::cout << table[w][n] << "     ";
      }
      std::cout << std::endl;
    }
  }
  //end do not delete this code

  //figure out which items are in the bag based on the table
  std::vector<int> bag;
  fillBag(table, items, W, &bag);
  return bag;
}

// Gets the value in a bag
////////////////////////////////////////////////////////////
int valueBag( std::vector<Item> const& items, std::vector<int> const& bag ) {
  std::vector<int>::const_iterator it   = bag.begin(),
                              it_e = bag.end();

  int accum = 0;
  //std::cout << "Bag ";
  for ( ; it != it_e; ++it) {
    accum += items[ *it ].value;
    //std::cout << *it << " ";
  }
  //std::cout << std::endl;
  return accum;
}

// predeclaration
int knapsackRecMemAux(std::vector<Item> const& items,
  std::vector<std::vector<int> > & table, int w, int i);

/*****************************************************************************/
/*!
\brief
  Contains the recursive dynamic programming solution of the knapsack
  problem (uses memoization).

\param items
  The items that can potentially be placed in the knapsack.
\param W
  The total weight that the knapsack can hold.

\return A vector containing the indicies of which items were added to the
  knapsack.
*/
/*****************************************************************************/
std::vector<int> knapsackRecMem( std::vector<Item> const& items, int const& W )
{
  int num_items = items.size();
  // create the table
  std::vector<std::vector<int> > table;
  // resizing table to accept all values
  table.resize(W + 1);
  for(unsigned r = 0; r < table.size(); ++r){
    std::vector<int> & row = table[r];
    row.resize(items.size() + 1);
    // initialize all values to 0 and -1
    // to represent what has and has not been calculated
    for(unsigned v = 0; v < row.size(); ++v){
      if(v == 0 || r  == 0)
        row[v] = 0;
      else
        row[v] = -1;
    }
  }
  knapsackRecMemAux(items, table, W, items.size());

  //print table - debugging?
    //do not delete this code
    if ( num_items + W < 50 ) { //print only if table is not too big
        std::cout << "   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << n << "     "; }
        std::cout << "  items\n        ";
        for ( int n=0; n<num_items; ++n) {
            std::cout << items[n].weight << "," << items[n].value<<"   ";
        }
        std::cout << "\n   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << "------"; }
        std::cout << std::endl;

        for ( int w=0; w<=W; ++w) {
            std::cout << w << "| ";
            for ( int n=0; n<=num_items; ++n) {
                std::cout << table[w][n] << "     ";
            }
            std::cout << std::endl;
        }
    }
    //end do not delete this code

  //figure out which items are in the bag based on the table
  std::vector<int> bag;
  fillBag(table, items, W, &bag);
  return bag;
}

/*!
\brief The actual recursive implementation of the knapsack problem.
\param items The items that can be placed in the knapsack.
\param table The dynamic programming table.
\param w The current weigth index.
\param i The current item index.
\return The value at table[w][i]
*/
int knapsackRecMemAux(std::vector<Item> const& items,
  std::vector<std::vector<int> > & table, int w, int i)
{
  // base case
  if(table[w][i] != -1)
    return table[w][i];
  // not base case
  // i is 1 based, hence the i - 1
  const Item & item = items[i - 1];
  if(w - item.weight < 0){
    table[w][i] = knapsackRecMemAux(items, table, w, i - 1);
  }
  else{
    int value1 = knapsackRecMemAux(items, table, w, i - 1);
    int value2 = item.value + knapsackRecMemAux(items, table, w - item.weight, i - 1);
    table[w][i] = max(value1, value2);
  }
  return table[w][i];
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
