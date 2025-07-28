#include "stdafx.h"
#include "InternalDACPPS_Tab.h"
#include "../../user_messages.hpp"
#include "resource\Phrases.h"
//*************************************************************************************************
IMPLEMENT_DYNAMIC(CInternalDACPPS_Tab, CTab)
//*************************************************************************************************
CInternalDACPPS_Tab::CInternalDACPPS_Tab(CWnd* pParent /*=NULL*/)
	: CTab(CInternalDACPPS_Tab::IDD, pParent)
	, m_Activate(FALSE)
	, m_Channel(nullptr)
{

}

CInternalDACPPS_Tab::~CInternalDACPPS_Tab()
{
}

void CInternalDACPPS_Tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ENABLE_DAC_PPS, m_Activate);
}

BEGIN_MESSAGE_MAP(CInternalDACPPS_Tab, CTab)
	ON_MESSAGE(WM_USER_SAVE, &CInternalDACPPS_Tab::OnUserSave)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_DAC_PPS, &CInternalDACPPS_Tab::OnBnClickedCheckEnableDacPps)
END_MESSAGE_MAP()

BOOL CInternalDACPPS_Tab::OnInitDialog()
{
	CTab::OnInitDialog();
	//*********************************************************************************************
	GetDlgItem(IDC_STATIC_STATE_DAC_PPS)->SetWindowText(g_sControl + L" PPS");
	//*********************************************************************************************
	UpdateData(FALSE);
	CheckState();
	//*********************************************************************************************
	return TRUE;
}

void CInternalDACPPS_Tab::SetAdapter(zetlab::std::channel* const channel__)
{
	m_Channel = channel__;
	
	long Activate(0);
	if (m_Channel->Get1PPSBuiltinGeneratorSettings(Activate))
	{	
		m_Activate = Activate == 1 ? TRUE : FALSE;
		
		UpdateData(FALSE);
		CheckState();
	}
}

afx_msg LRESULT CInternalDACPPS_Tab::OnUserSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	m_Channel->Set1PPSBuiltinGeneratorSettings(m_Activate != FALSE ? 1 : 0);
	return 0;
}

void CInternalDACPPS_Tab::CheckState()
{
	UpdateData(TRUE);

	const static CString sEnabled(g_sOn);
	const static CString sDisEnabled(g_sDisabled);
	if (m_Activate)
		GetDlgItem(IDC_CHECK_ENABLE_DAC_PPS)->SetWindowText(sEnabled);
	else
		GetDlgItem(IDC_CHECK_ENABLE_DAC_PPS)->SetWindowText(sDisEnabled);
}

void CInternalDACPPS_Tab::OnBnClickedCheckEnableDacPps()
{
	CheckState();
}