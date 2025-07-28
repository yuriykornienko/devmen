#pragma once
#ifndef bytes_conv_hpp__
#define bytes_conv_hpp__

#include "../../../types.hpp"

#include <boost/format.hpp>
#include "resource\Phrases.h"

namespace zetlab { namespace utils {

template< typename T >
inline const tstring bytes_to_string( const T& bytes)
{
	uint64_t b = (uint64_t)bytes;
	tstring sString(_T(""));
	
	if (b >= 1024 * 1024 * 1024)
		sString = (boost::wformat(_T("%.1f %s")) % ((double)b / 1024. / 1024. / 1024.) % (g_sUNIT_GB).GetString()).str();
	else if (b >= 1024 * 1024)
		sString = (boost::wformat(_T("%.1f %s")) % ((double)b / 1024. / 1024.) % (g_sUNIT_MB).GetString()).str();
	else if (b >= 1024)
		sString = (boost::wformat(_T("%.1f %s")) % ((double)b / 1024.) % (g_sUNIT_KB).GetString()).str();
	else
		sString = (boost::wformat(_T("%d %s")) % b % (g_sUNIT_byte).GetString()).str();
	
	return sString;
}
}} // namespace zetlab::utils

#endif // bytes_conv_hpp__
