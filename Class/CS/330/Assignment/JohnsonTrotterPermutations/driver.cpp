#include "perm-jt.h"
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

int main() {
	PermJohnsonTrotter pjt(5);

	Print printer;

	do {
		printer ( pjt.Get() );
		//pjt.Print();
	} while ( pjt.Next() );
}
