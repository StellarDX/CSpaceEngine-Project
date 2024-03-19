#include "CSE/Base/Random.h"

_CSE_BEGIN

std::random_device _Rd;
Probability::_CSE_Random_Engine<std::mersenne_twister_engine
    <unsigned long long, 64, 312, 156, 31, 0xB5026F5AA96619E9ULL, 29,
    0x5555555555555555ULL, 17, 0x71D67FFFEDA60000ULL, 37, 0xFFF7EEE000000000ULL, 43, 0x5851F42D4C957F2DULL>>
    random(_Rd());

_CSE_END
