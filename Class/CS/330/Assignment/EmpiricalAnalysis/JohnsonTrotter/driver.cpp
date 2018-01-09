#include "perm-jt.h"
#include <stdlib.h>
#include <iterator> //ostream_iterator
#include <algorithm>
#include <iostream>

struct Print {
  void operator() ( std::vector<int> const& v) const {
    std::copy( v.begin(), v.end(),
        std::ostream_iterator<int>(std::cout, " ") );
    std::cout << std::endl;
  }
};


int main(int argc, char * argv[]) {

  if(argc < 2)
    return 1;
  int set_size = atoi(argv[1]);
  PermJohnsonTrotter pjt(set_size);
  do {
  } while ( pjt.Next() );
}
