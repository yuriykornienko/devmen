#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

//#include <boost/bind.hpp>
//#include <boost/function.hpp>

//#include <vector>

class CDescriptionTab : public CTab
{
public:
	CDescriptionTab(CWnd* pParent = NULL);
	virtual ~CDescriptionTab();
	enum { IDD = IDD_DESCRIPTION_TAB };

public:
	void SetAdapter( zetlab::std::device* const device__ );

	void Activate() {}
	void Disactivate() {}

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();

	afx_msg void    OnUserSave();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

protected:
	CString        m_description;

private:
	zetlab::std::device*            device_;
	zetlab::std::device_memento_ptr memento_;
};
