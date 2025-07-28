#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "selftest_tab.h"
#include "resource/Phrases.h"

selftest_tab::selftest_tab(CWnd* pParent /*=nullptr*/)
	: CTab(selftest_tab::IDD, pParent)
{}

void selftest_tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SELFTEST, m_combo_selftest);
}

BEGIN_MESSAGE_MAP(selftest_tab, CTab)
	ON_MESSAGE_VOID(WM_USER_CANCEL, OnUserCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_SELFTEST, on_selftest_request)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(selftest_tab)
	ANCHOR_MAP_ENTRY(NULL, ANF_TOP | ANF_LEFTRIGHT)
END_ANCHOR_MAP()

BOOL selftest_tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_SELFTEST)->SetWindowTextW(g_s_selftest_list);
	
	return TRUE;
}

void selftest_tab::SetAdapter(zetlab::std::device* const device)
{
	m_device = device;
	m_memento = boost::make_shared< zetlab::std::device_memento >(device);
	
	m_combo_selftest.ResetContent();
	m_selftest_list.clear();

	auto current = m_device->get_selftest();
	auto selftest_list = m_device->get_selftest_list();
	for (const auto& selftest : selftest_list)
	{
		auto i = m_combo_selftest.AddString(TranslateString(selftest.second.c_str()));
		m_selftest_list.emplace(i, selftest.first);
		if (current == selftest.first)
			m_combo_selftest.SetCurSel(i);
	}
}

void selftest_tab::on_selftest_request()
{
	auto i = m_combo_selftest.GetCurSel();
	try
	{
		m_device->set_selftest(m_selftest_list.at(i));
	}
	catch (const std::out_of_range&) {}
}

void selftest_tab::OnCancel()
{
	if (nullptr == m_device)
		return;

	m_memento->restore();

	CTab::OnCancel();
}

void selftest_tab::OnUserCancel()
{
	if (nullptr == m_device)
		return;

	m_memento->restore();
}