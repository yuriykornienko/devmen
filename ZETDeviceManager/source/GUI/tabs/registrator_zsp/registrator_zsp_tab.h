#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

class registrator_zsp_tab : public CTab
{
public:
	registrator_zsp_tab(CWnd* pParent = nullptr);
	virtual ~registrator_zsp_tab() = default;

	enum { IDD = IDD_REGISTRATOR_ZSP_TAB };

	void SetAdapter(zetlab::std::device* const device);

	void Activate() {}
	void Disactivate() {}

protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;

	afx_msg void OnUserSave();
	afx_msg void on_zsp_record_start_mode_request();
	afx_msg void on_zsp_record_continuous_on_off();
	afx_msg void on_zsp_record_duration_change_request();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

private:
	zetlab::std::device* m_device = nullptr;
	
	CComboBox m_combo_record_start;
	CButton m_check_record_continuous;
	
	zetlab::std::start_record_t m_start_record = zetlab::std::start_record_t::undefined;
	bool m_b_record_continuous = false;
	uint32_t m_record_minutes = 0;
	CString m_record_minutes_str;
};
