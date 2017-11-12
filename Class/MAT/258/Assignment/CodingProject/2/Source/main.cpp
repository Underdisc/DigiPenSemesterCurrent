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
  for(int i = 0; i < 26; ++i)
    std::cout << std::setw(2) << i << " shift: " << shifts[i] << std::endl;
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

int max_letter_frequency(const std::string & message,
  std::vector<int> * frequencies)
{
  // filling vector with 0s
  frequencies->resize(26, 0);
  // finding letter frequencies
  for(const char & character : message){
    if(character <= 25)
      (*frequencies)[(int)character] += 1;
  }
  // find index of most common letter
  int max_i = 0;
  int num_frequencies = frequencies->size();
  for(int i = 1; i < num_frequencies; ++i){
    if((*frequencies)[i] > (*frequencies)[max_i])
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
  std::vector<int> frequencies;
  int max_frequency = max_letter_frequency(message, &frequencies);
  int most_frequent_letter = 4;
  int shift_amount = most_frequent_letter - max_frequency;
  if(shift_amount < 0)
    shift_amount += 26;
  std::string decoded_mesage = shift(message, shift_amount);
  normalize_A_Z(&decoded_mesage);
  // printing results
  std::cout << "~ Resluts ~" << std::endl;
  std::cout << "Shift (a): " << shift_amount << std::endl;
  std::cout << "Decoded Message: " << decoded_mesage << std::endl;
  // printing frequency table
  std::cout << "- Frequency Table -" << std::endl;
  char current_letter = 65;
  for(int i = 0; i < 26; ++i, ++current_letter)
    std::cout << current_letter << ": " << std::setw(2) << frequencies[i]
      << std::endl;
  std::cout << std::endl;
}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////


int bf_inverse_mod(unsigned long long a, unsigned long long b)
{
  // find inverse mod using brute force technique
  for(unsigned long long int i = 0; i < b; ++i){
    unsigned long long int product = i * a;
    if((product % b) == 1){
      return i;
    }
  }
  // inverse mod does not exist
  return -1;
}

int gcd(int a, int b)
{
  int r = a % b;
  if(r == 0)
    return b;
  return gcd(b, r);
}

void problem3()
{
  std::cout << "<Problem 3>" << std::endl;
  // grab values
  int a;
  int b;
  std::cout << "Input A: ";
  std::cin >> a;
  std::cout << "Input B: ";
  std::cin >> b;
  // find gcd
  int gcd_value = gcd(a, b);
  // print results
  std::cout << "~ Results ~" << std::endl
    << "GCD: " << gcd_value << std::endl;
  if(gcd_value == 1) {
    // find inverse mod if it exists
    int inverse_mod = bf_inverse_mod(a, b);
    std::cout << "Inverse Mod: " << inverse_mod << std::endl;
  }
  else{
    std::cout << "Inverse does not exist" << std::endl;
  }
}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

unsigned long long chinese_remainder(
  const std::vector<std::pair<int, int> > & pairs)
{
  int num_pairs = pairs.size();
  // find m
  unsigned long long int m = 1;
  for(int i = 0; i < num_pairs; ++i)
    m *= pairs[i].second;
  // find m hats
  std::vector<unsigned long long int> m_hats;
  for(int i = 0; i < num_pairs; ++i)
    m_hats.push_back(m / pairs[i].second);
  // find y hats
  std::vector<unsigned long long int> y_hats;
  for(int i = 0; i < num_pairs; ++i){
    unsigned long long int m_hat_simp = m_hats[i];
    m_hat_simp = m_hat_simp % pairs[i].second;
    y_hats.push_back(bf_inverse_mod(m_hat_simp, pairs[i].second));
  }
  unsigned long long residue = 0;
  for(int i = 0; i < num_pairs; ++i)
    residue += pairs[i].first * m_hats[i] * y_hats[i];
  residue = residue % m;
  return residue;
}

void problem4()
{
  std::cout << "<Problem 4>" << std::endl;
  int k;
  std::cout << "Input Number of People (k): ";
  std::cin >> k;
  // input pairs
  std::vector<std::pair<int, int> > pairs;
  pairs.resize(k);
  std::cout << "- Input Pairs -" << std::endl;
  for(int i = 0; i < k; ++i){
    std::cout << "- " << i << " -" << std::endl;
    std::cout << "d: ";
    std::cin >> pairs[i].second;
    std::cout << "r: ";
    std::cin >> pairs[i].first;
  }
  int num_pairs = pairs.size();
  // make sure all ms are relatively prime
  for(int i = 0; i < num_pairs; ++i){
    for(int j = i + 1; j < num_pairs; ++j){
      if(gcd(pairs[i].second, pairs[j].second) != 1){
        std::cout << "~ Results ~" << std::endl;
        std::cout << "Not all ds are relatively prime" << std::endl;
        return;
      }
    }
  }
  // calculate
  unsigned long long value = chinese_remainder(pairs);
  // results
  std::cout << "~ Results ~" << std::endl;
  std::cout << "Pairs: ";
  for(const std::pair<int, int> & pair : pairs)
    std::cout << "(" << pair.second << ", " << pair.first << "), ";
  std::cout << std::endl << "Value: " << value << std::endl;

}
