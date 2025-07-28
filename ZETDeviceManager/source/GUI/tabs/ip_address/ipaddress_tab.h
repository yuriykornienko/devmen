#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

class CIpAddressTab : public CTab
{
public:
    CIpAddressTab( CWnd* pParent = NULL );
    virtual ~CIpAddressTab();
    enum { IDD = IDD_IPADDRESS_TAB };

public:
    void SetAdapter( zetlab::std::device* const device__ );

	void Activate(){}
	void Disactivate(){}

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();

    afx_msg void OnUserSave();
    afx_msg void on_dhcp_on_off();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    CIPAddressCtrl m_ip;
    CIPAddressCtrl m_mask;
    CIPAddressCtrl m_gateway;
    int            m_port;
    CString        m_mac;
    int            m_timeout;
    int            m_mode;
    int            m_speed;
    CButton        m_dynamic_ip;
    CButton        m_ftp;

private:
    zetlab::std::device* device_;
public:
	CString m_ComputerIP;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
