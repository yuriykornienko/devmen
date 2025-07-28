#pragma once
#ifndef mac_conv_hpp__
#define mac_conv_hpp__

#include "../../../types.hpp"

namespace zetlab { namespace utils {

inline const tstring mac_to_string( const unsigned char* mac )
{
    char mac_str[18];

    sprintf_s( mac_str, 18, "%02X-%02X-%02X-%02X-%02X-%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

    return tstring(mac_str, mac_str + 18);
}

}} // namespace zetlab::utils

#endif // mac_conv_hpp__
