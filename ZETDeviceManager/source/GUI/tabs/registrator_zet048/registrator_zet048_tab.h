#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../default_tab.h"
#include "../../../algo/std/device/device_std.hpp"
#include "../../progressctrlex/ProgressCtrlEx.h"
#include "../../../../resource/resource.h"
#include <ZETView_basic_class/CDSRV.h>
#include "../standalone_registrator/stanRegDeviceParam.h"
#include "..\..\..\..\resource/phrases.h"

#define PARAM_BUTTON_WIDTH	137

class CRegistratorZet048Tab : public CTab
{
public:
	CRegistratorZet048Tab(CWnd* pParent = NULL);
	virtual ~CRegistratorZet048Tab();
	enum {IDD = IDD_REGISTRATOR_ZET048_TAB};
	void SetAdapter(zetlab::std::device* const device__);
	void Activate();
	void Disactivate();
	CListBox m_listFlashFiles;
protected:
	CProgressCtrlEx m_progressBarFlashState;
	CProgressCtrlEx m_progressBarBatteryState;
	LRESULT enableCopyIndicators(WPARAM wParaOnBlockControlsm, LPARAM lParam);
	LRESULT disableCopyIndicators(WPARAM wParaOnBlockControlsm, LPARAM lParam);
	LRESULT updateCopyIndicators(WPARAM wParaOnBlockControlsm, LPARAM lParam);
	CEdit    _pcDir;
	CButton m_buttonFormatFlash;
	CButton m_buttonStop;
	CButton m_buttonRemoveFile;
	CButton m_buttonRegistratorSettings;
	CButton m_buttonGodownload;
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();
	void initSdState(bool from_exclusive_mode = false);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonFormatFlash();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonRemoveFile();
	afx_msg void OnBnClickedButtonGoToCopying();
	afx_msg void OnBnClickedButtonRegistratorSettings();
	afx_msg void OnLbnSelchangeListFlashFiles();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonGoToDownload();
	afx_msg void OnChoisePcDir();
	afx_msg void OnStop();
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedButtonTestRtc();
	afx_msg void OnBnClickedProfile();
	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()	

private:
	long WriteFileToSD(CString FilePC, CStringA FileSD);
	long writeSdFileToPc(CString fComp_, const char* fSd_);
	long SmartwriteSdFileToPc(CString fComp_, const char* fSd_, unsigned long  size, long long offset);
	long ReadFileEprom(CString fileNameonPC);
	long LoadFromEpromOnSD();
	long CheckFileEpromAndSD(std::vector<char> data, uint32_t size, CString fileNameOnPC, CString fileNameOnDevice);
	long CheckStop();
	long CopySdToPc();
	void createRegInfoDacFile();
	void createRegCalibrCfgFile();
	void deleteDacDAtFile();
	long getSdListFileSize(long numItem_);
	CString GetRegistryPath(CString name_);
	void    SetRegistryPath(CString name_, CString newVal_);
	void    SetSelectedPCDirText(CString selectedDir_);
	void    SDblock();
	void    SDdeblock();
	void    SDwaitDeblock();
	void	CheckCfg();
	CString GetTypeNumberStr();
	bool    FileExists(CString fileName_);
	zetlab::std::device* device_;	
	unsigned long long m_AvailableSize;		// количество свободного места на флешке
	CDSRV _server;							// экземпляр сервера
	bool bFirstActivate;
	bool _bButtonStop;
	bool m_bTestRtc = false;
	long    _blockSD;
	long long _freeSize;
	CString _deviceName;
	currCopyProgressState createProgressStruct(CString name_, long currSendTotalSize_, unsigned long rate_);
	CZetTime m_tCreation;
	CRITICAL_SECTION g_CritSect;
	CProgressCtrlEx _totalRegCopyProgress;
	CProgressCtrlEx _currFRegCopyProgress;
	boost::thread _copyThread;
	long _currentFileTotalSize;
	long long _totalSelectedFilesSize;
	long long _totalSelectedFilesSizeOffset;
	int _copyState;
	CString _currPCDir;
	std::vector<zetlab::std::DIR_FILE> _dirSD;	
	
	boost::thread m_testRtcThread;
	void testRtc();
	LRESULT onTestRtc(WPARAM wParam, LPARAM lParam);
	CZetTime getRtcTime();
	CProgressCtrlEx m_TestRtcProgress;
	CString m_sTestRtcResult;
	CZetTimeSpan m_testRtcTime;

};
