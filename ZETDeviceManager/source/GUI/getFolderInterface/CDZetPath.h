// CDZetPath.h  : Declaration of ActiveX Control wrapper class(es) created by Microsoft Visual C++

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDZetPath

class CDZetPath : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDZetPath)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x81A7C169, 0x2312, 0x4D88, { 0xA8, 0x7A, 0x82, 0xC2, 0xFB, 0xA7, 0x0, 0xE8 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attributes
public:

// Operations
public:

	CString ZetPath(BSTR * param)
	{
		CString result;
		static BYTE parms[] = VTS_PBSTR ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, param);
		return result;
	}
	long ZetLetPath(BSTR * param, BSTR * path)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, param, path);
		return result;
	}
	CString GetNextFileInDir(BSTR * dir, BSTR * file, BSTR * ext)
	{
		CString result;
		static BYTE parms[] = VTS_PBSTR VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, dir, file, ext);
		return result;
	}
	CString ZetPathW(BSTR * param)
	{
		CString result;
		static BYTE parms[] = VTS_PBSTR ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, param);
		return result;
	}
	long ZetLetPathW(BSTR * param, BSTR * path)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, param, path);
		return result;
	}
	long ZetSetPath(BSTR * param, BSTR * path)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, param, path);
		return result;
	}
//	CString GetNextDirInDir(BSTR * bszDir, BSTR * bszName)
//	{
//		CString result;
//		static BYTE parms[] = VTS_PBSTR VTS_PBSTR ;
//		InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, bszDir, bszName);
//		return result;
//	}
	CString GetNextSignals()
	{
		CString result;
		static BYTE parms = VTS_NONE;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, &parms);
		return result;
	}

};
