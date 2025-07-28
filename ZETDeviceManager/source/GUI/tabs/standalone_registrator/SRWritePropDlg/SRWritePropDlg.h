#pragma once
#include "../../../../../resource/resource.h"
#include "../../../../algo/std/device/device_std.hpp"
//#include "../../../../activeX/CDSRV/CDSRV.h"
#include <ZETView_basic_class/CDSRV.h>
#include "../stanRegDeviceParam.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define SIZE_BOOL_B_OPTIONS 9
// CSRWritePropDlg dialog

class CSRWritePropDlg : public CDialog
{
	DECLARE_DYNAMIC(CSRWritePropDlg)
private:
	float    _maxSDWriteTime;
	float   _maxTotalSampleRate[100];
	float   _maxChannelSampleRate[100];
	CString _configFileParam;
	CString _dacFileParam;
	long    _type;
	long    _dsp;
	CDSRV   _zetServer;
	float 	_chanFreq;
	long	_chanQuan;
	long	_CountWords;
	long long    _freeDiskSpace;
	bool    _bConfigFileReadOk;

	long GetDeviceParam();
	long SetDeviceParam();
	long GetDeviceParamExt();
	long SetDeviceParamExt();
	void UpdateRegInfoListBox();
	void Set210WindowDesign();
	void SetWindowDesignNew(bool[SIZE_BOOL_B_OPTIONS]);
	void Set110WindowDesign();
	void SetA17U8WindowDesign();
	void SetA17U2WindowDesign();
    void Set220WindowDesign();
	void Set230WindowDesign();
	void Set048WindowDesign();
	void Set240WindowDesign();
	bool SaveRegParameters();
	void SaveDeltaRegParameters();
	double GetMinLevel( long numChan_ );
	void writeTempDACFile();
	bool GetMaxbitrate();
	bool isHaveBuiltinGeneratorWithCheck(long module);

	bool GetDevicelong();

	void SetDefaultParams();
public:
	CSRWritePropDlg(long type_, long dsp_, CString fileNameCFG_, CString fileNameDAC_, long long freeSpaceSize_, long countwords_, bool bReadConfig, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSRWritePropDlg();

// Dialog Data
	enum { IDD = IDD_SR_PARAM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	PARAM    _param;
	PARAMEXT _paramdelta;
// Data
	CListBox      _listChanSettings;
	CComboBox     _writeTimeType;

	CEdit         _writeTime;
	CEdit         _writeDelay;
	CButton       _checkEventStart;
	CButton       _digEventStart;
	CButton       _anaEventStart;
	CButton       _tmrEventStart;
	CComboBox     _eventStartChanNum;
	CComboBox     _eventStartFront;
	CComboBox     _eventStartTreshold;

	CDateTimeCtrl _dateStart;
	CDateTimeCtrl _timeStart;
	CMFCButton    _addStartTime;
	CMFCButton    _clearStartTime;

	CButton       _primaryData;
	CButton       _secondaryData;

	CButton       _multiEventsMode;
	CButton       _genGPSSynMode;

	CButton       _checkSinGenEnable;
	CEdit         _sinGenFreq;
	CEdit         _sinGenLevel;
	CEdit         _sinGenOffset;

	CButton       _checkLevelControl;
	CComboBox     _channelControl;
	CComboBox     _channelControlFront;
	CEdit         _channelControlLevel;
	CComboBox     _numBitControl;
	CComboBox     _activeLevelControl;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnEventStartCheckbox();
	afx_msg void OnBnEnableDigStart();
	afx_msg void OnBnEnableAnaStart();
	afx_msg void OnBnEnableTmrStart();
	afx_msg void OnBnEnablePrimaryData();
	afx_msg void OnBnEnableSecondaryData();
	afx_msg void OnBnEnableSinGen();
	afx_msg void OnBnEnableLevelControl();
	afx_msg void OnBnSaveParam();
	afx_msg void OnBnCancel();
	afx_msg void OnBnSettingsData();
	afx_msg void OnBnAddTime();
	afx_msg void OnBnClearTime();

	void FillWindowFields();
	void FillWindowFieldsDelta();
	void SetWindowDesign();
};
