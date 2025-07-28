#pragma once

#include <AnchoredDialog/AnchoredDialog.h>
#include <Localized/Localized.h>

#include "../../algo/object/object.hpp"
#include "../tabs/tabs.h"
#include "toolbar/clipboard_toolbar.h"

#include "afxwin.h"

class properties_dlg_base :
	public CAnchoredDialog,
	public CLocalized
{
public:
	properties_dlg_base(::std::vector<zetlab::object*> objects, UINT nIDTemplate, CWnd* pParent = NULL);
	
	void set_tab(int tab);

	zetlab::object* get_object() { return m_object; }

protected:
	virtual BOOL OnInitDialog() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

	HACCEL m_h_accel;
	CTabs m_tabs;
	CClipboardToolBar m_tool_bar;
	
	std::vector<zetlab::object*> m_objects;
	zetlab::object* m_object = nullptr;

	int m_tab;

private:
	
};
