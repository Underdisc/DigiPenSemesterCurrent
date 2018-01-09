#include "knapsack_brute_force_minchange.h"
#include <iostream>
#include <fstream> //ostream, istream
#include <vector>
#include <algorithm>

void readItems(char const* filename, std::vector<Item> & items, Weight& W) {
    std::ifstream in( filename, std::ifstream::in );
    if ( in.is_open() ) {
        in >> W;

        Item item( Weight(), 0 );
        in >> item;
        while ( in.good() ) {
            items.push_back( item );
            in >> item;
        }
    } else {
        std::cout << "problem reading " << filename << std::endl;
    }
}

void print_bag( std::vector<bool> const& bag, std::vector<Item> const& store, Weight const& W ) {
    Weight total_weight;
    int total_value = 0;
    for ( unsigned int i=0; i != bag.size(); ++i) {
        if ( bag[i] ) {std::cout << store[ i ] << " - ";
            total_value  += store[ i ].GetValue();
            total_weight += store[ i ].GetWeight();
        }
    }
    std::cout << "\nTotal grab " << total_value << " weighing " << total_weight << " (max " << W << ")\n";
}

void test_grey_code( int s )
{
    std::vector<bool> bag( s, false ); // empty - all positions are false
    GreyCode gc( s );
    bool go = true;
    // print initial state
    // print vector from the end - same direction as binary representation
    for ( int i=bag.size()-1; i >= 0; --i) {
        std::cout << bag[ i ];
    }
    std::cout << std::endl;
    // the rest 
    while ( go ) {
        std::pair< bool, std::pair< bool, int > > r = gc.Next();
        go = r.first;
        bool add = r.second.first;
        int  pos = r.second.second;
        if ( add ) {
            bag[ pos ] = true;
            std::cout << "change to 1 at position " << pos << std::endl;
        } else {
            bag[ pos ] = false;
            std::cout << "change to 0 at position " << pos << std::endl;
        }
    for ( int i=bag.size()-1; i >= 0; --i) {
            std::cout << bag[ i ];
        }
        std::cout << std::endl;
    }
}

void test0() {
    test_grey_code( 3 );
}
void test1() {
    test_grey_code( 6 );
}

void test_knapsack( char const * filename, int const& delay ) {
    Weight W;
    std::vector<Item> store;
    readItems( filename, store, W );
    Weight::SetDelay( delay );
    std::vector<bool> bag = knapsack_brute_force( store, W );
    Weight::SetDelay( 0 );
    print_bag( bag, store, W );
}

void test2() { test_knapsack( "items0", 100 ); } // set delay to 1/10 s. There are 16-1 iterations, so total time should be about 1.5+epsilon if implemented using min change
void test3() { test_knapsack( "items1", 0 ); }
void test4() { test_knapsack( "items2", 0 ); }
void test5() { test_knapsack( "items3", 0 ); }
void test6() { test_knapsack( "items4", 0 ); } // 7m 58.754s !!!!!!!


void (*pTests[])() = { 
    test0,test1,test2,test3,test4,test5,test6
}; 


int main(int argc, char ** argv) try {
    if (argc == 2) {
		int test = 0;
		std::sscanf(argv[1],"%i",&test);
		pTests[ test ]();
    } else {
        std::cout <<  "./" << argv[0] << " <test number 0.." << sizeof(pTests)/sizeof(*pTests) << ">\n";
        return 1;
    }

} catch( char const* str) {
    std::cout << str << std::endl;
}
