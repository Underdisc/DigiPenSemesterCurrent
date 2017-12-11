#ifndef DEFINITIONS
#define DEFINITIONS
#include <fstream> // ostream, istream
#include <chrono>  // duration C++11

// imitates integer, but allows adding a delay in some operations
class Weight {
    public:
        // Weight( Weight const& original ); // compiler's version is OK
        friend std::ostream& operator<< (std::ostream& os, Weight const& w);
        friend std::istream& operator>> (std::istream& os, Weight & w);
        Weight & operator+=( Weight const& op2 );
        Weight & operator-=( Weight const& op2 );
        Weight const operator+( Weight const& op2 ) const;
        Weight const operator-( Weight const& op2 ) const;
        bool operator<( Weight const& op2 ) const;
        bool operator>( Weight const& op2 ) const;
        bool operator<=( Weight const& op2 ) const;
        bool operator>=( Weight const& op2 ) const;
        bool operator==( Weight const& op2 ) const;
        bool operator!=( Weight const& op2 ) const;
        static void SetDelay( int const& delay_ms );
    private:
        int weight                             {0};
        //static
        static bool delay_on;
        static std::chrono::milliseconds delay;
};

class Item {
    public:
        Item( Weight const& weight, int const& value );
        Item( Item const& original );
        friend std::ostream& operator<< (std::ostream& os, Item const& item);
        friend std::istream& operator>> (std::istream& os, Item & item);
        Weight const& GetWeight() const;
        int           GetValue() const;
    private:
        Weight weight;
        int value;
};
#endif
