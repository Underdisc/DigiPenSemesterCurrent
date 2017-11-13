#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "closestpair.h"

void readPoints(char const* filename, std::vector<Point> & points) {
    std::ifstream in(filename,std::ifstream::in);

    Point p;
    in >> p;
    while ( in.good() ) {
        points.push_back( p );
        in >> p;
    }
}

void test0() { //simple
    Point p [] = { {0,0}, {3,20}, {20,3}, {1,0} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test1() { //horisontal
    Point p [] = { {0,0}, {1,0}, {4,0}, {5,0} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test2() { //horisontal
    Point p [] = { {0,0}, {2,0}, {3,0}, {5,0} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test3() { //horisontal
    Point p [] = { {0,0}, {1,0}, {2,0}, {3,0}, {100,0} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test4() { //vertical
    Point p [] = { {0,0}, {0,1}, {0,4}, {0,5} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test5() { //vertical
    Point p [] = { {0,0}, {0,2}, {0,3}, {0,5} }; //answer 1
    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test6() {
    Point p [] = { 
        {0,10}, {1,20}, {2,100}, //min dist here is about 10.6
        {3,-100}, {4,20}, {5,1000} }; //min dist here is about 120
    //but there is a pair across left and right with distance 3

    std::vector<Point> points( p, p+sizeof(p)/sizeof(p[0]) );
    std::cout << closestPair( points ) << std::endl;
}

void test7() {
    std::vector<Point> points;
    readPoints( "points1", points );
    std::cout << closestPair( points ) << std::endl;
}

void test8() {
    std::vector<Point> points;
    readPoints( "points2", points );
    std::cout << closestPair( points ) << std::endl;
}

void test9() {
    std::vector<Point> points;
    readPoints( "points3", points );
    std::cout << closestPair( points ) << std::endl;
}

void test10() {
    std::vector<Point> points;
    readPoints( "points4", points );
    std::cout << closestPair( points ) << std::endl;
}

void test11() {
    std::vector<Point> points;
    readPoints( "points5", points );
    std::cout << closestPair( points ) << std::endl;
}

void test12() {
    std::vector<Point> points;
    readPoints( "points6", points );
    std::cout << closestPair( points ) << std::endl;
}

void test13() {
    std::vector<Point> points;
    readPoints( "points7", points );
    std::cout << closestPair( points ) << std::endl;
}

void test14() {
    std::vector<Point> points;
    readPoints( "points8", points );
    std::cout << closestPair( points ) << std::endl;
}

void (*pTests[])(void) = { test0,test1,test2,test3,test4,test5,test6,test7,test8,test9,test10,test11,test12,test13,test14 };

int main( int argc, char ** argv) {
    if (argc!=2) return 1;
    else {
        int test = 0;
        std::sscanf(argv[1],"%i",&test);
        pTests[ test ]();
    }
    return 0;
}
