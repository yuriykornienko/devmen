#pragma once
#ifndef double_bubble_sort_hpp__
#define double_bubble_sort_hpp__

#include "../../../types.hpp"

namespace zetlab { namespace utils {

inline void double_bubble_sort( float* x, float* y, size_t size )
{
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            if (x[i] < x[j])
                swap( x[i], x[j] ),
                swap( y[i], y[j] );
        }
    }
}

}} // namespace zetlab::utils

#endif // double_bubble_sort_hpp__
