#pragma once
#ifndef shield_access_hpp__
#define shield_access_hpp__

#include "../../../types.hpp"

#include <Shield/Shield.h>

namespace zetlab { namespace utils {

enum access_type {
    access_unknown = -1,
    access_operator = 0,
    access_dispatcher,
    access_metrology
};

enum user_type {
    user_unknown = -1,
    user_operator = 0,
    user_dispatcher,
    user_metrology
};

inline const user_type getUserType()
{
#define SHIELD_DISPATCHER (0x800000)
#define SHIELD_METROLOGY  (0x400000)

    unsigned long mask;
    unsigned long ext;

    if (ShieldAll( &mask, &ext ) < 0)
        return user_unknown;

    if (SHIELD_METROLOGY ==  (mask & SHIELD_METROLOGY))
        return user_metrology;

    if (SHIELD_DISPATCHER == (mask & SHIELD_DISPATCHER))
        return user_dispatcher;

    return user_operator;
};

}} // namespace zetlab::utils

#endif // shield_access_hpp__
