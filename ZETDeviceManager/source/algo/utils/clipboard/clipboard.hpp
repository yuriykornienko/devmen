#pragma once
#ifndef clipboard_hpp__
#define clipboard_hpp__

namespace zetlab { namespace utils {

template< typename T, typename U>
void copy_to_clipboard( LPCTSTR lpszFormat, T* object )
{
    UINT uFormat(RegisterClipboardFormat( lpszFormat ));

    if (OpenClipboard( NULL ))
    {
        EmptyClipboard();

// 		HGLOBAL hGlobal(GlobalAlloc(GMEM_DDESHARE, sizeof(T::clipboard)));
// 		T::clipboard* buffer(reinterpret_cast<T::clipboard*>(GlobalLock(hGlobal)));
// 		*buffer = object->toClipboard();
// 		GlobalUnlock(hGlobal);
// 		SetClipboardData(uFormat, hGlobal);
    
// Allocate global memory for the clipboard data
        HGLOBAL hGlobal = GlobalAlloc(GMEM_DDESHARE, sizeof(U));
        if (hGlobal != NULL)
        {
            // Lock the global memory to get a pointer to it
            PVOID xz = GlobalLock(hGlobal);
            if (xz != NULL)
            {
                // Copy the data to the allocated global memory
                U* buffer = reinterpret_cast<U*>(xz);
                *buffer = object->toClipboard();

                // Unlock the global memory
                GlobalUnlock(hGlobal);

                // Set the clipboard data with the specified format
                SetClipboardData(uFormat, hGlobal);
            }
        }

        // Close the clipboard
        CloseClipboard();
    }
}

template< typename T, typename U>
const bool paste_from_clipboard( LPCTSTR lpszFormat, T* object )
{
    bool result(false);
    UINT uFormat(RegisterClipboardFormat( lpszFormat ));

    if (OpenClipboard(NULL))
    {
        HANDLE hGlobal = GetClipboardData(uFormat);

        if (hGlobal)
        {
            PVOID xz = GlobalLock(hGlobal);
            if (xz != NULL)
            {
                U* buffer(reinterpret_cast<U*>(xz));
                object->fromClipboard(*buffer);
                // Unlock the global memory
                GlobalUnlock(hGlobal);

                result = true;
            }
        }

        CloseClipboard();
    }

    return result;
}

inline const bool is_clipboard_format_available( LPCTSTR lpszFormat )
{
    bool result (false);
    UINT uFormat(RegisterClipboardFormat( lpszFormat ));

    if (OpenClipboard( NULL )) 
    {
        result = (NULL != GetClipboardData( uFormat ));

        CloseClipboard();
    }

    return result;
}

}} // namespace zetlab::utils

#endif // clipboard_hpp__
