#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "ipaddress_tab.h"

#include "../../../algo/utils/mac_conv/mac_conv.hpp"
#include "../../../algo/utils/zsp_dev_detect/zsp_dev_detect.hpp"
#include "resource/Phrases.h"

#include <mathZet.h>

CIpAddressTab::CIpAddressTab( CWnd* pParent ) :
	CTab( CIpAddressTab::IDD, pParent )
	, device_(nullptr)
	, m_port(0)
	, m_timeout(0)
	, m_mode(0)
	, m_speed(0)
	, m_ComputerIP(_T(""))
{
}

CIpAddressTab::~CIpAddressTab()
{
}

void CIpAddressTab::DoDataExchange( CDataExchange* pDX )
{
	CTab::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IP_ADDR, m_ip);
	DDX_Control(pDX, IDC_NET_MASK, m_mask);
	DDX_Control(pDX, IDC_GATEWAY, m_gateway);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Text(pDX, IDC_MAC_ADDR, m_mac);
	DDX_Text(pDX, IDC_TIMEOUT, m_timeout);
	DDX_Radio(pDX, IDC_RADIO_DUPLEX, m_mode);
	DDX_Radio(pDX, IDC_RADIO_100MBI, m_speed);
	DDX_Text(pDX, IDC_EDIT_USERPC_IP, m_ComputerIP);
	DDX_Control(pDX, IDC_CHECK_DYNAMIC_IP, m_dynamic_ip);
	DDX_Control(pDX, IDC_CHECK_FTP, m_ftp);
}

BEGIN_MESSAGE_MAP( CIpAddressTab, CTab )
	ON_MESSAGE_VOID( WM_USER_SAVE, OnUserSave )
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_DYNAMIC_IP, on_dhcp_on_off)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CIpAddressTab )
END_ANCHOR_MAP()

BOOL CIpAddressTab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_IP_ADDR)->SetWindowText(g_sIPAddress + L":");
	GetDlgItem(IDC_LABEL_NET_MASK)->SetWindowText(g_sSubnetMaskBig + L":");
	GetDlgItem(IDC_LABEL_GATEWAY)->SetWindowText(g_sMainGateway + L":");
	GetDlgItem(IDC_LABEL_PORT)->SetWindowText(g_sPort + L":");
	GetDlgItem(IDC_LABEL_MAC_ADDR)->SetWindowText(g_sMACAddress + L":");
	GetDlgItem(IDC_LABEL_TIMEOUT)->SetWindowText(g_sTimeout + L", " + g_sUNIT_MINUTES + L":");//"Òàéìàóò, ìèí:"
	GetDlgItem(IDC_GROUP_MODE)->SetWindowText(g_sTransmissionMode);
	GetDlgItem(IDC_RADIO_DUPLEX)->SetWindowText(g_sDuplex);
	GetDlgItem(IDC_RADIO_HALF_DUPLEX)->SetWindowText(g_sHalfDuplex);
	GetDlgItem(IDC_GROUP_SPEED)->SetWindowText(g_sBitRate);
	GetDlgItem(IDC_RADIO_100MBI)->SetWindowTextW(L"100 " + g_sUNIT_Mbps);
	GetDlgItem(IDC_RADIO_10MBI)->SetWindowTextW(L"10 " + g_sUNIT_Mbps);
	GetDlgItem(IDC_CHECK_DYNAMIC_IP)->SetWindowTextW(g_s_dynamic_ip);
	GetDlgItem(IDC_CHECK_FTP)->SetWindowTextW(g_s_ftp);
	return TRUE;
}

#include <WS2tcpip.h>
#pragma comment (lib, "Iphlpapi.lib")
CString getComputerIP()
{
	CString s = g_sNoNetworkConnections;
	ULONG size(0);
	auto rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW)
	{
		return s;
	}

	PIP_ADAPTER_ADDRESSES adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS)
	{
		free(adapter_addresses);
		return s;
	}

	std::vector<std::string> ips;
	std::vector<std::string> masks;
	for (PIP_ADAPTER_ADDRESSES aa = adapter_addresses; aa != NULL; aa = aa->Next)
	{
		for (PIP_ADAPTER_UNICAST_ADDRESS ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next)
		{
			if (ua->Address.lpSockaddr->sa_family != AF_INET)
				continue;

			char buf[BUFSIZ] = "";
			memset(buf, 0, BUFSIZ);
			getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
			std::string s = buf;
			const bool bLocal = s.compare("127.0.0.1") == 0;
			if (!bLocal)
				ips.push_back(buf);
			//ìàñêà ïîäñåòè
			in_addr adr = { 0 };
			ConvertLengthToIpv4Mask(ua->OnLinkPrefixLength, &adr.S_un.S_addr);
			s = inet_ntoa(adr);
			if (!bLocal)
				masks.push_back(s.c_str());
		}
	}
	free(adapter_addresses);

	if (ips.empty())
		return s;
	else if (ips.size() == 1)
	{
		s = g_sIPaddressÑomputer;
		s.Append(L": " + CString(ips.front().c_str()) + L" (");
		s.Append(g_sSubnetmask);
		s.Append(L" " + CString(masks.front().c_str()) + L")");
	}
	else
	{
		s = g_sIPaddressÑomputer + L": ";

		for (auto& ip : ips)
			s.Append(CString(ip.c_str()) + L", ");
		s.Delete(s.GetLength() - 2, 2);
	}

	return s;
}

void CIpAddressTab::SetAdapter( zetlab::std::device* const device__ )
{
	device_ = device__;
	
	unsigned char ip[4];
	unsigned char mask[4];
	unsigned char gateway[4];
	unsigned short port;
	unsigned char mac[6];
	unsigned short timeout;
	unsigned char duplex;
	unsigned char speed;

	device_->readEthSettings( ip, mask, gateway, port, mac, timeout, duplex, speed );

	m_ip.      SetAddress( ip[0], ip[1], ip[2], ip[3] );
	m_mask.    SetAddress( mask[0], mask[1], mask[2], mask[3] );
	m_port = port;

	if (is_zsp(device_->getType()))
	{
		GetDlgItem(IDC_LABEL_GATEWAY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GATEWAY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_MAC_ADDR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MAC_ADDR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TIMEOUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIMEOUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GROUP_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_DUPLEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_HALF_DUPLEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GROUP_SPEED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_100MBI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_10MBI)->ShowWindow(SW_HIDE);
		m_dynamic_ip.SetCheck(device_->get_dhcp());
		auto dhcp = m_dynamic_ip.GetCheck();
		GetDlgItem(IDC_IP_ADDR)->EnableWindow(!dhcp);
		GetDlgItem(IDC_NET_MASK)->EnableWindow(!dhcp);
		if (device_->has_ftp())
			m_ftp.SetCheck(device_->get_ftp());
		else
			GetDlgItem(IDC_CHECK_FTP)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_DYNAMIC_IP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_FTP)->ShowWindow(SW_HIDE);
		m_gateway.SetAddress(gateway[0], gateway[1], gateway[2], gateway[3]);
		m_mac = zetlab::utils::mac_to_string(mac).c_str();
		m_timeout = timeout / 60;
		m_mode = (duplex) ? 0 : 1;
		m_speed = (speed) ? 0 : 1;
	}

	m_ComputerIP = getComputerIP();
	UpdateData(FALSE);

	SetTimer(0, 5000, NULL);
}

void CIpAddressTab::OnUserSave()
{
	UpdateData();

	unsigned char ip[4];
	unsigned char mask[4];
	unsigned char gateway[4];
	unsigned char duplex = {};
	unsigned char speed = {};

	m_ip.      GetAddress( ip[0], ip[1], ip[2], ip[3] );
	m_mask.    GetAddress( mask[0], mask[1], mask[2], mask[3] );
	if (is_zsp(device_->getType()))
	{
		device_->set_dhcp(m_dynamic_ip.GetCheck());
		device_->set_ftp(m_ftp.GetCheck());
	}
	else
	{
		m_gateway.GetAddress(gateway[0], gateway[1], gateway[2], gateway[3]);
		duplex = m_mode ? 0 : 1;
		speed = m_speed ? 0 : 1;
	}
	device_->writeEthSettings( ip, mask, gateway, m_timeout * 60, duplex, speed );
}

void CIpAddressTab::on_dhcp_on_off()
{
	auto dhcp = m_dynamic_ip.GetCheck();
	GetDlgItem(IDC_IP_ADDR)->EnableWindow(!dhcp);
	GetDlgItem(IDC_NET_MASK)->EnableWindow(!dhcp);
}

void CIpAddressTab::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case 0:
			m_ComputerIP = getComputerIP();
			UpdateData(FALSE);
			break;
		default:
			break;
	}

	CTab::OnTimer(nIDEvent);
}
