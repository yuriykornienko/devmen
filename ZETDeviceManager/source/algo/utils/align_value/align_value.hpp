#pragma once
#ifndef align_value_hpp__
#define align_value_hpp__

#include <math.h>
#include <boost/math/special_functions/fpclassify.hpp>
#include <sstream>


template< typename T >
inline const T align_value( T value, int significant = 2 )
{
    if (!value || !boost::math::isfinite( value ))
        return T(0);


	std::stringstream stream;
	stream << std::fixed << std::setprecision(significant) << value;
	stream >> value;

    return value;
}

#endif // align_value_hpp__
