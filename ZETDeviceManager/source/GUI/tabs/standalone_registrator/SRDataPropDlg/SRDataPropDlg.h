#pragma once
#include "../../../../../resource/resource.h"

class CSRDataPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CSRDataPropDlg)
public:
	CSRDataPropDlg(long waitCopyDataMode_, long splitFilesMode_, long replaceOldFilesMode_, long continuousRecordMode_, CWnd* pParent = NULL);
	virtual ~CSRDataPropDlg();

	enum { IDD = IDD_SR_SETTING_DATA_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	long _waitCopyDataMode;
	long _splitFilesMode;
	long _replaceOldFilesMode;
	long _continuousRecordMode;

	CButton _offCopyFilesModeButton;
	CButton _usbCopyFilesModeButton;
	CButton _ethCopyFilesModeButton;

	CButton _splitDaysModeButton;
	CButton _splitHoursModeButton;
	CButton _noSplitModeButton;

	CButton _replaceOldFilesModeButton;
	CButton _continuousRecordModeButton;

	afx_msg void OnBnOffCopyModeButton();
	afx_msg void OnBnUsbCopyModeButton();
	afx_msg void OnBnEthCopyModeButton();

	afx_msg void OnBnSplitDaysModeButton();
	afx_msg void OnBnSplitHoursModeButton();
	afx_msg void OnBnNoSplitModeButton();

	afx_msg void OnBnReplaceOldFilesModeButton();
	afx_msg void OnBnContinuousRecordModeButton();

	afx_msg void OnBnSave();
	afx_msg void OnBnCancel();

	virtual BOOL OnInitDialog();
};
