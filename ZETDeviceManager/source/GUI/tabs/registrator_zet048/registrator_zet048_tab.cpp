#include "stdafx.h"
#include "registrator_zet048_tab.h"
#include "../../user_messages.hpp"
#include "../../../algo/utils/bytes_conv/bytes_conv.hpp"
#include "../../../algo/utils/time_conv/time_conv.hpp"
#include "source\GUI\tabs\standalone_registrator\SRWritePropDlg\SRWritePropDlg.h"
#include "source/GUI/tabs/standalone_registrator/SRChoiseProfDlg/SRChoiseProfDlg.h"
#include <ZetFile\include\ZETFile.h>
#include "../../../algo/std/device/device_std.hpp"
#include "../../user_messages.hpp"
#include "resource\Phrases.h"
#include "ZETPath/include/ZETPath.h"
#include <pugixml/auto_link.h>


#define MAX_COUNT_SIG 32
#define WM_USER_ENABLE_PROGRESS_ELEMENTS	WM_USER + 1
#define WM_USER_DISABLE_PROGRESS_ELEMENTS	WM_USER + 2
#define WM_USER_UPDATE_PROGRESS_ELEMENTS	WM_USER + 3
#define WM_USER_SAVE_PARAMETERS_TO_SD		WM_USER + 4
#define WM_USER_TEST_RTC					WM_USER + 5

const CString g_delimetr = _T("          ");
bool bStop(false);

CRegistratorZet048Tab::CRegistratorZet048Tab(CWnd* pParent) :
	CTab(CRegistratorZet048Tab::IDD, pParent)
	, device_(nullptr)

	, m_AvailableSize(NULL)
	, bFirstActivate(true)
	, _totalSelectedFilesSizeOffset(0)
	, _totalSelectedFilesSize(0)
	, _blockSD(0)
	, _copyState(0)
	, _freeSize(0)
	, _currentFileTotalSize(0)
	, _bButtonStop(false)
	, m_bTestRtc(false)
{
	// Т.к. в PropertiesDlg имеется по одному экземпляру для каждого типа вкладки, отличаем когда вкладка проинициализирована, а когда нет
	//_zetPath.Create(GetActiveWindow());			// Инициализация АктивХ элемента, взятия стандартных путей ZetLab
	CString bstrDirSignal = CString(zpGetDirSignal().c_str());
	_currPCDir = GetRegistryPath(bstrDirSignal);	// Берём текущую директорию, куда сохраняются файлы сигналов

	wchar_t lpszFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, lpszFilePath, MAX_PATH);
	WIN32_FILE_ATTRIBUTE_DATA fileInfo = { 0 };
	GetFileAttributesEx(lpszFilePath, GetFileExInfoStandard, &fileInfo);

	m_tCreation = fileInfo.ftCreationTime;
}
CRegistratorZet048Tab::~CRegistratorZet048Tab()
{}

void CRegistratorZet048Tab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESSBAR_FLASH_STATE, m_progressBarFlashState);
	DDX_Control(pDX, IDC_PROGRESSBAR_BATTERY_STATE, m_progressBarBatteryState);
	DDX_Control(pDX, IDC_LIST_FLASH_FILES, m_listFlashFiles);
	DDX_Control(pDX, IDC_BUTTON_FORMAT_FLASH, m_buttonFormatFlash);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_FILE, m_buttonRemoveFile);
	DDX_Control(pDX, IDC_BUTTON_REGISTRATOR_SETTINGS, m_buttonRegistratorSettings);
	DDX_Control(pDX, IDC_BUTTON_GO_TO_DOWNLOAD, m_buttonGodownload);
	DDX_Control(pDX, IDC_SR_CURR_ADR, _pcDir);
	DDX_Control(pDX, IDC_SR_TOTAL_COPY_PROGRESS, _totalRegCopyProgress);
	DDX_Control(pDX, IDC_SR_CURRF_COPY_PROGRESS, _currFRegCopyProgress);
	DDX_Control(pDX, IDC_STOP_DOWNLOAD, m_buttonStop);
	DDX_Control(pDX, IDC_TEST_RTC_PROGRESS, m_TestRtcProgress);
}

BEGIN_MESSAGE_MAP(CRegistratorZet048Tab, CTab)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_FORMAT_FLASH, &CRegistratorZet048Tab::OnBnClickedButtonFormatFlash)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CRegistratorZet048Tab::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_FILE, &CRegistratorZet048Tab::OnBnClickedButtonRemoveFile)
	ON_BN_CLICKED(IDC_SR_PC_CHOICE_FILE_BUTTON, OnChoisePcDir)
	ON_BN_CLICKED(IDC_STOP_DOWNLOAD, &CRegistratorZet048Tab::OnStop)
	ON_LBN_SELCHANGE(IDC_LIST_FLASH_FILES, &CRegistratorZet048Tab::OnLbnSelchangeListFlashFiles)
	ON_BN_CLICKED(IDC_BUTTON_GO_TO_COPYING, &CRegistratorZet048Tab::OnBnClickedButtonGoToCopying)
	ON_BN_CLICKED(IDC_BUTTON_REGISTRATOR_SETTINGS, &CRegistratorZet048Tab::OnBnClickedButtonRegistratorSettings)
	ON_BN_CLICKED(IDC_BUTTON_GO_TO_DOWNLOAD, &CRegistratorZet048Tab::OnBnClickedButtonGoToDownload)
	ON_MESSAGE(WM_USER_ENABLE_PROGRESS_ELEMENTS, enableCopyIndicators)
	ON_MESSAGE(WM_USER_DISABLE_PROGRESS_ELEMENTS, disableCopyIndicators)
	ON_MESSAGE(WM_USER_UPDATE_PROGRESS_ELEMENTS, updateCopyIndicators)
	ON_MESSAGE(WM_USER_TEST_RTC, onTestRtc)
	ON_BN_CLICKED(IDC_OPEN, &CRegistratorZet048Tab::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_BUTTON_TEST_RTC, &CRegistratorZet048Tab::OnBnClickedButtonTestRtc)
	ON_BN_CLICKED(IDC_Profile, &CRegistratorZet048Tab::OnBnClickedProfile)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(CRegistratorZet048Tab)
END_ANCHOR_MAP()

BOOL CRegistratorZet048Tab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_FLASH_STATE)->SetWindowTextW(g_sStatusFlashDrive);
	GetDlgItem(IDC_STOP_DOWNLOAD)->SetWindowTextW(g_sStop);
	GetDlgItem(IDC_LABEL_BATTERY_STATE)->SetWindowTextW(g_sBatteryStatus);
	GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->SetWindowTextW(g_sFormat);
	GetDlgItem(IDC_BUTTON_REMOVE_FILE)->SetWindowTextW(g_sDelete);
	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->SetWindowTextW(g_sDownload);
	GetDlgItem(IDC_BUTTON_UPDATE)->SetWindowTextW(g_sUpdate);
	GetDlgItem(IDC_BUTTON_TEST_RTC)->SetWindowText(g_sTestRTC);
	GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->SetWindowTextW(g_sOption);
	GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->SetWindowTextW(g_sConvert);
	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->SetWindowTextW(g_sDownload);
	GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->SetWindowTextW(g_sRegistrationParameters);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->SetWindowTextW(g_sTotalStatusRecording + L":");
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->SetWindowTextW(g_sRecordingStatusCurrentFile + L":");
	GetDlgItem(IDC_WAIT_COPY)->SetWindowTextW(g_sCopyProcessFinish);
	GetDlgItem(IDC_OPEN)->SetWindowTextW(g_sOpen);

	m_progressBarFlashState.SetRange(0, 100);
	m_progressBarBatteryState.SetRange(0, 100);
	m_buttonRemoveFile.EnableWindow(FALSE);
	m_buttonGodownload.EnableWindow(FALSE);

	AddToolTip(GetDlgItem(IDC_SR_CURR_ADR), _currPCDir);

	GetDlgItem(IDC_SR_CURR_ADR)->SetWindowTextW(_currPCDir);

	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_WAIT_COPY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_WAIT_COPY)->UpdateData();

	_server.Create(L"", WS_CHILD, CRect(0, 0, 0, 0), this, 1000);
	//BSTR bstrDirSignal = _T("DirSignal");
	//_convert.SetSignalDir(_zetPath.ZetPath(&bstrDirSignal));
	CString bstrDirSignal = CString(zpGetDirSignal().c_str());
	CString	bstrDirCompress = CString(zpGetDirCompressed().c_str());

	//_convert.SetSignalDir(bstrDirSignal);
	//BSTR bstrDirCompres = _T("DirCompressed");
	//_convert.SetCompresDir(bstrDirCompress);
	GetDlgItem(IDC_Profile)->SetWindowTextW(g_sProfileSensor);


	return TRUE;
	SetSelectedPCDirText(_currPCDir);
}

void CRegistratorZet048Tab::SetAdapter(zetlab::std::device* const device__)
{
	device_ = device__;
	ADC_INFO adc_info;

	device_->GetRawDataFromDevice(&adc_info);
	// если это анализатор то отображаем кнопку регистрации параметров
	if ((device_->getType() == KDU8500 && adc_info.Bits <= 16) || device_->getType() == KDU2500 || device_->getType() == KDU1616 || device_->getType() == KDU0424 || device_->getType() == KDU1624)
	{
		// 				CRect Rect;
		// 		 		m_progressBarBatteryState.GetWindowRect(Rect);
		// 		 		ScreenToClient(Rect);
		// 		 		Rect.right -= PARAM_BUTTON_WIDTH;
		// 		 		m_progressBarBatteryState.MoveWindow(Rect);
		m_buttonRegistratorSettings.ShowWindow(SW_SHOW);
	}
	else
	{
		m_buttonRegistratorSettings.ShowWindow(SW_HIDE);
	}

	if (device_->getType() != ZET_DEVICE_110)
		GetDlgItem(IDC_Profile)->ShowWindow(SW_HIDE);
}

void CRegistratorZet048Tab::OnTimer(UINT_PTR nIDEvent)
{
	if (0 == nIDEvent)
	{
		if (nullptr != device_)
		{
			unsigned char batteryCharge(0);
			char boardTemperature(0);
			unsigned short batteryVoltage(0);
			if (device_->readBatteryState(batteryCharge, boardTemperature, batteryVoltage))
			{
				CString s;
				s.Format(L"%s %d%%", g_sCapacity.GetString(), batteryCharge);
				if (batteryVoltage > 0)
					s.AppendFormat(L", %s %.1f%s", g_sVoltage.GetString(), (float)batteryVoltage / 1000.f, g_sUNIT_V);
				m_progressBarBatteryState.SetShowText(s);
				m_progressBarBatteryState.SetPos(batteryCharge);
			}
			else
			{
				m_progressBarBatteryState.SetShowText(g_sInformationNotAvailable);
				m_progressBarBatteryState.SetPos(0);
			}
		}
	}

	CTab::OnTimer(nIDEvent);
}

void CRegistratorZet048Tab::Activate()
{
	if (bFirstActivate)
	{
		initSdState();
		CheckCfg();
		bFirstActivate = false;

	}
	SetTimer(0, 1000, nullptr);
}

void CRegistratorZet048Tab::Disactivate()
{
	KillTimer(0);
}

// Заполнение вектора и listbox контрола с sd карты
void CRegistratorZet048Tab::initSdState(bool from_exclusive_mode)
{
	COMMAND_INFO_USB CommandData = { 0 };
	m_listFlashFiles.ResetContent();
	m_listFlashFiles.SetHorizontalExtent(0);

	if (nullptr == device_)
		return;
	//**********************
	long size(0), sizedat(0), sizedisk(0), sizeused(0), *prom(0);
	long err(0);
	long numFiles(0);
	long numUSBPackets(0);
	unsigned long long sdSize(0), busySize(0), availableSize(0);

	const int64_t _week = 60 * 60 * 24 * 7;
	const int64_t _1024_week = _week * 1024;
	const CZetTimeSpan _1024_week_ts(_1024_week, 0);

	::std::vector<zetlab::std::DIR_FILE> files;

	if (device_->readSdState(sdSize, busySize, availableSize, files, from_exclusive_mode))
	{
		if (!files.empty())
		{
			if (files.size() <= 65000)
			{
				uint32_t index(0);
				std::for_each(files.begin(), files.end(), [&](const zetlab::std::DIR_FILE& file)
				{
					CString sTime(L"");
					SYSTEMTIME st = { 0 }, lt = { 0 };
					memset(&st, 0, sizeof(SYSTEMTIME));
					memset(&lt, 0, sizeof(SYSTEMTIME));
					GetSystemTime(&st);
					st.wYear = (file.crt_date.date.year & 0x7f) + 1980;
					st.wMonth = file.crt_date.date.month & 0xf;
					st.wDay = file.crt_date.date.day & 0x1f;
					st.wHour = file.crt_time.time.hour & 0x1f;
					st.wMinute = file.crt_time.time.min & 0x3f;
					st.wSecond = (file.crt_time.time.x2sec & 0x1f) * 2;
					if (FALSE != SystemTimeToTzSpecificLocalTime(NULL, &st, &lt))
						st = lt;

					CZetTime t(st);
					while (true)
					{
						auto ts = t - m_tCreation;
						auto sec = ts.GetSeconds();
						if (sec < 0)
						{
							sec = abs(sec) / _week;
							if (sec > 512)
								t += _1024_week_ts;
							else
								break;
						}
						else
							break;
					}
					wchar_t buffer[20] = {0};
					t.ConvertToString(buffer, 20, 1., _T("dd.MM.yyyy HH:mm:ss"), false);
					sTime = CString(buffer);

					CString sString = sTime + g_delimetr + CA2W(file.filename) + _T(" (") +
						CString(zetlab::utils::bytes_to_string(file.sizebyte).c_str()) + _T(")");
					m_listFlashFiles.InsertString(index, sString);

					_dirSD.push_back(file);
					index++;
					//_dirSD[index++] = file;
				});

				m_listFlashFiles.SetHorizontalExtent(350);
			}
			else
			{
				MessageBox(g_sLimitSdCard, L"",  MB_ICONWARNING);

			}
		}

		const auto availableTime = availableSize / (2 * device_->GetWordsADC()) / device_->numChannels() / device_->getAdcFrequency();

		m_AvailableSize = availableSize;
		CString s = g_sTotalVolume + _T(": ") +
			CString(zetlab::utils::bytes_to_string(sdSize).c_str()) +
			_T(", ") + g_sBusyStatus + _T(": ") +
			CString(zetlab::utils::bytes_to_string(busySize).c_str()) +
			_T(", ") + g_sAvailable + _T(": ") +
			CString(zetlab::utils::bytes_to_string(availableSize).c_str()) +
			_T(" (") +
			CString(zetlab::utils::time_duration_to_string(availableTime).c_str()) +
			_T(")");

		m_progressBarFlashState.SetShowText(s);
		m_progressBarFlashState.SetPos((int)((double)busySize * 100. / (double)sdSize));
	}
	else
	{
		m_AvailableSize = 0;
		m_progressBarFlashState.SetShowText(g_sErrorWorkingSD);
		m_progressBarFlashState.SetPos(0);
	}

	SDdeblock();
}


void CRegistratorZet048Tab::OnBnClickedButtonFormatFlash()
{
	if (nullptr == device_)
		return;

	_server.Stop();

	if (MessageBox(g_sWantFormatMemoryCard,
		g_sMemoryCardFormatting,
		MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		device_->formatSd();
		Sleep(1000);
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		initSdState();
	}

	_server.Start();
}

void CRegistratorZet048Tab::OnBnClickedButtonUpdate()
{
	initSdState();
}

void CRegistratorZet048Tab::OnBnClickedButtonRemoveFile()
{
	if (nullptr == device_)
		return;

	const auto selCount = m_listFlashFiles.GetSelCount();
	UINT res = IDCANCEL;
	if (selCount == 1)
		res = MessageBox(g_sDeleteSelectedFile,
			g_sConfirmationDeletions,
			MB_OKCANCEL | MB_ICONQUESTION);
	else if (selCount > 1)
		res = MessageBox(g_sDeleteSelectedFiles,
			g_sConfirmationDeletions,
			MB_OKCANCEL | MB_ICONQUESTION);

	if (IDOK == res)
	{
		std::vector<zetlab::tstring> files;
		const auto count = m_listFlashFiles.GetCount();
		for (int i = 0; i < count; ++i)
		{
			if (m_listFlashFiles.GetSel(i) > 0)
			{
				CString sFileName(L"");
				m_listFlashFiles.GetText(i, sFileName);
				auto pos = sFileName.Find(g_delimetr);
				if (-1 != pos)
				{
					pos += g_delimetr.GetLength();
					sFileName = sFileName.Mid(pos);
					pos = sFileName.Find(L" (");
					if (-1 != pos)
					{
						sFileName = sFileName.Mid(0, pos);
						files.push_back(sFileName.GetString());
					}
				}
			}
		}
		device_->deleteFilesFromSd(files);
		initSdState();
	}
}

void CRegistratorZet048Tab::OnLbnSelchangeListFlashFiles()
{
	if (m_listFlashFiles.GetSelCount() > 0)
	{
		m_buttonRemoveFile.EnableWindow(TRUE);
		m_buttonGodownload.EnableWindow(TRUE);
	}
	else
	{
		m_buttonRemoveFile.EnableWindow(FALSE);
		m_buttonGodownload.EnableWindow(FALSE);
	}
}

void CRegistratorZet048Tab::OnLButtonDown(UINT nFlags, CPoint point)
{
	const auto count = m_listFlashFiles.GetCount();
	for (int i = 0; i < count; ++i)
		m_listFlashFiles.SetSel(i, FALSE);

	m_buttonRemoveFile.EnableWindow(FALSE);
	m_buttonGodownload.EnableWindow(FALSE);

	CTab::OnLButtonDown(nFlags, point);
}

void CRegistratorZet048Tab::OnBnClickedButtonGoToCopying()
{
	CString params;
	params.Format(L"-device %d %d", device_->getType(), device_->getDSP());
	ShellExecute(NULL, _T("open"), L"SilentConverter.exe", params, _T(""), SW_SHOW);
}

void CRegistratorZet048Tab::OnBnClickedButtonRegistratorSettings()
{
	long _deviceDSP = device_->getDSP();

	CString tmpFile, tmpFileRes, tmpFile2, tmpFileRes2;
	CString tmpPath;
	CFileStatus tempFStatus;

	tmpFile.Format(_T("temp%d.par"), device_->getType());
	GetTempPath(MAX_PATH, tmpPath.GetBuffer(MAX_PATH));
	tmpPath.ReleaseBuffer();
	GetTempFileName(tmpPath, tmpFile, 1, tmpFileRes.GetBuffer(MAX_PATH));
	tmpFileRes.ReleaseBuffer();

	tmpFile2.Format(_T("DAC%d.dat"), device_->getType());
	GetTempPath(MAX_PATH, tmpPath.GetBuffer(MAX_PATH));
	tmpPath.ReleaseBuffer();
	GetTempFileName(tmpPath, tmpFile2, 2, tmpFileRes2.GetBuffer(MAX_PATH));
	tmpFileRes2.ReleaseBuffer();

	bool bConfigRead = false;
	if (!tmpFileRes.IsEmpty())
	{
		if (ZExclusive1Open(device_->getType(), device_->getDSP()) == 0)
		{
			if (SUCCEEDED(writeSdFileToPc(tmpFileRes, "config.par")))
				bConfigRead = true;
			if (ZExclusive1Close(device_->getType(), device_->getDSP()) != 0)
				zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");
		}
		else
		{
			zfAddLog(L"", L"FALLED ExclusiveOpen", L"DeviceManager");
			long outForWhile(0);
			while (ZExclusive1Open(device_->getType(), device_->getDSP()) != 0)
			{
				Sleep(1000);
				if (outForWhile == 10)
					break;

				outForWhile++;
			}
			if (outForWhile != 10)
			{
				writeSdFileToPc(tmpFileRes, "config.par");
				bConfigRead = true;
				if (ZExclusive1Close(device_->getType(), device_->getDSP()) != 0)
					zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");
			}
		}
	}

	deleteDacDAtFile();

	if (CFile::GetStatus(tmpFileRes2, tempFStatus) != FALSE)
		CFile::Remove(tmpFileRes2);

	CSRWritePropDlg *propDlg(nullptr);
	propDlg = new CSRWritePropDlg(device_->getType(), _deviceDSP, tmpFileRes, tmpFileRes2,
	m_AvailableSize / 1024, device_->GetWordsADC(), bConfigRead);
	if (propDlg->DoModal() == IDC_SRPD_SAVE_PARAM)
	{

	}
	if (propDlg != nullptr)
	{
		delete propDlg;
		propDlg = nullptr;
	}

	ZOpen(device_->getType(), _deviceDSP);

	if (device_->getType() != ZET_DEVICE_110)
		WriteFileToSD(tmpFileRes, "config.par");

	if (CFile::GetStatus(tmpFileRes2, tempFStatus) != FALSE)
		WriteFileToSD(tmpFileRes2, "DAC.DAT");

	createRegInfoDacFile();
	createRegCalibrCfgFile();

	ZClose(device_->getType(), _deviceDSP);

	initSdState();
}

// Не используется
long CRegistratorZet048Tab::WriteFileToSD(CString FilePC, CStringA FileSD)
{
	long size(0), err(0);
	COMMAND_INFO_USB commandData;
	long retVal(0);

	long currCurrentSize(0);
	long totalCurrentSize(0);
	long currTotalSize(0);
	long totalTotalSize(0);

	err = ZExclusive1Open(device_->getType(), device_->getDSP());
	if (err == 0)
	{
		memset(commandData.Data, 0x00, 504);
		commandData.Command = 0x116;
		long lenFSdName = strlen(FileSD) + 2;
		strcpy_s((char*)commandData.Data, lenFSdName, FileSD);
		size = sizeof(commandData);
		err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);	// Вызов процедуры копирования в SD

		if (err == 0)
		{
			size = sizeof(commandData);
			err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size); // Успешно ли открыт файл в приборе
			if (err == 0)
			{
			}
			if (commandData.Error == 0)
			{
				FILE *in(nullptr);
				long currPacketSize(0);
				if (zfOpenFile(FilePC, L"rb", &in) == 0)
				{
					long sizeFile(0);

					fseek(in, 0, SEEK_END);
					sizeFile = ftell(in);
					rewind(in);

					currCurrentSize = 0;
					totalCurrentSize = sizeFile;
					retVal = totalCurrentSize;

					while (true)
					{
						currPacketSize = (long)fread(commandData.Data, 1, 504, in);
						if (currPacketSize <= 0)
							break;
						else
						{
							currCurrentSize += currPacketSize;
							currTotalSize += currPacketSize;

							CString tempFileName = FilePC;
							if (tempFileName.ReverseFind(L'\\') != -1)
								tempFileName = tempFileName.Mid(tempFileName.ReverseFind(L'\\') + 1, tempFileName.GetLength());
						}

						commandData.Size = currPacketSize;
						commandData.Command = 0x117;

						size = sizeof(commandData);
						Sleep(1);
						err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);
						if (err != 0)
							break;
					}

					if (err == 0)
					{
						size = sizeof(commandData);
						commandData.Command = 0x118;
						err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);
						if (err == 0)
						{
							size = sizeof(commandData);
							err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size);
						}
					}
					initSdState(true);
					zfCloseFile(in);
				}
			}
		}
		if (ZExclusive1Close(device_->getType(), device_->getDSP()) != 0)
			zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");
	}
	else 
		zfAddLog(L"", L"FALLED ExclusiveOpen", L"DeviceManager");

	return retVal;
}

// Для новой прошивки бидончиков, быстро скачиваем файлы
long CRegistratorZet048Tab::SmartwriteSdFileToPc(CString fComp_, const char* fSd_, unsigned long  size, long long  offset)
{

	BYTE* readPart(nullptr);

	long retVal(0);
	SDwaitDeblock();
	SDblock();
	currCopyProgressState progressState;

	COMMAND_INFO_USB commandData = { 0 };
	FILE* out(nullptr);
	struct Context {
		Context()
			: maxsize(100)
			, pos_(0)
			, out_(nullptr)
			, readPart_(nullptr)
			, m_hwnd_(NULL)
			, fSd_(NULL)
		{
		}
		unsigned long maxsize;
		unsigned long pos_;
		FILE* out_;
		BYTE* readPart_;
		currCopyProgressState progressState_;
		HWND m_hwnd_;
		const char* fSd_;
	};

	if (zfOpenFile(fComp_, L"wb", &out) == 0)
	{
		HWND m_hwnd = this->GetSafeHwnd();

		auto func = [](const void *data, unsigned long size, unsigned long offset, unsigned long rate, void *arg) -> long
		{
			auto pCtx = static_cast<Context *>(arg);

			if (data)
			{
				memcpy_s(&pCtx->readPart_[pCtx->pos_], pCtx->maxsize - pCtx->pos_, data, size);
				pCtx->pos_ += size;
				currCopyProgressState retStruct;
				retStruct._name = pCtx->fSd_;
				unsigned long shift = pCtx->maxsize - pCtx->pos_;
				if (shift > 0)
					retStruct._currSendCurrFSize = pCtx->maxsize - shift;
				if (retStruct._currSendCurrFSize < 1)
					retStruct._currSendCurrFSize = 1;

				retStruct._rate = rate;
				pCtx->progressState_ = retStruct;
				::SendMessage(pCtx->m_hwnd_, WM_USER_UPDATE_PROGRESS_ELEMENTS, (WPARAM)&pCtx->progressState_, NULL);

				if (bStop == true)
				{
					return -1;
				}
			}
			return 0;
		};

		readPart = nullptr;
		readPart = new BYTE[size];
		
		Context ctx;
		ctx.readPart_ = readPart;
		ctx.maxsize = size;
		ctx.pos_ = 0;
		ctx.progressState_ = progressState;
		ctx.m_hwnd_ = m_hwnd;
		ctx.fSd_ = fSd_;

		long commandResult = ZReadFileEx(device_->getType(), device_->getDSP(), fSd_, 0, size, func, &ctx);

		if (commandResult == 0)
		{
			retVal += (ctx.pos_);
		}
		else
		{
			CString temp(L"Ошибка в функции readfileex code: ");
			temp.AppendFormat(L"%d", commandResult);
			zfAddLog(L"", temp, L"DeviceManager");
			if (bStop != true)
			{
				// Даем попытку на скачивание, если не ноль
				for (long i = 0; i < 5; i++)
				{
					commandResult = ZReadFileEx(device_->getType(), device_->getDSP(), fSd_, 0, size, func, &ctx);
					if (commandResult == 0)
					{
						retVal += (ctx.pos_);
						break;
					}
					else
					{
						CString temp2(L"5 попыток в функции readfileex code: ");
						temp2.AppendFormat(L"%d", commandResult);
						zfAddLog(L"", temp2, L"DeviceManager");
					}
				}
			}
		}

		fwrite(readPart, retVal, sizeof(BYTE), out);

		zfCloseFile(out);
	}

	SDdeblock();

	if (readPart != NULL)
	{
		delete[] readPart;
		readPart = nullptr;
	}
	return retVal;
}

// Не используется
long CRegistratorZet048Tab::writeSdFileToPc(CString fComp_, const char* fSd_)
{
	long size(0);
	COMMAND_INFO_USB commandData = { 0 };
	FILE* out(nullptr);
	long retVal(-1);
	long currCurrCopyFileSize(0);
	currCopyProgressState progressState;

	SDwaitDeblock();
	SDblock();

	memset(commandData.Data, 0x00, 504);
	commandData.Command = 0x110; // Команда чтения файла
	long lenFSdName = strlen(fSd_) + 2;
	strcpy_s((char*)commandData.Data, lenFSdName, fSd_);
	size = sizeof(commandData);

	// Запрос на получение данных из файла
	long err(0);
	err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);
	if (err == 0)
	{
		size = sizeof(commandData);
		err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size);
		if ((err == 0) && (commandData.Error == 0))
		{
			retVal = 0;
			if (zfOpenFile(fComp_, L"wb", &out) == 0)
			{
				while (((commandData.Command == 0x110) || (commandData.Command == 0x111)))
				{
					currCurrCopyFileSize += commandData.Size;
					progressState = createProgressStruct(CString(fSd_), currCurrCopyFileSize, 0);
					SendMessage(WM_USER_UPDATE_PROGRESS_ELEMENTS, (WPARAM)&progressState);

					if (commandData.Size > 0)
						fwrite(commandData.Data, commandData.Size, 1, out);
					size = sizeof(commandData);

					err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size);
					if ((err != 0) || (commandData.Error != 0))
						break;
				}
				//initSdState();
				zfCloseFile(out);
			}
		}
	}

	SDdeblock();
	

	return retVal;
}

void CRegistratorZet048Tab::createRegInfoDacFile()
{
	COMMAND_INFO_USB commandData;
	long size(0), err(0);

	commandData.Command = 0x120;
	size = sizeof(commandData);
	err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);
	if (err == 0)
	{
		size = sizeof(commandData);
		err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size);
	}
}

void CRegistratorZet048Tab::createRegCalibrCfgFile()
{
	FILE* out(nullptr);
	float sense(0.01f);
	CString tmpFile, tmpPath, tmpFileRes;

	tmpFile.Format(_T("temp%d.par"), device_->getType());
	GetTempPath(MAX_PATH, tmpPath.GetBuffer(MAX_PATH));
	tmpPath.ReleaseBuffer();
	GetTempFileName(tmpPath, tmpFile, 1, tmpFileRes.GetBuffer(MAX_PATH));
	tmpFileRes.ReleaseBuffer();

	if (zfFileExists(tmpFileRes) == 1)
		CFile::Remove(tmpFileRes);

	//if (fopen_s(&out, CW2A(tmpFileRes), "wt") == 0)
	if (zfOpenFile(tmpFileRes, L"wt", &out) == 0)
	{
		_server.Connect();
		for (int i = 0; i < device_->numChannels(); i++)
		{
			if ((_server.TypeAdc(i) == device_->getType()) && (_server.NumDSP(i) == device_->getDSP()))
			{
				CString commentary = _server.Commentary(i);
				CString conversion = _server.Conversion(i);
				float shift = _server.ShifLevel(i);
				float reference = _server.Reference(i);

				CStringA sa;
				sa = CW2A(commentary.GetString());
				fprintf_s(out, "%s\n", sa.GetString());
				sa = CW2A(conversion.GetString());
				fprintf_s(out, "%s\n", sa.GetString());

				sense = _server.Sense(i);
				fprintf_s(out, "%g\n", sense);
				fprintf_s(out, "%g\n", shift);
				fprintf_s(out, "%g\n", reference);
			}
		}
		fflush(out);
		_server.Disconnect();
	}
	initSdState();
	zfCloseFile(out);

	WriteFileToSD(tmpFileRes, "CALIBR.CFG");
}

void CRegistratorZet048Tab::deleteDacDAtFile()
{
	std::vector<zetlab::tstring> files;
	files.push_back(L"dac.dat");
	if (device_->deleteFilesFromSd(files))
		initSdState();
}

void CRegistratorZet048Tab::OnBnClickedButtonGoToDownload()
{
	if (zfDirectoryExists(_currPCDir) == 1)
		_copyThread = boost::thread(boost::bind(&CRegistratorZet048Tab::CopySdToPc, this));
	else
		MessageBox(g_sCheckChooseFolder, L"", MB_ICONWARNING);
}

long CRegistratorZet048Tab::CopySdToPc()
{
	long outForWhile(0);
	if (_server.Stop() != 0)
		zfAddLog(L"", L"Не остановилось АЦП", L"DeviceManager");

	bool allow_download(false);
	bStop = false;

	if (ZExclusive1Open(device_->getType(), device_->getDSP()) == 0)
	{
		SendMessage(WM_USER_ENABLE_PROGRESS_ELEMENTS, 0);

		_copyState = 1;
		GetParent()->SendMessage(WM_USER_REGISTRATOR, _copyState, 0);
		KillTimer(0);

		// Скачивание device.cfg или calibr.cfg
		// Если Нашли и скачали devices , то calibr не качаем!
		bool m_bNeedDownloadCalibr(true);

		for (int i = 0, size = m_listFlashFiles.GetCount(); i < size; i++)
		{
			CString tempString(L""); long m_lCfgSize(0);
			m_listFlashFiles.GetText(i, tempString);
			if ((tempString.Find(_T("DEVICES.CFG")) != -1))
			{
				m_lCfgSize = getSdListFileSize(i);

				if (m_lCfgSize > 0)
				{
					// Проверим размеры, на епромке и на sd карте

					if (ZReadFileEx(device_->getType(), device_->getDSP(), NULL, 0, 0, NULL, NULL) == 0)
					{
						// Проверим размеры, на епромке и на sd карте
						std::vector<char> data; long readSize(65536); data.resize(readSize);
						ZReadFile(device_->getType(), device_->getDSP(), ":#3", data.data(), &readSize);

						if (readSize == 0)
						{
							// епромки нет, старая прошивка. качаем с sd
							SmartwriteSdFileToPc(_currPCDir + L"DEVICES.CFG", "DEVICES.CFG", m_lCfgSize, NULL);
							allow_download = true;
							zfAddLog(L"", L" епромки нет", L"DeviceManager");
						}
						else
						{
							if (readSize == m_lCfgSize)
							{
								// Если размеры сошлись, то проверим на серийный номер, чтоб наверняка 
								if (CheckFileEpromAndSD(data, m_lCfgSize, _currPCDir + L"DEVICES.CFG", L"DEVICES.CFG") != 0)
								{
									if (::AfxMessageBox(g_sChangeDEVICES + L"\n" + g_sChangeCFG, MB_SYSTEMMODAL) == IDOK)
									{
										if (ReadFileEprom(_currPCDir + L"DEVICES.CFG") > 0)
											allow_download = true;

										LoadFromEpromOnSD();
									}
								}
								else
									allow_download = true;
								// Если серийники сошлись, то оставляем, так как у нас уже скачан нормальный cfg
							}
							else
							{
								zfAddLog(L"", L"Размер cfg на sd не равен на епромке", L"DeviceManager");
								if (::AfxMessageBox(g_sDAMAGEDEVICES + L"\n" + g_sChangeCFG, MB_SYSTEMMODAL) == IDOK)
								{
									if (ReadFileEprom(_currPCDir + L"DEVICES.CFG") > 0)
										allow_download = true;

									LoadFromEpromOnSD();
								}
							}
						}
					}
					else
					{
						writeSdFileToPc(_currPCDir + L"DEVICES.CFG", "DEVICES.CFG");
						allow_download = true;
					}
				}
				// В случае если на sd карте devices.cfg имеет нулевой размер
				else
				{
					zfAddLog(L"", L"Размер cfg на sd равен нулю", L"DeviceManager");
					if (::AfxMessageBox(g_sNULLSIZECFG + L"\n" + g_sChangeCFG, MB_SYSTEMMODAL) == IDOK)
					{
						if (ReadFileEprom(_currPCDir + L"DEVICES.CFG") > 0)
							allow_download = true;

						LoadFromEpromOnSD();
					}
				}
				m_bNeedDownloadCalibr = false;
			}
		}

		if (m_bNeedDownloadCalibr)
		{
			for (int i = 0, size = m_listFlashFiles.GetCount(); i < size; i++)
			{
				CString tempString(L""); long m_lCfgSize(0);
				m_listFlashFiles.GetText(i, tempString);
				if ((tempString.Find(_T("CALIBR.CFG")) != -1))
				{
					m_lCfgSize = getSdListFileSize(i);
					if (m_lCfgSize > 0)
					{
						if (ZReadFileEx(device_->getType(), device_->getDSP(), NULL, 0, 0, NULL, NULL) == 0)
						{
							if (SmartwriteSdFileToPc(_currPCDir + L"CALIBR.CFG", "CALIBR.CFG", m_lCfgSize, NULL) > 0)
								allow_download = true;
						}
						else
						{
							writeSdFileToPc(_currPCDir + L"CALIBR.CFG", "CALIBR.CFG");
							allow_download = true;
						}
					}
				}
			}
		}

		if (CheckStop() == 0)
			_bButtonStop = true;
		else
			_bButtonStop = false;

		if (!allow_download)
		{
			if (::AfxMessageBox(g_sCFGNotDonload + L"\n" + g_sContinue, MB_SYSTEMMODAL | MB_OKCANCEL) == IDCANCEL)
			{
				if (ZExclusive1Close(device_->getType(), device_->getDSP()) !=0)
					zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");
				_server.Start();
				SetTimer(0, 1000, NULL);
				GetParent()->SendMessage(WM_USER_REGISTRATOR, 0, 0);
				PostMessage(WM_USER_DISABLE_PROGRESS_ELEMENTS, 0);
				initSdState();
				return -1;
			}
		}
		// Скачивание выбранных файлов
		long long totalSelectedFilesSize(0);  // выделено файлов

		for (int i = 0, size = m_listFlashFiles.GetCount(); i < size; i++)
		{
			if (m_listFlashFiles.GetSel(i) > 0)
			{
				totalSelectedFilesSize += getSdListFileSize(i);
			}
		}
		_totalSelectedFilesSize = totalSelectedFilesSize;
		_totalSelectedFilesSizeOffset = 0;

		if (m_listFlashFiles.GetSelCount() > 0)
		{
			for (int i = 0, size = m_listFlashFiles.GetCount(); i < size; i++)
			{
				if (_copyState)
				{
					if (m_listFlashFiles.GetSel(i) > 0)
					{
						CString sFileName(L"");
						m_listFlashFiles.GetText(i, sFileName);
						_currentFileTotalSize = getSdListFileSize(i);
						auto pos = sFileName.Find(g_delimetr);
						if (-1 != pos)
						{
							pos += g_delimetr.GetLength();
							sFileName = sFileName.Mid(pos);
							pos = sFileName.Find(L" (");
							if (-1 != pos)
							{
								sFileName = sFileName.Mid(0, pos);
							}
						}
						if (_currentFileTotalSize > 0)
						{
							if (ZReadFileEx(device_->getType(), device_->getDSP(), NULL, 0, 0, NULL, NULL) == 0)
							{
								zfAddLog(L"", L"Скачивание ZReadFileEx", L"DeviceManager");
								if (SmartwriteSdFileToPc(_currPCDir + sFileName, CW2A(sFileName), _currentFileTotalSize, _totalSelectedFilesSizeOffset) != _currentFileTotalSize && bStop != true)
								{
									int retval = ::AfxMessageBox(g_sErrorDownload, MB_ABORTRETRYIGNORE);
									if (retval == MB_RETRYCANCEL)
									{
										bStop = false;
										long l_outForWhile(0);
										if (ZExclusive1Close(device_->getType(), device_->getDSP()) != 0)
											zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");
										Sleep(1000);

										if (ZExclusive1Open(device_->getType(), device_->getDSP()) != 0)
										{
											while (ZExclusive1Open(device_->getType(), device_->getDSP()) != 0)
											{
												Sleep(1000);
												if (l_outForWhile == 10)
													break;

												l_outForWhile++;
											}
										}
										continue;
									}
									else if (retval == MB_YESNOCANCEL)
										break;
									else if (retval == MB_YESNO)
									{
										long l_outForWhile(0);
										if (ZExclusive1Close(device_->getType(), device_->getDSP()) !=0)
											zfAddLog(L"", L"Serial is NUL", L"DeviceManager");
										Sleep(1000);
										long err = ZExclusive1Open(device_->getType(), device_->getDSP());
										if (err != 0)
										{
											while (ZExclusive1Open(device_->getType(), device_->getDSP()) != 0)
											{
												Sleep(1000);
												if (l_outForWhile == 10)
													break;

												l_outForWhile++;
											}
										}

										if (l_outForWhile !=10 || err == 0)
											SmartwriteSdFileToPc(_currPCDir + sFileName, CW2A(sFileName), _currentFileTotalSize, _totalSelectedFilesSizeOffset);
										else 
											zfAddLog(L"", L"Так и не удалось скачать файл после кнопки повторить", L"DeviceManager");
									}
								}
								bStop = false;
							}
							else
							{
								zfAddLog(L"", L"ZSendCommand", L"DeviceManager");
								writeSdFileToPc(_currPCDir + sFileName, CW2A(sFileName));
							}
						}
						_totalSelectedFilesSizeOffset += _currentFileTotalSize;
						m_listFlashFiles.SetSel(i, FALSE);
						// проверяем название файла, его расширение должно состоять из 3 символов и начинаться на S
						int Index = sFileName.GetLength() - (sFileName.ReverseFind(L'.') + 1);
					}
				}
			}
		}

		if (ZExclusive1Close(device_->getType(), device_->getDSP()) != 0)
			zfAddLog(L"", L"FALLED ExclusiveClose", L"DeviceManager");

		initSdState();
		SetTimer(0, 1000, NULL);

		_copyState = 0;
		GetParent()->SendMessage(WM_USER_REGISTRATOR, _copyState, 0);
		PostMessage(WM_USER_DISABLE_PROGRESS_ELEMENTS, 0);
	}
	else
	{
		zfAddLog(L"", L"FALLED ExclusiveOpen", L"DeviceManager");
		while (ZExclusive1Open(device_->getType(), device_->getDSP()) != 0)
		{
			Sleep(1000);
			if (outForWhile == 10)
				break;

			outForWhile++;
		}

		if (outForWhile != 10)
			CopySdToPc();
		else
			zfAddLog(L"", L"Не вошли в эксклюзивный режим", L"DeviceManager");
	}

	_server.Start();
	return 0;
}

CString CRegistratorZet048Tab::GetTypeNumberStr()
{
	CString tmpStrName = _deviceName; // номер прибора

	tmpStrName.Trim(_T("ZETzet"));
	tmpStrName.Replace((g_sNumber), _T("n"));
	return tmpStrName;
}

bool CRegistratorZet048Tab::FileExists(CString fileName)
{
	WIN32_FIND_DATA FindFileData;
	//ищем файл с таким исенем
	HANDLE hw = FindFirstFile(fileName, &FindFileData);
	//если файл найден
	if (hw != INVALID_HANDLE_VALUE)
	{
		FindClose(hw);
		hw = NULL;
		return true;
	}
	else
	{
		if (hw != NULL)
		{
			FindClose(hw);
			hw = NULL;			
		}
		return false;
	}
}

CString CRegistratorZet048Tab::GetRegistryPath(CString name_)
{
	CString str = name_;

	//BSTR bstrStr = name_.AllocSysString();
	//str = _zetPath.ZetPathW(&bstrStr);

	//SysFreeString(bstrStr);

	CString temp = str;
	temp = temp.Right(1);
	if (temp != _T("\\"))
		str += _T("\\");

	return str;
}

void CRegistratorZet048Tab::SetRegistryPath(CString name_, CString newVal_)
{
	/*BSTR bstrStr = CString(name_).AllocSysString();
	CString m_sLastProject;
	m_sLastProject = _zetPath.ZetPathW(&bstrStr);

	BSTR bstrPath = CString(newVal_).AllocSysString();
	_zetPath.ZetSetPath(&bstrStr, &bstrPath);
	SysFreeString(bstrStr);
	SysFreeString(bstrPath);*/
}

void CRegistratorZet048Tab::SetSelectedPCDirText(CString selectedDir_)
{
	_pcDir.SetWindowText(selectedDir_);
	_pcDir.SetSel(selectedDir_.GetLength());
}

void CRegistratorZet048Tab::SDblock()
{
	_blockSD = 1;
}

void CRegistratorZet048Tab::SDdeblock()
{
	_blockSD = 0;
}

void CRegistratorZet048Tab::SDwaitDeblock()
{
	while (true)
	{
		if (_blockSD == 0)
			break;
		else
			Sleep(10);
	}
}

currCopyProgressState CRegistratorZet048Tab::createProgressStruct(CString name_, long currSendTotalSize_, unsigned long rate_)
{
	currCopyProgressState retStruct;

	retStruct._name = name_;

	retStruct._currSendCurrFSize = currSendTotalSize_;
	if (retStruct._currSendCurrFSize < 1)
		retStruct._currSendCurrFSize = 1;

	retStruct._rate = rate_;

	return retStruct;
}

LRESULT CRegistratorZet048Tab::enableCopyIndicators(WPARAM wParam, LPARAM lParam)
{
	if (_bButtonStop == true)
	{
		GetDlgItem(IDC_STOP_DOWNLOAD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STOP_DOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP_DOWNLOAD)->RedrawWindow();
		GetDlgItem(IDC_STOP_DOWNLOAD)->UpdateData();
	}
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS)->RedrawWindow();
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS)->UpdateData();
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->RedrawWindow();
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_WAIT_COPY)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_WAIT_COPY)->UpdateData();

	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS)->RedrawWindow();
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS)->UpdateData();
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->RedrawWindow();
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->UpdateData();

	GetDlgItem(IDC_SR_CURR_ADR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_CURR_ADR)->UpdateData();

	GetDlgItem(IDC_LIST_FLASH_FILES)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_FLASH_FILES)->UpdateData();

	GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->UpdateData();

	GetDlgItem(IDC_BUTTON_REMOVE_FILE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_REMOVE_FILE)->UpdateData();

	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->UpdateData();

	GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->UpdateData();
	GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_UPDATE)->UpdateData();
	GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->UpdateData();
	GetDlgItem(IDC_BUTTON_TEST_RTC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TEST_RTC)->UpdateData();

	GetDlgItem(IDC_OPEN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OPEN)->UpdateData();
	return NULL;
}

LRESULT CRegistratorZet048Tab::disableCopyIndicators(WPARAM wParam, LPARAM lParam)
{
	if (_bButtonStop == true)
	{
		GetDlgItem(IDC_STOP_DOWNLOAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STOP_DOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP_DOWNLOAD)->RedrawWindow();
		GetDlgItem(IDC_STOP_DOWNLOAD)->UpdateData();
	}
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS)->UpdateData();
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_TOTAL_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS)->UpdateData();
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SR_CURRF_COPY_PROGRESS_TEXT)->UpdateData();

	GetDlgItem(IDC_WAIT_COPY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_WAIT_COPY)->UpdateData();

	GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->UpdateData();

	GetDlgItem(IDC_SR_CURR_ADR)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SR_CURR_ADR)->UpdateData();

	GetDlgItem(IDC_LIST_FLASH_FILES)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LIST_FLASH_FILES)->UpdateData();

	GetDlgItem(IDC_LIST_FLASH_FILES)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LIST_FLASH_FILES)->UpdateData();

	ADC_INFO adc_info;
	device_->GetRawDataFromDevice(&adc_info);

	if ((device_->getType() == KDU8500 && adc_info.Bits <= 16) || device_->getType() == KDU2500 || device_->getType() == KDU1616 || device_->getType() == KDU0424 || device_->getType() == KDU1624)
	{
		GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->UpdateData();
	}

	GetDlgItem(IDC_BUTTON_REMOVE_FILE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON_REMOVE_FILE)->UpdateData();

	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->UpdateData();

	GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->UpdateData();
	GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_UPDATE)->UpdateData();
	GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->UpdateData();
	GetDlgItem(IDC_BUTTON_TEST_RTC)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_TEST_RTC)->UpdateData();

	GetDlgItem(IDC_OPEN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_OPEN)->UpdateData();
	return NULL;
}

LRESULT CRegistratorZet048Tab::updateCopyIndicators(WPARAM wParam, LPARAM lParam)
{
	currCopyProgressState* progressState = reinterpret_cast<currCopyProgressState*>(wParam);
	CString tempString;

	_totalRegCopyProgress.SetRange(0, 1000);
	_currFRegCopyProgress.SetRange(0, 1000);

	if (_currentFileTotalSize == 0)
		_currentFileTotalSize = 1;
	_currFRegCopyProgress.SetPosition(int(((float)progressState->_currSendCurrFSize * 1000) / (float)_currentFileTotalSize));
	if (_totalSelectedFilesSize == 0)
		_totalSelectedFilesSize = 1;

	progressState->_rate = progressState->_rate / 1024;

	_totalRegCopyProgress.SetPosition(int((((float)progressState->_currSendCurrFSize + (float)_totalSelectedFilesSizeOffset) * 1000) / (float)_totalSelectedFilesSize));


	if (((float)progressState->_currSendCurrFSize / (float)_currentFileTotalSize) * 100 <= 100)
	{
		tempString.Format(_T("  ---  %3.2f %%, %d "), ((float)progressState->_currSendCurrFSize / (float)_currentFileTotalSize) * 100, progressState->_rate);
		tempString = progressState->_name + tempString;
		tempString = tempString + g_sUNIT_KBSec;
		_currFRegCopyProgress.SetShowText(tempString);
	}
	else
		zfAddLog(L"", L"Вышли за диапазон скачивания прогресс бара", L"DeviceManager");

	if ((((float)progressState->_currSendCurrFSize + (float)_totalSelectedFilesSizeOffset) * 100 / (float)_totalSelectedFilesSize) <= 100)
	{
		tempString.Format(_T("%3.2f %%"), (((float)progressState->_currSendCurrFSize + (float)_totalSelectedFilesSizeOffset) * 100 / (float)_totalSelectedFilesSize));
		_totalRegCopyProgress.SetShowText(tempString);
	}
	else
		zfAddLog(L"", L"Вышли за диапазон скачивания прогресс бара", L"DeviceManager");

	_currFRegCopyProgress.UpdateData();
	_totalRegCopyProgress.UpdateData();

	return NULL;
}

afx_msg void CRegistratorZet048Tab::OnChoisePcDir()
{
	CFolderDialog cd(g_sSelectingFolderFilesSignal + L"... ", L"", NULL, NULL);

	cd.SetCurFolder(_currPCDir);

	if (cd.DoModal() == IDOK)
	{
		_currPCDir = cd.GetSelectedFolder();
		AddToolTip(GetDlgItem(IDC_SR_CURR_ADR), _currPCDir);
		// Определяем есть ли слеш в конце строки, если нет, то добавляем
		long currPos = 0;
		long currPosOld = 0;
		while (currPos >= 0)
		{
			currPosOld = currPos;
			currPos = _currPCDir.Find(L"\\", currPos + 1);
		}

		if (currPosOld < (_currPCDir.GetLength() - 1))
			_currPCDir = _currPCDir + L"\\";

		SetSelectedPCDirText(_currPCDir);
	}
}

long CRegistratorZet048Tab::getSdListFileSize(long numItem_)
{
	long ret(0);
	if ((long)_dirSD.size() > numItem_)
		ret = _dirSD[numItem_].sizebyte;
	return ret;
}

void CRegistratorZet048Tab::OnBnClickedOpen()
{
	ShellExecute(this->m_hWnd, _T("open"), _T("explorer.exe"), _currPCDir, _T(""), SW_SHOWNORMAL);

}

afx_msg void CRegistratorZet048Tab::OnStop()
{
	bStop = true;
}

long CRegistratorZet048Tab::CheckStop()
{
	COMMAND_INFO_USB commandData = { 0 };
	long ptype(0);
	commandData.Command = 0x11A;
	long size = sizeof(commandData);
	long err = ZSendCommand(device_->getType(), device_->getDSP(), &commandData, &size);
	if (err == 0)
	{
		size = sizeof(commandData);
		err = ZReceiveCommand(device_->getType(), device_->getDSP(), &commandData, &size);
		if (err == 0)
		{
			if (ZGetTypeConnection(device_->getType(), device_->getDSP(), &ptype) == 0)
			{
				if (ptype == 3)
					return 0;
				else
					return -1;
			}
			else
				return -1;
		}
		else
			return -1;
	}
	else
		return -1;
}

long CRegistratorZet048Tab::ReadFileEprom(CString fileNameonPC)
{
	std::vector<char> data; long readSize(65536); data.resize(readSize);
	long result = ZReadFile(device_->getType(), device_->getDSP(), ":#3", data.data(), &readSize);
	if (result == 0)
	{
		FILE* out(nullptr);
		if (zfOpenFile(fileNameonPC, L"wb", &out) == 0)
		{
			fwrite(data.data(), readSize, sizeof(BYTE), out);
			zfCloseFile(out);
		}
	}
	else 
		zfAddLog(L"", L"Ошибка в ReadFileEprom в ZReadFile :#3", L"DeviceManager");
	return readSize;
}

long CRegistratorZet048Tab::LoadFromEpromOnSD()
{
	long ret(0);
	std::vector<char> data; long size = 65536; data.resize(size);
	ret = ZReadFile(device_->getType(), device_->getDSP(), ":#3", data.data(), &size);
	if (ret == 0)
	{
		ret = ZSaveFile(device_->getType(), device_->getDSP(), "DEVICES.CFG", data.data(), size);
	}
	else 
		zfAddLog(L"", L"Ошибка в LoadFromEpromOnSD в ZReadFile :#3", L"DeviceManager");

	return ret;
}

long CRegistratorZet048Tab::CheckFileEpromAndSD(std::vector<char> data, uint32_t size, CString fileNameOnPC, CString fileNameOnDevice)
{
	long m_lReturn(0);
	long retVal(0);
	BYTE* readPart(nullptr);
	
	readPart = new BYTE[size];
	FILE* out(nullptr);

	struct Context
	{
		unsigned long maxsize;
		unsigned long pos_;
		BYTE* readPart_;
		Context()
			: maxsize(100)
			, pos_(0)
			, readPart_(nullptr)
		{
		}

		static long WINAPI ReadCallback(const void *data, unsigned long size, unsigned long offset, unsigned long rate, void *arg)
		{
			auto pCtx = static_cast<Context *>(arg);
			if (data)
			{
				memcpy_s(&pCtx->readPart_[pCtx->pos_], pCtx->maxsize - pCtx->pos_, data, size);
				pCtx->pos_ += size;
			}
			return 0;
		};
	};

	if (zfOpenFile(fileNameOnPC, L"wb", &out) == 0)
	{
		Context ctx;
		ctx.readPart_ = readPart;
		ctx.maxsize = size;
		ctx.pos_ = 0;
		CStringA cFileName = (CStringA)fileNameOnDevice;
		long commandResult = ZReadFileEx(device_->getType(), device_->getDSP(), cFileName, 0, size, Context::ReadCallback, &ctx);

		if (commandResult == 0)
		{
			retVal += (ctx.pos_);
		}

		fwrite(readPart, retVal, sizeof(BYTE), out);
		zfCloseFile(out);
	}
	else
		zfAddLog(L"", L"Falled OpenFile foo CheckFileEpromAndSD", L"DeviceManager");

	pugi::xml_document xmlDoc; pugi::xml_node xmlNode; pugi::xml_node xmlNodedevice;
	//*********************
	// Получаем серийник с sd
	long serialSD(0);
	xmlDoc.load_file(fileNameOnPC);
	if (!xmlDoc.empty())
	{
		xmlNode = xmlDoc.child(L"Config");
		xmlNodedevice = xmlNode.child(L"Device");
		while (xmlNodedevice != nullptr)
		{
			serialSD = xmlNodedevice.attribute(L"serial").as_int();
			xmlNodedevice = xmlNodedevice.next_sibling(L"Device");
		}
	}
	//*********************
	// Получаем серийник с епромки
	long serialEEPROM(0);
	FILE* EEPROM(nullptr);
	CString fileNameonPC = fileNameOnPC;
	fileNameonPC.Replace(L"DEVICES.CFG", L"DEVICEESEPROM.CFG");
	if (zfOpenFile(fileNameonPC, L"wb", &EEPROM) == 0)
	{
		fwrite(data.data(), retVal/*Мы уже проверяли что размер одинаковый */, sizeof(BYTE), EEPROM);
		zfCloseFile(EEPROM);
	}

	xmlDoc.load_file(fileNameonPC);
	if (!xmlDoc.empty())
	{
		xmlNode = xmlDoc.child(L"Config");
		xmlNodedevice = xmlNode.child(L"Device");
		while (xmlNodedevice != nullptr)
		{
			serialEEPROM = xmlNodedevice.attribute(L"serial").as_int();
			xmlNodedevice = xmlNodedevice.next_sibling(L"Device");
		}
	}
	//*********************
	// Проверяем серийники
	if (serialEEPROM > 0 && serialSD > 0)
	{
		if (serialEEPROM == serialSD)
			m_lReturn = 0;
		else
		{
			zfDeleteFileW(fileNameOnPC);// удалили из папки cfg sd
			zfDeleteFileW(fileNameonPC);// удалили из папки cfg епром
			zfAddLog(L"", L"Serial on SD and Serial on EEPROM is not equal", L"DeviceManager");
			m_lReturn = -1;
		}
	}
	else
	{
		zfDeleteFileW(fileNameOnPC);
		zfDeleteFileW(fileNameonPC);
		zfAddLog(L"", L"Serial is NUL", L"DeviceManager");
		m_lReturn = -2;
	}

	zfDeleteFileW(fileNameonPC);

	if (readPart != nullptr)
	{
		delete[] readPart;
		readPart = nullptr;
	}
	return m_lReturn;
}

void CRegistratorZet048Tab::OnBnClickedButtonTestRtc()
{
	static bool double_wait = false;
	static bool handled = false;

	if (double_wait)
	{
		m_testRtcTime = CZetTimeSpan(3600, 0);
		m_bTestRtc = !m_bTestRtc;
		if (m_bTestRtc)
		{
			m_testRtcThread = boost::thread(boost::bind(&CRegistratorZet048Tab::testRtc, this));
		}
		else
		{
			m_testRtcThread.join();
		}
		handled = true;

		return;
	}
	else
		handled = false;

	double_wait = true;

	UINT time = GetDoubleClickTime();
	UINT slice = time / 10;

	for (int i = 0; i < 10 && !handled; ++i)
	{
		if (i == 9)
			Sleep(slice + time % 10);
		else
			Sleep(slice);

		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0,
			PM_NOYIELD | PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	if (!handled)
	{
		m_testRtcTime = CZetTimeSpan(30, 0);
		m_bTestRtc = !m_bTestRtc;
		if (m_bTestRtc)
		{
			m_testRtcThread = boost::thread(boost::bind(&CRegistratorZet048Tab::testRtc, this));
		}
		else
		{
			m_testRtcThread.join();
		}
	}
	double_wait = false;
}

void CRegistratorZet048Tab::testRtc()
{
	_server.Stop();

	m_sTestRtcResult.Empty();
	uint32_t size((uint32_t)(m_testRtcTime.GetSeconds() * 500.));
	const CZetTimeSpan sec_0_1(0, 100000000);
	CZetTimeSpan sec30(m_testRtcTime);

	PostMessage(WM_USER_TEST_RTC, 0, 0);
	GetParent()->PostMessage(WM_USER_REGISTRATOR, 1, 0);

	auto err = ZOpen(device_->getType(), device_->getDSP());
	if (err != 0)
		m_bTestRtc = false;

	std::vector<std::pair<CZetTime, CZetTime>> t(size);
	uint32_t i(0);
	CZetTime tPrev;
	tPrev.SetCurrentTime();
	auto tStart = tPrev;
	while (m_bTestRtc)
	{
		CZetTime tNow(ZetTimeMin);
		tNow.SetCurrentTime();
		auto tRtc = getRtcTime();
		if (ZetTimeMin == tRtc)
			m_bTestRtc = false;

		t[i++] = std::make_pair(tNow, tRtc);
		if (i >= size || tNow - tStart >= sec30)
			m_bTestRtc = false;

		if (tNow - tPrev > sec_0_1)
		{
			tPrev += sec_0_1;
			double val1 = (double)i / (double)size;
			double val2 = (double)(tNow - tStart) / (double)sec30;
			if (val1 > val2)
				PostMessage(WM_USER_TEST_RTC, size, i);
			else
				PostMessage(WM_USER_TEST_RTC, (int)((double)(sec30) * 1000), (int)((double)(tNow - tStart) * 1000.));
		}

		Sleep(1);
	}
	ZClose(device_->getType(), device_->getDSP());

	if (i != 0)
	{
		size = i;
		t.resize(size);

		std::vector<std::pair<double, int>> dt;
		auto tCur = t[0];
		for (auto tt : t)
		{
			if (tt.second != tCur.second)
			{
				if (tCur != t[0])
				{
					auto dt1 = tt.first - tCur.first;
					auto dt2 = tt.second - tCur.second;
					dt.push_back(std::make_pair(dt1, (int)((double)dt2 + 0.5)));
					CString s;
					s.Format(L"%lf = %lf\n", (double)dt2, (double)dt1);
					OutputDebugString(s);
				}
				tCur = tt;
			}
		}
		if (dt.size() <= 1)
			m_sTestRtcResult.Format(g_sRTCTestFailed);
		else
		{
			auto dt1 = dt.begin()->second;
			if (!std::all_of(dt.begin(), dt.end(), [&](const std::pair<double, int>& dt_)
			{
				return dt1 == dt_.second;
			}))
				m_sTestRtcResult.Format(g_sRTCTestFailedJumps);
			else
			{
				double sum(0.), sum2(0.);
				for (auto dt_ : dt)
				{
					sum += dt_.first;
					sum2 += dt_.first * dt_.first;
				}
				auto mean = sum / dt.size() / dt1;
				auto stddev = sqrt(sum2 / dt.size() / dt1 / dt1 - mean * mean);
				auto speed = 1. - mean;
				m_sTestRtcResult = g_sRTCTestPassed + L"\n" + g_sValueSecond;
				m_sTestRtcResult.AppendFormat(L" %.3lf ", mean);
				m_sTestRtcResult.Append(g_sUNIT_S + L"\n" + g_sStabilitySecond);
				m_sTestRtcResult.AppendFormat(L" %.3lf ", stddev * 1000.);
				m_sTestRtcResult.Append(g_sUNIT_MilleSec + L"\n");
				if (speed > 0.)
					m_sTestRtcResult.Append(g_sDelay);
				else
					m_sTestRtcResult.Append(g_sTiming);
				m_sTestRtcResult.AppendFormat(L" %.3lf ", abs(speed) * 1000.);
				m_sTestRtcResult.Append(g_sUNIT_MilleSec + L" / " + g_sUNIT_S);
			}
		}
	}
	else
		m_sTestRtcResult.Format(g_sRTCTestFailedSubtract);

	PostMessage(WM_USER_TEST_RTC, 1, 0);
	GetParent()->PostMessage(WM_USER_REGISTRATOR, 0, 0);

	_server.Start();
}

LRESULT CRegistratorZet048Tab::onTestRtc(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
	{
		GetDlgItem(IDC_BUTTON_TEST_RTC)->SetWindowText(g_sSuspender);
		GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIST_FLASH_FILES)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->EnableWindow(FALSE);
		GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_TEST_RTC_PROGRESS)->ShowWindow(SW_SHOW);
		KillTimer(0);
		m_TestRtcProgress.SetRange(0, 100);
		m_TestRtcProgress.SetPosition(0);
		m_TestRtcProgress.SetShowText(L"0.0 %");
	}
	else if (wParam == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_RTC)->SetWindowText(g_sTestRTC);
		GetDlgItem(IDC_BUTTON_REGISTRATOR_SETTINGS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REMOVE_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIST_FLASH_FILES)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_GO_TO_DOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FORMAT_FLASH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_GO_TO_COPYING)->EnableWindow(TRUE);
		GetDlgItem(IDC_SR_PC_CHOICE_FILE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_TEST_RTC_PROGRESS)->ShowWindow(SW_HIDE);
		SetTimer(0, 1000, NULL);
		m_TestRtcProgress.SetPosition(100);
		m_TestRtcProgress.SetShowText(L"100.0 %");
		if (!m_sTestRtcResult.IsEmpty())
			AfxMessageBox(m_sTestRtcResult);
	}
	else
	{
		double percent = (double)lParam / (double)wParam * 100.;
		CString s;
		s.Format(g_sTestRTC + L" %.1lf%%\n", percent);
		m_TestRtcProgress.SetPosition((int)percent);
		m_TestRtcProgress.SetShowText(s);
	}

	return 0;
}

CZetTime CRegistratorZet048Tab::getRtcTime()
{
	CZetTime tRtc(ZetTimeMin);

	COMMAND_INFO_USB commandInfoUsb = { 0 };
	commandInfoUsb.Command = 0x108;
	commandInfoUsb.Size = sizeof(COMMAND_INFO_USB);
	long lSize = sizeof(COMMAND_INFO_USB);

	if (!ZProcessCommand(device_->getType(), device_->getDSP(), &commandInfoUsb, &lSize) &&
		commandInfoUsb.Command == 0x108)
	{
		SYSTEMTIME systemTime = { 0 };

		systemTime.wYear = commandInfoUsb.Data[4] + 1980;
		systemTime.wMonth = commandInfoUsb.Data[3];
		systemTime.wDay = commandInfoUsb.Data[2];
		systemTime.wHour = short((commandInfoUsb.Data[0] >> 11)) & 0x1f;
		systemTime.wMinute = short((commandInfoUsb.Data[0] >> 5)) & 0x3f;
		systemTime.wSecond = short((commandInfoUsb.Data[0] & 0x1f)) * 2;
		tRtc = systemTime;
	}

	return tRtc;
}

// Проверим на наличие cfg
void CRegistratorZet048Tab::CheckCfg()
{
	bool DeviceHaveCfg(false);
	for (int i = 0, size = m_listFlashFiles.GetCount(); i < size; i++)
	{
		CString tempString(L""); long m_lCfgSize(0);
		m_listFlashFiles.GetText(i, tempString);
		if ((tempString.Find(_T("DEVICES.CFG")) != -1) || (tempString.Find(_T("CALIBR.CFG")) != -1))
		{
			DeviceHaveCfg = true;
			break;
		}
	}
	if (DeviceHaveCfg == false)
	{
		ZExclusive1Open(device_->getType(), device_->getDSP());
		LoadFromEpromOnSD();
		ZExclusive1Close(device_->getType(), device_->getDSP());
		SetTimer(0, 1000, NULL);
		GetParent()->SendMessage(WM_USER_REGISTRATOR, 0, 0);
		_dirSD.clear();
		initSdState();
	}
}


void CRegistratorZet048Tab::OnBnClickedProfile()
{
	CSRChoiseProfDlg dlg(device_->getType(), device_->getDSP());
	dlg.DoModal();
}
