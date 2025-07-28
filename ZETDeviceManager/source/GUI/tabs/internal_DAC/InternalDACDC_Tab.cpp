#include "stdafx.h"
#include "source\GUI\tabs\internal_DAC\InternalDACDC_Tab.h"
#include "../../user_messages.hpp"
#include "InternalDACDC_Tab.h"
#include "resource\Phrases.h"

CInternalDACDC_Tab::CInternalDACDC_Tab(CWnd* pParent /*=NULL*/)
	: CTab(CInternalDACDC_Tab::IDD, pParent)
	, m_Activate(FALSE)
	, m_OffsetVal(_T(""))
	, m_Device(nullptr)
{
}

CInternalDACDC_Tab::~CInternalDACDC_Tab()
{
}

void CInternalDACDC_Tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_Edit_Offset);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_OffsetVal);
	DDX_Check(pDX, IDC_CHECK_ENABLE_DAC, m_Activate);
}

BEGIN_MESSAGE_MAP(CInternalDACDC_Tab, CTab)
	ON_MESSAGE(WM_USER_SAVE, &CInternalDACDC_Tab::OnUserSave)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_DAC, &CInternalDACDC_Tab::OnBnClickedCheckEnableDac)
END_MESSAGE_MAP()

BOOL CInternalDACDC_Tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_GROUP_DAC)->SetWindowText(g_sControlInternalDAC);
	GetDlgItem(IDC_GROUP_DAC_PARAM)->SetWindowText(g_sParametersInternalDAC);
	GetDlgItem(IDC_STATIC_STATE_DAC)->SetWindowText(g_sStatus + L":");
	GetDlgItem(IDC_STATIC_OFFSET_DAC)->SetWindowText(g_sOffsetGenerator + L", " + g_sUNIT_V);

	m_Edit_Offset.SetUseOnlyNumbers();

	UpdateData(FALSE);
	CheckState();

	return TRUE;
}

void CInternalDACDC_Tab::SetAdapter(zetlab::std::device* const device__)
{
	m_Device = device__;

	long Activate(0);
	double Freq(0.0), Ampl(0.0), Offset(0.0);
	if (m_Device->GetBuiltinGeneratorSettings(Activate, Freq, Ampl, Offset))
	{
		m_Activate = Activate == 1 ? TRUE : FALSE;
		m_OffsetVal.Format(L"%.3f", Offset);

		float MaxLevel((float)m_Device->getMaxLevelDAC());
		m_Edit_Offset.SetRange64(-MaxLevel, MaxLevel);

		UpdateData(FALSE);
		CheckState();
	}
}

LRESULT CInternalDACDC_Tab::OnUserSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	// необходимо отдельно сохранять настройку Enabled
	m_Device->SetBuiltinGeneratorSettings(m_Activate != FALSE ? 1 : 0, 0, 0, _wtof(m_OffsetVal));
	m_Device->SetBuiltinGeneratorEnabled(m_Activate != FALSE ? 1 : 0);
	return 0;
}

void CInternalDACDC_Tab::CheckState()
{
	UpdateData(TRUE);

	const static CString sEnabled(g_sOn);
	const static CString sDisEnabled(g_sDisabled);
	if (m_Activate)
		GetDlgItem(IDC_CHECK_ENABLE_DAC)->SetWindowText(sEnabled);
	else
		GetDlgItem(IDC_CHECK_ENABLE_DAC)->SetWindowText(sDisEnabled);

	GetDlgItem(IDC_STATIC_OFFSET_DAC)->EnableWindow(m_Activate);
	GetDlgItem(IDC_EDIT_OFFSET)		 ->EnableWindow(m_Activate);
}

void CInternalDACDC_Tab::OnBnClickedCheckEnableDac()
{
	CheckState();
}