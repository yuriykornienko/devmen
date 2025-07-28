#pragma once

#include "properties_dlg_base.h"

#include <IdTabsDeviceManager.h>

#include "../tabs/zet037_tenso_channel/zet037_tenso_channel_tab.h"

#include "toolbar/clipboard_toolbar.h"

class properties_dlg_zet037 :
	public properties_dlg_base
{
public:
	properties_dlg_zet037(::std::vector<zetlab::object*> objects, CWnd* pParent = NULL);

	enum { IDD = IDD_PROPERTIES_ZET037 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	virtual BOOL refresh();
	virtual BOOL refresh_channel();
	
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	
	afx_msg LRESULT OnUserModify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	
private:
	zet037_tenso_channel_tab  m_zet037_tenso_channel;
};
