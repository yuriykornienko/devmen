#pragma once

#ifndef _SECURE_ATL
#   define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#   define VC_EXTRALEAN            // Исключите редко используемые компоненты из заголовков Windows
#endif

#define _ApplicationName L"ZETDeviceManager.exe"

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // некоторые конструкторы CString будут явными

// отключает функцию скрытия некоторых общих и часто пропускаемых предупреждений MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // основные и стандартные компоненты MFC
#include <afxext.h>         // расширения MFC
#include <afxdisp.h>        // классы автоматизации MFC
#ifndef _AFX_NO_OLE_SUPPORT
#   include <afxdtctl.h>           // поддержка MFC для типовых элементов управления Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#   include <afxcmn.h>             // поддержка MFC для типовых элементов управления Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // поддержка MFC для лент и панелей управления
#include <afxdisp.h>
#include <afxdisp.h>
#include <afxdisp.h>
#include <afxdisp.h>
#include <afxdisp.h>

#include <Dialog_ZET\Translate.h>

//#define UPDATE_FOR_ZETDEVICEMANAGER

#ifdef _UNICODE
#   if defined _M_IX86
#       pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   elif defined _M_X64
#       pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   else
#       pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   endif
#endif

#define LOAD_ICON__(INSTANCE, ID, cx, cy) \
    (HICON)::LoadImage( INSTANCE, MAKEINTRESOURCE( ID ), IMAGE_ICON, cx, cy, LR_SHARED | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS )

#define LOAD_ICON(ID, cx, cy) \
    LOAD_ICON__( AfxGetInstanceHandle(), ID, cx, cy )

#define USING_CFG_FILE_FOR_CREATE_TAB
