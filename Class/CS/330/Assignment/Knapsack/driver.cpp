#include "knapsack-dp.h"
#include <iostream>
#include <fstream> //ostream, istream
#include <vector>
#include <algorithm>

void readItems(char const* filename, std::vector<Item> & items, int& W) {
    std::ifstream in(filename,std::ifstream::in);
    in >> W;

    Item item;
    in >> item;
    while ( in.good() ) {
        items.push_back( item );
        in >> item;
    }
}

void print_bag( std::vector<int> const& bag, std::vector<Item> const& store, int const& W ) {
    int total_weight = 0, total_value = 0;
    std::vector<int>::const_iterator it = bag.begin(), it_e = bag.end();
    for ( ; it != it_e; ++it) {
        std::cout << store[ *it ] << " - ";
        total_value  += store[ *it ].value;
        total_weight += store[ *it ].weight;
    }
    std::cout << "\nTotal grab " << total_value << " weighing " << total_weight << " (max " << W << ")\n";
}

void test0() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items0", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test1() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items1", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test2() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items2", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test3() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items3", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test4() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items4", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test5() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items5", store, W );
    std::vector<int> bag = knapsackDP( store, W );
    print_bag( bag, store, W );
}

void test6() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items0", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}

void test7() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items1", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}

void test8() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items2", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}

void test9() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items3", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}

void test10() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items4", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}

void test11() {
    int W = 0;
    std::vector<Item> store;
    readItems( "items5", store, W );
    std::vector<int> bag = knapsackRecMem( store, W );
    print_bag( bag, store, W );
}


void (*pTests[])() = { 
    test0,test1,test2,test3,test4,test5,
    test6,test7,test8,test9,test10,test11
}; 

int main(int argc, char ** argv) try {
    std::vector<Item> store;
    std::vector<int> bag;
    int W = 0;

    if (argc==2) { //use test[ argv[1] ]
        int test = 0;
        std::sscanf(argv[1],"%i",&test);
        pTests[test]();
        return 0;
    }

    if (argc == 3) {
        readItems( argv[2], store, W );
        int algorithm = 0;
        std::sscanf(argv[1],"%i",&algorithm);
        print ( store );
        std::cout << std::endl << "=========================" << std::endl;
        if ( algorithm == 1 ) {
            std::cout << "Using recursive with memoization with items in \"" << argv[2] << "\"\n";
            bag = knapsackRecMem( store, W );
        } else {
            std::cout << "Using dynamic programming with items in \"" << argv[2] << "\"\n";
            bag = knapsackDP( store, W );
        }
        print_bag( bag, store, W );
    } else {
        std::cout << "Usage: " << argv[0] << " <algorithm ( 1 - recursive with memoization, 2 - dynamic programming)> <input file>\n";
        std::cout << "or     " << argv[0] << " <test number 0.." << sizeof(pTests)/sizeof(*pTests) << ">\n";
        return 1;
    }

    print_bag( bag, store, W );
} catch( char const* str) {
    std::cout << str << std::endl;
}
