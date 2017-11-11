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

// ENCODING/DECODING HELPERS //////////////////////////////////////////////////

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
    else
      shifted_message[c] = message[c];
  }
  return shifted_message;
}

void normalize_0_25(std::string * message)
{
  // bring string to 0 - 25 value range
  // only shifting characters that are explicitly letters
  int message_length = message->length();
  for(int c = 0; c < message_length; ++c){
    if((*message)[c] >= 65 && (*message)[c] <= 90)
      (*message)[c] -= 65;
    else if((*message)[c] >= 97 && (*message)[c] <= 122)
      (*message)[c] -= 97;
  }
}

void normalize_A_Z(std::string * message)
{
  // bring string to A to Z range
  for(char & character : *message){
    // ignoring characters that are not letters
    if(character <=  25)
      character += 65;
  }
}

void normalize_a_z(std::string * message)
{
  // bring string to A to Z range
  for(char & character : *message){
    // ignoring characters that are not letters
    if(character <=  25)
      character += 97;
  }
}


// PROBLEM 1 CODE /////////////////////////////////////////////////////////////


void generate_shifts(std::string message, std::vector<std::string> * shifts)
{
  normalize_0_25(&message);
  for(int shift_amount = 0; shift_amount < 26; ++shift_amount){
    shifts->push_back(shift(message, shift_amount));
    std::string & shift = shifts->back();
    normalize_A_Z(&shift);
  }
}

void problem1()
{
  std::cout << "<Problem 1>" << std::endl;
  std::string message;
  std::cout << "Input Encoded Message: ";
  std::cin.ignore();
  std::getline(std::cin, message);
  std::vector<std::string> shifts;
  generate_shifts(message, &shifts);
  // printing all possible shifts
  std::cout << "~ Results ~" << std::endl << "- Possible Shifts -" <<
    std::endl;
  for(const std::string & shift : shifts)
    std::cout << shift << std::endl;
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

int max_letter_frequency(const std::string & message)
{
  std::vector<int> frequencies;
  // filling vector with 0s
  frequencies.resize(26, 0);
  // finding letter frequencies
  for(const char & character : message){
    if(character <= 25)
      frequencies[(int)character] += 1;
  }
  // find index of most common letter
  int max_i = 0;
  int num_frequencies = frequencies.size();
  for(int i = 1; i < num_frequencies; ++i){
    if(frequencies[i] > frequencies[max_i])
      max_i = i;
  }
  return max_i;
}

void problem2()
{
  std::cout << "<Problem 2>" << std::endl;
  std::string message;
  std::cout << "Input Encoded Message: ";
  std::cin.ignore();
  std::getline(std::cin, message);
  // find letter frequencies and shifting according to letter frequency
  normalize_0_25(&message);
  int max_frequency = max_letter_frequency(message);
  int most_frequent_letter = 4;
  int shift_amount = most_frequent_letter - max_frequency;
  if(shift_amount < 0)
    shift_amount += 26;
  std::string decoded_mesage = shift(message, shift_amount);
  normalize_A_Z(&decoded_mesage);
  // printing results
  std::cout << "~ Resluts ~" << std::endl;
  std::cout << "Decoded Message: " << decoded_mesage << std::endl;
}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

void problem3()
{
 std::cout << "Problem 3" << std::endl;

}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

void problem4()
{

}
