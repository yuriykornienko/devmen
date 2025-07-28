#pragma once
#ifndef string_conv_hpp__
#define string_conv_hpp__

#include "../../../types.hpp"

#include <ZetDictionary/include/ZetDictionary.h>

#ifdef UNICODE

#include <codecvt/codecvt_cp1251.hpp>

namespace zetlab { namespace utils {

inline const tstring cp1251_to_string( const char *src, size_t len )
{
    return WStringFromCP1251( src, len );
}

inline const tstring cp1251_to_string( const char *src )
{
    return WStringFromCP1251( src );
}

inline ::std::string string_to_cp1251( const tstring &src )
{
    return WStringToCP1251( src );
}

inline void translate(_Inout_ tstring& str, _In_ tstring& param)
{
	DWORD dwSize(MAX_SIZE_PHRASE_IN_LETTES);
	wchar_t pBuffer[MAX_SIZE_PHRASE_IN_LETTES];
	if (0 == zdTranslate(str.c_str(), param.c_str(), pBuffer, &dwSize))
		str = pBuffer;
}

}} // namespace zetlab::utils

#else

namespace zetlab { namespace utils {

inline const tstring cp1251_to_string( const char *src, size_t len )
{
    return tstring( CString(src, len) );
}

inline const tstring cp1251_to_string( const char *src )
{
    return tstring( CString(src) );
}

inline ::std::string string_to_cp1251( const tstring &src )
{
    return ::std::string( src );
}

}} // namespace zetlab::utils

#endif

#endif // time_conv_hpp__
