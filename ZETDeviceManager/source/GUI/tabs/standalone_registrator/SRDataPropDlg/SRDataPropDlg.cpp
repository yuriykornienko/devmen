#include "stdafx.h"
#include "Localized/Localized.h"
#include "SRDataPropDlg.h"

#include <mathZet.h>
#include "resource\Phrases.h"

CSRDataPropDlg::CSRDataPropDlg( long waitCopyDataMode_, long splitFilesMode_, long replaceOldFilesMode_, long continuousRecordMode_, CWnd* pParent )
	: CDialog(CSRDataPropDlg::IDD, pParent)
{
	_waitCopyDataMode     = waitCopyDataMode_;
	_splitFilesMode       = splitFilesMode_;
	_replaceOldFilesMode  = replaceOldFilesMode_;
	_continuousRecordMode = continuousRecordMode_;
}

CSRDataPropDlg::~CSRDataPropDlg( )
{

}

IMPLEMENT_DYNAMIC(CSRDataPropDlg, CDialog)

BEGIN_MESSAGE_MAP(CSRDataPropDlg, CDialog)
	ON_BN_CLICKED( IDC_SRSDD_WCD_OFF_RADIO,      OnBnOffCopyModeButton )
	ON_BN_CLICKED( IDC_SRSDD_WCD_USB_RADIO,      OnBnUsbCopyModeButton )
	ON_BN_CLICKED( IDC_SRSDD_WCD_ETHERNET_RADIO, OnBnEthCopyModeButton )

	ON_BN_CLICKED( IDC_SRSDD_OPTFILEPARSE_DAY,   OnBnSplitDaysModeButton )
	ON_BN_CLICKED( IDC_SRSDD_OPTFILEPARSE_HOURS, OnBnSplitHoursModeButton )
	ON_BN_CLICKED( IDC_SRSDD_NO_OPTFILEPARSE,    OnBnNoSplitModeButton )

	ON_BN_CLICKED( IDC_SRSDD_COF_CHECK,          OnBnReplaceOldFilesModeButton )
	ON_BN_CLICKED( IDC_SRSDD_NOSTOPWRITE_CHECK,  OnBnContinuousRecordModeButton )

	ON_BN_CLICKED( IDC_SRSDD_OK,                 OnBnSave )
	ON_BN_CLICKED( IDC_SRSDD_CANCEL,             OnBnCancel )
END_MESSAGE_MAP()


void CSRDataPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_SRSDD_WCD_OFF_RADIO,      _offCopyFilesModeButton );
	DDX_Control( pDX, IDC_SRSDD_WCD_USB_RADIO,      _usbCopyFilesModeButton );
	DDX_Control( pDX, IDC_SRSDD_WCD_ETHERNET_RADIO, _ethCopyFilesModeButton );

	DDX_Control( pDX, IDC_SRSDD_OPTFILEPARSE_DAY,   _splitDaysModeButton );
	DDX_Control( pDX, IDC_SRSDD_OPTFILEPARSE_HOURS, _splitHoursModeButton );
	DDX_Control( pDX, IDC_SRSDD_NO_OPTFILEPARSE,    _noSplitModeButton );

	DDX_Control( pDX, IDC_SRSDD_COF_CHECK,          _replaceOldFilesModeButton );
	DDX_Control( pDX, IDC_SRSDD_NOSTOPWRITE_CHECK,  _continuousRecordModeButton );
}

BOOL CSRDataPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DECLARE_LOC_SUPPORT()

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif
	SetWindowTextW(sDebug + g_sAdjustingJobData);
	GetDlgItem(IDC_SRSDD_OK)->SetWindowTextW(g_sSave);
	GetDlgItem(IDC_SRSDD_CANCEL)->SetWindowTextW(g_sCancel);
	GetDlgItem(IDC_SRSDD_WCD_GROUP)->SetWindowTextW(g_sWaitingDataCopying);
	GetDlgItem(IDC_SRSDD_WCD_OFF_RADIO)->SetWindowTextW(g_sOff);
	GetDlgItem(IDC_SRSDD_WCD_USB_RADIO)->SetWindowTextW(g_sSaveByUSB);
	GetDlgItem(IDC_SRSDD_WCD_ETHERNET_RADIO)->SetWindowTextW(g_sSaveByEthernet);
	GetDlgItem(IDC_SRSDD_COF_CHECK)->SetWindowTextW(g_sReplacingOlderFiles);
	GetDlgItem(IDC_SRSDD_NOSTOPWRITE_CHECK)->SetWindowTextW(g_sContinuousRecording);
	GetDlgItem(IDC_SRSDD_OPTFILEPARSE_GROUP)->SetWindowTextW(g_sBreakdownFiles);
	GetDlgItem(IDC_SRSDD_OPTFILEPARSE_DAY)->SetWindowTextW(g_sSplitByDayssmall);
	GetDlgItem(IDC_SRSDD_OPTFILEPARSE_HOURS)->SetWindowTextW(g_sSplitByHoursssmall);
	GetDlgItem(IDC_SRSDD_NO_OPTFILEPARSE)->SetWindowTextW(g_sWithoutSplit);

	switch( _waitCopyDataMode )
	{
	case 0:
		_offCopyFilesModeButton.SetCheck(1);
		_usbCopyFilesModeButton.SetCheck(0);
		_ethCopyFilesModeButton.SetCheck(0);
		break;
	case 1:
		_offCopyFilesModeButton.SetCheck(0);
		_usbCopyFilesModeButton.SetCheck(1);
		_ethCopyFilesModeButton.SetCheck(0);
		break;
	case 2:
		_offCopyFilesModeButton.SetCheck(0);
		_usbCopyFilesModeButton.SetCheck(0);
		_ethCopyFilesModeButton.SetCheck(1);
		break;
	default:
		_offCopyFilesModeButton.SetCheck(0);
		_usbCopyFilesModeButton.SetCheck(0);
		_ethCopyFilesModeButton.SetCheck(0);
		break;
	}

	switch ( _splitFilesMode )
	{
	case 0:
		_splitDaysModeButton.SetCheck(0);
		_splitHoursModeButton.SetCheck(0);
		_noSplitModeButton.SetCheck(1);
		break;
	case 1:
		_splitDaysModeButton.SetCheck(1);
		_splitHoursModeButton.SetCheck(0);
		_noSplitModeButton.SetCheck(0);
		break;
	case 2:
		_splitDaysModeButton.SetCheck(0);
		_splitHoursModeButton.SetCheck(1);
		_noSplitModeButton.SetCheck(0);
		break;
	default:
		_splitDaysModeButton.SetCheck(0);
		_splitHoursModeButton.SetCheck(0);
		_noSplitModeButton.SetCheck(0);
		break;
	}

	_replaceOldFilesModeButton.SetCheck( _replaceOldFilesMode );
	_continuousRecordModeButton.SetCheck( _continuousRecordMode );

	return TRUE;
}

void CSRDataPropDlg::OnBnOffCopyModeButton()
{
	_waitCopyDataMode = 0;
}

void CSRDataPropDlg::OnBnUsbCopyModeButton()
{
	_waitCopyDataMode = 1;
}

void CSRDataPropDlg::OnBnEthCopyModeButton()
{
	_waitCopyDataMode = 2;
}

void CSRDataPropDlg::OnBnSplitDaysModeButton()
{
	_splitFilesMode = 1;
}

void CSRDataPropDlg::OnBnSplitHoursModeButton()
{
	_splitFilesMode = 2;
}

void CSRDataPropDlg::OnBnNoSplitModeButton()
{
	_splitFilesMode = 0;
}

void CSRDataPropDlg::OnBnReplaceOldFilesModeButton()
{
	if ( _replaceOldFilesModeButton.GetCheck() == 1 )
		_replaceOldFilesMode = 1;
	else
		_replaceOldFilesMode = 0;
}

void CSRDataPropDlg::OnBnContinuousRecordModeButton()
{
	if ( _continuousRecordModeButton.GetCheck() == 1 )
		_continuousRecordMode = 1;
	else
		_continuousRecordMode = 0;
}

void CSRDataPropDlg::OnBnSave()
{
	EndDialog(IDC_SRSDD_OK);
}

void CSRDataPropDlg::OnBnCancel()
{
	EndDialog(IDC_SRSDD_CANCEL);
}
