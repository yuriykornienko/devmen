// SRWritePropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SRWritePropDlg.h"
#include "afxdialogex.h"
#include "../SRDataPropDlg/SRDataPropDlg.h"
#include <Localized/Localized.h>

#include "source\types.hpp"
#include "source\algo\utils\time_conv\time_conv.hpp"

#include <mathZet.h>
#include <ZetFile\include\ZETFile.h>
#include "resource\Phrases.h"

#define NUMBIT_017 4 // номер бита при запуске через цифровой порт

#define LoadBitmap_(nID) \
	(HBITMAP)::LoadImage( AfxGetInstanceHandle(), \
	MAKEINTRESOURCE( nID ), \
	IMAGE_BITMAP, 16, 16, LR_SHARED | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS )

// CSRWritePropDlg dialog

IMPLEMENT_DYNAMIC(CSRWritePropDlg, CDialog)

CSRWritePropDlg::CSRWritePropDlg(long type_, long dsp_, CString fileNameCFG_, CString fileNameDAC_, long long freeDiskSpace_, long countwords_, bool bReadConfig, CWnd* pParent /*=NULL*/)
	: CDialog(CSRWritePropDlg::IDD, pParent)
	, _chanFreq(1)
	, _chanQuan(1)
	, _maxSDWriteTime(0.f)

{

	_type = type_;
	_dsp = dsp_;
	_configFileParam = fileNameCFG_;
	_dacFileParam = fileNameDAC_;
	_freeDiskSpace = freeDiskSpace_;
	_CountWords = countwords_;

	_bConfigFileReadOk = bReadConfig;
	ZeroMemory(_maxTotalSampleRate, 100 * sizeof(float));
	ZeroMemory(_maxChannelSampleRate, 100 * sizeof(float));
}

CSRWritePropDlg::~CSRWritePropDlg()
{
}

void CSRWritePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SRPD_REG_INFO, _listChanSettings);
	DDX_Control(pDX, IDC_SRPD_WD_ETEXT, _writeTime);
	DDX_Control(pDX, IDC_SRPD_WP_ETEXT, _writeDelay);
	DDX_Control(pDX, IDC_SRPD_ESCHECKBOX, _checkEventStart);
	DDX_Control(pDX, IDC_SRPD_EXTDS_BUTTON, _digEventStart);
	DDX_Control(pDX, IDC_SRPD_EXTAS_BUTTON, _anaEventStart);
	DDX_Control(pDX, IDC_SRPD_EXTTS_BUTTON, _tmrEventStart);
	DDX_Control(pDX, IDC_SRPD_SBNUM_COMBOBOX, _eventStartChanNum);
	DDX_Control(pDX, IDC_SRPD_ES_COMBOBOX, _eventStartFront);
	DDX_Control(pDX, IDC_SRPD_TL_COMBOBOX, _eventStartTreshold);
	DDX_Control(pDX, IDC_SRPD_START_DATE, _dateStart);
	DDX_Control(pDX, IDC_SRPD_START_TIME, _timeStart);
	DDX_Control(pDX, IDC_SRPD_ADD_TIME, _addStartTime);
	DDX_Control(pDX, IDC_SRPD_CLEAR_TIME, _clearStartTime);
	DDX_Control(pDX, IDC_SRPD_PRIMARY_RADIO, _primaryData);
	DDX_Control(pDX, IDC_SRPD_SECONDARY_RADIO, _secondaryData);
	DDX_Control(pDX, IDC_SRPD_CONTINUOUS_RECORDING, _multiEventsMode);
	DDX_Control(pDX, IDC_SRPD_AGSYNC_CHECK, _genGPSSynMode);
	DDX_Control(pDX, IDC_SRPD_ENA_SIN_GEN_CHECK, _checkSinGenEnable);
	DDX_Control(pDX, IDC_SRPD_SIN_GEN_FREQ_ETEXT, _sinGenFreq);
	DDX_Control(pDX, IDC_SRPD_SIN_GEN_RMS_ETEXT, _sinGenLevel);
	DDX_Control(pDX, IDC_SRPD_SIN_GEN_OFFSET_ETEXT, _sinGenOffset);
	DDX_Control(pDX, IDC_SRPD_GEN_LEVEL_CONTROL_CHECK, _checkLevelControl);
	DDX_Control(pDX, IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX, _channelControl);
	DDX_Control(pDX, IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX, _channelControlFront);
	DDX_Control(pDX, IDC_SRPD_GEN_CONTROL_TL_ETEXT, _channelControlLevel);
	DDX_Control(pDX, IDC_SRPD_GEN_NUM_BIT_COMBOBOX, _numBitControl);
	DDX_Control(pDX, IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX, _activeLevelControl);
	DDX_Control(pDX, IDC_SRPD_DELAY_TIME_TYPE, _writeTimeType);
}


BEGIN_MESSAGE_MAP(CSRWritePropDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SRPD_ESCHECKBOX, OnBnEventStartCheckbox)
	ON_BN_CLICKED(IDC_SRPD_EXTDS_BUTTON, OnBnEnableDigStart)
	ON_BN_CLICKED(IDC_SRPD_EXTAS_BUTTON, OnBnEnableAnaStart)
	ON_BN_CLICKED(IDC_SRPD_EXTTS_BUTTON, OnBnEnableTmrStart)
	ON_BN_CLICKED(IDC_SRPD_PRIMARY_RADIO, OnBnEnablePrimaryData)
	ON_BN_CLICKED(IDC_SRPD_SECONDARY_RADIO, OnBnEnableSecondaryData)
	ON_BN_CLICKED(IDC_SRPD_ENA_SIN_GEN_CHECK, OnBnEnableSinGen)
	ON_BN_CLICKED(IDC_SRPD_GEN_LEVEL_CONTROL_CHECK, OnBnEnableLevelControl)
	ON_BN_CLICKED(IDC_SRPD_SAVE_PARAM, OnBnSaveParam)
	ON_BN_CLICKED(IDC_SRPD_CANCEL, OnBnCancel)
	ON_BN_CLICKED(IDC_SRPD_SETTINGS_DATA_BUTTON, OnBnSettingsData)
	ON_BN_CLICKED(IDC_SRPD_ADD_TIME, OnBnAddTime)
	ON_BN_CLICKED(IDC_SRPD_CLEAR_TIME, OnBnClearTime)
END_MESSAGE_MAP()


// CSRWritePropDlg message handlers


BOOL CSRWritePropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DECLARE_LOC_SUPPORT()

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif
	SetWindowTextW(sDebug + g_sRegistrationParameters);

	GetDlgItem(IDC_SRPD_SAVE_PARAM)->SetWindowTextW(g_sSave);
	GetDlgItem(IDC_SRPD_CANCEL)->SetWindowTextW(g_sCancel);
	GetDlgItem(IDC_SRPD_WP_GROUP)->SetWindowTextW(g_sRecordModeParameters);
	GetDlgItem(IDC_SRPD_WD_TEXT)->SetWindowTextW(g_sRecordingDuration);
	GetDlgItem(IDC_SRPD_WAP_TEXT)->SetWindowTextW(g_sDelayBeforeMeasurement + L", " + g_sUNIT_S);
	GetDlgItem(IDC_SRPD_ESCHECKBOX)->SetWindowTextW(g_sStartEvent);
	GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->SetWindowTextW(g_sExternalDigitalSignal);
	GetDlgItem(IDC_SRPD_EXTAS_BUTTON)->SetWindowTextW(g_sExternalAnalogSignal);
	GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->SetWindowTextW(g_sStartTimer);
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->SetWindowTextW(g_sBitNumber);
	GetDlgItem(IDC_SRPD_ES_TEXT)->SetWindowTextW(g_sInitialFront);
	GetDlgItem(IDC_SRPD_TL_TEXT)->SetWindowTextW(g_sThresholdLevel);
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->SetWindowTextW(g_sUnitMeasure);
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->SetWindowTextW(g_sStartTimeDate);
	GetDlgItem(IDC_SRPD_WRP_TEXT)->SetWindowTextW(g_sRecordingParameters);
	GetDlgItem(IDC_SRPD_PRIMARY_RADIO)->SetWindowTextW(g_sPrimary);
	GetDlgItem(IDC_SRPD_SECONDARY_RADIO)->SetWindowTextW(g_sSecondary);
	GetDlgItem(IDC_SRPD_CONTINUOUS_RECORDING)->SetWindowTextW(g_sContinuousRecording);
	GetDlgItem(IDC_SRPD_AGSYNC_CHECK)->SetWindowTextW(g_sAdjustmentExactGeneratorGPS);
	GetDlgItem(IDC_SRPD_SIG_GEN_GROUP)->SetWindowTextW(g_sSignalGeneration);
	GetDlgItem(IDC_SRPD_ENA_SIN_GEN_CHECK)->SetWindowTextW(g_sIncludeGeneratorSine);
	GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->SetWindowTextW(g_sFrequency + L", " + g_sUNIT_Hz);
	GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->SetWindowTextW(g_sLevel + L" (" + g_sRMS + L"), " + g_sUNIT_MV);
	GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->SetWindowTextW(g_sOffset + L", " + g_sUNIT_MV);
	GetDlgItem(IDC_SRPD_GEN_EXCESS_LEVEL_GROUP)->SetWindowTextW(g_sLevelExceeding);
	GetDlgItem(IDC_SRPD_GEN_LEVEL_CONTROL_CHECK)->SetWindowTextW(g_sTracingLevel);
	GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->SetWindowTextW(g_sChannel);
	GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->SetWindowTextW(g_sUpperLowerLevel);
	GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->SetWindowTextW(g_sThresholdLevel);
	GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->SetWindowTextW(g_sUnitMeasure);
	GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->SetWindowTextW(g_sBitNumber);
	GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->SetWindowTextW(g_sLevelActive);
	GetDlgItem(IDC_SRPD_REGISTRATOR_INFO_TEXT)->SetWindowTextW(g_sInformationRegistrar);
	GetDlgItem(IDC_SRPD_SETTINGS_DATA_BUTTON)->SetWindowTextW(g_sAdjustingJobData);

	_maxSDWriteTime = 0;
	_zetServer.Create(L"", WS_CHILD, CRect(0, 0, 0, 0), this, 1000);
	_zetServer.Connect();

	_addStartTime.SetBitmap(LoadBitmap_(IDB_SR_ADD_START_TIME));
	_clearStartTime.SetBitmap(LoadBitmap_(IDB_SR_CLEAR_START_TIME));

	_writeTimeType.AddString(g_sUNIT_S);
	_writeTimeType.AddString(g_sUNIT_MINUTES);
	_writeTimeType.AddString(g_sUNIT_HOURS);

	_maxTotalSampleRate[ZET_DEVICE_210] = 20;	  // 20 КГц
	_maxTotalSampleRate[ZET_DEVICE_110] = 50;	  // 50 КГц
	_maxTotalSampleRate[ZET_DEVICE_A17U8] = 400;	  // 400 КГц
	_maxTotalSampleRate[ZET_DEVICE_A17U2] = 250;	  // 250 КГц
	_maxTotalSampleRate[ZET_DEVICE_220] = 8;		  // 8 КГц
	_maxTotalSampleRate[ZET_DEVICE_230] = 50;	  // 50 КГц
	_maxTotalSampleRate[ZET_DEVICE_ZET048] = 80;	  // 80 КГц
	_maxTotalSampleRate[ZET_DEVICE_240] = 80;	  // 80 КГц

	_maxChannelSampleRate[ZET_DEVICE_210] = 8;	  // 8 КГц
	_maxChannelSampleRate[ZET_DEVICE_110] = 50;	  // 50 КГц
	_maxChannelSampleRate[ZET_DEVICE_A17U8] = 50;	  // 50 КГц
	_maxChannelSampleRate[ZET_DEVICE_A17U2] = 250;	  // 250 КГц
	_maxChannelSampleRate[ZET_DEVICE_220] = 8;	  // 8 КГц
	_maxChannelSampleRate[ZET_DEVICE_230] = 100;	  // 100 КГц
	_maxChannelSampleRate[ZET_DEVICE_ZET048] = 2.5;	  // 2.5 КГц
	_maxChannelSampleRate[ZET_DEVICE_240] = 2.5;	  // 2.5 КГц

	if (_type == ZET_DEVICE_110)
	{
		GetDeviceParamExt();
		FillWindowFieldsDelta();
	}
	else
	{
		GetDeviceParam();
		FillWindowFields();
	}
	UpdateRegInfoListBox();

	SetWindowDesign();

	return TRUE;
}

void CSRWritePropDlg::FillWindowFields()
{
	// Длительность записи
	CString tempString;
	long writeTime = *(long*)(&_param.low_timereg);
	tempString.Format(_T("%d"), writeTime);
	_writeTimeType.SetCurSel(0);
	if (writeTime > 60)
	{
		tempString.Format(_T("%f"), (float)writeTime / 60);
		_writeTimeType.SetCurSel(1);
	}
	if (writeTime > 3600)
	{
		tempString.Format(_T("%f"), (float)writeTime / 3600);
		_writeTimeType.SetCurSel(2);
	}

	_writeTime.SetWindowText(tempString);
	// Задержка перед измерениями (только для 110)
	_writeDelay.SetWindowText(L"");
	// Запуск по цифровому сигналу
	_checkEventStart.SetCheck(_param.enazap);
	OnBnEventStartCheckbox();
	// Какой именно режим запуска
	if (_type != ZET_DEVICE_A17U8 && _type != ZET_DEVICE_A17U2)
	{
		if (_checkEventStart.GetCheck() == 1)
		{
			switch (_param.digana)
			{
			case 0:
				_digEventStart.SetCheck(1);
				OnBnEnableDigStart();
				break;
			case 1:
				_anaEventStart.SetCheck(1);
				OnBnEnableAnaStart();
				break;
			case 2:
				_tmrEventStart.SetCheck(1);
				OnBnEnableTmrStart();
				break;
			}
		}
	}
	// Тип записываемых данных (регулируется только для дельты, так как только он пересчитывает данные)
	// Многократное срабатывание
	_multiEventsMode.SetCheck(_param.MultiThreshold);
	// Синхронизация точного генераотра по GPS
	if (_param.precision == -1)
		_genGPSSynMode.SetCheck(FALSE);
	else
		_genGPSSynMode.SetCheck(TRUE);
	// Включение генератора синуса
	_checkSinGenEnable.SetCheck(_param.enaAutoGen);
	tempString.Format(_T("%f"), _param.AGSygnalParam1);
	GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->SetWindowText(tempString);
	tempString.Format(_T("%f"), _param.AGSygnalParam2);
	GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->SetWindowText(tempString);
	tempString.Format(_T("%f"), _param.AGSygnalParam3);
	GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->SetWindowText(tempString);
	OnBnEnableSinGen();
	// Отслеживать уровень
	_checkLevelControl.SetCheck(_param.enagetlevel);
	OnBnEnableLevelControl();
}

void CSRWritePropDlg::FillWindowFieldsDelta()
{
	// Длительность записи
	CString tempString;
	long writeTime = *(long*)(&_paramdelta.low_timereg);
	tempString.Format(_T("%d"), writeTime);
	_writeTimeType.SetCurSel(0);
	if (writeTime > 60)
	{
		tempString.Format(_T("%f"), (float)writeTime / 60);
		_writeTimeType.SetCurSel(1);
	}
	if (writeTime > 3600)
	{
		tempString.Format(_T("%f"), (float)writeTime / 3600);
		_writeTimeType.SetCurSel(2);
	}
	_writeTime.SetWindowText(tempString);
	// Задержка перед измерениями (только для 110)
	tempString.Format(_T("%d"), _paramdelta.preDelay);
	_writeDelay.SetWindowText(tempString);
	// Запуск по цифровому сигналу
	_checkEventStart.SetCheck(_paramdelta.enazap);
	OnBnEventStartCheckbox();
	// Какой именно режим запуска
	if (_checkEventStart.GetCheck() == 1)
	{
		_paramdelta.digana = 1;
		_anaEventStart.SetCheck(1);
		OnBnEnableAnaStart();

	}
	// Тип записываемых данных (регулируется только для дельты, так как только он пересчитывает данные)
	if (_paramdelta.ModeRec == 0)
		_primaryData.SetCheck(1);
	else
		_secondaryData.SetCheck(1);
	// Многократное срабатывание
	// Синхронизация точного генераотра по GPS
	// Включение генератора синуса
	_checkSinGenEnable.SetCheck(0);
	OnBnEnableSinGen();
	// Отслеживать уровень
	_checkLevelControl.SetCheck(0);
	OnBnEnableLevelControl();
}

void CSRWritePropDlg::SetWindowDesign()
{
	// Массив отвечающий за количество опций 
	bool BoolbOptions[8][SIZE_BOOL_B_OPTIONS] =
	{ true,	true,	true,	true,	true,	true,	false,	true,	true,		// 210
		true,	false,	true,	true,	false,	true,	true,	true,	true,		// 110
		true,	true,	false,	true,	true,	true,	false,	true,	true,		// A17U8
		true,	true,	false,	true,	true,	true,	false,	true,	true,		// A17U2
		false,	true,	false,	false,	true,	true,	false,	false,	true,		// 220
		false,	true,	false,	true,	true,	true,	false,	false,	true,		// 230
		false,	true,	false,	false,	true,	false,	true,	false,	false,		// ZET048
		false,	true,	false,	false,	true,	true,	true,	false,	false };	// 240

// Сопоставляет тип устройства и строку массива
	std::map <int, int> MapBoolbOptions = { { ZET_DEVICE_210, 0 },
											{ ZET_DEVICE_110, 1 },
											{ ZET_DEVICE_A17U8, 2 },
											{ ZET_DEVICE_A17U2, 3 },
											{ ZET_DEVICE_220, 4 },
											{ ZET_DEVICE_230, 5 },
											{ ZET_DEVICE_ZET048, 6 },
											{ ZET_DEVICE_240, 7 } };

	if ((_type == ZET_DEVICE_A17U8) || (_type == ZET_DEVICE_A17U2))
	{
		CString escheckboxText;
		escheckboxText.Format(L"%s %u", g_sRunningThroughDigitalPort.GetString(), 1 + NUMBIT_017); // нумерация с 0, а нам нужно с 1
		GetDlgItem(IDC_SRPD_CONTINUOUS_RECORDING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_ESCHECKBOX)->SetWindowTextW(escheckboxText);
	}
	else
		GetDlgItem(IDC_SRPD_ESCHECKBOX)->SetWindowTextW(g_sStartEvent);

	// ДЛя R7 все отключаем 
	if (_type == ZET_DEVICE_A17U8 && GetDevicelong())
	{
		GetDlgItem(IDC_SRPD_WD_TEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_WD_ETEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_WP_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_ENA_SIN_GEN_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_SIG_GEN_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_DELAY_TIME_TYPE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SRPD_ESCHECKBOX)->ShowWindow(SW_HIDE);
	}


	auto it = MapBoolbOptions.find(_type);
	SetWindowDesignNew(BoolbOptions[it->second]);
}

long CSRWritePropDlg::GetDeviceParam()
{
	long retVal(-1);
	//memset(&_param, 0x00, sizeof(PARAM));

	if (_bConfigFileReadOk)
	{
		FILE *paramFile(nullptr);
		//if ((_wfopen_s(&paramFile, _configFileParam, _T("rb")) == 0) && (paramFile != nullptr))
		if (zfOpenFile(_configFileParam, L"rb", &paramFile) == 0)
		{
			long rSize = (long)fread(&_param, 1, sizeof(PARAM), paramFile);
			if (rSize == sizeof(PARAM))
			{
				retVal = 0;
			}
			zfCloseFile(paramFile);
		}
	}
	else
	{
		// если возникли ошибки в флешке, заносим параметры по умолчанию в нее
		SetDefaultParams();
		SetDeviceParam();
	}

	return retVal;
}

long CSRWritePropDlg::SetDeviceParam()
{
	long retVal(-1);

	FILE *paramFile(nullptr);
	//if ((_wfopen_s(&paramFile, _configFileParam, L"wb") == 0) && (paramFile != nullptr))
	if (zfOpenFile(_configFileParam, L"wb", &paramFile) == 0)
	{
		long rSize = (long)fwrite(&_param, 1, sizeof(PARAM), paramFile);
		if (rSize == sizeof(PARAM))
		{
			retVal = 0;
		}
		zfCloseFile(paramFile);
	}

	return retVal;
}

long CSRWritePropDlg::GetDeviceParamExt()
{
	long retVal(-1);
	long err(0), sSize(0);
	memset(&_paramdelta, 0x00, sizeof(PARAMEXT));
	COMMAND_INFO_USB command;

	command.Command = 0x131;
	command.Error = 0;
	command.Size = 0;
	sSize = sizeof(command);

	err = ZExclusive1Open(_type, _dsp);
	if (err != 0)
	{
		long outForWhile(0);
		while (ZExclusive1Open(_type, _dsp) != 0)
		{
			Sleep(1000);
			if (outForWhile == 10)
				break;

			outForWhile++;
		}

		if (outForWhile != 10)
			err = 0;
	}

	if (err == 0)
	{
		Sleep(5);
		err = ZSendCommand(_type, _dsp, &command, &sSize);
		if (err == 0)
		{
			sSize = sizeof(command);
			err = ZReceiveCommand(_type, _dsp, &command, &sSize);
			if ((err == 0) && (command.Error == 0))
			{
				memcpy_s(&_paramdelta, sizeof(PARAMEXT), command.Data, sizeof(PARAMEXT));
				retVal = 0;
			}
		}
		ZExclusive1Close(_type, _dsp);
	}
	return retVal;
}

long CSRWritePropDlg::SetDeviceParamExt()
{
	long retVal(-1);
	long err(0), sSize(0);
	COMMAND_INFO_USB command;

	command.Command = 0x130;
	command.Error = 0;
	command.Size = 0;
	memcpy_s(command.Data, sizeof(PARAMEXT), &_paramdelta, sizeof(PARAMEXT));
	sSize = sizeof(command);

	err = ZExclusive1Open(_type, _dsp);

	if (err != 0)
	{
		long outForWhile(0);
		while (ZExclusive1Open(_type, _dsp) != 0)
		{
			Sleep(1000);
			if (outForWhile == 10)
				break;

			outForWhile++;
		}

		if (outForWhile != 10)
			err = 0;
	}

	if (err == 0)
	{
		Sleep(5);
		err = ZSendCommand(_type, _dsp, &command, &sSize);
		if (err == 0)
		{
			sSize = sizeof(command);
			err = ZReceiveCommand(_type, _dsp, &command, &sSize);
			if ((err == 0) && (command.Error == 0))
			{
				retVal = 0;
			}
		}
		ZExclusive1Close(_type, _dsp);
	}
	return retVal;
}

void CSRWritePropDlg::UpdateRegInfoListBox()
{
	bool firstPrint = true;
	CString tempString, tempString2;
	_chanFreq = 0.f;
	_listChanSettings.ResetContent();

	_chanQuan = 0;
	bool maxChanFreqLimit = false;
	for (unsigned int i = 0, numChan = _zetServer.QuanChan(); i < (unsigned int)numChan; i++)
	{
		if (_zetServer.TypeAdc(i) == _type)
			if (_zetServer.NumDSP(i) == _dsp)
			{
				if (firstPrint == true)
				{
					_chanFreq = _zetServer.Frequency(i) / 1000;
					tempString.Format(_T("%7.2f"), _chanFreq);
					tempString = g_sFrequencyEachChannel + L": " + tempString + L" " + g_sUNIT_kHz + L"\n\n";
					if (_chanFreq > _maxChannelSampleRate[_type])
						maxChanFreqLimit = true;
					if (_type == 19 && _chanFreq == 2.5f)
					{
						MessageBox(g_sRegistrationSamplingFrequency, g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);
					}
					_listChanSettings.AddString(tempString);
				}
				firstPrint = false;
				tempString.Format(_T("%d: "), _zetServer.NumPhChan(i) + 1);
				tempString = g_sChannel + L" " + tempString;
				tempString2.Format(_T("%f"), _zetServer.MaxLevel(i));
				tempString2 = L" - " + g_sMaximumLevel + L": " + tempString2 + L" ";
				tempString = tempString + _zetServer.Commentary(i) + tempString2 + _zetServer.Conversion(i) + L"\n";
				_listChanSettings.AddString(tempString);
				_chanQuan++;
			}
	}
	if (maxChanFreqLimit == true)
	{// Частота дискретизации устройства 100.1 кГц превысила максимальное значение 30.5 кГц
		tempString.Format(L" %.1f ", _chanFreq);
		CString localTempString = g_sSamplingFrequencyDevice + tempString + g_sUNIT_kHz + L" ";
		tempString.Format(L" %.1f ", _maxChannelSampleRate[_type]);
		localTempString += g_sExceededMaximumValue + tempString + g_sUNIT_kHz;
		// 		localTempString.Format(_T("%f"), _maxChannelSampleRate[_type]);
		// 		localTempString = g_Threshold_maximum_recording_frequency + L" " + localTempString + L" " + g_UNIT_kHz;
		MessageBox(localTempString, g_sThresholdExceeded, MB_OKCANCEL | MB_ICONWARNING);
	}

	long enable(0);
	double freq(0.);
	double level(0.);
	double offset(0.);

	if (_zetServer.GetSineBuiltinGenParam(0, &enable, &freq, &level, &offset) == 0)
	{
		CString OffsetAndEnable(L"");

		if (enable == 1)
		{
			OffsetAndEnable.Format(L" %0.3f ", offset);
			OffsetAndEnable = g_sOffsetGenerator + OffsetAndEnable + g_sUNIT_V;
		}
		else
		{
			OffsetAndEnable = g_sOffsetGeneratorOFF;

		}
		_listChanSettings.AddString(OffsetAndEnable);
	}

	_listChanSettings.AddString(L"\n_________________________________________________\n");
	tempString.Format(_T("%7.2f"), _chanFreq*_chanQuan);
	tempString = g_sSummSamplingFrequency + L": " + tempString + L" " + g_sUNIT_kHz + L"\n";

	if (!GetMaxbitrate())
	{
		if (_chanFreq*_chanQuan > _maxTotalSampleRate[_type])
		{
			CString tempText;
			tempText.Format(_T("%f"), _maxTotalSampleRate[_type]);
			tempText = g_sTotalSamplingFrequency + ": " + tempText + L" " + g_sUNIT_kHz + L".";
			MessageBox(tempText, g_sThresholdExceeded, MB_OKCANCEL | MB_ICONWARNING);
		}
	}
	// Для R7 8 каналов + генератор
	else
	{
		MessageBox(g_sThresholdExceededWithGenerator, g_sOFFChannel, MB_OK | MB_ICONWARNING);
	}
	float WriteTime = (float)_freeDiskSpace / _chanFreq / _chanQuan / (2.f * _CountWords);
	_maxSDWriteTime = WriteTime;

	_listChanSettings.AddString(tempString);
	_listChanSettings.AddString(L"\n" + g_sMaximumRecordingTime + L": " + CString(zetlab::utils::time_duration_to_string(WriteTime).c_str()));

	bool continuousWMFlag = (_param.ModeSD >> 10) & 0x1;
	if (continuousWMFlag)
	{
		tempString.Format(_T("%f"), WriteTime / 3600.f);
		_writeTimeType.SetCurSel(2);
		_writeTime.SetWindowText(tempString);

		_writeTime.EnableWindow(FALSE);
		_writeTimeType.EnableWindow(FALSE);
	}


	_listChanSettings.AddString(L"_________________________________________________\n\n");
}

void CSRWritePropDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		_zetServer.NumFileUsed(0L);
	}

	CDialog::OnTimer(nIDEvent);
}

void CSRWritePropDlg::OnBnEventStartCheckbox()
{
	if ((_checkEventStart.GetCheck() == false) || (_type == ZET_DEVICE_A17U8) || (_type == ZET_DEVICE_A17U2))
	{
		GetDlgItem(IDC_SRPD_START_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_START_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_START_DATE)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_START_DATE)->UpdateData();
		GetDlgItem(IDC_SRPD_ADD_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_ADD_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_CLEAR_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_CLEAR_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_DAT_START_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_DAT_START_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->UpdateData();
		GetDlgItem(IDC_SRPD_EXTAS_BUTTON)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_EXTAS_BUTTON)->UpdateData();
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->UpdateData();
		GetDlgItem(IDC_SRPD_SBNUM_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SBNUM_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_ES_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_ES_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_ES_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_ES_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_TL_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_TL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_TL_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_TL_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_MEASURE_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_MEASURE_TEXT)->UpdateData();
	}
	else
	{
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->UpdateData();
		GetDlgItem(IDC_SRPD_EXTAS_BUTTON)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_EXTAS_BUTTON)->UpdateData();
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->UpdateData();

		if (_digEventStart.GetCheck() == 1)
			OnBnEnableDigStart();

		if (_anaEventStart.GetCheck() == 1)
			OnBnEnableAnaStart();

		if (_tmrEventStart.GetCheck() == 1)
			OnBnEnableTmrStart();
	}

	SetWindowDesign();
}

void CSRWritePropDlg::OnBnEnableDigStart()
{
	_anaEventStart.SetCheck(0);
	_tmrEventStart.SetCheck(0);

	GetDlgItem(IDC_SRPD_START_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_START_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_START_DATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_START_DATE)->UpdateData();
	GetDlgItem(IDC_SRPD_ADD_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_ADD_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_TL_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ES_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->UpdateData();

	// Заполняем все поля
	long maxDigPorts(0);
	long err(0);
	err = ZOpen(_type, _dsp);
	if (err == 0)
	{
		ZGetQuantityChannelDigPort(_type, _dsp, &maxDigPorts);
		ZClose(_type, _dsp);
	}

	_eventStartChanNum.ResetContent();
	for (int i = 0; i < maxDigPorts; i++)
	{
		CString tempString;
		tempString.Format(_T("%d"), i + 1);
		_eventStartChanNum.AddString(tempString);
	}

	_eventStartFront.ResetContent();
	_eventStartFront.AddString(L"0->1");
	_eventStartFront.AddString(L"1->0");

	_eventStartChanNum.SetCurSel(_param.numbit);
	_eventStartFront.SetCurSel(_param.level);

	SetWindowDesign();
}

void CSRWritePropDlg::OnBnEnableAnaStart()
{
	_digEventStart.SetCheck(0);
	_tmrEventStart.SetCheck(0);

	GetDlgItem(IDC_SRPD_START_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_START_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_START_DATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_START_DATE)->UpdateData();
	GetDlgItem(IDC_SRPD_ADD_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_ADD_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ES_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_TL_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->UpdateData();

	// Заполняем все поля
	long numChan = _zetServer.QuanChan();
	bool flagFirst = true;
	long firstChanNum(0);

	_eventStartChanNum.ResetContent();
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->SetWindowText(g_sChannel);

	for (unsigned int i = 0; i < (unsigned int)numChan; i++)
	{
		if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
		{
			if (flagFirst == true)
			{
				firstChanNum = i;
				flagFirst = false;
			}
			_eventStartChanNum.AddString(_zetServer.Commentary(i));
		}
	}

	_eventStartFront.ResetContent();
	_eventStartFront.AddString(g_sHigher);
	_eventStartFront.AddString(g_sLower);

	if (_type == ZET_DEVICE_110)
	{
		_eventStartFront.SetCurSel(_paramdelta.level);
		_eventStartChanNum.SetCurSel(_paramdelta.numbit);
		GetDlgItem(IDC_SRPD_MEASURE_TEXT)->SetWindowTextW(_zetServer.Conversion(firstChanNum + _paramdelta.numbit));

		CString tempString;
		float porog = (_paramdelta.porog / 32767.f) *_zetServer.MaxLevel(firstChanNum + _paramdelta.numbit);
		tempString.Format(_T("%.2f"), porog);
		GetDlgItem(IDC_SRPD_TL_COMBOBOX)->SetWindowText(tempString);
	}
	else
	{
		_eventStartFront.SetCurSel(_param.level);
		_eventStartChanNum.SetCurSel(_param.numbit);
		GetDlgItem(IDC_SRPD_MEASURE_TEXT)->SetWindowTextW(_zetServer.Conversion(firstChanNum + _param.numbit));

		CString tempString;
		long lpor = *(long*)&_param.low_porog;
		float porog = (float)(lpor + 0.5)*(float)GetMinLevel(firstChanNum + _param.numbit);
		tempString.Format(_T("%.2f"), porog);
		GetDlgItem(IDC_SRPD_TL_COMBOBOX)->SetWindowText(tempString);
	}

	SetWindowDesign();
}

void CSRWritePropDlg::OnBnEnableTmrStart()
{
	_digEventStart.SetCheck(0);
	_anaEventStart.SetCheck(0);

	GetDlgItem(IDC_SRPD_START_TIME)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_START_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_START_DATE)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ADD_TIME)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_ADD_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_CLEAR_TIME)->UpdateData();
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->ShowWindow(TRUE);
	GetDlgItem(IDC_SRPD_DAT_START_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_SBNUM_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_SBNUM_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_ES_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_ES_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_TL_TEXT)->UpdateData();
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_TL_COMBOBOX)->UpdateData();
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->ShowWindow(FALSE);
	GetDlgItem(IDC_SRPD_MEASURE_TEXT)->UpdateData();

	SetWindowDesign();

	__time64_t tempTime = _param.StartTime;
	tm         tempTmTime = { 0 };
	SYSTEMTIME tempSysTime = { 0 };
	if (tempTime != 0)
	{
		_localtime64_s(&tempTmTime, &tempTime);
		_mktime64(&tempTmTime);

		tempSysTime.wHour = tempTmTime.tm_hour;
		tempSysTime.wMinute = tempTmTime.tm_min;
		tempSysTime.wSecond = tempTmTime.tm_sec;
		tempSysTime.wYear = tempTmTime.tm_year + 1900;
		tempSysTime.wMonth = tempTmTime.tm_mon + 1;
		tempSysTime.wDay = tempTmTime.tm_mday;

		_dateStart.SetTime(tempSysTime);
		_timeStart.SetTime(tempSysTime);
	}
}

void CSRWritePropDlg::OnBnEnablePrimaryData()
{
	_secondaryData.SetCheck(0);
}

void CSRWritePropDlg::OnBnEnableSecondaryData()
{
	_primaryData.SetCheck(0);
}

void CSRWritePropDlg::OnBnEnableSinGen()
{
	if (_checkSinGenEnable.GetCheck() == false)
	{
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->UpdateData();
	}
	else
	{
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->UpdateData();
	}

	SetWindowDesign();
}

void CSRWritePropDlg::OnBnEnableLevelControl()
{
	if (_checkLevelControl.GetCheck() == false)
	{
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->UpdateData();
	}
	else
	{
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->ShowWindow(TRUE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->UpdateData();

		long numChan = _zetServer.QuanChan();
		long quanChan(0);
		long numFirstChan(0);
		bool firstChan(false);
		_channelControl.ResetContent();
		for (unsigned int i = 0; i < (unsigned int)numChan; i++)
		{
			if (_zetServer.TypeAdc(i) == _type)
				if (_zetServer.NumDSP(i) == _dsp)
				{
					if (firstChan == false)
					{
						numFirstChan = i;
						firstChan = true;
					}
					_channelControl.AddString(_zetServer.Commentary(i));
					quanChan++;
				}
		}

		if (_param.num_ana_port >= quanChan)
			_param.num_ana_port = (short)quanChan - 1;
		if (_param.num_ana_port < 0)
			_param.num_ana_port = 0;

		_channelControl.SetCurSel(_param.num_ana_port);
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->SetWindowText(_zetServer.Conversion(numFirstChan + _param.num_ana_port));

		_channelControlFront.AddString(g_sHigher);
		_channelControlFront.AddString(g_sLower);
		if (_param.front > 1)
			_param.front = 1;
		if (_param.front < 0)
			_param.front = 0;
		_channelControlFront.SetCurSel(_param.front);

		CString tempString;
		tempString.Format(_T("%f"), (float)_param.level_ana * _zetServer.MaxLevel(_param.num_ana_port + firstChan) / 32767);
		_channelControlLevel.SetWindowText(tempString);

		long digOutEnable(0), numBits(0), selNum(0);
		long currDevice = _zetServer.NumModule(firstChan);
		digOutEnable = _zetServer.GetDigOutEnable(currDevice);
		long maxDigPorts(0);
		long err(0);
		err = ZOpen(_type, _dsp);
		if (err == 0)
		{
			ZGetQuantityChannelDigPort(_type, _dsp, &maxDigPorts);
			ZClose(_type, _dsp);
		}
		_numBitControl.ResetContent();
		for (unsigned int i = 0; i < (unsigned int)maxDigPorts; i++)
		{
			if ((digOutEnable & 0x1) != 0)
			{
				if (i == _param.num_dig_port)
					selNum = numBits;
				numBits++;
				CString t;
				t.Format(_T("%u"), i + 1);
				_numBitControl.AddString(t);
			}
			digOutEnable = digOutEnable >> 1;
		}

		if (_param.num_dig_port >= numBits)
			_param.num_dig_port = (short)numBits - 1;
		if (_param.num_dig_port < 0)
			_param.num_dig_port = 0;
		_numBitControl.SetCurSel(selNum);

		_activeLevelControl.ResetContent();
		_activeLevelControl.AddString(L"0 " + g_sUNIT_V + L".");//L"0 В."
		_activeLevelControl.AddString(L"3.3 " + g_sUNIT_V);//L"3.3 В"
		if (_param.level_activ > 1)
			_param.level_activ = 1;
		if (_param.level_activ < 0)
			_param.level_activ = 0;
		_activeLevelControl.SetCurSel(_param.level_activ);
	}

	SetWindowDesign();
}

double CSRWritePropDlg::GetMinLevel(long numChan_)
{
	long numBits(0);
	double realMinLevel(0.0);

	switch (_type)
	{
	case ZET_DEVICE_110:
		numBits = 32;
		break;
	case ZET_DEVICE_220:
		numBits = 32;
		break;
	case ZET_DEVICE_230:
		numBits = 32;
		break;
	case ZET_DEVICE_240:
		numBits = 32;
		break;
	default:
		numBits = 16;
		break;
	}

	realMinLevel = (double)_zetServer.MaxLevel(numChan_)*2.0 / pow(double(2), numBits);
	return realMinLevel;
}

void CSRWritePropDlg::SetWindowDesignNew(bool option[SIZE_BOOL_B_OPTIONS])
{
	if (option[0] == true)
	{
		GetDlgItem(IDC_SRPD_START_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_START_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_START_DATE)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_START_DATE)->UpdateData();
		GetDlgItem(IDC_SRPD_ADD_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_ADD_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_CLEAR_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_CLEAR_TIME)->UpdateData();
		GetDlgItem(IDC_SRPD_DAT_START_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_DAT_START_TEXT)->UpdateData();
	}

	if (option[1] == true)
	{
		GetDlgItem(IDC_SRPD_WAP_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_WAP_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_WP_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_WP_ETEXT)->SetWindowTextW(L"");
		GetDlgItem(IDC_SRPD_WP_ETEXT)->UpdateData();
	}

	if (option[2] == true)
	{
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_EXTDS_BUTTON))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_EXTDS_BUTTON)->UpdateData();
	}

	if (option[3] == true)
	{
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_EXTTS_BUTTON))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_EXTTS_BUTTON)->UpdateData();
	}

	if (option[4] == true)
	{
		GetDlgItem(IDC_SRPD_WRP_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_WRP_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_PRIMARY_RADIO)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_PRIMARY_RADIO))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_PRIMARY_RADIO)->UpdateData();
		GetDlgItem(IDC_SRPD_SECONDARY_RADIO)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_SECONDARY_RADIO))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_SECONDARY_RADIO)->UpdateData();
	}

	if (option[5] == true)
	{
		GetDlgItem(IDC_SRPD_AGSYNC_CHECK)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_AGSYNC_CHECK))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_AGSYNC_CHECK)->UpdateData();
	}

	if (option[6] == true)
	{
		GetDlgItem(IDC_SRPD_SIG_GEN_GROUP)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIG_GEN_GROUP)->UpdateData();
		GetDlgItem(IDC_SRPD_ENA_SIN_GEN_CHECK)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_ENA_SIN_GEN_CHECK))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_ENA_SIN_GEN_CHECK)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->SetWindowTextW(L"");
		GetDlgItem(IDC_SRPD_SIN_GEN_FREQ_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->SetWindowTextW(L"");
		GetDlgItem(IDC_SRPD_SIN_GEN_RMS_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->SetWindowTextW(L"");
		GetDlgItem(IDC_SRPD_SIN_GEN_OFFSET_ETEXT)->UpdateData();
	}

	if (option[7] == true)
	{
		GetDlgItem(IDC_SRPD_GEN_EXCESS_LEVEL_GROUP)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_EXCESS_LEVEL_GROUP)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_LEVEL_CONTROL_CHECK)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SRPD_GEN_LEVEL_CONTROL_CHECK))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SRPD_GEN_LEVEL_CONTROL_CHECK)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_CHAN_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_UPAD_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->SetWindowTextW(L"");
		GetDlgItem(IDC_SRPD_GEN_CONTROL_TL_ETEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_TRESHOLD_MEASURE_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_NUM_BIT_COMBOBOX)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_TEXT)->UpdateData();
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_GEN_ACTIVE_LEVEL_COMBOBOX)->UpdateData();
	}

	if (option[8] == true)
	{
		GetDlgItem(IDC_SRPD_SETTINGS_DATA_BUTTON)->ShowWindow(FALSE);
		GetDlgItem(IDC_SRPD_SETTINGS_DATA_BUTTON)->UpdateData();
	}
}

void CSRWritePropDlg::OnBnSaveParam()
{
	bool bRes = true;
	if (_type == ZET_DEVICE_110)
	{
		SaveDeltaRegParameters();
		SetDeviceParamExt();
	}
	else
	{
		bRes = SaveRegParameters();
		SetDeviceParam();
	}

	if (bRes)
		EndDialog(NULL);
}

void CSRWritePropDlg::OnBnCancel()
{
	EndDialog(NULL);
}

bool CSRWritePropDlg::SaveRegParameters()
{
	float writeTime;
	CString tempString;

	SetDefaultParams();

	if ((_type != ZET_DEVICE_ZET048) && (_type != ZET_DEVICE_240))
		_param.ModeSD = 0;

	_writeTime.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &writeTime);
	if (_writeTimeType.GetCurSel() == 1)
		writeTime = writeTime * 60;
	else
		if (_writeTimeType.GetCurSel() == 2)
			writeTime = writeTime * 3600;


	// Проверка на минимальное значение времени записи
	if (writeTime < 1)
	{//Время записи не может быть меньше одной секунды. Установлено время записи в 1 секунду. Некорректное время записи.	
		MessageBox(g_sRecordingTimeNotLess1s + L". " + g_sSetRecordingTime1s + L". ", g_sIncorrectRecordingTime + L".", MB_ICONWARNING);
		writeTime = 1.f;
	}

	float MaxTime = 0;

	if ((_type == ZET_DEVICE_A17U8) || (_type == ZET_DEVICE_A17U2))
	{
		_param.numbit = NUMBIT_017;
		_param.level = 1; // при переходе 1->0
		_multiEventsMode.SetCheck(_checkEventStart.GetCheck());
	}
	bool bShowError = false;
	// Проверка на максимальное значение времени записи
	if (writeTime > _maxSDWriteTime)
	{
		bShowError = true;

		CString TimeDuration = CString(zetlab::utils::time_duration_to_string(_maxSDWriteTime).c_str());

		CString TimeDurationStr;
		TimeDurationStr.Format(g_sSetTimeRecord + L" %s?", TimeDuration.GetString());//L"Установить время записи %s?"
		//L"Максимальная длительность записи при заданных параметрах устройства составляет"
		if (MessageBox(g_sMaximumDurationEntry + L": " +
			TimeDuration + L". " + g_sIncreaseTotalRecordingTime + L". " +
			//L"Некорректное время записи"
			TimeDurationStr, g_sIncorrectRecordingTime, MB_OKCANCEL | MB_ICONWARNING) == IDOK)
		{
			writeTime = _maxSDWriteTime;
		}
		else
			return false;
	}

	if (!bShowError)
	{
		int CountRecords = int(_maxSDWriteTime / writeTime);
		if (CountRecords < 100)
			//L"Максимальное количество записей при заданных настройках: "
			MessageBox(g_sMaximumNumberRecordsSettings + L": " + (CString)std::to_wstring(CountRecords).c_str(), L"", MB_ICONINFORMATION);
	}

	*(long*)(&_param.low_timereg) = (long)writeTime;
	if (_type != ZET_DEVICE_240)
		_param.timereg = (short)writeTime;

	_param.enazap = _checkEventStart.GetCheck();

	if (_type != ZET_DEVICE_A17U8 && _type != ZET_DEVICE_A17U2)
	{
		if (_digEventStart.GetCheck() == 1)
			_param.digana = 0;
		else
			if (_anaEventStart.GetCheck() == 1)
				_param.digana = 1;
			else
				if (_tmrEventStart.GetCheck() == 1)
					_param.digana = 2;

		_param.numbit = _eventStartChanNum.GetCurSel();

		_param.level = _eventStartFront.GetCurSel();
	}

	SYSTEMTIME selectedDate = { 0 };
	SYSTEMTIME selectedTime = { 0 };
	tm         tmSelectedTime;
	__time64_t temptime;
	_dateStart.GetTime(&selectedDate);
	_timeStart.GetTime(&selectedTime);
	selectedDate.wHour = selectedTime.wHour;
	selectedDate.wMinute = selectedTime.wMinute;
	selectedDate.wSecond = selectedTime.wSecond;

	tmSelectedTime.tm_year = selectedDate.wYear - 1900;
	tmSelectedTime.tm_mon = selectedDate.wMonth - 1;
	tmSelectedTime.tm_mday = selectedDate.wDay;
	tmSelectedTime.tm_hour = selectedDate.wHour;
	tmSelectedTime.tm_min = selectedDate.wMinute;
	tmSelectedTime.tm_sec = selectedDate.wSecond;

	temptime = _mktime64(&tmSelectedTime);

	if ((temptime <= _time64(NULL)) && (_tmrEventStart.GetCheck() == 1))
		MessageBox(g_sWrongStartTimeSet, g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);

	_param.StartTime = (ULONG64)temptime;

	long porog(0);
	long chanOffset(0);

	for (unsigned int i = 0; i < (unsigned int)_zetServer.QuanChan(); i++)
	{
		if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
		{
			chanOffset = i;
			break;
		}
	}
	_eventStartTreshold.GetWindowText(tempString);
	swscanf_s(tempString, _T("%d"), &porog);
	if (porog >= _zetServer.MaxLevel(_param.numbit + chanOffset))
		porog = (long)((float)_zetServer.MaxLevel(_param.numbit + chanOffset)*(float)0.99);
	if (porog <= -_zetServer.MaxLevel(_param.numbit + chanOffset))
		porog = -(long)((float)_zetServer.MaxLevel(_param.numbit + chanOffset)*(float)0.99);
	*(long*)&_param.low_porog = (long)(porog / GetMinLevel(_param.numbit + chanOffset));

	if (_multiEventsMode.GetCheck() == 1)
		_param.MultiThreshold = 1;
	else
		_param.MultiThreshold = 0;

	/*
	if ( _genGPSSynMode.GetCheck() == 1 )
		_param.precision = 1;
	else
	Проверка GPS*/

	_param.precision = -1;

	float tempVal;
	_sinGenFreq.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &tempVal);
	_param.AGSygnalParam1 = tempVal;
	_sinGenLevel.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &tempVal);
	_param.AGSygnalParam2 = tempVal;
	_sinGenOffset.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &tempVal);
	_param.AGSygnalParam3 = tempVal;
	_param.AGSygnalType = 0;	// sin

	_param.enaAutoGen = (short)(_checkSinGenEnable.GetCheck());
	if (_param.enaAutoGen == 1) // Необходимо записать сигнал синуса в DAC.DAT
		writeTempDACFile();

	_param.enagetlevel = _checkLevelControl.GetCheck();
	if (_param.enagetlevel)
	{
		long currBit = _numBitControl.GetCurSel();
		long currDig(0);
		long savePort(0);
		long currDevice = _zetServer.NumModule(chanOffset);
		long digOutEnable = _zetServer.GetDigOutEnable(currDevice);
		long maxDigPorts(0);
		long err(0);
		err = ZOpen(_type, _dsp);
		if (err == 0)
		{
			ZGetQuantityChannelDigPort(_type, _dsp, &maxDigPorts);
		}
		ZClose(_type, _dsp);

		for (unsigned int i = 0; i < (unsigned int)maxDigPorts; i++)
		{
			if ((digOutEnable & 0x1) != 0)
			{
				if (currDig == currBit)
					savePort = i;
				currDig++;
			}
			digOutEnable = digOutEnable >> 1;
		}

		_param.num_dig_port = (short)savePort;
		_param.num_ana_port = _channelControl.GetCurSel();
		_param.front = _channelControlFront.GetCurSel();
		CString t;
		_channelControlLevel.GetWindowText(t);
		float porog;
		swscanf_s(t, _T("%f"), &porog);
		float maxL = _zetServer.MaxLevel(_param.num_ana_port + chanOffset);
		_param.level_ana = (short)(porog / _zetServer.MaxLevel(_param.num_ana_port + chanOffset) * 32767);
		_param.level_activ = _activeLevelControl.GetCurSel();
	}

	return true;
}

void CSRWritePropDlg::SaveDeltaRegParameters()
{

	strcpy_s(_paramdelta.Signature, "Param");
	_paramdelta.contrast = 50;
	_paramdelta.timereg = 10;
	_paramdelta.porog = 10;
	_paramdelta.typecalc = 1;
	_paramdelta.fast = 0;
	_paramdelta.write = 0;
	_paramdelta.enazap = 0;
	_paramdelta.digana = 0;
	_paramdelta.numbit = 0;
	_paramdelta.level = 0;
	_paramdelta.dBON = 0;
	_paramdelta.ModeRec = 0;
	_paramdelta.FDiap = 0;
	_paramdelta.FltrNM = 0;
	_paramdelta.CVel = 0;
	_paramdelta.MaxTimeWrite = 0;
	_paramdelta.CurrCal[0] = 0;
	_paramdelta.CurrCal[1] = 0;
	_paramdelta.CurrCal[2] = 0;
	_paramdelta.currTypeDevice = 2;
	_paramdelta.TestCode[0] = 0x1234;
	_paramdelta.TestCode[1] = 0x5678;
	_paramdelta.TestCode[2] = -25924;		// == 0x9abc;
	_paramdelta.low_timereg = 0;
	_paramdelta.high_timereg = 0;
	_paramdelta.preDelay = 0;


	float tempFloatVal(0.f);
	long  tempLongVal(0);
	CString tempString;
	_writeTime.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &tempFloatVal);
	if (_writeTimeType.GetCurSel() == 1)
		tempFloatVal = tempFloatVal * 60;
	else
		if (_writeTimeType.GetCurSel() == 2)
			tempFloatVal = tempFloatVal * 3600;

	// Проверка на минимальное значение времени записи
	if (tempFloatVal < 0)
	{
		MessageBox(g_sEnteredNegativeRecordTime, g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);
		tempFloatVal = 0;
	}
	// Проверка на максимальное значение времени записи
	if (tempFloatVal > _maxSDWriteTime)
	{
		tempFloatVal = _maxSDWriteTime;
		float tempVal(0.f);
		CString localTempString;

		if (_writeTimeType.GetCurSel() == 1)
			tempVal = tempFloatVal / 60;
		else
			if (_writeTimeType.GetCurSel() == 2)
				tempVal = tempFloatVal / 3600;

		localTempString.Format(_T("%f"), tempVal);
		MessageBox(g_sEnteredTooMuchRecordTime, g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);
	}

	*(long*)(&_paramdelta.low_timereg) = (long)tempFloatVal;

	_writeDelay.GetWindowText(tempString);
	swscanf_s(tempString, _T("%d"), &tempLongVal);
	_paramdelta.preDelay = (short)tempLongVal;

	if ((_anaEventStart.GetCheck() == 1) && (_checkEventStart.GetCheck() == 1))
	{
		_paramdelta.digana = 1;
		_paramdelta.enazap = 1;
	}
	else
	{
		_paramdelta.digana = 0;
		_paramdelta.enazap = 0;
	}

	_paramdelta.numbit = _eventStartChanNum.GetCurSel();
	_paramdelta.level = _eventStartFront.GetCurSel();

	_eventStartTreshold.GetWindowText(tempString);
	swscanf_s(tempString, _T("%f"), &tempFloatVal);

	long chanOffset(0);
	for (unsigned int i = 0; i < (unsigned int)_zetServer.QuanChan(); i++)
		if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
		{
			chanOffset = i;
			break;
		}
	_paramdelta.porog = (short)(tempFloatVal*32767.f / _zetServer.MaxLevel(chanOffset + _paramdelta.numbit));

	if (_primaryData.GetCheck() == 1)
		_paramdelta.ModeRec = 0;

	if (_secondaryData.GetCheck() == 1)
		_paramdelta.ModeRec = 1;
}

void CSRWritePropDlg::OnBnSettingsData()
{
	long waitCopyDataMode(0);
	long splitFilesMode(0);
	long replaceOldFilesMode(0);
	long continuousRecordMode(0);

	waitCopyDataMode = 0;
	if ((_param.ModeSD & 1) == 1)
		waitCopyDataMode = 1;
	if (((_param.ModeSD >> 1) & 1) == 1)
		waitCopyDataMode = 2;

	splitFilesMode = 0;
	if (((_param.ModeSD >> 8) & 1) == 1)
		splitFilesMode = 1;
	if (((_param.ModeSD >> 11) & 1) == 1)
		splitFilesMode = 2;

	replaceOldFilesMode = (_param.ModeSD >> 9) & 1;
	continuousRecordMode = (_param.ModeSD >> 10) & 1;

	CSRDataPropDlg dataPropDlg(waitCopyDataMode, splitFilesMode, replaceOldFilesMode, continuousRecordMode);

	if (dataPropDlg.DoModal() == IDC_SRSDD_OK)
	{
		switch (dataPropDlg._waitCopyDataMode)
		{
		case 0:
			_param.ModeSD &= ~(0x1);
			_param.ModeSD &= ~(0x2);
			break;
		case 1:
			_param.ModeSD &= ~(0x1);
			_param.ModeSD &= ~(0x2);
			_param.ModeSD |= 0x1;
			break;
		case 2:
			_param.ModeSD &= ~(0x1);
			_param.ModeSD &= ~(0x2);
			_param.ModeSD |= 0x2;
			break;
		default:
			break;
		}

		switch (dataPropDlg._splitFilesMode)
		{
		case 0:
			_param.ModeSD &= ~((short)0x1 << 8);
			_param.ModeSD &= ~((short)0x1 << 11);
			break;
		case 1:
			_param.ModeSD &= ~((short)0x1 << 8);
			_param.ModeSD &= ~((short)0x1 << 11);
			_param.ModeSD |= ((short)0x1 << 8);
			break;
		case 2:
			_param.ModeSD &= ~((short)0x1 << 8);
			_param.ModeSD &= ~((short)0x1 << 11);
			_param.ModeSD |= ((short)0x1 << 11);
			break;
		default:
			break;
		}

		_param.ModeSD &= ~((short)0x1 << 9);
		if (dataPropDlg._replaceOldFilesMode == 1)
			_param.ModeSD |= ((short)0x1 << 9);

		_param.ModeSD &= ~((short)0x1 << 10);
		if (dataPropDlg._continuousRecordMode == 1)
			_param.ModeSD |= ((short)0x1 << 10);
	}
}

void CSRWritePropDlg::OnBnAddTime()
{
	CString tempString = L"";
	_SYSTEMTIME sysTime = { 0 }, sysDate = { 0 };
	_dateStart.GetTime(&sysDate);
	_timeStart.GetTime(&sysTime);

	tempString.Format(_T("%02d.%02d.%04d - %02d:%02d:%02d."), sysDate.wDay, sysDate.wMonth, sysDate.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	tempString = g_sAddedStartTimeRegistration + L": " + tempString;
	_listChanSettings.AddString(tempString);
	_listChanSettings.SetCurSel(_listChanSettings.GetCount() - 1);
}

void CSRWritePropDlg::OnBnClearTime()
{
	CString tempString1, tempString2, tempString3;

	tempString1 = g_sAddedStartTimeRegistration;
	tempString2 = g_sTimeTriggeringListCleared;

	for (unsigned int i = 0, i_end = _listChanSettings.GetCount(); i < i_end; i++)
	{
		_listChanSettings.GetText(i, tempString3);

		if ((tempString3.Find(tempString1) != -1) || (tempString3.Find(tempString2) != -1))
		{
			_listChanSettings.DeleteString(i);
			i--;
			i_end--;
		}
	}

	_listChanSettings.AddString(tempString2);
	_listChanSettings.SetCurSel(_listChanSettings.GetCount() - 1);
}

void CSRWritePropDlg::writeTempDACFile()
{
	FILE* out(nullptr);
	long err(0);
	double digRes(0), freqDac(0);
	long numBits(0);
	long size(0);
	short dataDac(0);
	long  dataDac2(0);

	err = ZOpen(_type, _dsp);
	if (err == 0)
	{
		ZGetDigitalResolChanDAC(_type, _dsp, 0, &digRes);
		if (digRes <= 0)
			ZGetDigitalResolutionDAC(_type, _dsp, &digRes);

		digRes = digRes * 1000;	 // в мВ

		ZGetFreqDAC(_type, _dsp, &freqDac);
		ZGetBitsDAC(_type, _dsp, &numBits);
		ZClose(_type, _dsp);
	}

	//fopen_s( &out, CW2A(_dacFileParam), "wb" );
	//if ( out != NULL )
	if (zfOpenFile(_dacFileParam, L"wb", &out) == 0)
	{
		if (_param.AGSygnalParam1 < 0)
			_param.AGSygnalParam1 = 0;

		if (_param.AGSygnalParam2 < 0)
			_param.AGSygnalParam2 = 0;

		if (numBits <= 16)
		{
			if (_param.AGSygnalParam2 > digRes * 20000)
				_param.AGSygnalParam2 = (float)digRes * (float)20000.;

			if (_param.AGSygnalParam3 < -digRes * 30000)
				_param.AGSygnalParam3 = -(float)digRes * (float)30000.;

			if (_param.AGSygnalParam3 > digRes * 30000)
				_param.AGSygnalParam3 = (float)digRes * (float)30000.;
		}
		else
		{
			if (_param.AGSygnalParam2 > digRes * 20000 * 32768)
				_param.AGSygnalParam2 = (float)digRes * (float)20000 * (float)32768;

			if (_param.AGSygnalParam3 < -digRes * 30000 * 32768)
				_param.AGSygnalParam3 = -(float)digRes * (float)30000 * (float)32768;

			if (_param.AGSygnalParam3 > digRes * 30000 * 32768)
				_param.AGSygnalParam3 = (float)digRes * (float)30000 * (float)32768;
		}

		if (_param.AGSygnalParam1 > freqDac / 4)
			_param.AGSygnalParam1 = (float)freqDac / (float)4;

		if (_param.AGSygnalParam1 < freqDac / 6000)
			_param.AGSygnalParam1 = (float)freqDac / (float)6000;

		size = static_cast<long>(freqDac / _param.AGSygnalParam1);

		if (size < 4)
			size = 4;

		if (numBits <= 16)
		{
			if (size > 6000)
				size = 6000;

			for (long i = 0; i < size; i++)
			{
				dataDac = (short)((_param.AGSygnalParam3 + _param.AGSygnalParam2*sqrt(2.)*sin(2.*M_PI*(double)i / (double)size)) / digRes);
				fwrite(&dataDac, 1, sizeof(short), out);
			}
		}
		else
		{
			if (size > 30000)
				size = 30000;

			for (long i = 0; i < size; i++)
			{
				dataDac2 = (long)((_param.AGSygnalParam3 + _param.AGSygnalParam2*sqrt(2.)*sin(2.*M_PI*(double)i / (double)size)) / digRes);
				fwrite(&dataDac2, 1, sizeof(long), out);
			}
		}

		zfCloseFile(out);
	}
}

void CSRWritePropDlg::SetDefaultParams()
{
	_param.sizeSelf = sizeof(_param);
	if (_type != ZET_DEVICE_210)
	{
		_param.timereg = 10;
		_param.porog = 10;
		_param.typecalc = 1;
		_param.ICP = 0;
		_param.fast = 0;
		_param.write = 0;
		_param.enazap = 0;
		_param.digana = 0;
		_param.numbit = 0;
		_param.level = 0;
		_param.enagetlevel = 0;
		_param.num_dig_port = 0;
		_param.num_ana_port = 0;
		_param.front = 0;
		_param.level_ana = 0;
		_param.level_activ = 0;
		_param.low_porog = 0;
		_param.high_porog = 0;
		_param.low_timereg = 10;
		_param.high_timereg = 0;
		_param.enaAutoGen = 0;
		_param.AGSygnalType = 0;
		_param.AGSygnalParam1 = 0;
		_param.AGSygnalParam2 = 0;
		_param.AGSygnalParam3 = 0;
		_param.AGSygnalParam4 = 0;
		_param.StartTime = 0;
		_param.precision = -1;
		//_param.ModeSD = 0;
		_param.MultiThreshold = 0;
	}
	else
	{
		_param.timereg = 10;
		_param.porog = 0;
		_param.typecalc = 0;
		_param.ICP = 0;
		_param.fast = 0;
		_param.write = 0;
		_param.enazap = 0;
		_param.digana = 0;
		_param.numbit = 0;
		_param.level = 0;
		_param.enagetlevel = 0;
		_param.num_dig_port = 0;
		_param.num_ana_port = 0;
		_param.front = 0;
		_param.level_ana = 0;
		_param.level_activ = 0;
		_param.low_porog = 0;
		_param.high_porog = 0;
		_param.low_timereg = 10;
		_param.high_timereg = 0;
		_param.enaAutoGen = 0;
		_param.AGSygnalType = 0;
		_param.AGSygnalParam1 = 0;
		_param.AGSygnalParam2 = 0;
		_param.AGSygnalParam3 = 0;
		_param.AGSygnalParam4 = 0;
		_param.StartTime = 0;
		_param.precision = -1;
		//_param.ModeSD = 0;
		_param.MultiThreshold = 0;

	}
}

bool CSRWritePropDlg::GetMaxbitrate()
{
	if (NULL == _zetServer.m_hWnd || _type != 14)
		return false;
	long chanOffset(0);
	long channelOnDevice(0);
	for (unsigned int i = 0; i < (unsigned int)_zetServer.QuanChan(); i++)
	{
		if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
		{
			chanOffset = i;
			channelOnDevice++;
		}
	}

	long retvers(0);
	long version(0);
	long currDevice = _zetServer.NumModule(chanOffset);
	long ret = _zetServer.GetDeviceLong(currDevice, ZADC_LONG_VERSION, &version);
	if (ret == 0)
	{
		if (version != 0)
			retvers = version >> 16;
	}
	else
		return false;

	if (retvers == 7)
	{
		if (_zetServer.Frequency(chanOffset) >= 50000)
		{
			if (isHaveBuiltinGeneratorWithCheck(currDevice))
			{
				// Если генератор врублен, и еще 7 или 8 других каналов 
				if (channelOnDevice > 7)
					return true;
				else 
				    return false;
			}
		}
		else
			return false;
	}
	else
		return false;

	return false;
}

bool CSRWritePropDlg::isHaveBuiltinGeneratorWithCheck(long module)
{
	if (NULL == _zetServer.m_hWnd)
		return false;

	bool retVal = false;
	if (_zetServer.IsHaveSineBuiltinGen(module) == 1)
	{
		for (long i = 0; i < (long)_zetServer.QuanChan(); i++)
		{
			if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
			{
				if (_zetServer.IsFeedBackChan(i) == 1)
				{
					long err(0);
					err = ZOpen(_type, _dsp);
					if (err == 0)
					{
						long Enable(0);
						if (ZGetStartDAC(_type, _dsp, &Enable) == 0)
							retVal = Enable == 0;
						ZClose(_type, _dsp);
					}
				}
			}
		}
	}

	return retVal;
}

bool CSRWritePropDlg::GetDevicelong()
{
	if (NULL == _zetServer.m_hWnd || _type != 14)
		return false;

	bool retvers(false);
	long chanOffset(0);
	for (unsigned int i = 0; i < (unsigned int)_zetServer.QuanChan(); i++)
	{
		if ((_zetServer.TypeAdc(i) == _type) && (_zetServer.NumDSP(i) == _dsp))
		{
			chanOffset = i;
		}
	}

	long version(0);
	long currDevice = _zetServer.NumModule(chanOffset);
	long ret = _zetServer.GetDeviceLong(currDevice, ZADC_LONG_VERSION, &version);
	long m_lversion(0);
	if (ret == 0)
	{
		if (version != 0)
			m_lversion = version >> 16;
	}

	if (m_lversion == 7)
		retvers = true;
		
	return retvers;
}