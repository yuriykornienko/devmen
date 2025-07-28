#pragma once
#include "resource/resource.h"
#include "../default_tab.h"
#include "../../../algo/std/channel/channel.hpp"

#include "../../../algo/std/device/device_std.hpp"
#include "afxwin.h"
#include "Controls/SpinEdit/SpinEdit.h"
#include "../../../../resource/phrases.h"
//*************************************************************************************************
#define CEDIT_BASE_USE_TRANSLATE
//*************************************************************************************************
class CInternalDACDC_Tab : public CTab
{
public:
	CInternalDACDC_Tab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInternalDACDC_Tab();
	//*********************************************************************************************
	enum { IDD = IDD_INTERNAL_DAC_DC };
public:	//*****************************************************************************************
	void Activate() override { }
	void Disactivate() override { }
	//*********************************************************************************************
	void SetAdapter(zetlab::std::device* const device__);
	void CheckState();					// устанавливаем текст на галочку
private:	//*************************************************************************************
	zetlab::std::device* m_Device;		// устройство
	// параметры генератора
	BOOL m_Activate;					// активен/не активен
	CString m_OffsetVal;				// смещение
protected:	//*************************************************************************************
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckEnableDac();
	afx_msg LRESULT OnUserSave(WPARAM wParam, LPARAM lParam);
public:	//*****************************************************************************************
	CSpinEdit m_Edit_Offset;
};	//*********************************************************************************************