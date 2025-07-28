#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

class mseed_metadata_tab : public CTab
{
public:
	mseed_metadata_tab(CWnd* pParent = nullptr);
	virtual ~mseed_metadata_tab() = default;

	enum { IDD = IDD_MSEED_METADATA_TAB };

	void SetAdapter(zetlab::std::device* const device);

	void Activate() {}
	void Disactivate() {}

protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;

	afx_msg void OnCancel();
	afx_msg void OnUserCancel();
	afx_msg void OnNetworkChangeRequest();
	afx_msg void OnStationChangeRequest();
	afx_msg void OnLatitudeChangeRequest();
	afx_msg void OnLongitudeChangeRequest();
	afx_msg void OnElevationChangeRequest();
	afx_msg void OnSiteNameChangeRequest();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

private:
	void process_string_as_double(CString& value);

	zetlab::std::device* m_device = nullptr;
	zetlab::std::device_memento_ptr m_memento;
	
	CString m_network;
	CString m_station;
	CString m_latitude;
	CEdit m_latitude_edit;
	CString m_longitude;
	CEdit m_longitude_edit;
	CString m_elevation;
	CEdit m_elevation_edit;
	CString m_site_name;
};
