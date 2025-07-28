#include "stdafx.h"
#include "../../../application/application.h"
#include "common_tab.h"

#include "../../../types.hpp"

#include "../../../algo/utils/bytes_conv/bytes_conv.hpp"
#include "../../../algo/utils/time_conv/time_conv.hpp"
#include "../../../algo/utils/zsp_dev_detect/zsp_dev_detect.hpp"

#include <Zadc.h>

#include <mathZet.h>
#include "resource\Phrases.h"
#include <TrendWriter\DriveChecker.h>


#define LoadBitmap_(nID) \
    (HBITMAP)::LoadImage( AfxGetInstanceHandle(), \
    MAKEINTRESOURCE( nID ), \
    IMAGE_BITMAP, 64, 64, LR_SHARED | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS )

CCommonTab::CCommonTab( CWnd* pParent ) : CTab( CCommonTab::IDD, pParent ),
    m_serial(0),
	m_version(_T("")),
	m_flashState(_T("")),
	m_batteryState(_T("")),
	m_fDiskState(_T(""))
{
}

CCommonTab::~CCommonTab()
{
}

void CCommonTab::DoDataExchange( CDataExchange* pDX )
{
    CTab::DoDataExchange( pDX );

    DDX_Control( pDX, IDC_ICON,    m_icon );
    DDX_Text   ( pDX, IDC_CAPTION, m_caption );
    DDX_Text   ( pDX, IDC_TYPE,    m_typeStr );
    DDX_Text   ( pDX, IDC_VERSION, m_version );
	DDX_Text   ( pDX, IDC_FLASH_STATE, m_flashState );
	DDX_Text   ( pDX, IDC_HARDDISK_STATE, m_fDiskState);
	DDX_Text   ( pDX, IDC_BATTERY_STATE, m_batteryState );
	DDX_Text   ( pDX, IDC_SYNC_STATE, m_syncState );
}

BEGIN_MESSAGE_MAP( CCommonTab, CTab )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CCommonTab )
END_ANCHOR_MAP()

BOOL CCommonTab::OnInitDialog()
{
    CTab::OnInitDialog();

	GetDlgItem(IDC_STATIC_ERROR_CFG7XXX)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_ERROR_CFG7XXX)->SetWindowTextW(g_sErrorReadConfiguration);
	GetDlgItem(IDC_LABEL_TYPE)->SetWindowTextW(g_sDeviceType + L":");
 	GetDlgItem(IDC_LABEL_VERSION)->SetWindowTextW(g_sVersion + L":");
	GetDlgItem(IDC_LABEL_SD_STATE)->SetWindowTextW(g_sFlashDrive + L":");
	GetDlgItem(IDC_LABEL_HD_STATE)->SetWindowTextW(g_sLocalDisk + L":");
	GetDlgItem(IDC_LABEL_ACCUM_STATE)->SetWindowTextW(g_sBatteryСharge + L":");
	GetDlgItem(IDC_LABEL_SYNC_STATE)->SetWindowTextW(g_sSynchronization + L" GPS:");
	GetDlgItem(IDC_UPTIMEDEVICE_LABEL)->SetWindowTextW(g_sDeviceUptime + L":");
	GetDlgItem(IDC_UPTIMEDRIVER_LABEL)->SetWindowTextW(g_sDriverUptime + L":");

	m_zPath.Create(L"", WS_CHILD, CRect(0, 0, 0, 0), this, 3);

	return TRUE;
}

void CCommonTab::SetAdapter( zetlab::std::device* const device_,
                             const zetlab::tstring& name )
{
    m_caption = name.c_str();
    m_typeStr = device_->getTypeStr();
    m_serial  = device_->getSerial();
    m_version = device_->getVersion().c_str();
	
    switch (device_->getType())
    {
    case KDU1616:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZET210 ) );
        break;

    case KDU1624:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZET220 ) );
        break;

    case KDU0424:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZET230 ) );
        break;

    case KDU0824:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZET240 ) );
        break;

    case KDU2500:
        m_icon.SetBitmap( LoadBitmap_( IDB_A17U2 ) );
        break;

    case KDU8500:
    case ZADC_ZDS:
        m_icon.SetBitmap( LoadBitmap_( IDB_A17U8 ) );
        break;

    case KDUVN:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZET110 ) );
        break;

	case 30:
		if (auto name = device_->getNameDevice(); name.find(_T("-P")) != zetlab::tstring::npos)
			m_icon.SetBitmap(LoadBitmap_(IDB_ZET030P));
		else if (name.find(_T("-I")) != zetlab::tstring::npos)
			m_icon.SetBitmap(LoadBitmap_(IDB_ZET030I));
		else
			m_icon.SetBitmap(LoadBitmap_(IDB_DEMO));
		break;

	case 211:
		m_icon.SetBitmap(LoadBitmap_(IDB_ZET211));
		break;

	case 221:
		m_icon.SetBitmap(LoadBitmap_(IDB_ZET221));
		break;

	case 37:
		m_icon.SetBitmap(LoadBitmap_(IDB_ZET037));
		break;

	case 48:
		m_icon.SetBitmap(LoadBitmap_(IDB_ZET048_E24));
		break;

    default:
        m_icon.SetBitmap( LoadBitmap_( IDB_DEMO ) );
        break;
    }

	BSTR bstrDirSignal = _T("DirSignal");
	CString path(m_zPath.ZetPath(&bstrDirSignal));
	CDriveChecker m_DriveChecker;
	m_fDiskState = retStrConvTime(m_DriveChecker.GetFreeDiskSize(path), (long)device_->getAdcFrequency()*device_->numChannels());
	GetDlgItem(IDC_LABEL_HD_STATE)->SetWindowTextW(g_sLocalDisk + L" ("+ path[0]+L")" + L":");

	const auto bFlash = device_->isHaveSd() && !is_zsp(device_->getType());
	GetDlgItem(IDC_LABEL_SD_STATE)->ShowWindow(bFlash);
	GetDlgItem(IDC_FLASH_STATE)->ShowWindow(bFlash);
	unsigned long long availableSize(0);
	if (bFlash && device_->readSdAvailableSize(availableSize))
	{
		const auto availableTime = availableSize / 
									2 / device_->GetWordsADC() / 
									device_->numChannels() / 
									device_->getAdcFrequency();
		m_flashState = g_sAvailable + _T(": ") +
					   CString(zetlab::utils::bytes_to_string(availableSize).c_str()) +
					   _T(" (") +
					   CString(zetlab::utils::time_duration_to_string(availableTime).c_str()) +
					   _T(")");
	}
	else
		m_flashState = g_sErrorWorkingSD;

	GetDlgItem(IDC_LABEL_ACCUM_STATE)->ShowWindow(bFlash);
	GetDlgItem(IDC_BATTERY_STATE)->ShowWindow(bFlash);
	unsigned char batteryCharge(0);
	char boardTemperature(0);
	unsigned short batteryVoltage(0);
	if (device_->getType() == KDU0824 && device_->readBatteryState(batteryCharge, boardTemperature, batteryVoltage))
	{
		m_batteryState.Format(L"%s %d%%", g_sCapacity.GetString(), batteryCharge);
		if (batteryVoltage > 0)
			m_batteryState.AppendFormat(L", %s %.1f%s", g_sVoltage.GetString(), (float)batteryVoltage / 1000.f, g_sUNIT_V.GetString());
	}
	else
	{
		m_batteryState = g_sInformationNotAvailable;
	}

	const auto bSync = device_->isHaveGps();
	GetDlgItem(IDC_LABEL_SYNC_STATE)->ShowWindow(bSync);
	GetDlgItem(IDC_SYNC_STATE)->ShowWindow(bSync);

	__time64_t time(0);
	bool synced(false);
	unsigned char pps(0);
	unsigned char sats(0);
	if (bSync && device_->readGpsInfo(time, synced, pps, sats))
	{
		m_syncState = synced ? g_sYes : g_sNo;
		m_syncState.AppendFormat(_T(", %d ") + g_sSatellites + L".", sats);
	}
	else
		m_syncState = g_sInformationNotAvailable;

	unsigned long m_UptmDevice(device_->GetUptimeDevice());
	if (m_UptmDevice != 0)
	{
		CString temp(L""); 
		long res_days = m_UptmDevice / 86400;
		if (res_days > 0)
		{
			temp.AppendFormat(L"%d", res_days);
			temp+=L" "+ g_sUNIT_DAY + L". ";
		}
		long res_hours = (m_UptmDevice % 86400) / 3600;
		if (res_hours > 0)
		{
			temp.AppendFormat(L"%d", res_hours);
			temp += L" " + g_sUNIT_HOURS + L". ";

		}
		long res_minutes = ((m_UptmDevice % 86480) % 3600) / 60;
		if (res_minutes > 0)
		{
			temp.AppendFormat(L"%d", res_minutes);
			temp += L" " + g_sUNIT_MINUTES + L". ";
		}
		long res_seconds = ((m_UptmDevice % 86480) % 3600) % 60;
		if (res_seconds > 0)
		{
			temp.AppendFormat(L"%d", res_seconds);
			temp += L" " + g_sUNIT_S + L". ";
		}
		GetDlgItem(IDC_UPTIMEDEVICE)->SetWindowTextW(temp);
	}
	else
	{
		GetDlgItem(IDC_UPTIMEDEVICE_LABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UPTIMEDEVICE)->ShowWindow(SW_HIDE);
	}

	unsigned long m_UptmDriver(device_->GetUptimeDriver());
	if (m_UptmDriver != 0)
	{
		CString temp(L""); 
		long res_days = m_UptmDriver / 86400;
		if (res_days > 0)
		{
			temp.AppendFormat(L"%d", res_days);
			temp += L" " + g_sUNIT_DAY + L". ";
		}
		long res_hours = (m_UptmDriver % 86400) / 3600;
		if (res_hours > 0)
		{
			temp.AppendFormat(L"%d", res_hours);
			temp += L" " + g_sUNIT_HOURS + L". ";
		}
		long res_minutes = ((m_UptmDriver % 86480) % 3600) / 60;
		if (res_minutes > 0)
		{
			temp.AppendFormat(L"%d", res_minutes);
			temp += L" " + g_sUNIT_MINUTES + L". ";
		}
		long res_seconds = ((m_UptmDriver % 86480) % 3600) % 60;
		if (res_seconds > 0)
		{
			temp.AppendFormat(L"%d", res_seconds);
			temp += L" " + g_sUNIT_S + L". ";
		}
		GetDlgItem(IDC_UPTIMEDRIVER)->SetWindowTextW(temp);
	}
	else
	{
		GetDlgItem(IDC_UPTIMEDRIVER_LABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UPTIMEDRIVER)->ShowWindow(SW_HIDE);
	}

	// Сдвиг полей, если что то спрятали
	ShiftControl(device_);

	UpdateData( FALSE );

	GetDlgItem(IDC_VERSION)->ShowWindow(TRUE);
	GetDlgItem(IDC_LABEL_VERSION)->ShowWindow(TRUE);
}

void CCommonTab::SetAdapter( zetlab::_7xxx::device* const device_,
                             const zetlab::tstring& name )
{
    m_caption = name.c_str();
    m_typeStr = device_->getTypeStr().c_str();

    switch (device_->getType())
    {
    case zetlab::_7xxx::controller:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZETCONTROLLER ) );
        break;

    case zetlab::_7xxx::sensor:
        m_icon.SetBitmap( LoadBitmap_( IDB_ZETSENSOR ) );
        break;

    default:
        m_icon.SetBitmap( LoadBitmap_( IDB_DEMO ) );
        break;
    }

    UpdateData( FALSE );

	GetDlgItem(IDC_VERSION)->ShowWindow(FALSE);
	GetDlgItem(IDC_LABEL_VERSION)->ShowWindow(FALSE);
	GetDlgItem(IDC_LABEL_SD_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_LABEL_HD_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_FLASH_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_HARDDISK_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_LABEL_ACCUM_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_BATTERY_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_LABEL_SYNC_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_SYNC_STATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_UPTIMEDEVICE_LABEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_UPTIMEDRIVER_LABEL)->ShowWindow(FALSE);
}


void CCommonTab::ShowMessageError7xxx()
{
	GetDlgItem(IDC_STATIC_ERROR_CFG7XXX)->ShowWindow(TRUE);
	CRect rectLabel, rectType;
	GetDlgItem(IDC_LABEL_TYPE)->GetWindowRect(&rectLabel);
	GetDlgItem(IDC_LABEL_TYPE)->GetParent()->ScreenToClient(&rectLabel);

	GetDlgItem(IDC_TYPE)->GetWindowRect(&rectType);
	GetDlgItem(IDC_TYPE)->GetParent()->ScreenToClient(&rectType);
	rectLabel.top += rectLabel.Height() + 70;
	rectLabel.right = rectType.right - 40;
	rectLabel.left -= 5;
	rectLabel.bottom = rectLabel.top + 50;
	GetDlgItem(IDC_STATIC_ERROR_CFG7XXX)->MoveWindow(rectLabel);
}

CString CCommonTab::retStrConvTime(unsigned long long Disksize, long freq)
{
	CString ret(g_sAvailable);
	ret += L": ";
	if (Disksize >= 1024 * 1024 * 1024)
		ret.AppendFormat(L"%i " + g_sUNIT_GB, Disksize / 1024 / 1024 / 1024);
	else if (Disksize >= 1024 * 1024)
		ret.AppendFormat(L"%i " + g_sUNIT_MB, Disksize / 1024 / 1024);
	else if (Disksize >= 1024)
		ret.AppendFormat(L"%i " + g_sUNIT_KB, Disksize / 1024);
	else
		ret.AppendFormat(L"%i " + g_sUNIT_byte, Disksize);

	if (freq != 0)
	{
		unsigned long long ltime(Disksize / long long((freq * (sizeof(float)))));
		ret += L" (~";
		if (ltime >= 60 * 60 * 24 * 30 * 12)
		{
			long long year = ltime / 60 / 60 / 24 / 30 / 12;
			if (GetYearsPlural(uint32_t(year)) == 1)
			{
				ret.AppendFormat(L"%i " + g_sUNIT_YEAR, year);
			}
			else if (GetYearsPlural(uint32_t(year)) == 2)
			{
				ret.AppendFormat(L"%i " + g_sUNIT_YEARS, year);
			}
			else
			{
				ret.AppendFormat(L"%i " + g_sUNIT_LET, year);
			}
		}

		else if (ltime >= 60 * 60 * 24 * 30)
			ret.AppendFormat(L"%i " + g_sUNIT_MONTH, ltime / 60 / 60 / 24 / 30);
		else if (ltime >= 60 * 60 * 24)
			ret.AppendFormat(L"%i " + g_sUNIT_DAY, ltime / 60 / 60 / 24);
		else if (ltime >= 60 * 60)
			ret.AppendFormat(L"%i " + g_sUNIT_HOURS, ltime / 60 / 60);
		else if (ltime >= 60)
			ret.AppendFormat(L"%i " + g_sUNIT_MINUTES, ltime / 60);
		else
			ret.AppendFormat(L"%i " + g_sUNIT_S, ltime);

		ret += L".)";
	}
	return ret;
}

// Склонение года
const char CCommonTab::GetYearsPlural(uint32_t years)
{
	if (years % 100 < 5 || years % 100 > 20)
	{
		switch (years % 10)
		{
		case 1:
			return 1;
		case 2:
		case 3:
		case 4:
			return 2;
		}
	}
	return 3;
}

void CCommonTab::ShiftControl(zetlab::std::device* const device_)
{
	if (device_->isHaveSd() == SW_HIDE)
	{
		CRect rectSd; CRect rectFlash;
		GetDlgItem( IDC_LABEL_SD_STATE )		->GetWindowRect(&rectSd);
		GetDlgItem( IDC_LABEL_SD_STATE )		->GetParent()->ScreenToClient(&rectSd);
		GetDlgItem( IDC_FLASH_STATE )			->GetWindowRect(&rectFlash);
		GetDlgItem( IDC_FLASH_STATE )			->GetParent()->ScreenToClient(&rectFlash);
		CRect rectAcum;	CRect rectBattery;
		GetDlgItem( IDC_LABEL_ACCUM_STATE )		->GetWindowRect(&rectAcum);
		GetDlgItem( IDC_LABEL_ACCUM_STATE )		->GetParent()->ScreenToClient(&rectAcum);
		GetDlgItem( IDC_BATTERY_STATE )			->GetWindowRect(&rectBattery);
		GetDlgItem( IDC_BATTERY_STATE )			->GetParent()->ScreenToClient(&rectBattery);
		CRect rectLABELUPTIMEDEVICE; CRect rectUPTIMEDEVICE;
		GetDlgItem( IDC_UPTIMEDEVICE_LABEL )	->GetWindowRect(&rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE_LABEL )	->GetParent()->ScreenToClient(&rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE )			->GetWindowRect(&rectUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE )			->GetParent()->ScreenToClient(&rectUPTIMEDEVICE);
			
		GetDlgItem( IDC_UPTIMEDEVICE_LABEL )	->MoveWindow(rectSd);
		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->MoveWindow(rectAcum);
		GetDlgItem( IDC_LABEL_SYNC_STATE )		->MoveWindow(rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE )			->MoveWindow(rectFlash);
		GetDlgItem( IDC_UPTIMEDRIVER )			->MoveWindow(rectBattery);
		GetDlgItem( IDC_SYNC_STATE )			->MoveWindow(rectUPTIMEDEVICE);
	}

	if (device_->GetUptimeDevice() == SW_HIDE)
	{
		CRect rectLABELUPTIMEDEVICE; CRect rectUPTIMEDEVICE;
		GetDlgItem( IDC_UPTIMEDEVICE_LABEL )	->GetWindowRect(&rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE_LABEL )	->GetParent()->ScreenToClient(&rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE )			->GetWindowRect(&rectUPTIMEDEVICE);
		GetDlgItem( IDC_UPTIMEDEVICE )			->GetParent()->ScreenToClient(&rectUPTIMEDEVICE);
		CRect rectLABELDriver; CRect rectUPTIMEDriver;
		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->GetWindowRect(&rectLABELDriver);
		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->GetParent()->ScreenToClient(&rectLABELDriver);
		GetDlgItem( IDC_UPTIMEDRIVER )			->GetWindowRect(&rectUPTIMEDriver);
		GetDlgItem( IDC_UPTIMEDRIVER )			->GetParent()->ScreenToClient(&rectUPTIMEDriver);

		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->MoveWindow(rectLABELUPTIMEDEVICE);
		GetDlgItem( IDC_LABEL_SYNC_STATE )		->MoveWindow(rectLABELDriver);
		GetDlgItem( IDC_UPTIMEDRIVER )			->MoveWindow(rectUPTIMEDEVICE);
		GetDlgItem( IDC_SYNC_STATE )			->MoveWindow(rectUPTIMEDriver);
	}

	if (device_->GetUptimeDriver() == SW_HIDE)
	{
		CRect rectLABELDriver; CRect rectUPTIMEDriver;
		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->GetWindowRect(&rectLABELDriver);
		GetDlgItem( IDC_UPTIMEDRIVER_LABEL )	->GetParent()->ScreenToClient(&rectLABELDriver);
		GetDlgItem( IDC_UPTIMEDRIVER )			->GetWindowRect(&rectUPTIMEDriver);
		GetDlgItem( IDC_UPTIMEDRIVER )			->GetParent()->ScreenToClient(&rectUPTIMEDriver);

		GetDlgItem( IDC_LABEL_SYNC_STATE )		->MoveWindow(rectLABELDriver);
		GetDlgItem( IDC_SYNC_STATE )			->MoveWindow(rectUPTIMEDriver);
	}
}
