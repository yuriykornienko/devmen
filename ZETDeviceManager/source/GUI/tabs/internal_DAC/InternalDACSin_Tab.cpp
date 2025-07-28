#include "stdafx.h"
#include "source\GUI\tabs\internal_DAC\InternalDACSin_Tab.h"
#include "../../user_messages.hpp"
#define _USE_MATH_DEFINES
#include "math.h"
#include "resource\Phrases.h"
//*************************************************************************************************
// формат используемый для задания амплитуды и смещения генератора
#define FORMAT_FOR_AMPL_AND_OFFSET L"%.3f"
//*************************************************************************************************
CInternalDACSin_Tab::CInternalDACSin_Tab(CWnd* pParent /*=NULL*/)
	: CTab(CInternalDACSin_Tab::IDD, pParent)
	, m_Activate	(FALSE)
	, m_FrequencyVal(_T(""))
	, m_AmplitudeVal(_T(""))
	, m_OffsetVal	(_T(""))
	, m_Channel	(nullptr)
{
}

CInternalDACSin_Tab::~CInternalDACSin_Tab()
{
}

void CInternalDACSin_Tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);
	DDX_Check(pDX,		IDC_CHECK_ENABLE_DAC,	m_Activate);
	DDX_Text(pDX,		IDC_EDIT_FREQUENCY,		m_FrequencyVal);
	DDX_Text(pDX,		IDC_EDIT_AMPLITUDE,		m_AmplitudeVal);
	DDX_Text(pDX,		IDC_EDIT_OFFSET,		m_OffsetVal);
	DDX_Control(pDX,	IDC_EDIT_FREQUENCY,		m_Edit_Frequency);
	DDX_Control(pDX,	IDC_EDIT_AMPLITUDE,		m_Edit_Amplitude);
	DDX_Control(pDX,	IDC_EDIT_OFFSET,		m_Edit_Offset);
}

BEGIN_MESSAGE_MAP(CInternalDACSin_Tab, CTab)
	ON_MESSAGE(WM_USER_SAVE,					&CInternalDACSin_Tab::OnUserSave)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_DAC,			&CInternalDACSin_Tab::OnBnClickedCheckEnableDac)
END_MESSAGE_MAP()

BOOL CInternalDACSin_Tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_GROUP_DAC)->SetWindowText(g_sControlInternalDAC);
	GetDlgItem(IDC_GROUP_DAC_PARAM)->SetWindowText(g_sParametersInternalDAC);
	GetDlgItem(IDC_STATIC_STATE_DAC)->SetWindowText(g_sStatus + L":");
	GetDlgItem(IDC_STATIC_FREQ_DAC)->SetWindowText(g_sGeneratorFrequency + L", " + g_sUNIT_Hz);
	GetDlgItem(IDC_STATIC_AMPL_DAC)->SetWindowText(g_sGeneratorLevel + L", " + g_sUNIT_V);
	GetDlgItem(IDC_STATIC_OFFSET_DAC)->SetWindowText(g_sOffsetGenerator + L", " + g_sUNIT_V);

	m_Edit_Frequency.SetRange32(0, 10000);
	m_Edit_Frequency.UsePositiveDigitsOnly();
	m_Edit_Frequency.SetUseOnlyIntegersValue();
	m_Edit_Amplitude.SetUseOnlyNumbers();
	m_Edit_Amplitude.UsePositiveDigitsOnly();
	m_Edit_Offset	.SetUseOnlyNumbers();

	UpdateData(FALSE);
	CheckState();

	return TRUE;
}

void CInternalDACSin_Tab::SetAdapter(zetlab::std::channel* const channel__)
{
	m_Channel = channel__;

	long Activate(0);
	double Freq(0.0), Ampl(0.0), Offset(0.0);
	if (m_Channel->GetBuiltinGeneratorSettings(Activate, Freq, Ampl, Offset))
	{
		m_Activate = Activate == 1 ? TRUE : FALSE;
		m_FrequencyVal	.Format(L"%.0f", Freq);
		m_AmplitudeVal	.Format(FORMAT_FOR_AMPL_AND_OFFSET, Ampl);
		m_OffsetVal		.Format(FORMAT_FOR_AMPL_AND_OFFSET, Offset);
		
		float MaxLevel((float)m_Channel->getMaxLevelDAC());
		
		m_Edit_Amplitude.SetRange64(0,			 (float)(MaxLevel / M_SQRT2));
		m_Edit_Offset	.SetRange64	(-MaxLevel,	 MaxLevel);


		m_Edit_Amplitude.SetMinMaxLimits(0,			(float)(MaxLevel / M_SQRT2));
		m_Edit_Offset	.SetMinMaxLimits(-MaxLevel, MaxLevel);
		
		UpdateData(FALSE);
		CheckState();
	}
}

afx_msg LRESULT CInternalDACSin_Tab::OnUserSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	double Freq(_wtof(m_FrequencyVal)), Ampl(_wtof(m_AmplitudeVal)), Offset(_wtof(m_OffsetVal));
	// необходимо отдельно сохранять настройку Enabled
	//m_Channel->SetBuiltinGeneratorEnabled (m_Activate != FALSE ? 1 : 0);
	m_Channel->SetBuiltinGeneratorSettings(m_Activate != FALSE ? 1 : 0, Freq, Ampl, Offset);
	return 0;
}

void CInternalDACSin_Tab::CheckState()
{
	UpdateData(TRUE);

	const static CString sEnabled(g_sOn);
	const static CString sDisEnabled(g_sDisabled);
	if (m_Activate)
		GetDlgItem(IDC_CHECK_ENABLE_DAC)->SetWindowText(sEnabled);
	else
		GetDlgItem(IDC_CHECK_ENABLE_DAC)->SetWindowText(sDisEnabled);
}

void CInternalDACSin_Tab::OnBnClickedCheckEnableDac()
{
	CheckState();
}