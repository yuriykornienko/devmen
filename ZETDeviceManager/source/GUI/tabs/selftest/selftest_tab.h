#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

class selftest_tab : public CTab
{
public:
	selftest_tab(CWnd* pParent = nullptr);
	virtual ~selftest_tab() = default;

	enum { IDD = IDD_SELFTEST_TAB };

	void SetAdapter(zetlab::std::device* const device);

	void Activate() {}
	void Disactivate() {}

protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;

	afx_msg void OnCancel();
	afx_msg void OnUserCancel();
	afx_msg void on_selftest_request();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

private:
	zetlab::std::device* m_device = nullptr;
	zetlab::std::device_memento_ptr m_memento;
	
	CComboBox m_combo_selftest;

	std::map<uint32_t, zetlab::tstring> m_selftest_list;
};
