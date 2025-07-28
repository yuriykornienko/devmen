#include "stdafx.h"

#include "properties_dlg_base.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

properties_dlg_base::properties_dlg_base(::std::vector<zetlab::object*> objects, UINT nIDTemplate, CWnd* pParent)
	: CAnchoredDialog(nIDTemplate, pParent),
	m_objects(objects),
	m_tab(-1)
{
	m_h_accel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
	if (!objects.empty())
		m_object = objects.front();
}

BEGIN_MESSAGE_MAP(properties_dlg_base, CAnchoredDialog)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(properties_dlg_base)
	ANCHOR_MAP_ENTRY(NULL, ANF_AUTOMATIC)
END_ANCHOR_MAP()

BOOL properties_dlg_base::OnInitDialog()
{
	CAnchoredDialog::OnInitDialog();

	if (!m_object)
		return FALSE;

	m_tool_bar.Create(this, IDR_CLIPBOARD_TOOLBAR);

	csLocalization = g_sProperties;
	csLocalization.Append(L": ");
	csLocalization.Append(m_object->getName().c_str());
	if (auto channel = m_object->getAsChannel())
	{
		if (auto slot_serial = channel->get_slot_serial())
		{
			csLocalization.Append(L" (");
			csLocalization.Append(g_sSlot);
			csLocalization.AppendFormat(L" #%u)", slot_serial);
		}
	}
	SetWindowText(csLocalization);

	GetDlgItem(IDOK)->SetWindowTextW(g_sApply);
	GetDlgItem(IDCANCEL)->SetWindowTextW(g_sCancel);

	return TRUE;
}

BOOL properties_dlg_base::PreTranslateMessage(MSG* pMsg)
{
	if (m_h_accel)
	{
		if (::TranslateAccelerator(m_hWnd, m_h_accel, pMsg))
			return TRUE;
	}

	return CAnchoredDialog::PreTranslateMessage(pMsg);
}

void properties_dlg_base::set_tab(int tab)
{
	m_tab = tab;
}
