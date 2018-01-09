Implement iterative brute force solution for knapsack problem
using minimal change ordering of subsets:

1)  implement Grey code ordering - see class notes.
    You class GreyCode should have at least 2 methods:
        - ctor taking integer - the number of elements
        - Next that returns 3 values. Remember that next subset is different 
          from previous in one position/element only, so to define the subset
          we may define how it is different from previous:
            boolean: true - if this is the last subset
            boolean: true - if element is to be added, false is removed
            integer: position to be modified 
    Note -  client has to remember that the first subset is empty, and then call 
            next 2^n-1 times to get all subsets.

2)  implement iterative brute force solution for knapsack problem using GreyCode
    class. Return value is std::vector<bool> same size as the number of 
    available items. i'th position in the vector is true if the 
    corresponding (i'th) item is part of the solution.

Note on grading. The key test is test2 - I use my class Weight instead of integer and 
there is a delay 100ms in operator += (and -=,+,-). Therefore if you implement inefficient
total weight (by adding ALL objects in the bag instead of adding/removing just one)
you will timeout. Efficient implementation requires 15 adds/subtractions. So
timeout is set to 1.6s. 

test5 (extra credit) timeout is set to 1.5s. The test uses file "items" with 25 items.
If your implementation uses inefficient Next function - specifically the part which 
figures out which of the bits has changed (to decide which item is added/removed 
compared to the previous bag) it will fail the test (such implementation takes about 4s.
To get the credit you will have to non-ANSI (compiler specific) functions for bit manipulation.
Hint: on GNU it is "ffs" (find first bit set in int) from string library.
