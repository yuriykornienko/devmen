#include "stdafx.h"
//#include "../../../types.hpp"
#include "../../../application/application.h"
#include "../../user_messages.hpp"

#include "sync_tab.h"

#include <CDPI/cdpi.h>

#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include <mathZet.h>
#include "resource\Phrases.h"

CSyncTab::CSyncTab( CWnd* pParent ) :
	CTab( CSyncTab::IDD, pParent )
	, device_(nullptr)
	, m_synced(0)
	, m_enableMaster2(0)
	, m_enableSlave2(0)
	, m_satellites(0)
	, m_pps(0)
	, m_enableMaster(0)
	, m_enableSlave(0)
	, m_domain(0)
	, m_absolutePriority(0)
	, m_relativePriority(0)
	, m_Adjustment(_T(""))
{
}

CSyncTab::~CSyncTab()
{
}

void CSyncTab::DoDataExchange( CDataExchange* pDX )
{
	CTab::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_DEVICEADJUSTMENT, m_Adjustment);
	DDX_Text(pDX, IDC_DEVICE_TIME, m_time);
	DDX_Check(pDX, IDC_SYNC_FLAG, m_synced);
	DDX_Check(pDX, IDC_ENABLE_MASTER2, m_enableMaster2);
	DDX_Check(pDX, IDC_ENABLE_SLAVE2, m_enableSlave2);
	DDX_Text(pDX, IDC_SATELLITES, m_satellites);
	DDX_Control(pDX, IDC_PPS, m_ppsIcon);
	DDX_Check(pDX, IDC_ENABLE_MASTER, m_enableMaster);
	DDX_Check(pDX, IDC_ENABLE_SLAVE, m_enableSlave);
	DDX_Text(pDX, IDC_DOMAIN, m_domain);
	DDX_Text(pDX, IDC_ABSOLUTE_PRIORITY, m_absolutePriority);
	DDX_Text(pDX, IDC_RELATIVE_PRIORITY, m_relativePriority);
}

BEGIN_MESSAGE_MAP( CSyncTab, CTab )
	ON_WM_TIMER     ()
	ON_CONTROL_RANGE( BN_CLICKED, IDC_ENABLE_MASTER, IDC_ENABLE_SLAVE,   OnChange )
	ON_CONTROL_RANGE( BN_CLICKED, IDC_ENABLE_MASTER2, IDC_ENABLE_SLAVE2, OnChange )
	ON_CONTROL_RANGE( EN_CHANGE, IDC_DOMAIN, IDC_RELATIVE_PRIORITY,      OnChange )
	ON_MESSAGE_VOID ( WM_USER_SAVE,                                      OnUserSave )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CSyncTab )
END_ANCHOR_MAP()

BOOL CSyncTab::OnInitDialog()
{
	CTab::OnInitDialog();

	SetWindowTextW(g_sSynchronization);

	GetDlgItem(IDC_LABEL_ADJUSTMENT_LEVEL)->SetWindowTextW(g_sAdjustmentLevel + L":");
	GetDlgItem(IDC_LABEL_DEVICE_TIME)->SetWindowTextW(g_sDeviceTime + L":");
	GetDlgItem(IDC_ENABLE_MASTER2)->SetWindowTextW(g_sMaster);
	GetDlgItem(IDC_ENABLE_SLAVE2)->SetWindowTextW(g_sSlave);
	GetDlgItem(IDC_SYNC_FLAG)->SetWindowTextW(g_sSynchronized);
	GetDlgItem(IDC_GROUP_DP_SYNC)->SetWindowTextW(g_sSynchronizationDigitalPort);
	GetDlgItem(IDC_GROUP_GPS)->SetWindowTextW(L"GPS");
	GetDlgItem(IDC_LABEL_SATELLITES)->SetWindowTextW(g_sNumberSatelites + L":");
	GetDlgItem(IDC_LABEL_PPS)->SetWindowTextW(g_sPresenceSignal + L" PPS:");
	GetDlgItem(IDC_GROUP_PTP)->SetWindowTextW(L"PTP");
	GetDlgItem(IDC_ENABLE_MASTER)->SetWindowTextW(g_sMaster);
	GetDlgItem(IDC_ENABLE_SLAVE)->SetWindowTextW(g_sSlave);
	GetDlgItem(IDC_LABEL_DOMAIN)->SetWindowTextW(g_sDomain + L":");
	GetDlgItem(IDC_LABEL_ABSOLUTE_PRIORITY)->SetWindowTextW(g_sAbsolutePriority + L" *:");//L"Абсолютный приоритет *:"
	GetDlgItem(IDC_LABEL_RELATIVE_PRIORITY)->SetWindowTextW(g_sRelativePriority + L" *:");
	GetDlgItem(IDC_LABEL_PRIORITY)->SetWindowTextW(L"* " + g_sNOTELowerLalueHigherPriority + L":");

	m_groupGps.SubclassDlgItem( IDC_GROUP_GPS, this );
	return TRUE;
}

void CSyncTab::InitToolTips()
{
	CTab::InitToolTips();
	AddToolTip(GetDlgItem(IDC_ENABLE_MASTER2), g_sAllowDeviceMaster);
	AddToolTip(GetDlgItem(IDC_ENABLE_SLAVE2), g_sАllowDeviceSlave);
	AddToolTip(GetDlgItem(IDC_ENABLE_MASTER), g_sAllowDeviceMaster);
	CString strTmp(L"");
	strTmp = g_sGreenPPSYes + L"\n" + g_sGreyPPSNo;//L"Зеленый - PPS есть\nСерый - PPS отсутствует"
	AddToolTip(GetDlgItem(IDC_PPS), strTmp);
	AddToolTip(GetDlgItem(IDC_ENABLE_SLAVE), g_sАllowDeviceSlave);
	AddToolTip(GetDlgItem(IDC_DOMAIN), g_sVirtualSubnetNumber);
	AddToolTip(GetDlgItem(IDC_ABSOLUTE_PRIORITY), g_sPriorityMasterDevice);
	AddToolTip(GetDlgItem(IDC_RELATIVE_PRIORITY), g_sAppliesAbsolutePriorities);

}

void CSyncTab::SetAdapter( zetlab::std::device* const device__ )
{
	device_ = device__;

	int type(0);

	if (device_->isHaveDigSync())
	{
		bool enableMaster2;
		bool enableSlave2;

		device_->readDigSyncSettings( enableMaster2, enableSlave2 );
		{
			m_enableMaster2 = int(enableMaster2);
			m_enableSlave2  = int(enableSlave2);
		}

		type |= 0x1;
	}

	if (device_->isHaveGps())
	{
		//SetTimer( 1, 1000, NULL );

		type |= 0x2;

		// У ZET048 R6 есть старая и новая логика
		if (device_->getType() == KDU0824)
		{
			// Определяем версию устройства
			const unsigned long vmin = 10; // начиная с этой версии логика новая
			const zetlab::tstring vmatch = _T(" v6."); // только для R6

			auto vstr = device_->getVersion();
			auto vpos = vstr.find(vmatch);
			if (vpos != vstr.npos)
			{
				vstr = vstr.substr(vpos + vmatch.length());
				unsigned long vnum = _tcstoul(vstr.c_str(), NULL, 10);
				if (vnum < vmin)
				{
					// Старая логика
					type |= 0x10;
				}
			}
		}
		m_pps = 0;
		m_satellites = 0;
	}

	{
		bool enableMaster;
		bool enableSlave;
		unsigned char domain;
		unsigned char absolutePriority;
		unsigned char relativePriority;

		m_enableMaster = 0;
		m_enableSlave = 0;
		m_domain = 0;
		m_absolutePriority = 0;
		m_relativePriority = 0;

		if (device_->readSyncSettings( enableMaster, enableSlave,
			domain, absolutePriority, relativePriority ))
		{
			m_enableMaster       = int(enableMaster);
			m_enableSlave        = int(enableSlave);
			m_domain             = int(domain);
			m_absolutePriority   = int(absolutePriority);
			m_relativePriority   = int(relativePriority);
			GetDlgItem( IDC_ABSOLUTE_PRIORITY )->EnableWindow( m_enableMaster );
			GetDlgItem( IDC_RELATIVE_PRIORITY )->EnableWindow( m_enableMaster );

			// Если есть GPS и версия старая, то режим Slave активирован быть не может
			if ((type & 0x12) == 0x12)
			{
				m_enableSlave = 0;
				GetDlgItem( IDC_ENABLE_SLAVE )->EnableWindow( FALSE );
			}

			//KillTimer( 1 );
			//SetTimer( 1, 1000, NULL );

			type |= 0x4;
		}
	}
	
	if (device_->isHaveAdjustmentLevel())
	{
		type |= 0x8;
		m_Adjustment.Format(L"%u%%", device_->GetAdjustmentLevel());

		KillTimer( 2 );
		SetTimer( 2, 1000, NULL );
	}
	
	HandleVisible( type );
	HandleGpsEnabled();
	UpdateData   ( FALSE );
}

void CSyncTab::HandleVisible( int type )
{
	/*
		type - битовая комбинация следующих значений:
			0x1 - Синхронизация по цифровому порту
			0x2 - GPS
			0x4 - PTP
			0x8 - Уровень подстройки
	*/
	CDPI dpiAware;
	int delta(0);
	// если не удалось прочитать уровень подстройки
	if ((type & 0x8) != 0x8)
	{
		GetDlgItem(IDC_LABEL_ADJUSTMENT_LEVEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DEVICEADJUSTMENT)->ShowWindow(SW_HIDE);

		MoveUp(GetDlgItem(IDC_LABEL_DEVICE_TIME),	-dpiAware.ScaleY(12));
		MoveUp(GetDlgItem(IDC_DEVICE_TIME),			-dpiAware.ScaleY(12));
		MoveUp(GetDlgItem(IDC_SYNC_FLAG),			-dpiAware.ScaleY(12));
	}
	else
		delta = 24;

	if (((type & 0x2) != 0x2) && ((type & 0x4) != 0x4))
	{
		GetDlgItem( IDC_LABEL_DEVICE_TIME )->               ShowWindow( SW_HIDE );
		GetDlgItem( IDC_DEVICE_TIME )->                     ShowWindow( SW_HIDE );
		GetDlgItem( IDC_SYNC_FLAG )->                       ShowWindow( SW_HIDE );

		MoveUp( GetDlgItem( IDC_GROUP_DP_SYNC ),           -dpiAware.ScaleY( 39 - delta ) );
		MoveUp( GetDlgItem( IDC_ENABLE_MASTER2 ),          -dpiAware.ScaleY( 39 - delta ) );
		MoveUp( GetDlgItem( IDC_ENABLE_SLAVE2 ),           -dpiAware.ScaleY( 39 - delta ) );
	}

	if ((type & 0x1) != 0x1)
	{
		GetDlgItem( IDC_GROUP_DP_SYNC )->                   ShowWindow( SW_HIDE );
		GetDlgItem( IDC_ENABLE_MASTER2 )->                  ShowWindow( SW_HIDE );
		GetDlgItem( IDC_ENABLE_SLAVE2 )->                   ShowWindow( SW_HIDE );

		MoveUp( &m_groupGps,                               -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_SATELLITES ),        -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_SATELLITES ),              -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_PPS ),               -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_PPS ),                     -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_GROUP_PTP ),               -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_ENABLE_MASTER ),           -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_ABSOLUTE_PRIORITY ), -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_ABSOLUTE_PRIORITY ),       -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_RELATIVE_PRIORITY ), -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_RELATIVE_PRIORITY ),       -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_PRIORITY ),          -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_ENABLE_SLAVE ),            -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_LABEL_DOMAIN ),            -dpiAware.ScaleY( 52 ) );
		MoveUp( GetDlgItem( IDC_DOMAIN ),                  -dpiAware.ScaleY( 52 ) );
	}

	if ((type & 0x2) != 0x2)
	{
		m_groupGps.                                         ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_SATELLITES )->                ShowWindow( SW_HIDE );
		GetDlgItem( IDC_SATELLITES )->                      ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_PPS )->                       ShowWindow( SW_HIDE );
		GetDlgItem( IDC_PPS )->                             ShowWindow( SW_HIDE );

		MoveUp( GetDlgItem( IDC_GROUP_PTP ),               -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_ENABLE_MASTER ),           -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_LABEL_ABSOLUTE_PRIORITY ), -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_ABSOLUTE_PRIORITY ),       -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_LABEL_RELATIVE_PRIORITY ), -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_RELATIVE_PRIORITY ),       -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_LABEL_PRIORITY ),          -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_ENABLE_SLAVE ),            -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_LABEL_DOMAIN ),            -dpiAware.ScaleY( 51 ) );
		MoveUp( GetDlgItem( IDC_DOMAIN ),                  -dpiAware.ScaleY( 51 ) );
	}

	if ((type & 0x4) != 0x4)
	{
		GetDlgItem( IDC_GROUP_PTP )->                       ShowWindow( SW_HIDE );
		GetDlgItem( IDC_ENABLE_MASTER )->                   ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_ABSOLUTE_PRIORITY )->         ShowWindow( SW_HIDE );
		GetDlgItem( IDC_ABSOLUTE_PRIORITY )->               ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_RELATIVE_PRIORITY )->         ShowWindow( SW_HIDE );
		GetDlgItem( IDC_RELATIVE_PRIORITY )->               ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_PRIORITY )->                  ShowWindow( SW_HIDE );
		GetDlgItem( IDC_ENABLE_SLAVE )->                    ShowWindow( SW_HIDE );
		GetDlgItem( IDC_LABEL_DOMAIN )->                    ShowWindow( SW_HIDE );
		GetDlgItem( IDC_DOMAIN )->                          ShowWindow( SW_HIDE );
	}
}

void CSyncTab::HandleGpsEnabled()
{
	m_groupGps.                         EnableWindow( !m_enableSlave );
	GetDlgItem( IDC_LABEL_SATELLITES )->EnableWindow( !m_enableSlave );
	GetDlgItem( IDC_SATELLITES )->      EnableWindow( !m_enableSlave );
	GetDlgItem( IDC_LABEL_PPS )->       EnableWindow( !m_enableSlave );
	GetDlgItem( IDC_PPS )->             EnableWindow( !m_enableSlave );
	if (m_enableSlave)
		m_ppsIcon.SetIcon( LOAD_ICON( IDI_CHANNEL_DISABLED, 16, 16 ) );
}

void CSyncTab::MoveUp( CWnd* pWnd, int cy )
{
	CRect rc;

	pWnd->GetWindowRect( rc );
	ScreenToClient( &rc );
	pWnd->SetWindowPos( &wndTop, rc.left, rc.top + cy, 0, 0,
						SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER );
}

void CSyncTab::OnTimer( UINT_PTR nIDEvent )
{
	CTab::OnTimer( nIDEvent );

	switch (nIDEvent)
	{
	case 1:
		{
			UpdateData();

			CString temp;
			temp.Format(L"Domain %d   priority absolute %d   relative %d\n", m_domain, m_absolutePriority, m_relativePriority);
			OutputDebugString(temp);

			__time64_t    time;
			bool          synced;
			unsigned char pps;
			unsigned char sats;

			device_->readGpsInfo( time, synced, pps, sats );
			{
				using namespace boost::posix_time;
				using namespace boost::date_time;

				ptime pt;
				try
				{
					pt = c_local_adjustor< ptime >::utc_to_local(from_time_t(time));
				}
				catch (std::out_of_range&)
				{
					pt = ptime(boost::gregorian::date(1970, 1, 1));
				}

			#if UNICODE
				m_time       = to_simple_wstring( pt ).c_str();
			#else
				m_time       = to_simple_string( pt ).c_str();
			#endif
				m_synced     = int(synced);
				m_satellites = int(m_enableSlave ? 0 : sats);
				if (pps != m_pps && !m_enableSlave)
					m_ppsIcon.SetIcon( LOAD_ICON( IDI_CHANNEL_ENABLED,  16, 16 ) );
				else
					m_ppsIcon.SetIcon( LOAD_ICON( IDI_CHANNEL_DISABLED, 16, 16 ) );
				m_pps        = pps;

				UpdateData( FALSE );
			}
		}
		break;
	case 2:
		UpdateData(TRUE);
		m_Adjustment.Format(L"%u%%", device_->GetAdjustmentLevel());
		UpdateData(FALSE);
		break;
	}
}

void CSyncTab::OnChange( UINT nID )
{
	switch ( nID )
	{
	case  IDC_ENABLE_MASTER:
		{
			UpdateData();
			GetDlgItem(IDC_ABSOLUTE_PRIORITY)->EnableWindow(m_enableMaster);
			GetDlgItem(IDC_RELATIVE_PRIORITY)->EnableWindow(m_enableMaster);
			break;
		}
	case  IDC_ENABLE_MASTER2:
		{
			UpdateData();
			m_enableSlave2 = m_enableMaster2 ? 0 : m_enableSlave2;
			UpdateData(FALSE);
			break;
		}
	case  IDC_ENABLE_SLAVE2:
		{
			UpdateData();
			m_enableMaster2 = m_enableSlave2 ? 0 : m_enableMaster2;
			UpdateData(FALSE);
			break;
		}
	case  IDC_ABSOLUTE_PRIORITY:
		{
			CString sValue;
			GetDlgItem(IDC_ABSOLUTE_PRIORITY)->GetWindowText(sValue);
			if (sValue.GetLength() == 0)
				GetDlgItem(IDC_ABSOLUTE_PRIORITY)->SetWindowText(L"0");
			break;
		}
	case  IDC_RELATIVE_PRIORITY:
		{
			CString sValue;
			GetDlgItem(IDC_RELATIVE_PRIORITY)->GetWindowText(sValue);
			if (sValue.GetLength() == 0)
				GetDlgItem(IDC_RELATIVE_PRIORITY)->SetWindowText(L"0");
			break;
		}
	case  IDC_DOMAIN:
		{
			CString sValue;
			GetDlgItem(IDC_DOMAIN)->GetWindowText(sValue);
			if (sValue.GetLength() == 0)
				GetDlgItem(IDC_DOMAIN)->SetWindowText(L"0");
			break;
		}
	case  IDC_ENABLE_SLAVE:
		{
			UpdateData();
			HandleGpsEnabled();
			UpdateData(FALSE);
			break;
		}
	}
}

void CSyncTab::OnUserSave()
{
	UpdateData();

	if (device_->isHaveDigSync())
	{
		bool enableMaster2         = (1 == m_enableMaster2);
		bool enableSlave2          = (1 == m_enableSlave2);

		device_->writeDigSyncSettings( enableMaster2, enableSlave2 );
	}

	bool enableMaster              = (1 == m_enableMaster);
	bool enableSlave               = (1 == m_enableSlave);
	unsigned char domain           = unsigned char(m_domain);
	unsigned char absolutePriority = unsigned char(m_absolutePriority);
	unsigned char relativePriority = unsigned char(m_relativePriority);

	device_->writeSyncSettings( enableMaster, enableSlave,
		domain, absolutePriority, relativePriority );
}

void CSyncTab::Activate()
{
	if (device_->isHaveGps())
	{
		KillTimer(1);
		SetTimer(1, 1000, NULL);
	}

	bool enableMaster;
	bool enableSlave;
	unsigned char domain;
	unsigned char absolutePriority;
	unsigned char relativePriority;

	m_satellites = 0;
	m_pps = 0;

	m_enableMaster = 0;
	m_enableSlave = 0;
	m_domain = 0;
	m_absolutePriority = 0;
	m_relativePriority = 0;

	if (device_->readSyncSettings(enableMaster, enableSlave,
		domain, absolutePriority, relativePriority))
	{
		m_enableMaster       = int(enableMaster);
		m_enableSlave        = int(enableSlave);
		m_domain             = int(domain);
		m_absolutePriority   = int(absolutePriority);
		m_relativePriority   = int(relativePriority);

		KillTimer(1);
		SetTimer(1, 1000, NULL);
	}
}

void CSyncTab::Disactivate()
{
	KillTimer(1);
}
