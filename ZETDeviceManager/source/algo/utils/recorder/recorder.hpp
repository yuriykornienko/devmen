#pragma once
#ifndef recorder_hpp__
#define recorder_hpp__

#include "../../../types.hpp"

#include <cctype>    // std::tolower
#include <algorithm> // std::equal

namespace zetlab { namespace utils {

inline bool check_convertible(const tstring &filename)
{
    size_t dot = filename.rfind(_T("."));
    if (dot != filename.npos && dot > 0 && dot < filename.size() - 1)
    {
        tstring ext = filename.substr(dot + 1);

#if UNICODE
        if (ext.size() == 3 && ::iswdigit(ext[0]) && ::iswdigit(ext[1]) && ::iswdigit(ext[2]))
            return true;

        ::std::transform(ext.begin(), ext.end(), ext.begin(), ::towupper);
#else
        if (ext.size() == 3 && ::isdigit(ext[0]) && ::isdigit(ext[1]) && ::isdigit(ext[2]))
            return true;

        ::std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
#endif
        if (ext == _T("ZDT") || ext == _T("DAT") || ext == _T("MSEED") || ext == _T("ZSDATA"))
            return true;
    }

    return false;
}

}} // namespace zetlab::utils

#endif // hex_hpp__
