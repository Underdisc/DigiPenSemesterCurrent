#include "GreyCode.h"
#include <stdlib.h>
#include <utility>

int main(int argc, char ** argv)
{
    if(argc < 2)
      return 1;
    int size = atoi(argv[1]);

    GreyCode gc(size);
    bool go = true;
    while(go){
      std::pair< bool, std::pair< bool, int > > r = gc.Next();
      go = r.first;
    }
}
