#include "stdafx.h"
#include "../../../../application/application.h"
#include "start_page.h"
#include <Localized/Localized.h>

#include "../../../../types.hpp"

#include <Zadc.h>
#include <mathZet.h>
#include "resource\Phrases.h"

#define LoadBitmap_(nID) \
    (HBITMAP)::LoadImage( AfxGetInstanceHandle(), \
    MAKEINTRESOURCE( nID ), \
    IMAGE_BITMAP, 64, 64, LR_SHARED | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS )

CStartPage::CStartPage() :
    CPropertyPage(CStartPage::IDD)
	, object_(nullptr)
	, m_serial(0)
{
    m_psp.dwFlags |= PSP_USEHEADERTITLE;
}

CStartPage::~CStartPage()
{
}

void CStartPage::DoDataExchange( CDataExchange* pDX )
{
    CPropertyPage::DoDataExchange( pDX );

    DDX_Control( pDX, IDC_ICON,    m_icon );
    DDX_Text   ( pDX, IDC_TYPE,    m_type );
    DDX_Text   ( pDX, IDC_VERSION,  m_serial );
}

BEGIN_MESSAGE_MAP( CStartPage, CPropertyPage )
END_MESSAGE_MAP()

BOOL CStartPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

	SetWindowTextW(g_sHowToUse + L" %s?");

	GetDlgItem(IDC_LABEL_TYPE)->SetWindowTextW(g_sDeviceType + L":");
	GetDlgItem(IDC_LABEL_VERSION)->SetWindowTextW(g_sVersion + L":");
	GetDlgItem(IDC_COMMAND_SETUP)->SetWindowTextW(g_sSetUpYourDevice);
	GetDlgItem(IDC_COMMAND_REUSE)->SetWindowTextW(g_sUseDeviceReplaceOldOne);

	csLocalization = g_sSetADCDACSamplingRate;
    ((CButton*)GetDlgItem( IDC_COMMAND_SETUP ))->SetNote( csLocalization );

	csLocalization = g_sApplySettingsOtherDevice;
    ((CButton*)GetDlgItem( IDC_COMMAND_REUSE ))->SetNote( csLocalization );

    return TRUE;
}

void CStartPage::OnShowWindow( BOOL bShow, UINT nStatus )
{
    ::ShowWindow( ::GetWindow( GetParent()->m_hWnd, GW_HWNDFIRST ), SW_HIDE );
}

void CStartPage::SetAdapter( zetlab::object* const object__ )
{
    if (nullptr == object__)
        return;

    object_ = object__;

	csLocalization.Format(g_sHowToUse + L" %s?", object_->getName().c_str());//L"Как использовать %s?"
    m_psp.pszHeaderTitle = csLocalization;
}

void CStartPage::SetAdapterImpl( zetlab::std::device* const device_ )
{
    if (nullptr == device_)
        return;

    m_type    = device_->getTypeStr();
    m_serial  = device_->getSerial();

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

    default:
        m_icon.SetBitmap( LoadBitmap_( IDB_DEMO ) );
        break;
    }

    UpdateData( FALSE );
}

BOOL CStartPage::OnSetActive()
{
    if (nullptr == object_)
        return FALSE;

    SetAdapterImpl( object_->getAsDevice() );

    ((CPropertySheet*)GetParent())->SetWizardButtons( PSWIZB_CANCEL );

    return CPropertyPage::OnSetActive();
}
