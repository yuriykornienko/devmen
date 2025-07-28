#pragma once
#ifndef bin_hpp__
#define bin_hpp__

#include "../../../types.hpp"
#include <bitset>

namespace zetlab { namespace utils {

template< typename T >
inline const tstring bin(const T& value)
{
	auto str = std::bitset<sizeof(T) * 8>(value).to_string();
	tstring tstr(str.begin(), str.end());
	tstr.append(_T("b"));
    return tstr;
}

template< typename T >
inline const tstring bin2(const T& value)
{
	auto str = std::bitset<2>(value).to_string();
	tstring tstr(str.begin(), str.end());
	tstr.append(_T("b"));
	return tstr;
}

template< typename T >
inline const tstring bin4(const T& value)
{
	auto str = std::bitset<4>(value).to_string();
	tstring tstr(str.begin(), str.end());
	tstr.append(_T("b"));
	return tstr;
}

}} // namespace zetlab::utils

#endif // bin_hpp__
