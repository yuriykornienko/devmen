#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "registrator_zsp_tab.h"
#include "resource/Phrases.h"

#include <boost/lexical_cast.hpp>

registrator_zsp_tab::registrator_zsp_tab(CWnd* pParent /*=nullptr*/)
	: CTab(registrator_zsp_tab::IDD, pParent)
{}

void registrator_zsp_tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_ZSP_RECORD_START_MODE, m_combo_record_start);
	DDX_Control(pDX, IDC_CHECK_ZSP_RECORD_CONTINUOUS, m_check_record_continuous);
	DDX_Text(pDX, IDC_EDIT_ZSP_RECORD_DURATION, m_record_minutes_str);
}

BEGIN_MESSAGE_MAP(registrator_zsp_tab, CTab)
	ON_MESSAGE_VOID(WM_USER_SAVE, OnUserSave)
	ON_CBN_SELCHANGE(IDC_COMBO_ZSP_RECORD_START_MODE, on_zsp_record_start_mode_request)
	ON_BN_CLICKED(IDC_CHECK_ZSP_RECORD_CONTINUOUS, on_zsp_record_continuous_on_off)
	ON_EN_CHANGE(IDC_EDIT_ZSP_RECORD_DURATION, on_zsp_record_duration_change_request)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(registrator_zsp_tab)
	ANCHOR_MAP_ENTRY(NULL, ANF_TOP | ANF_LEFTRIGHT)
END_ANCHOR_MAP()

BOOL registrator_zsp_tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_ZSP_RECORD_START_MODE)->SetWindowTextW(g_s_zsp_record_start_mode);
	GetDlgItem(IDC_CHECK_ZSP_RECORD_CONTINUOUS)->SetWindowTextW(g_s_zsp_record_continuous);
	GetDlgItem(IDC_LABEL_ZSP_RECORD_DURATION)->SetWindowTextW(g_s_zsp_record_duration);

	m_combo_record_start.ResetContent();
	m_combo_record_start.AddString(g_s_zsp_record_start_mode_button);
	m_combo_record_start.AddString(g_s_zsp_record_start_mode_auto);

	return TRUE;
}

void registrator_zsp_tab::SetAdapter(zetlab::std::device* const device)
{
	m_device = device;
	
	m_start_record = m_device->get_sd_start_record();
	if (auto start_record = static_cast<uint16_t>(m_start_record); start_record > 0)
		m_combo_record_start.SetCurSel(start_record - 1);

	m_b_record_continuous = m_device->get_sd_record_continuous();
	m_check_record_continuous.SetCheck(m_b_record_continuous);
	GetDlgItem(IDC_EDIT_ZSP_RECORD_DURATION)->EnableWindow(!m_b_record_continuous);

	m_record_minutes = m_device->get_sd_record_minutes();
	if (0 == m_record_minutes)
		m_record_minutes = 10;
	SetDlgItemText(IDC_EDIT_ZSP_RECORD_DURATION, std::to_wstring(m_record_minutes).c_str());
}

void registrator_zsp_tab::on_zsp_record_start_mode_request()
{
	m_start_record = static_cast<zetlab::std::start_record_t>(m_combo_record_start.GetCurSel() + 1);
}

void registrator_zsp_tab::on_zsp_record_continuous_on_off()
{
	m_b_record_continuous = m_check_record_continuous.GetCheck();
	GetDlgItem(IDC_EDIT_ZSP_RECORD_DURATION)->EnableWindow(!m_b_record_continuous);
}

void registrator_zsp_tab::OnUserSave()
{
	if (nullptr == m_device)
		return;

	m_device->set_sd_start_record(m_start_record);
	if (m_b_record_continuous)
		m_device->set_sd_record_continuous(true);
	else
		m_device->set_sd_record_minutes(m_record_minutes);
}

void registrator_zsp_tab::on_zsp_record_duration_change_request()
{
	CEdit* e = (CEdit*)GetDlgItem(IDC_EDIT_ZSP_RECORD_DURATION);
		
	CString record_minutes = m_record_minutes_str;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		m_record_minutes = boost::lexical_cast<uint32_t>((LPCTSTR)m_record_minutes_str);
		if (m_record_minutes < 1)
			m_record_minutes = 1;
		if (m_record_minutes > 30)
			m_record_minutes = 30;
	}
	catch (boost::bad_lexical_cast&)
	{	
	}

	m_record_minutes_str.Format(L"%u", m_record_minutes);
	
	UpdateData(FALSE);
	e->SetSel(ret);
}
