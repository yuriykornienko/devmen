#include "stdafx.h"
/*#include "../../application/application.h"
#include "../user_messages.hpp"*/
#include "../../algo/utils/zsp_dev_detect/zsp_dev_detect.hpp"
#include "properties_dlg_zet037.h"

/*#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <mathZet.h>
#include <Dialog_ZET\ConnectToZetTools.h>*/
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

properties_dlg_zet037::properties_dlg_zet037(std::vector<zetlab::object*> objects, CWnd* pParent)
	: properties_dlg_base(objects, properties_dlg_zet037::IDD, pParent)
{
}

void properties_dlg_zet037::DoDataExchange(CDataExchange* pDX)
{
	properties_dlg_base::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TABS_ZET037, m_tabs);
}

BEGIN_MESSAGE_MAP(properties_dlg_zet037, properties_dlg_base)
	ON_MESSAGE(WM_USER_MODIFY, OnUserModify)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
END_MESSAGE_MAP()

BOOL properties_dlg_zet037::OnInitDialog()
{
	if (FALSE == properties_dlg_base::OnInitDialog())
		return FALSE;

	return refresh();
}

BOOL properties_dlg_zet037::refresh()
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	switch (m_object->getType())
	{
		case zetlab::object_type::channel:
			return refresh_channel();
	}

	return FALSE;
}

BOOL properties_dlg_zet037::refresh_channel()
{
	if (m_tabs.GetItemCount() > 0)
		return TRUE;

	auto channel = m_object->getAsChannel();
	if (!channel)
		return FALSE;

	m_tabs.InsertItem< zet037_tenso_channel_tab >(0, &m_zet037_tenso_channel, -1, ID_TAB_CHANNEL, TRUE, g_sMeasureName);
	m_zet037_tenso_channel.SetAdapter(channel, const_cast<zetlab::std::device*>(channel->getDevice()));

	m_tabs.SetActiveTab(0);
		
	m_tool_bar.ShowWindow(TRUE);
	m_tool_bar.SetPasteAvailable(m_zet037_tenso_channel.IsClipboardAvailable());
		
	return TRUE;
}

void properties_dlg_zet037::OnOK()
{
	if (IsWindow(m_zet037_tenso_channel.m_hWnd)) 
		m_zet037_tenso_channel.SendMessage(WM_USER_SAVE);

	CAnchoredDialog::OnOK();
}

void properties_dlg_zet037::OnCancel()
{
	if (IsWindow(m_zet037_tenso_channel.m_hWnd)) 
		m_zet037_tenso_channel.SendMessage(WM_USER_CANCEL);
	
	CAnchoredDialog::OnCancel();
}

void properties_dlg_zet037::OnCopy()
{
	if (IsWindow(m_zet037_tenso_channel.m_hWnd))
		m_zet037_tenso_channel.CopyToClipboard();

	m_tool_bar.SetPasteAvailable(m_zet037_tenso_channel.IsClipboardAvailable());
}

void properties_dlg_zet037::OnPaste()
{
	if (IsWindow(m_zet037_tenso_channel.m_hWnd))  
		m_zet037_tenso_channel.PasteFromClipboard();
}

LRESULT properties_dlg_zet037::OnUserModify(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDOK)->EnableWindow(static_cast<BOOL>(lParam));

	return 0;
}