#pragma once
#include "resource/resource.h"
#include "../default_tab.h"
#include "../../../algo/std/channel/channel.hpp"
#include "afxwin.h"
#include "Controls/SpinEdit/SpinEdit.h"
//*************************************************************************************************
#define CEDIT_BASE_USE_TRANSLATE
//*************************************************************************************************
class CInternalDACSin_Tab : public CTab
{
public:
	CInternalDACSin_Tab(CWnd* pParent = NULL);		// standard constructor
	virtual ~CInternalDACSin_Tab();
	//*********************************************************************************************
	enum { IDD = IDD_INTERNAL_DAC_SIN_TAB };
public:	//*****************************************************************************************
	void Activate() override { }
	void Disactivate() override { }
	//*********************************************************************************************
	void SetAdapter(zetlab::std::channel* const channel__);
	void CheckState();				// ������������� ����� �� �������
private:	//*************************************************************************************
	zetlab::std::channel* m_Channel;	// ����������
	// ��������� ����������
	BOOL m_Activate;				// �������/�� �������
	CString m_FrequencyVal;			// �������
	CString m_AmplitudeVal;			// ���������
	CString m_OffsetVal;			// ��������
protected:	//*************************************************************************************
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckEnableDac();
	afx_msg LRESULT OnUserSave(WPARAM wParam, LPARAM lParam);
public:	//*****************************************************************************************
	CSpinEdit m_Edit_Frequency;
	CSpinEdit m_Edit_Amplitude;
	CSpinEdit m_Edit_Offset;
};	//*********************************************************************************************
