#include "stdafx.h"
#include "SRChoiseProfDlg.h"
#include "Localized/Localized.h"

#include "../../../../algo/path/path.h"

#include <mathZet.h>
#include "resource\Phrases.h"

IMPLEMENT_DYNAMIC(CSRChoiseProfDlg, CDialog)

CSRChoiseProfDlg::CSRChoiseProfDlg( long type_, long dsp_, CWnd* pParent /*=NULL*/ )
	: CDialog(CSRChoiseProfDlg::IDD, pParent)
{
	_type = type_;
	_dsp = dsp_;
}

CSRChoiseProfDlg::~CSRChoiseProfDlg()
{

}

void CSRChoiseProfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SRSPD_SENSORS_LIST, _profilesList);
}


BEGIN_MESSAGE_MAP(CSRChoiseProfDlg, CDialog)
	ON_BN_CLICKED(IDC_SRSPD_SAVE_SETTINGS_BUTTON, OnBnSave)
	ON_BN_CLICKED(IDC_SRSPD_EXIT_BUTTON, OnBnCancel)
	ON_NOTIFY(NM_RCLICK, IDC_SRSPD_SENSORS_LIST, OnRBMouseDown)
	ON_COMMAND_EX_RANGE(ID_CONTEXT_MENU_SP0, ID_CONTEXT_MENU_SP0 + 1000, OnContextMenuClick)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SRSPD_SENSORS_LIST, OnCustomDraw)
END_MESSAGE_MAP()



BOOL CSRChoiseProfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DECLARE_LOC_SUPPORT()

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif

	SetWindowTextW(sDebug + g_sListParameters);
	GetDlgItem(IDC_SRSPD_SENSORS_LIST_TEXT)->SetWindowTextW(g_sListProfileSensors);
	GetDlgItem(IDC_SRSPD_SAVE_SETTINGS_BUTTON)->SetWindowTextW(g_sSave);
	GetDlgItem(IDC_SRSPD_EXIT_BUTTON)->SetWindowTextW(g_sCancel);

	_profilesList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	
// Добавляем столбцы
	LVCOLUMNW tempColumn;
	CString   tempResString;
	memset( &tempColumn, 0x00, sizeof(LVCOLUMNW) );
	tempColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	tempColumn.fmt = LVCFMT_CENTER;
	tempResString = g_sNumber;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 35;
	_profilesList.InsertColumn(0, &tempColumn);

	tempResString = g_sSRSPDSensitivity;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 70;	
	_profilesList.InsertColumn(1, &tempColumn);

	tempResString = g_sSRSPDUnity;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 70;	
	_profilesList.InsertColumn(2, &tempColumn);

	tempResString = g_sSRSPDAmpCoef;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 70;	
	_profilesList.InsertColumn(3, &tempColumn);

	tempResString = g_sSRSDPreamplifier;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 130;	
	_profilesList.InsertColumn(4, &tempColumn);

	tempResString = g_sUNIT_dB;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 80;	
	_profilesList.InsertColumn(5, &tempColumn);

	tempResString = g_sSRSPDFreqResp;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 50;	
	_profilesList.InsertColumn(6, &tempColumn);

	tempResString = g_sOffsetZero;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 100;	
	_profilesList.InsertColumn(7, &tempColumn);

	tempResString = g_sName;
	tempColumn.pszText = (WCHAR*)tempResString.GetString();
	tempColumn.cx = 129;	
	_profilesList.InsertColumn(8, &tempColumn);

// Добавляем группы
	LVGROUP tempGroup;
	CString tempString;
	memset(&tempGroup, 0x00, sizeof(LVGROUP));

	tempGroup.cbSize = sizeof(LVGROUP);
	tempGroup.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_ALIGN;
	tempGroup.iGroupId = 0;
	tempGroup.uAlign = LVGA_HEADER_CENTER;
	tempString = g_sSRSPDAudioNoiseMeter;
	tempGroup.pszHeader= tempString.GetBuffer();
	tempGroup.cchHeader= tempString.GetLength();
	_profilesList.InsertGroup(0, &tempGroup);

	tempGroup.iGroupId = 1;
	tempString = g_sVibrometer;
	tempGroup.pszHeader= tempString.GetBuffer();
	tempGroup.cchHeader= tempString.GetLength();
	_profilesList.InsertGroup(1, &tempGroup);

	tempGroup.iGroupId = 2;
	tempString = g_sRegistrator;
	tempGroup.pszHeader= tempString.GetBuffer();
	tempGroup.cchHeader= tempString.GetLength();
	_profilesList.InsertGroup(2, &tempGroup);

	// Добавление "болванок" датчиков
	for ( unsigned int i = 0; i < 3; i++ )
	{
		for ( unsigned int j = 0; j < 5; j++ )
		{
			CString tempString;
			CString tempLocResString;

			long itemNum = _profilesList.InsertItem(i*5 + j, L"");

			tempString.Format(_T("%d"), j+1);
			_profilesList.SetItemText(i*5 + j, 0, tempString);
			_profilesList.SetItemText(i*5 + j, 8, tempString);
			for ( unsigned int m = 1; m < 8; m++ )
				_profilesList.SetItemText(i*5 + j, m, L"---");
			tempLocResString = g_sNumberSensor;
			tempString.Format(_T("%d"), j+1);
			tempLocResString = tempLocResString + L" " + tempString;
			_profilesList.SetItemText(i*5 + j, 8, tempString);

			LVITEM lvi;
			memset( &lvi, 0x00, sizeof(lvi) );

			lvi.iItem = itemNum;
			lvi.mask = LVIF_GROUPID;
			lvi.iGroupId = i;
			_profilesList.SetItem(&lvi);
		}
	}

	_profilesList.EnableGroupView(true);
	SetWindowTheme( _profilesList.m_hWnd, L"explorer", 0);

	SetProfilesInfo();

	path path_;

	path_.Create( this );
	CString s = _T("DirConfig");
	BSTR bstrDirConfig = s.AllocSysString();
	SysFreeString(bstrDirConfig);
	CString	sensorsConfig = path_.ZetPath( &bstrDirConfig ) + _T("sensors.cfg");

	_sensors.load( (LPCTSTR)sensorsConfig );

	GetDeviceParamExt();

	return TRUE;
}

void CSRChoiseProfDlg::OnBnSave()
{
	CString tempString;
	CALIBR_ZET110 sensorCalibr;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	for ( unsigned int dev = 0; dev < 3; dev++ )
	{
		for ( unsigned int sensor = 0; sensor < 5; sensor++ )
		{
			sensorCalibr.sizeSelf = sizeof( CALIBR );
			sensorCalibr.TestCode[0] = (short)0x1234;
			sensorCalibr.TestCode[1] = (short)0x5678;
			sensorCalibr.TestCode[2] = (short)0x9abc;

			tempString = _profilesList.GetItemText( dev*5 + sensor, 1 );
			swscanf_s( tempString, _T("%f"), &sensorCalibr.sense );

			tempString = _profilesList.GetItemText( dev*5 + sensor, 2 );
			memset( sensorCalibr.conv, 0x00, 16 );
			memcpy_s( sensorCalibr.conv, CStringA(tempString).GetLength(), CStringA(tempString).GetBuffer(), CStringA(tempString).GetLength() );

			tempString = _profilesList.GetItemText( dev*5 + sensor, 3 );
			swscanf_s( tempString, _T("%f"), &sensorCalibr.ExtAmp );

			tempString = _profilesList.GetItemText( dev*5 + sensor, 4 );
			int ICP = 0;
			swscanf_s( tempString, _T("%d"), &ICP );
			if ( ( ICP == 2 ) || ( ICP == 1 ) )
				sensorCalibr.ICP = 1;
			else
				sensorCalibr.ICP = 0;

			tempString = _profilesList.GetItemText( dev*5 + sensor, 5 );
			swscanf_s(tempString, _T("%f"), &sensorCalibr.refer);

			tempString = _profilesList.GetItemText( dev*5 + sensor, 7 );
			swscanf_s(tempString, _T("%f"), &sensorCalibr.DC);

			tempString = _profilesList.GetItemText( dev*5 + sensor, 8 );
			memset( sensorCalibr.name, 0x00, 128 );
			memcpy_s(&sensorCalibr.name, CStringA(tempString).GetLength(), CStringA(tempString).GetBuffer(), CStringA(tempString).GetLength() );

			tempString = CString("CALIBR");
			memset( sensorCalibr.Signature, 0x00, 16 );
			memcpy_s( &sensorCalibr.Signature, CStringA(tempString).GetLength(), CStringA(tempString.GetBuffer()), CStringA(tempString).GetLength() );

			long err = SetSensor( dev, sensor, &sensorCalibr );
			if ( err != 0 )
				MessageBox(L"Fatal error !!!", g_sRecordingErrorOccurred, MB_OKCANCEL | MB_ICONERROR);

			Sleep( 10 );
		}
	}

	SetDeviceParamExt();

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	EndDialog( 0 );
}

void CSRChoiseProfDlg::OnBnCancel()
{
	EndDialog( 0 );
}

void CSRChoiseProfDlg::OnRBMouseDown( NMHDR* pNMHDR, LRESULT* pResult )
{
	CMenu m_Menu;
	POINT current_point;
	CString tempString;
	GetCursorPos(&current_point);

	if ( _profilesList.GetSelectedCount() == 1 )
	{
		m_Menu.CreatePopupMenu();	

		tempString = g_sTypeActivate;
		m_Menu.AppendMenu(MF_STRING, ID_CONTEXT_MENU_SP0, tempString);
		tempString = g_sUseType;
		m_Menu.AppendMenu(MF_STRING, ID_CONTEXT_MENU_SP0+1, tempString);

		m_Menu.AppendMenu( MF_SEPARATOR );

		for ( unsigned int i = 0; i < _sensors.get().size(); i++ )
		{
			tempString = _sensors.get()[i]->name.c_str();
			m_Menu.AppendMenu(MF_STRING, ID_CONTEXT_MENU_SP0+2+i, tempString);
		}

		m_Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
			current_point.x,
			current_point.y,
			this);

		m_Menu.DestroyMenu();
	}
}

long CSRChoiseProfDlg::GetSensor( unsigned short dev_, unsigned short sensor_, PCALIBR_ZET110 calibr_ )
{
	long err(0);
	long returnVal(-1);
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


	if ( err == 0 )
	{
		Sleep(5);

		COMMAND_INFO_USB command;
		long sizeSend, sizeRec;

		if ( err == 0 )
		{
			Sleep(5);
			memset(&command, 0x00, sizeof(command));
			command.Command = 0x139;
			command.Data[0] = dev_;
			command.Data[1] = sensor_;

			sizeSend = sizeof(command);
			err = ZSendCommand(_type, _dsp, &command, &sizeSend);
			if ( err == 0 )
			{
				Sleep(5);
				sizeRec = sizeof(command);
				err = ZReceiveCommand(_type, _dsp, &command, &sizeRec);
				if ( err == 0 )
				{
					memcpy_s(calibr_, sizeof(*calibr_), command.Data, sizeof(*calibr_));
					returnVal = 0;
				}
			}
		}
		Sleep(5);
		err = ZExclusive1Close(_type, _dsp);
	}
	
	return returnVal;
}

long CSRChoiseProfDlg::SetSensor( unsigned short dev_, unsigned short sensor_, PCALIBR_ZET110 calibr_ )
{
	long err(0);
	long returnVal(-1);
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

		if (outForWhile!=10)
		err = 0;
	}


	if ( err ==0 )
	{
		Sleep(5);

		COMMAND_INFO_USB command;
		long sizeSend(0), sizeRec(0);

		if ( err == 0 )
		{
			Sleep(5);
			memset(&command, 0x00, sizeof(command));
			command.Command = 0x138;
			command.Data[0] = dev_;
			command.Data[1] = sensor_;
			memcpy_s( &command.Data[2], sizeof(*calibr_), calibr_, sizeof(*calibr_) );

			sizeSend = sizeof(command);
			err = ZSendCommand(_type, _dsp, &command, &sizeSend);
			if ( err == 0 )
			{
				Sleep(5);
				sizeRec = sizeof(command);
				err = ZReceiveCommand(_type, _dsp, &command, &sizeRec);
				if ( err == 0 )
				{
					returnVal = 0;
				}
			}
		}
		Sleep(5);
		err = ZExclusive1Close(_type, _dsp);
	}

	return returnVal;
}

void CSRChoiseProfDlg::SetProfilesInfo()
{
	CALIBR_ZET110 sensorCalibr;
	CString tempString;

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	for ( unsigned int dev = 0; dev < 3; dev ++ )
		for ( unsigned int sensor = 0; sensor < 5; sensor++ )
		{
			if ( GetSensor( dev, sensor, &sensorCalibr) == 0 )
			{
				if (sensorCalibr.ICP == 1) 
					sensorCalibr.ICP = 2;

				tempString.Format( _T("%f"), sensorCalibr.sense );
				_profilesList.SetItemText( dev*5 + sensor, 1, tempString );

				_profilesList.SetItemText( dev*5 + sensor, 2, CString(sensorCalibr.conv) );
				
				tempString.Format( _T("%7.2f"), sensorCalibr.ExtAmp );
				_profilesList.SetItemText( dev*5 + sensor, 3, tempString );

				tempString.Format( _T("%d"), sensorCalibr.ICP );
				_profilesList.SetItemText( dev*5 + sensor, 4, tempString );

				tempString.Format( _T("%f"), sensorCalibr.refer );
				_profilesList.SetItemText( dev*5 + sensor, 5, tempString );

				_profilesList.SetItemText( dev*5 + sensor, 6, _T("0") );

				tempString.Format( _T("%7.1f"), sensorCalibr.DC );
				_profilesList.SetItemText( dev*5 + sensor, 7, tempString );

				_profilesList.SetItemText( dev*5 + sensor, 8, CString(sensorCalibr.name) );

				Sleep(10);
			}
		}

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

BOOL CSRChoiseProfDlg::OnContextMenuClick( UINT nID )
{
	CString tempString;
	long selectedCMItem = (long)( nID - ID_CONTEXT_MENU_SP0 );

	// Происходит установка активного режима
	if ( selectedCMItem < 2 )
	{
		POSITION pos = _profilesList.GetFirstSelectedItemPosition();
		long selectedPLItem = (long)( _profilesList.GetNextSelectedItem( pos ) );

		switch ( selectedPLItem )
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				// Происходит выбор режима в шумомере
				if ( selectedCMItem == 0 )	// Просто выбор устройства
				{
					_paramdelta.CurrCal[0] = static_cast< short >(selectedPLItem);
				}
				if ( selectedCMItem == 1 )	// Выбор как активный
				{
					_paramdelta.currTypeDevice = 0;
					_paramdelta.CurrCal[0] = static_cast< short >(selectedPLItem);
				}
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				// Происходит выбор режима в виброметре
				if ( selectedCMItem == 0 )	// Просто выбор устройства
				{
					_paramdelta.CurrCal[1] = static_cast< short >(selectedPLItem - 5);
				}
				if ( selectedCMItem == 1 )	// Выбор как активный
				{
					_paramdelta.currTypeDevice = 1;
					_paramdelta.CurrCal[1] = static_cast< short >(selectedPLItem - 5);
				}
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				// Происходит выбор режима в регистраторе
				if ( selectedCMItem == 0 )	// Просто выбор устройства
				{
					_paramdelta.CurrCal[2] = static_cast< short >(selectedPLItem - 10);
				}
				if ( selectedCMItem == 1 )	// Выбор как активный
				{
					_paramdelta.currTypeDevice = 2;
					_paramdelta.CurrCal[2] = static_cast< short >(selectedPLItem - 10);
				}
				break;
			default:
				break;
		}
		_profilesList.RedrawItems(0, _profilesList.GetItemCount());

		AfxGetApp()->DoWaitCursor( 1 );
		SetDeviceParamExt();
		AfxGetApp()->DoWaitCursor( -1 );
	}

	// Происходит выбор типа датчика
	if ( selectedCMItem >= 2 )
	{
		selectedCMItem -= 2;

		POSITION pos = _profilesList.GetFirstSelectedItemPosition();
		long selectedPLItem = (long)( _profilesList.GetNextSelectedItem( pos ) );

		tempString.Format( _T("%f"), _sensors.get()[selectedCMItem]->sense );
		_profilesList.SetItemText( selectedPLItem, 1, tempString );

		_profilesList.SetItemText( selectedPLItem, 2, _sensors.get()[selectedCMItem]->unit.c_str() );

		tempString.Format( _T("%7.2f"), _sensors.get()[selectedCMItem]->amplify );
		_profilesList.SetItemText( selectedPLItem, 3, tempString );

		tempString.Format( _T("%d"), _sensors.get()[selectedCMItem]->icp );
		_profilesList.SetItemText( selectedPLItem, 4, tempString );

		tempString.Format( _T("%f"), _sensors.get()[selectedCMItem]->reference );
		_profilesList.SetItemText( selectedPLItem, 5, tempString );

		_profilesList.SetItemText( selectedPLItem, 6, _T("0") );

		tempString.Format( _T("%7.1f"), _sensors.get()[selectedCMItem]->shift );
		_profilesList.SetItemText( selectedPLItem, 7, tempString );

		_profilesList.SetItemText( selectedPLItem, 8, _sensors.get()[selectedCMItem]->name.c_str() );
	}
	
	return true;
}

long CSRChoiseProfDlg::GetDeviceParamExt()
{
	long retVal(-1);
	long err(0), sSize(0);
	memset(&_paramdelta, 0x00, sizeof(PARAMEXT));
	COMMAND_INFO_USB command;

	command.Command = 0x131;
	command.Error   = 0;
	command.Size    = 0;
	sSize           = sizeof(command);

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


	if ( err == 0 )
	{
		Sleep(5);
		err = ZSendCommand(_type, _dsp, &command, &sSize);
		if (err == 0)
		{
			sSize = sizeof(command);
			err = ZReceiveCommand(_type, _dsp, &command, &sSize);
			if ( (err == 0) && (command.Error == 0) )
			{
				memcpy_s(&_paramdelta, sizeof(PARAMEXT), command.Data, sizeof(PARAMEXT));
				retVal = 0;
			}
		}
		ZExclusive1Close(_type, _dsp);
	}
	return retVal;
}

long CSRChoiseProfDlg::SetDeviceParamExt()
{
	long retVal(-1);
	long err(0), sSize(0);
	COMMAND_INFO_USB command;

	command.Command = 0x130;
	command.Error   = 0;
	command.Size    = 0;
	memcpy_s(command.Data, sizeof(PARAMEXT), &_paramdelta, sizeof(PARAMEXT));
	sSize           = sizeof(command);

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

	if ( err == 0 )
	{
		Sleep(5);
		err = ZSendCommand(_type, _dsp, &command, &sSize);
		if (err == 0)
		{
			sSize = sizeof(command);
			err = ZReceiveCommand(_type, _dsp, &command, &sSize);
			if ( (err == 0) && (command.Error == 0) )
			{
				retVal = 0;
			}
		}
		ZExclusive1Close(_type, _dsp);
	}
	return retVal;
}

void CSRChoiseProfDlg::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	*pResult = CDRF_DODEFAULT;

	// 3 типа датчиков (тот тип датчика который установится при горячей смене режима регистрации)
	int noiseType = _paramdelta.CurrCal[0];
	int vibroType = _paramdelta.CurrCal[1] + 5;
	int regType = _paramdelta.CurrCal[2] + 10;
	int currType = _paramdelta.currTypeDevice*5 + _paramdelta.CurrCal[_paramdelta.currTypeDevice];

	switch(pLVCustomDraw->nmcd.dwDrawStage)
	{
		// при начале прорисовки листа указываем, что нам нужны нотификации на прорисовку каждой ячейки
	case CDDS_PREPAINT:
		*pResult |= CDRF_NOTIFYITEMDRAW;
		break;
		// а это они и есть. Каждую четную строку прорисовываем одним цветом, а нечетную — другим
	case CDDS_ITEMPREPAINT:
		if ( ( pLVCustomDraw->nmcd.dwItemSpec == noiseType ) ||
				( pLVCustomDraw->nmcd.dwItemSpec == vibroType ) ||
					( pLVCustomDraw->nmcd.dwItemSpec == regType ) )
		{
			if ( pLVCustomDraw->nmcd.dwItemSpec == currType )
				pLVCustomDraw->clrTextBk = RGB( 30, 255, 30 );
			else
				pLVCustomDraw->clrTextBk = RGB( 180, 255, 180 );
		}
		else
		{
			pLVCustomDraw->clrTextBk = RGB( 255, 255, 255 );
		}

		*pResult |= CDRF_NEWFONT;
		break;
	default:
		break;
	}
}