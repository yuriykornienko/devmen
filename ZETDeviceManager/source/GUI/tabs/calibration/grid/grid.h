#pragma once
#ifndef grid_h__
#define grid_h__

class CGrid : public CWnd
{
protected:
	DECLARE_DYNCREATE(CGrid)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x735c6a09, 0x9364, 0x496d, { 0xa8, 0x7d, 0xdc, 0x70, 0x50, 0x68, 0x45, 0x31 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	unsigned long GetClrFon();
	void SetClrFon(unsigned long);
	unsigned long GetClrGrf();
	void SetClrGrf(unsigned long);
	unsigned long GetClrGrd();
	void SetClrGrd(unsigned long);
	unsigned long GetClrCrs();
	void SetClrCrs(unsigned long);
	unsigned long GetClrDig();
	void SetClrDig(unsigned long);
	unsigned long GetClrLeg();
	void SetClrLeg(unsigned long);
	long GetSize();
	void SetSize(long);
	long GetNumber();
	void SetNumber(long);
	long GetTypeLine();
	void SetTypeLine(long);
	long GetTypeGrf();
	void SetTypeGrf(long);
	double GetXfirst();
	void SetXfirst(double);
	double GetXend();
	void SetXend(double);
	double GetMathlx();
	void SetMathlx(double);
	double GetMathdx();
	void SetMathdx(double);
	double GetMathly();
	void SetMathly(double);
	double GetMathdy();
	void SetMathdy(double);
	long GetPx();
	void SetPx(long);
	long GetInd();
	void SetInd(long);
	long GetLog();
	void SetLog(long);
	long GetValid();
	void SetValid(long);
	long GetTypeAbs();
	void SetTypeAbs(long);
	long GetMakeUpor();
	void SetMakeUpor(long);
	double GetUporVerh();
	void SetUporVerh(double);
	double GetUporNis();
	void SetUporNis(double);
	long GetNumVisiblePoints();
	void SetNumVisiblePoints(long);
	double GetUporDelta();
	void SetUporDelta(double);

// Operations
public:
	short Display();
	long Paint(float* buffer);
	void Formatxy(LPCTSTR fmt);
	void AltC(float* buffer);
	void AboutBox();
	void PushToClipBoard();
	long FormatX(LPCTSTR fmt);
	long FormatY(LPCTSTR fmt);
	void AutoScale();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // grid_h__
