#include "stdafx.h"
#include "../../application/application.h"
#include "SettingsDlg.h"

#include <mathZet.h>
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

CSettingsDlg::CSettingsDlg( CWnd* pParent ) :
    CAnchoredDialog(CSettingsDlg::IDD, pParent)
{
}

void CSettingsDlg::DoDataExchange( CDataExchange* pDX )
{
    CAnchoredDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CSettingsDlg, CAnchoredDialog )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CSettingsDlg )
    ANCHOR_MAP_ENTRY( NULL, ANF_AUTOMATIC )
END_ANCHOR_MAP()

BOOL CSettingsDlg::OnInitDialog()
{
    CAnchoredDialog::OnInitDialog();

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif

	SetWindowTextW(sDebug + g_sOptions);
	GetDlgItem(IDOK)->SetWindowTextW(g_sApply);
	GetDlgItem(IDCANCEL)->SetWindowTextW(g_sCancel);


    return FALSE;
}

void CSettingsDlg::OnOK()
{
    CAnchoredDialog::OnOK();
}
