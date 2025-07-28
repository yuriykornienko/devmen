#pragma once
#ifndef time_conv_hpp__
#define time_conv_hpp__

#include "../../../types.hpp"

#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include "resource\Phrases.h"

namespace zetlab { namespace utils {

#ifdef UNICODE
    typedef ::std::wistringstream tistringstream;
    typedef ::std::wostringstream tostringstream;
    typedef boost::posix_time::wtime_facet ttime_facet;
#else
    typedef ::std::istringstream tistringstream;
    typedef ::std::ostringstream tostringstream;
    typedef boost::posix_time::time_facet ttime_facet;
#endif

inline const boost::posix_time::time_duration get_utc_offset()
{
    typedef boost::posix_time::ptime ptime;
    typedef boost::date_time::c_local_adjustor< ptime > local_adj;

    ptime now_utc = boost::posix_time::second_clock::universal_time();
    ptime now_loc = local_adj::utc_to_local(now_utc);

    return now_loc - now_utc;
}

template< typename T >
inline const tstring time_to_string( const T& time, const TCHAR* fmt )
{
    typedef boost::posix_time::ptime ptime;

    ptime pt = boost::posix_time::from_time_t( time );
    pt += get_utc_offset();

    tostringstream stream;
    stream.imbue( ::std::locale(::std::cout.getloc(), new ttime_facet(fmt)) );
    stream << pt;

    return stream.str();
}

#if 0 // функция не работает, но она пока и не нужна
template< typename T >
inline const T string_to_time( const tstring& string, const TCHAR* fmt )
{
    typedef boost::posix_time::ptime ptime;
    typedef boost::gregorian::date date;

    tistringstream stream;

    stream.imbue( std::locale(std::cout.getloc(), new ttime_facet(fmt)) );
    stream.str( string );

    ptime pt;
    stream >> pt; // проблема в этом месте, где-то используется char вместо wchar_t
    pt -= get_utc_offset();

    return (T)((pt - ptime(date(1970, 1, 1))).total_seconds());
}
#endif

template< typename T >
inline const tstring time_duration_to_string(const T& seconds)
{
	tstring sString(_T(""));

	if (seconds >= 60 * 60 * 24)
		sString = (boost::wformat(_T("~ %d %s")) % (uint32_t)(seconds / 60. / 60. / 24.) % (g_sUNIT_DAY +L".").GetString()).str();
	else if (seconds >= 60 * 60)
		sString = (boost::wformat(_T("~ %d %s")) % (uint32_t)(seconds / 60. / 60.) % g_sUNIT_HOURS.GetString()).str();
	else if (seconds >= 60)
		sString = (boost::wformat(_T("~ %d %s")) % (uint32_t)(seconds / 60.) % g_sUNIT_MINUTES.GetString()).str();
	else
		sString = (boost::wformat(_T("~ %d %s")) % (uint32_t)(seconds) % g_sUNIT_S.GetString()).str();

	return sString;
}

}} // namespace zetlab::utils

#endif // time_conv_hpp__
