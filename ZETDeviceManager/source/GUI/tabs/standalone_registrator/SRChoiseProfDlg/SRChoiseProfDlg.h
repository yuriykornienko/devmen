#pragma once
#include "../../../../../resource/resource.h"
#include "../../../../algo/std/device/device_std.hpp"
#include "../../../../algo/sensors/sensors.hpp"
#include "../stanRegDeviceParam.h"

class CSRChoiseProfDlg : public CDialog
{
	DECLARE_DYNAMIC(CSRChoiseProfDlg)

public:
	CSRChoiseProfDlg(long type_, long dsp_, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSRChoiseProfDlg();

// Dialog Data
	enum { IDD = IDD_SR_SENSORS_PROFILES_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	long _type;
	long _dsp;
	PARAMEXT _paramdelta;
	zetlab::sensors _sensors;

	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	long GetSensor( unsigned short dev_, unsigned short sensor_, PCALIBR_ZET110 calibr_ );
	long SetSensor( unsigned short dev_, unsigned short sensor_, PCALIBR_ZET110 calibr_ );
	void SetProfilesInfo();
	long GetDeviceParamExt();
	long SetDeviceParamExt();
public:
	CListCtrl _profilesList;

	afx_msg void OnBnSave();
	afx_msg void OnBnCancel();
	afx_msg void OnRBMouseDown( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg BOOL OnContextMenuClick( UINT nID );
	virtual BOOL OnInitDialog();
};
