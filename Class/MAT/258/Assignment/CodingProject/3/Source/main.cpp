#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>

void problem1();
void problem2();
void problem3();
void problem4();

int main()
{
  bool running = true;
  while(running){
    unsigned problem = 0;
    std::cout << "Input problem number: ";
    std::cin >> problem;
    std::cout << "---------------------" << std::endl;
    switch (problem) {
    case 1:
      problem1();
      break;
    case 2:
      problem2();
      break;
    case 3:
      problem3();
      break;
    case 4:
      problem4();
      break;
    default:
      std::cout << std::endl << "Not an existing problem" << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    char choice = 0;
    std::cout << "Run another problem [y] [n]: ";
    std::cin >> choice;
    switch(choice){
    case 'y':
      break;
    case 'n':
      running = false;
      break;
    default:
      std::cout << "Not an option. Exiting.";
      running = false;
      break;
    }
  }
}

// PROBLEM 1 CODE /////////////////////////////////////////////////////////////

void problem1()
{
  std::cout << "<Problem 1>" << std::endl;
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

void problem2()
{
  std::cout << "<Problem 2>" << std::endl;
}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

void problem3()
{
  std::cout << "<Problem 3>" << std::endl;
}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

void problem4()
{
  std::cout << "<Problem 4>" << std::endl;
}
