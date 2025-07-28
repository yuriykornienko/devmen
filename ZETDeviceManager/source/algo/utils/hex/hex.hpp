#pragma once
#ifndef hex_hpp__
#define hex_hpp__

#include "../../../types.hpp"

#include <ios>

namespace zetlab { namespace utils {

template< typename T >
inline const tstring hex( const T& value )
{
    tstringstream stream;
    stream << ::std::hex << value;

    return stream.str();
}

}} // namespace zetlab::utils

#endif // hex_hpp__
