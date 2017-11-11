#include <iostream>
#include <string>
#include <vector>

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
      std::cout << std::endl << "Not an existing problem";
    }
    std::cout << std::endl << "---------------------" << std::endl;
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

#define SPACE_CHARACTER 32

std::string shift(const std::string & message, char amount)
{
  int message_size = message.length();
  std::string shifted_message;
  shifted_message.resize(message_size);
  for(int c = 0; c < message_size; ++c){
    // ignoring characters that are not letters
    if(message[c] <= 25)
      shifted_message[c] = (message[c] + amount) % 26;
  }
  return shifted_message;
}

void generate_shifts(std::string message, std::vector<std::string> * shifts)
{
  // bring string to 0 - 25 value range
  // only shifting characters that are explicitly letters
  int message_length = message.length();
  for(int c = 0; c < message_length; ++c){
    char character = message[c];
    if(character >= 65 && character <= 90)
      message[c] = character - 65;
    else if(character >= 97 && character <= 122)
      message[c] = character - 97;
  }
  for(int shift_amount = 0; shift_amount < 26; ++shift_amount){
    shifts->push_back(shift(message, shift_amount));
    std::string & shift = shifts->back();
    for(char & character : shift){
      // ignoring characters that are not letters
      if(character <=  25)
        character += 65;
    }
  }
}

void problem1()
{
  std::cout << "<Problem 1>" << std::endl;
  std::string message;
  std::cout << "Input Encoded Message:\n";
  std::cin.ignore();
  std::getline(std::cin, message);
  std::cout << message << std::endl;
  std::vector<std::string> shifts;
  generate_shifts(message, &shifts);
  std::cout << shifts[0];
  // printing all possible shifts
  std::cout << "~ Results ~" << std::endl << "- Possible Shifts -" <<
    std::endl;
  for(const std::string & shift : shifts)
    std::cout << shift << std::endl;
  // done
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

void problem2()
{

}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

void problem3()
{

}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

void problem4()
{

}
