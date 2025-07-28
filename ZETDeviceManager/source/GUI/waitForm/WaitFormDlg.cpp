#include "stdafx.h"
#include "../../application/application.h"
#include "WaitFormDlg.h"

#include <mathZet.h>
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

CWaitFormDlg::CWaitFormDlg( CWnd* pParent ) :
    CAnchoredDialog(CWaitFormDlg::IDD, pParent)
{
	parent = pParent;
}

void CWaitFormDlg::DoDataExchange( CDataExchange* pDX )
{
    CAnchoredDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP(CWaitFormDlg, CAnchoredDialog )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(CWaitFormDlg)
    ANCHOR_MAP_ENTRY( NULL, ANF_AUTOMATIC )
END_ANCHOR_MAP()

BOOL CWaitFormDlg::OnInitDialog()
{
    CAnchoredDialog::OnInitDialog();


	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif

	SetWindowTextW(sDebug + g_sZETDeviceManager);
	GetDlgItem(IDC_WAITTEXT)->SetWindowTextW(g_sWaitingReceivedData + L"...");



    return FALSE;
}

void CWaitFormDlg::OnDestroy()
{
	CAnchoredDialog::OnDestroy();
	delete this; 
}