#include "knapsack_brute_force_minchange.h"
//#include <iostream>

////////////////////////////////////////////////////////////////////////////////
std::pair< bool, std::pair< bool, int > > GreyCode::Next() {
    
    
    
    
    
    int  pos = // which position is modified 
    bool add = // is true if new value is 1 (add item), false otherwise 
    bool last = // is this the last permutation 
    return std::make_pair( !last, std::make_pair( add, pos ) );
}

////////////////////////////////////////////////////////////////////////////////
std::vector<bool> knapsack_brute_force( std::vector<Item> const& items, Weight const& W )
{
}
