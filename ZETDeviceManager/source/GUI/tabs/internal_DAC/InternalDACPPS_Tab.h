#pragma once
#include "resource/resource.h"
#include "../default_tab.h"
#include "../../../algo/std/channel/channel.hpp"
#include "afxwin.h"
#include "Controls/SpinEdit/SpinEdit.h"
//*************************************************************************************************
class CInternalDACPPS_Tab : public CTab
{
	DECLARE_DYNAMIC(CInternalDACPPS_Tab)

public:
	CInternalDACPPS_Tab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInternalDACPPS_Tab();
	//*********************************************************************************************
	enum { IDD = IDD_INTERNAL_DAC_PPS };
public:	//*****************************************************************************************
	void Activate() override { }
	void Disactivate() override { }
	//*********************************************************************************************
	void SetAdapter(zetlab::std::channel* const channel__);
	void CheckState();					// переключение галочки
private://*****************************************************************************************
	zetlab::std::channel* m_Channel;	// канал
protected:	//*************************************************************************************
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckEnableDacPps();
	afx_msg LRESULT OnUserSave(WPARAM wParam, LPARAM lParam);
public:	//*****************************************************************************************
	BOOL m_Activate;
};	//*********************************************************************************************
