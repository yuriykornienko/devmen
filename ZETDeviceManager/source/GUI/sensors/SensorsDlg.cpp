#include "stdafx.h"
#include "../../application/application.h"
#include "SensorsDlg.h"

#include "../../algo/path/path.h"

#include <mathZet.h>
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

CSensorsDlg::CSensorsDlg( CWnd* pParent ) : CAnchoredDialog(CSensorsDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon( IDI_SENSORS );
}

void CSensorsDlg::DoDataExchange( CDataExchange* pDX )
{
    CAnchoredDialog::DoDataExchange( pDX );

    DDX_Control( pDX, IDC_LIST,   m_list );
    DDX_Control( pDX, IDC_ADD,    m_add );
    DDX_Control( pDX, IDC_REMOVE, m_remove );
}

BEGIN_MESSAGE_MAP( CSensorsDlg, CAnchoredDialog )
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_LIST, OnSensorSelect )
    ON_BN_CLICKED( IDC_ADD, OnAdd )
    ON_BN_CLICKED( IDC_REMOVE, OnRemove )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CSensorsDlg )
    ANCHOR_MAP_ENTRY( NULL, ANF_AUTOMATIC )
END_ANCHOR_MAP()

BOOL CSensorsDlg::OnInitDialog()
{
    CAnchoredDialog::OnInitDialog();

    SetIcon( m_hIcon, FALSE );

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif
	SetWindowTextW(sDebug + g_sSensorsDatabase);

	GetDlgItem(IDC_ADD)->SetWindowTextW(L"+ " + g_sAdd);
	GetDlgItem(IDC_REMOVE)->SetWindowTextW(L"x " + g_sDelete);

	GetDlgItem(IDOK)->SetWindowTextW(g_sApply);
	GetDlgItem(IDCANCEL)->SetWindowTextW(g_sCancel);

    InitList();

    return TRUE;
}

void CSensorsDlg::InitList()
{
    m_list.SetItemsHeight( 20 );
	m_list.ShowScrollBar(SB_HORZ, TRUE);
	m_list.EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
	
	csLocalization = g_sName + L":";
    m_list.InsertColumn( csLocalization );

	
	csLocalization = g_sChannelSensitivitySensor + L":";
    m_list.InsertColumn( csLocalization );
	
	//csLocalization = g_sMeasureUnit + L":";;
 //   m_list.InsertColumn( csLocalization );

    m_list.InsertColumn( _T("ICP") );
    
	csLocalization = g_sConstantIncrease + L":";
    m_list.InsertColumn( csLocalization );
    
	csLocalization = g_sReferenceValueLevel + L":";
    m_list.InsertColumn( csLocalization );
    
	csLocalization = g_sOffsetDC + L":";
    m_list.InsertColumn( csLocalization );
    
	csLocalization = g_sAmendmentFile;
    m_list.InsertColumn( csLocalization );

	csLocalization = g_sInputType;
	m_list.InsertColumn(csLocalization);

	csLocalization = g_sCharge;
	m_list.InsertColumn(csLocalization);

    path path_;

    path_.Create( this );
    BSTR	bstrDirConfig = _T("DirConfig");
    CString	sensorsConfig = path_.ZetPath( &bstrDirConfig ) + _T("sensors.cfg");

    sensors_.load( (LPCTSTR)sensorsConfig );
    m_list.SetAdapter( sensors_.get() );
}

void CSensorsDlg::OnAdd()
{
	csLocalization = g_sUnnamed;
    sensors_.add( (LPCTSTR)csLocalization );
    m_list.SetAdapter( sensors_.get() );
}

void CSensorsDlg::OnRemove()
{
    int nSel(m_list.GetSelectionMark());

    if (-1 != nSel)
    {
        sensors_.remove( size_t(nSel) );
        m_list.SetAdapter( sensors_.get() );
        m_list.SetFocus();
    }
}

void CSensorsDlg::OnSensorSelect( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast< LPNMLISTVIEW >( pNMHDR );

    m_remove.EnableWindow(pNMLV->uNewState & LVIS_SELECTED); // LVIS_SELECTED
    m_list.RedrawItems( pNMLV->iItem, pNMLV->iItem + 1 );
}

void CSensorsDlg::OnOK()
{
    path path_;

    path_.Create( this );
    BSTR	bstrDirConfig = _T("DirConfig");
    CString	sensorsConfig = path_.ZetPath( &bstrDirConfig ) + _T("sensors.cfg");

    sensors_.save( (LPCTSTR)sensorsConfig );

    CAnchoredDialog::OnOK();
}
