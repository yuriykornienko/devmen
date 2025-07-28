#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "mseed_metadata_tab.h"
#include "resource/Phrases.h"

#include <boost/lexical_cast.hpp>

mseed_metadata_tab::mseed_metadata_tab(CWnd* pParent /*=nullptr*/)
	: CTab(mseed_metadata_tab::IDD, pParent)
{}

void mseed_metadata_tab::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_NETWORK, m_network);
	DDX_Text(pDX, IDC_STATION, m_station);
	DDX_Text(pDX, IDC_LATITUDE, m_latitude);
	DDX_Control(pDX, IDC_LATITUDE, m_latitude_edit);
	DDX_Text(pDX, IDC_LONGITUDE, m_longitude);
	DDX_Control(pDX, IDC_LONGITUDE, m_longitude_edit);
	DDX_Text(pDX, IDC_ELEVATION, m_elevation);
	DDX_Control(pDX, IDC_ELEVATION, m_elevation_edit);
	DDX_Text(pDX, IDC_SITE_NAME, m_site_name);

	CTab::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(mseed_metadata_tab, CTab)
	ON_MESSAGE_VOID(WM_USER_CANCEL, OnUserCancel)
	ON_EN_CHANGE(IDC_NETWORK, OnNetworkChangeRequest)
	ON_EN_CHANGE(IDC_STATION, OnStationChangeRequest)
	ON_EN_CHANGE(IDC_LATITUDE, OnLatitudeChangeRequest)
	ON_EN_CHANGE(IDC_LONGITUDE, OnLongitudeChangeRequest)
	ON_EN_CHANGE(IDC_ELEVATION, OnElevationChangeRequest)
	ON_EN_CHANGE(IDC_SITE_NAME, OnSiteNameChangeRequest)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(mseed_metadata_tab)
	ANCHOR_MAP_ENTRY(NULL, ANF_TOP | ANF_LEFTRIGHT)
END_ANCHOR_MAP()

BOOL mseed_metadata_tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_NETWORK)->SetWindowTextW(g_s_network + L":");
	GetDlgItem(IDC_LABEL_STATION)->SetWindowTextW(g_s_station + L":");
	GetDlgItem(IDC_LABEL_LATITUDE)->SetWindowTextW(g_s_latitude + L":");
	GetDlgItem(IDC_LABEL_LONGITUDE)->SetWindowTextW(g_s_longitude + L":");
	GetDlgItem(IDC_LABEL_ELEVATION)->SetWindowTextW(g_s_elevation + L":");
	GetDlgItem(IDC_LABEL_SITE_NAME)->SetWindowTextW(g_s_site_name + L":");
	
	return TRUE;
}

void mseed_metadata_tab::SetAdapter(zetlab::std::device* const device)
{
	m_device = device;
	m_memento = boost::make_shared< zetlab::std::device_memento >(device);
	
	m_network = device->get_network().c_str();
	m_station = device->get_station().c_str();
	m_latitude.Format(_T("%g"), device->get_latitude());
	m_longitude.Format(_T("%g"), device->get_longitude());
	m_elevation.Format(_T("%g"), device->get_elevation());
	m_site_name = device->get_site_name().c_str();

	UpdateData(FALSE);
}

void mseed_metadata_tab::OnCancel()
{
	if (nullptr == m_device)
		return;

	m_memento->restore();

	CTab::OnCancel();
}

void mseed_metadata_tab::OnUserCancel()
{
	if (nullptr == m_device)
		return;

	m_memento->restore();
}

void mseed_metadata_tab::OnNetworkChangeRequest()
{
	if (nullptr == m_device)
		return;

	UpdateData();

	if (m_network.GetLength() > 2)
		m_network.Left(2);

	m_device->set_network(m_network.GetString());
}

void mseed_metadata_tab::OnStationChangeRequest()
{
	if (nullptr == m_device)
		return;

	UpdateData();

	if (m_station.GetLength() > 5)
		m_station.Left(5);

	m_device->set_station(m_station.GetString());
}

void mseed_metadata_tab::OnLatitudeChangeRequest()
{
	if (nullptr == m_device)
		return;

	CString latitude = m_latitude;

	UpdateData();

	DWORD sel = m_latitude_edit.GetSel();

	process_string_as_double(m_latitude);

	try
	{
		zetlab::tstring l = m_latitude;
		auto latitude_value = boost::lexical_cast<double>(l);
		if (latitude_value < -90.)
		{
			latitude_value = -90.;
			m_latitude = L"-90.0";
		}
		else if (latitude_value > 90.)
		{
			latitude_value = 90.;
			m_latitude = L"90.0";
		}
		m_device->set_latitude(latitude_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_latitude.Compare(_T("-")))
			m_latitude = latitude;
	}

	UpdateData(FALSE);

	m_latitude_edit.SetSel(sel);
}

void mseed_metadata_tab::OnLongitudeChangeRequest()
{
	if (nullptr == m_device)
		return;

	CString longitude = m_longitude;

	UpdateData();

	DWORD sel = m_longitude_edit.GetSel();

	process_string_as_double(m_longitude);

	try
	{
		zetlab::tstring l = m_longitude;
		auto longitude_value = boost::lexical_cast<double>(l);
		if (longitude_value < -180.)
		{
			longitude_value = -180.;
			m_longitude = L"-180.0";
		}
		else if (longitude_value > 180.)
		{
			longitude_value = 180.;
			m_longitude = L"180.0";
		}
		m_device->set_longitude(longitude_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_longitude.Compare(_T("-")))
			m_longitude = longitude;
	}

	UpdateData(FALSE);

	m_longitude_edit.SetSel(sel);
}

void mseed_metadata_tab::OnElevationChangeRequest()
{
	if (nullptr == m_device)
		return;

	CString elevation = m_elevation;

	UpdateData();

	DWORD sel = m_elevation_edit.GetSel();

	process_string_as_double(m_elevation);

	try
	{
		zetlab::tstring e = m_elevation;
		auto elevation_value = boost::lexical_cast<double>(e);
		if (elevation_value < -10000.)
		{
			elevation_value = -10000.;
			m_elevation = L"-10000.0";
		}
		else if (elevation_value > 10000.)
		{
			elevation_value = 10000.;
			m_elevation = L"10000.0";
		}
		m_device->set_elevation(elevation_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_elevation.Compare(_T("-")))
			m_elevation = elevation;
	}

	UpdateData(FALSE);

	m_elevation_edit.SetSel(sel);
}

void mseed_metadata_tab::OnSiteNameChangeRequest()
{
	if (nullptr == m_device)
		return;

	UpdateData();

	m_device->set_site_name(m_site_name.GetString());
}

void mseed_metadata_tab::process_string_as_double(CString& value)
{
	if (value.Find(L",") != -1)
		value.Replace(L",", L".");
	if (value.Find(L"á") != -1)
		value.Replace(L"á", L".");
	if (value.Find(L"þ") != -1)
		value.Replace(L"þ", L".");
	if (value.Find(L"\\") != -1)
		value.Replace(L"\\", L".");
	if (value.Find(L"\\") != -1)
		value.Replace(L"\\", L".");
	if (value.Find(L"/") != -1)
		value.Replace(L"/", L".");
}