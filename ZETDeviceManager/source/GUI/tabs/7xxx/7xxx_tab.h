#pragma once

#include "../default_tab.h"

#include "../../../algo/7xxx/7xxx/device/config/config_7xxx.hpp"

#include "list/listparams.h"

class C7xxxTab : public CTab
{
public:
	typedef zetlab::_7xxx::device::config::page::param param;

	C7xxxTab(CCFGWorker* pCfgWorker,CWnd* pParent = NULL);
    virtual ~C7xxxTab();
    enum { IDD = IDD_7XXX_TAB };

public:
    void SetAdapter( const zetlab::_7xxx::device::config::page_ptr& page_, const bool& editable );

	void Activate(){}
	void Disactivate(){}
	
	std::vector<CompareParametres> getParam() const { return m_vParamModify; }
	int getItem() const { return m_item; }
protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();

    afx_msg LRESULT OnUserModify( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnUserSetItem( WPARAM wParam, LPARAM lParam );
    afx_msg void OnUserRestore();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    CString     m_desc;
    CFont       m_descFont;
    CParamsList m_list;
	std::vector<CompareParametres> m_vParamModify;
	int         m_item;

	CCFGWorker* m_pCfgWorker;

private:
    zetlab::_7xxx::device::config::page_ptr page_holder;
};
