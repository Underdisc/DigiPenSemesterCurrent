#include "definitions.h"
#include <thread>  // sleep_for C++11
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool Weight::delay_on                   {false};
std::chrono::milliseconds Weight::delay {0};
////////////////////////////////////////////////////////////////////////////////
void Weight::SetDelay( int const& delay_ms )
{
    std::cout << "Delay is set to " << delay_ms << "\n";
    if ( delay_ms > 0 ) {
        delay_on = true;
        delay    = std::chrono::milliseconds( delay_ms );
    } else {
        delay_on = false;
    }
}
////////////////////////////////////////////////////////////////////////////////
Weight & Weight::operator+=( Weight const& op2 )
{
    if ( delay_on ) { std::this_thread::sleep_for( delay ); }
    weight += op2.weight;
    return *this;
}
////////////////////////////////////////////////////////////////////////////////
Weight & Weight::operator-=( Weight const& op2 )
{
    if ( delay_on ) { std::this_thread::sleep_for( delay ); }
    weight -= op2.weight;
    return *this;
}
////////////////////////////////////////////////////////////////////////////////
Weight const Weight::operator+( Weight const& op2 ) const
{
    Weight ret_val( *this );
    return ret_val+=op2;
}
////////////////////////////////////////////////////////////////////////////////
Weight const Weight::operator-( Weight const& op2 ) const
{
    Weight ret_val( *this );
    return ret_val-=op2;
}
////////////////////////////////////////////////////////////////////////////////
bool Weight::operator<( Weight const& op2 ) const { return weight<op2.weight; }
bool Weight::operator>( Weight const& op2 ) const { return weight>op2.weight; }
bool Weight::operator<=( Weight const& op2 ) const { return weight<=op2.weight; }
bool Weight::operator>=( Weight const& op2 ) const { return weight>=op2.weight; }
bool Weight::operator==( Weight const& op2 ) const { return weight==op2.weight; }
bool Weight::operator!=( Weight const& op2 ) const { return weight!=op2.weight; }
////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, Weight const& w)
{
	os << w.weight;
	return os;
}
////////////////////////////////////////////////////////////////////////////////
std::istream& operator>> (std::istream& os, Weight & w)
{
	os >> w.weight;
	return os;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Item::Item( Weight const& weight, int const& value ) 
	: weight(weight), value(value) 
{
}

////////////////////////////////////////////////////////////////////////////////
Item::Item( Item const& original ) 
	: weight(original.weight), value(original.value)
{
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, Item const& item) 
{
	os << "(" << item.weight << "," << item.value << ")";
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::istream& operator>> (std::istream& os, Item & item) 
{
	os >> item.weight >> item.value;
	return os;
}
////////////////////////////////////////////////////////////////////////////////
Weight const& Item::GetWeight() const
{
    return weight;
}
////////////////////////////////////////////////////////////////////////////////
int Item::GetValue() const
{
    return value;
}
////////////////////////////////////////////////////////////////////////////////
