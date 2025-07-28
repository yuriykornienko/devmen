#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "../../../algo/utils/clipboard/clipboard.hpp"
#include "../../../algo/path/path.h"
#include "channel_tab.h"

#include <boost/range/algorithm/for_each.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <mathZet.h>
#include <memory> 
#include <Dialog_ZET\ConnectToZetViewResult.h>
#include "source\GUI\tenso\tenso_dlg\tenso_dlg.h"
#include "source\GUI\tenso\tenso_icp_dlg\tenso_icp_dlg.h"
#include "resource\Phrases.h"
#include "resource\Phrases.h"
#include<windows.h>


extern CString  g_sNameOrientation[7];

CChannelTab::CChannelTab( CWnd* pParent ) :
	CTab( CChannelTab::IDD, pParent )
	, channel_(nullptr)
	, m_state(0)
	, m_diff(0)
	, m_icp(0)
	, m_tensoIcp(0)
	, m_charge(0)
	, m_hpf(0)
	, m_selftest(0)
	, m_noise(0)
{}

void CChannelTab::WriteNewDataOnOK()
{
	OnSenseChangeRequest();
	OnUnitSenseChangeRequest();
	OnReferenceChangeRequest();
	OnShiftChangeRequest();
	OnAmplifyChangeRequest();
	OnRangeChangeRequestMoreChannel();
	//OnRangeChangeRequest(  AFX_NOTIFY::pNMHDR,   AFX_NOTIFY::pResult); //NMHDR *
}

void CChannelTab::DoDataExchange( CDataExchange* pDX )
{
	CTab::DoDataExchange(pDX);

	DDX_CBStringExact(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_NAME_LOCATION, m_name_location);
	DDX_Text(pDX, IDC_NAME_CHANNEL, m_name_channel);
	DDX_Text(pDX, IDC_COMMENT, m_comment);
	DDX_Text(pDX, IDC_COOR_X, m_CoordinateX);
	DDX_Text(pDX, IDC_COOR_Y, m_CoordinateY);
	DDX_Text(pDX, IDC_COOR_Z, m_CoordinateZ);
	DDX_CBStringExact(pDX, IDC_COOR_P, m_CoordinateP);
	DDX_Control(pDX, IDC_COOR_P, m_coordPCombo);
	DDX_Control(pDX, IDC_NAME, m_nameCombo);
	DDX_CBStringExact(pDX, IDC_UNIT, m_unit);
	DDX_Control(pDX, IDC_UNIT, m_unitCombo);
	DDX_CBStringExact(pDX, IDC_UNIT_SENSE, m_UnitSense);
	DDX_Control(pDX, IDC_UNIT_SENSE, m_UnitSenseCombo);
	DDX_Check(pDX, IDC_STATE, m_state);
	DDX_Check(pDX, IDC_DIFF, m_diff);
	DDX_Control(pDX, IDC_STATE, m_stateCheck);
	DDX_Control(pDX, IDC_DIFF, m_diffCheck);
	DDX_Text(pDX, IDC_SENSE, m_sense);
	DDX_Text(pDX, IDC_REFERENCE, m_reference);
	DDX_Text(pDX, IDC_SHIFT, m_shift);
	DDX_Text(pDX, IDC_AMPLIFY, m_amplify);
	DDX_Control(pDX, IDC_LEVEL, m_level);
	DDX_Text(pDX, IDC_RANGE, m_range);
	DDX_Control(pDX, IDC_RANGE, m_rangeStatic);
	DDX_Control(pDX, IDC_SPIN, m_spin);
	DDX_Check(pDX, IDC_ICP, m_icp);
	DDX_Control(pDX, IDC_ICP, m_icpCheck);
	DDX_Check(pDX, IDC_TENSO_ICP, m_tensoIcp);
	DDX_Control(pDX, IDC_TENSO_ICP, m_tensoIcpCheck);
	DDX_Control(pDX, IDC_TENSO_ICP_SETTINGS, m_tensoIcpSettingsButton);
	DDX_Check(pDX, IDC_CHARGE, m_charge);
	DDX_Control(pDX, IDC_CHARGE, m_chargeCheck);
	DDX_Check(pDX, IDC_HPF, m_hpf);
	DDX_Control(pDX, IDC_HPF, m_hpfCheck);
	DDX_Control(pDX, IDC_TENSO_BUTTON, m_tenso_button);
	DDX_Control(pDX, IDC_INPUT_RESISTANCE_COMBO, m_inputResistanceCombo);
	DDX_Check(pDX, IDC_SELFTEST, m_selftest);
	DDX_Control(pDX, IDC_SELFTEST, m_selftestCheck);
	DDX_Check(pDX, IDC_NOISE, m_noise);
	DDX_Control(pDX, IDC_NOISE, m_noiseCheck);
}

BEGIN_MESSAGE_MAP( CChannelTab, CTab )
	ON_WM_CTLCOLOR   ()
	ON_WM_TIMER      ()
	ON_BN_CLICKED    ( IDC_STATE,              OnEnableDisable )
	ON_BN_CLICKED    ( IDC_DIFF,               OnInphDiff )
    ON_CBN_EDITCHANGE( IDC_NAME,               OnNameChangeRequest)
	ON_CBN_SELCHANGE ( IDC_NAME,               OnSensorSelect )
	ON_EN_CHANGE	 ( IDC_NAME_LOCATION,      OnNameLocationChangeRequest)
	ON_EN_CHANGE	 ( IDC_NAME_CHANNEL,       OnNameChannelChangeRequest)
	ON_EN_CHANGE	 ( IDC_COMMENT,			   OnCommentChangeRequest )
	ON_EN_CHANGE	 ( IDC_COOR_X,			   OnCoordXChangeRequest )
	ON_EN_CHANGE	 ( IDC_COOR_Y,			   OnCoordYChangeRequest )
	ON_EN_CHANGE	 ( IDC_COOR_Z,			   OnCoordZChangeRequest )
	ON_CBN_SELCHANGE ( IDC_COOR_P,			   OnCoordPChangeRequest )
	ON_CBN_EDITCHANGE( IDC_COOR_P,			   OnCoordPEdit )
	ON_CBN_SELCHANGE ( IDC_UNIT_SENSE,		   OnUnitSenseChangeRequest )
	ON_CBN_EDITCHANGE( IDC_UNIT_SENSE,         OnUnitSenseEdit )
	ON_CBN_EDITCHANGE( IDC_UNIT,               OnUnitChangeRequest )
	ON_CBN_SELCHANGE ( IDC_UNIT,               OnUnitSelect )
	ON_EN_CHANGE     ( IDC_SENSE,              OnSenseChangeRequest )
	ON_EN_CHANGE     ( IDC_REFERENCE,          OnReferenceChangeRequest )
	ON_EN_CHANGE     ( IDC_SHIFT,              OnShiftChangeRequest )
	ON_EN_CHANGE     ( IDC_AMPLIFY,            OnAmplifyChangeRequest )
	ON_NOTIFY        ( UDN_DELTAPOS, IDC_SPIN, OnRangeChangeRequest )
	ON_BN_CLICKED    ( IDC_ICP,                OnIcpOnOff )
	ON_BN_CLICKED	 ( IDC_TENSO_ICP,		   OnTensoIcpOnOff)
	ON_BN_CLICKED	 ( IDC_TENSO_ICP_SETTINGS, OnTensoIcpSettingsClick)
	ON_BN_CLICKED	 ( IDC_CHARGE,			   OnChargeOnOff)
	ON_BN_CLICKED	 ( IDC_TEDS,			   OnTedsClick)
	ON_BN_CLICKED    ( IDC_TENSO_BUTTON,       OnTensoClick)
	ON_MESSAGE_VOID  ( WM_USER_CANCEL,         OnUserCancel )
	ON_MESSAGE		 ( WM_STATE_TEDS,		   OnStateTeds)
	ON_BN_CLICKED	 ( IDC_HPF,				   OnHpfOnOff)
	ON_CBN_SELCHANGE ( IDC_INPUT_RESISTANCE_COMBO, OnInputResistanceSelect)
	ON_BN_CLICKED	 ( IDC_SELFTEST,		   OnSelftestOnOff)
	ON_BN_CLICKED	 ( IDC_NOISE,			   OnNoiseOnOff)
END_MESSAGE_MAP()

BOOL CChannelTab::OnInitDialog()
{
	
	CTab::OnInitDialog();

	GetDlgItem(IDC_LABEL_NAME)->SetWindowText(g_sName + L":");
	GetDlgItem(IDC_LABEL_NAME_LOCATION)->SetWindowText(g_sNameLocation + L":");
	GetDlgItem(IDC_LABEL_NAME_CHANNEL)->SetWindowText(g_sNameChannel + L":");
	GetDlgItem(IDC_LABEL_COMMENT)->SetWindowText(g_sComment + L":");
	GetDlgItem(IDC_LABEL_STATE)->SetWindowText(g_sStatus + L":");
	GetDlgItem(IDC_STATE)->SetWindowText(g_sActivated);
	GetDlgItem(IDC_DIFF)->SetWindowText(g_sDifferential);
	GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L":");
	GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L":");
	GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L":");
	GetDlgItem(IDC_LABEL_AMPLIFY)->SetWindowText(g_sConstantIncrease + L":");
	GetDlgItem(IDC_LABEL_LEVEL)->SetWindowText(g_sIntegratedLevelSignal + L":");
	GetDlgItem(IDC_TEDS)->SetWindowTextW(g_sUse + L" TEDS");
	GetDlgItem(IDC_ICP)->SetWindowTextW(g_sUse + L" ICP");
	GetDlgItem(IDC_TENSO_ICP)->SetWindowTextW(g_sTensoBridge);
	GetDlgItem(IDC_TENSO_BUTTON)->SetWindowTextW(g_sTensoSettings);
	GetDlgItem(IDC_CHARGE)->SetWindowTextW(g_sCharge);
	GetDlgItem(IDC_HPF)->SetWindowTextW(g_sAc);
	GetDlgItem(IDC_STATIC_ERROR_STR)->SetWindowTextW(g_sGeneratorUsedAnotherProgram + L".");
	GetDlgItem(IDC_LABEL_COORDINATE)->SetWindowTextW(g_sCoordinate + L":");
	GetDlgItem(IDC_LABEL_INPUT_RESISTANCE)->SetWindowTextW(g_sInputResistance + L":");
	GetDlgItem(IDC_SELFTEST)->SetWindowTextW(g_sSelftestMode);
	GetDlgItem(IDC_NOISE)->SetWindowTextW(g_sNoiseMode);
	if (m_groups) 
	{
		GetDlgItem(IDC_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMMENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_COOR_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_COOR_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_COOR_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_COOR_P)->EnableWindow(FALSE);
	}

	InitUnits();

	SetTimer(2, 500, nullptr);

	return TRUE;
}

void CChannelTab::InitSensors()
{
	path path_;
	path_.Create( this );
	BSTR	bstrDirConfig = _T("DirConfig");
	CString	sensorsConfig = path_.ZetPath( &bstrDirConfig ) + _T("sensors.cfg");
	sensors_.load( (LPCTSTR)sensorsConfig );
}

void CChannelTab::InitUnits()
{
	m_unitCombo.AddString(g_sUNIT_G);
	m_unitCombo.AddString(g_sUNIT_MV);
	m_unitCombo.AddString(g_sUNIT_V);
	m_unitCombo.AddString(g_sUNIT_MA);
	m_unitCombo.AddString(g_sUNIT_A);
	m_unitCombo.AddString(g_sUNIT_H);
	m_unitCombo.AddString(g_sUNIT_KH);
	m_unitCombo.AddString(g_sUNIT_PA);
	m_unitCombo.AddString(g_sUNIT_KPA);
	m_unitCombo.AddString(g_sUNIT_MPA);
	m_unitCombo.AddString(g_sUNIT_MKM);
	m_unitCombo.AddString(g_sUNIT_MM);
	m_unitCombo.AddString(g_sUNIT_MKMM);
	m_unitCombo.AddString(g_sUNIT_MMPERS);
	m_unitCombo.AddString(g_sUNIT_MPERS);
	m_unitCombo.AddString(g_sUNIT_MPERS2);
	m_unitCombo.AddString(g_sUNIT_ATM);
	m_unitCombo.AddString(g_sUNIT_CEL);

	m_coordPCombo.AddString(g_sNameOrientation[0]);
	m_coordPCombo.AddString(g_sNameOrientation[1]);
	m_coordPCombo.AddString(g_sNameOrientation[2]);
	m_coordPCombo.AddString(g_sNameOrientation[3]);
	m_coordPCombo.AddString(g_sNameOrientation[4]);
	m_coordPCombo.AddString(g_sNameOrientation[5]);
	m_coordPCombo.AddString(g_sNameOrientation[6]);

	m_UnitSenseCombo.AddString(g_sUNIT_MKV);
	m_UnitSenseCombo.AddString(g_sUNIT_MV);
	m_UnitSenseCombo.AddString(g_sUNIT_V);
	m_UnitSenseCombo.AddString(g_sUNIT_KV);

	m_inputResistanceCombo.AddString(L"1 " + g_sUNIT_MOM);
	m_inputResistanceCombo.AddString(L"100 " + g_sUNIT_kOM);
}

void CChannelTab::update_range()
{
	CString range;
	if (channel_->isSupportAmplify())
		range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
	else
		range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
	if (m_range != range)
	{
		m_range = range;
		m_rangeStatic.SetWindowText(m_range);
	}
}

void CChannelTab::SetAdapter( zetlab::std::channel* channel__,
							  zetlab::std::device* device__ )
{
	
	if (!channel__->isSupportSensorStruct())
		InitSensors();     // для инициализации связанных с сенсорами параметров.
	
	channel_ = channel__;

	m_b_metadata = device__->has_mseed_metadata();  //Запоминает указатели на канал (channel_) и устройство (m_b_metadata).
			
	if (!memento_) memento_ = boost::make_shared< zetlab::std::channel_memento >( channel__ );  /// сохранение старого
	m_nameDevice. Format(_T("%s"), device__->getNameDevice().c_str());
	m_postfix.	  Format(_T("_%d_ %d"), device__->getSerial(), channel_->getDeviceID() + 1);
	m_name        = channel_->getName( true ).c_str();
	m_name.       Replace(L";", L"");
	m_name_location = channel_->get_location().c_str();
	m_name_channel = channel_->get_channel().c_str();
	m_comment	  = channel_->getComment(true).c_str();
	m_comment.    Replace(L";", L"");
	m_unit        = channel_->getUnit( true ).c_str(); // Единицы измерения (m_unit).
	m_state       = int(channel_->isEnabled());  //Устанавливает состояние канала (включен/выключен — m_state).
	m_stateCheck. EnableWindow( channel_->canEnableOrDisable() );  //Включает/выключает чекбоксы (m_stateCheck, m_diffCheck) 
	m_diff		  = int(channel_->getDiff());
	m_diffCheck.  EnableWindow( m_state == 1 );
	
	if (device__->getType() == 17 /*ZET_DEVICE_230 */ || device__->getType() == 10 /*ZET_DEVICE_210 */ ||
		device__->getType() == 16  /*ZET_DEVICE_220 */ || device__->getType() == 12  /*ZET_DEVICE_110 */ ||
		channel_->getType() == 211 /*ZET_DEVICE_211*/ || channel_->getType() == 221 /*ZET_DEVICE_221*/)
	{
		m_CoordinateX = L"0";
		m_CoordinateY = L"0";
		m_CoordinateZ = L"0";
		m_CoordinateP = L"";
		m_UnitSense = L"";
	}
	else
	{
		m_CoordinateX.Format(_T("%g"), channel_->CoordX(true));
		m_CoordinateY.Format(_T("%g"), channel_->CoordY(true));
		m_CoordinateZ.Format(_T("%g"), channel_->CoordZ(true));
		
		long _lCoordP(channel_->CoordP(true));
		if (_lCoordP >= 0 && _lCoordP <= 6)
		{
			if (m_coordPCombo.SetCurSel(_lCoordP) >= 0)
				m_coordPCombo.GetLBText(m_coordPCombo.GetCurSel(), m_CoordinateP);
		}
		else
		{
			if(m_coordPCombo.SetCurSel(0) >= 0)
				m_coordPCombo.GetLBText(m_coordPCombo.GetCurSel(), m_CoordinateP);
		}
	}

	// Из сохраненной в сервере чувствительности в V, переводим ее в mV или KV
	double _sense(channel_->Sense(true));
	m_UnitSense = channel_->getUnitSense(true).c_str();

	if (m_UnitSense == g_sUNIT_MKV)
		_sense *= 1000000;
	else if (m_UnitSense == g_sUNIT_MV)
		_sense *= 1000;
	else if (m_UnitSense == g_sUNIT_KV)
		_sense /= 1000;

	m_sense.      Format( _T("%g"), _sense);
	m_reference.  Format( _T("%g"), channel_->Reference( true ) );
	m_shift.      Format( _T("%g"), channel_->Shift( true ) );
	m_amplify.    Format( _T("%g"), channel_->Amplify() );
		
	update_range();
	/*if (channel_->isSupportAmplify())
		m_range.  Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
	else
		m_range.  Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
	m_rangeStatic.SetWindowText(m_range);*/
	// настройка входов и фильтров
	m_spin.       EnableWindow( channel_->isSupportAmplify() && !channel_->isSupportSensorStruct() );
	m_icp         = channel_->getICP() && !channel_->getTensoIcp();
	m_icpCheck.   EnableWindow( m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getCharge() && 0 == channel_->getTensoIcp());
	m_tensoIcp	  = channel_->getTensoIcp();
	bool bEnable = m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getCharge();
	if (bEnable)
	{
		if (channel_->getICP() && !channel_->getTensoIcp())
			bEnable = false;
	}
	m_tensoIcpCheck.EnableWindow(bEnable);
	m_tensoIcpSettingsButton.EnableWindow(bEnable && m_tensoIcp);
	
	m_charge	  = channel_->getCharge();
	m_chargeCheck.EnableWindow(m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getICP() && 0 == channel_->getTensoIcp());
	m_hpf		  = channel_->HPF();
	bool bEnableHpf = m_state == 1 && !channel_->isSupportSensorStruct();
	if (bEnableHpf)
	{
		if (channel_->isSupportInputModeAdc())
		{
			auto mode = channel_->getAdcInputMode();
			if (mode == 1 || mode == 3)
				bEnableHpf = false;
		}
	}
	m_hpfCheck.	  EnableWindow(bEnableHpf);
	m_inputResistanceCombo.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc());
	m_inputResistanceCombo.SetCurSel(channel_->getInputResistance());
	m_selftest = channel_->AdcInputMode() == 1;
	m_selftestCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 3);
	m_noise = channel_->AdcInputMode() == 3;
	m_noiseCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 1);
	m_level.      SetFocus();
	
	if (channel__->isSupportSensorStruct())
	{
		GetDlgItem( IDC_NAME )->EnableWindow( FALSE );
		GetDlgItem( IDC_COMMENT )->EnableWindow(FALSE);
		GetDlgItem( IDC_COOR_X )->EnableWindow(FALSE);
		GetDlgItem( IDC_COOR_Y )->EnableWindow(FALSE);
		GetDlgItem( IDC_COOR_Z )->EnableWindow(FALSE);
		GetDlgItem( IDC_COOR_P)->EnableWindow(FALSE);
		GetDlgItem( IDC_UNIT )->EnableWindow( FALSE );
		GetDlgItem( IDC_UNIT_SENSE )->EnableWindow(FALSE);
		GetDlgItem( IDC_STATE )->EnableWindow( FALSE );
		GetDlgItem( IDC_DIFF )->EnableWindow( FALSE );
		GetDlgItem( IDC_SENSE )->EnableWindow( FALSE );
		GetDlgItem( IDC_REFERENCE )->EnableWindow( FALSE );
		GetDlgItem( IDC_SHIFT )->EnableWindow( FALSE );
		GetDlgItem( IDC_AMPLIFY )->EnableWindow( FALSE );
		GetDlgItem( IDC_LEVEL )->EnableWindow( FALSE );
		GetDlgItem( IDC_RANGE )->EnableWindow( FALSE );
		GetDlgItem( IDC_SPIN )->EnableWindow( FALSE );
		GetDlgItem( IDC_ICP )->EnableWindow( FALSE );
		GetDlgItem( IDC_TENSO_ICP )->EnableWindow(FALSE);
		GetDlgItem( IDC_CHARGE )->EnableWindow(FALSE);
		GetDlgItem( IDC_HPF )->EnableWindow(FALSE);
		GetDlgItem( IDC_TEDS )->EnableWindow(FALSE);
		GetDlgItem( IDC_TENSO_ICP_SETTINGS )->EnableWindow(FALSE);
		GetDlgItem( IDC_SELFTEST )->EnableWindow(FALSE);
		GetDlgItem( IDC_NOISE )->EnableWindow(FALSE);
	}

	SetTimer (1, 400, nullptr);  // таймер для обновления   //////////////////////  БЫЛО 500 ////////////

	UpdateData(FALSE);

	// если нет информации о канале то скрываем все кроме кнопки включить
	
	if (zetlab::std::demo != device__->getConnectionType())   // групповая настройка канала
	{
		
		if (channel_->getServerID() == -1)
			SetVisible(SW_HIDE);
		else
			SetVisible(channel_->isFeedBackChannel() ? SW_HIDE : SW_SHOW);

		zetlab::sensors::sensors_type v_sensors = sensors_.get();
		sensors_.setName((LPCTSTR)m_nameDevice, 0);
		sensors_.addPostfix((LPCTSTR)m_postfix);
		boost::range::for_each(v_sensors, [&](zetlab::sensors::sensor_ptr& sensor_) {
			m_nameCombo.AddString(sensor_->name.c_str());
		});

		// если канал уже задействован какой-то программой
		// убираем галочку включить/отвключить и выдаем это сообщение
		if (channel_->IsBuiltinGenChanFree())
		{
			GetDlgItem(IDC_STATIC_ERROR_STR)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LABEL_STATE)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATIC_ERROR_STR)->ShowWindow(SW_HIDE);

			if (channel_->isFeedBackChannel() == true)
			{
				GetDlgItem(IDC_LABEL_STATE)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATE)->ShowWindow(SW_SHOW);
			}
			else 
			{
				GetDlgItem(IDC_LABEL_STATE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATE)->ShowWindow(SW_HIDE);
			}
		}
	}
	else 
	{
		SetVisibledemo();
	}

	// Отключение вкладки задействовать у анализоторов без встроенного генератора
	if (!channel_->isHaveBuiltinGenerator() && channel_->isFeedBackChannel())
	{
		GetDlgItem(IDC_STATE)->EnableWindow(FALSE);
	}

	if (!m_unit.IsEmpty())
	{
		auto sUnitSense = CString(channel_->getUnitSense().c_str());
		GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + sUnitSense + L"/" + m_unit + L":");
		GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L", " + m_unit + L":");
		GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
	}
	::PostMessage(m_coordPCombo, WM_LBUTTONDOWN, 0, 0);
	Sleep(50); ///////////////////////////////////////////////////////////////////////////////////////////////////  было 100
	::PostMessage(m_unitCombo, WM_LBUTTONDOWN, 0, 0);
	Sleep(50);   ///////////////////////////////////////////////////////////////////////////////////////////////////  было 100
	::PostMessage(m_nameCombo, WM_LBUTTONDOWN, 0, 0);
	if (device__->getType() == 17 /*ZET_DEVICE_230 */ || device__->getType() == 10 /*ZET_DEVICE_210 */ ||
		device__->getType() == 16  /*ZET_DEVICE_220 */ || device__->getType() == 12  /*ZET_DEVICE_110 */ ||
		channel_->getType() == 211 /*ZET_DEVICE_211*/ || channel_->getType() == 221 /*ZET_DEVICE_221*/)
	{	
		HideControls();
	}

	if (m_tensoIcp)
	{
		m_UnitSenseCombo.SelectString(-1, g_sUNIT_V);
		m_UnitSenseCombo.EnableWindow(FALSE);
		m_unitCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_SENSE)->EnableWindow(FALSE);
	}

}


void CChannelTab::SetAdapter_s(std::vector<zetlab::object*> objects__,
							   zetlab::std::device* device__,
							   bool m_group)
{
	
	channels_.clear();
	mementos.clear();

	m_groups = m_group;

	for (auto& element : objects__)
	{
		channels_.push_back(element->getAsChannel());

	}


	for (auto element : channels_)
	{

		mementos.push_back(boost::make_shared< zetlab::std::channel_memento >(element));

	}

}

void CChannelTab::CopyToClipboard() // Кнопка Копировать в групповом меню
{
	
	zetlab::utils::copy_to_clipboard<zetlab::std::channel, zetlab::std::channel::clipboard>(
		_T("CF_ZET_CHANNEL"), channel_ );
}

void CChannelTab::PasteFromClipboard()  // Кнопка Вставить в групповом меню
{

	if (channels_.size() > 1)
	{
		int countMomentos = 0;

		for (auto& current_channel : channels_)
		{
			if (zetlab::utils::paste_from_clipboard<zetlab::std::channel, zetlab::std::channel::clipboard>(
					_T("CF_ZET_CHANNEL"), current_channel))
			{
				SetAdapter(current_channel, const_cast<zetlab::std::device*>(current_channel->getDevice()));  // обновляет адаптер канала 
				
				auto old_name  = mementos[countMomentos]->getName();  // не меняет имя при групповой настройке
				this->channel_->setName(old_name); 
			}
			++countMomentos;
		}
	}
	else
	{
		if (zetlab::utils::paste_from_clipboard<zetlab::std::channel, zetlab::std::channel::clipboard>(
			_T("CF_ZET_CHANNEL"), channel_))
		{
			SetAdapter(channel_, const_cast<zetlab::std::device*>(channel_->getDevice()));
			auto old_name = memento_->getName();  // не меняет имя при single настройке
			this->channel_->setName(old_name);
		}
	}
}

BOOL CChannelTab::IsClipboardAvailable()   // Проверка можно ли вставить данные.
{
	return zetlab::utils::is_clipboard_format_available( _T("CF_ZET_CHANNEL") );
}

HBRUSH CChannelTab::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	if (IDC_SEPARATOR == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkMode   ( TRANSPARENT );
		pDC->SetTextColor( GetSysColor( COLOR_BTNSHADOW ) );
	}

	return CTab::OnCtlColor( pDC, pWnd, nCtlColor );
}

void CChannelTab::OnTimer( UINT_PTR nIDEvent )
{
	CTab::OnTimer( nIDEvent );

	switch (nIDEvent)
	{
	case 1:
		m_level.SetAmplitude( channel_->getLevel() );
		break;
	case 2:
		update_range();
		
		/*KillTimer(2);
		if (channel_->isSupportAmplify())
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
		else
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
		m_rangeStatic.SetWindowText(m_range);*/
		
		break;
	}
}

void CChannelTab::OnEnableDisable()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	channel_->  setEnabledPh( m_state == 1 );

	if (channel_->isFeedBackChannel())
		channel_->SetBuiltinGeneratorEnabled(m_state == 1 ? 1 : 0);
	
	m_diffCheck.EnableWindow( m_state == 1 );
	m_icpCheck.EnableWindow(m_state == 1 && 0 == channel_->getCharge() && 0 == channel_->getTensoIcp());
	m_hpfCheck. EnableWindow( m_state == 1 );
	m_chargeCheck.EnableWindow(m_state == 1 && 0 == channel_->getICP() && 0 == channel_->getTensoIcp());
}

void CChannelTab::OnInphDiff()
{
	if (nullptr == channel_)
		return;

	UpdateData();
	if (channels_.size() > 1)
	{
		long mask(0);
		for (auto& element : channels_)
		{
			if (element->isFeedBackChannel())
				continue;
			mask |= (1 << element->getDeviceID());
			
			if (element->is_zsp())
				element->setDiff(m_diff == 1, false);
		}

		channels_.front()->setDiff(mask, m_diff == 1);
		m_stateCheck.EnableWindow(channels_.front()->canEnableOrDisable());

	}
	else
	{
		channel_->setDiff(m_diff == 1);
		m_stateCheck.EnableWindow(channel_->canEnableOrDisable());
	}

	UpdateData( FALSE );
}

void CChannelTab::OnNameChangeRequest()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	m_name.Replace(L";", L"");

	channel_->setName( (LPCTSTR)m_name );
}

void CChannelTab::OnNameLocationChangeRequest()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	if (m_name_location.GetLength() > 2)
		m_name_location.Left(2);

	channel_->set_location(m_name_location.GetString());
}

void CChannelTab::OnNameChannelChangeRequest()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	if (m_name_channel.GetLength() > 3)
		m_name_channel.Left(3);

	channel_->set_channel(m_name_channel.GetString());
}

void CChannelTab::OnCommentChangeRequest()
{
	if (nullptr == channel_)
		return;
	UpdateData();

	if (m_comment.GetLength() > 250)
		m_comment.Left(250);

	m_comment.Replace(L";", L"");

	if (m_comment.IsEmpty())
		m_comment = L"⁣";

	channel_->setComment((LPCTSTR)m_comment);
}

void CChannelTab::OnCoordXChangeRequest()
{
	if (nullptr == channel_)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_COOR_X);

	CString CoordX = m_CoordinateX;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		if (m_CoordinateX.Find(L",") != -1)
			m_CoordinateX.Replace(L",", L".");
		if (m_CoordinateX.Find(L"б") != -1)
			m_CoordinateX.Replace(L"б", L".");
		if (m_CoordinateX.Find(L"ю") != -1)
			m_CoordinateX.Replace(L"ю", L".");
		if (m_CoordinateX.Find(L"\\") != -1)
			m_CoordinateX.Replace(L"\\", L".");
		if (m_CoordinateX.Find(L"\\") != -1)
			m_CoordinateX.Replace(L"\\", L".");
		if (m_CoordinateX.Find(L"/") != -1)
			m_CoordinateX.Replace(L"/", L".");
		if (channels_.size() > 1)
		{
			for (auto& element : channels_)
			{
				element->setCoordX(boost::lexical_cast<double>((LPCTSTR)m_CoordinateX));
			}
		}
		else
		{
			channel_->setCoordX(boost::lexical_cast<double>((LPCTSTR)m_CoordinateX));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_CoordinateX.Compare(_T("-")))
			m_CoordinateX = CoordX;
	}

	UpdateData(FALSE);
	e->SetSel(ret);

}

void CChannelTab::OnCoordYChangeRequest()
{
	if (nullptr == channel_)
		return;
	CEdit* e = (CEdit*)GetDlgItem(IDC_COOR_Y);

	CString CoordY = m_CoordinateY;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		if (m_CoordinateY.Find(L",") != -1)
			m_CoordinateY.Replace(L",", L".");
		if (m_CoordinateY.Find(L"б") != -1)
			m_CoordinateY.Replace(L"б", L".");
		if (m_CoordinateY.Find(L"ю") != -1)
			m_CoordinateY.Replace(L"ю", L".");
		if (m_CoordinateY.Find(L"\\") != -1)
			m_CoordinateY.Replace(L"\\", L".");
		if (m_CoordinateY.Find(L"\\") != -1)
			m_CoordinateY.Replace(L"\\", L".");
		if (m_CoordinateY.Find(L"/") != -1)
			m_CoordinateY.Replace(L"/", L".");

		if (channels_.size() > 1)
		{
			for (auto& element : channels_)
			{
				element->setCoordY(boost::lexical_cast<double>((LPCTSTR)m_CoordinateY));
			}
		}
		else
		{
			channel_->setCoordY(boost::lexical_cast<double>((LPCTSTR)m_CoordinateY));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_CoordinateY.Compare(_T("-")))
			m_CoordinateY = CoordY;
	}

	UpdateData(FALSE);
	e->SetSel(ret);
}

void CChannelTab::OnCoordZChangeRequest()
{
	if (nullptr == channel_)
		return;
	CEdit* e = (CEdit*)GetDlgItem(IDC_COOR_Z);

	CString CoordZ = m_CoordinateZ;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		if (m_CoordinateZ.Find(L",") != -1)
			m_CoordinateZ.Replace(L",", L".");
		if (m_CoordinateZ.Find(L"б") != -1)
			m_CoordinateZ.Replace(L"б", L".");
		if (m_CoordinateZ.Find(L"ю") != -1)
			m_CoordinateZ.Replace(L"ю", L".");
		if (m_CoordinateZ.Find(L"\\") != -1)
			m_CoordinateZ.Replace(L"\\", L".");
		if (m_CoordinateZ.Find(L"\\") != -1)
			m_CoordinateZ.Replace(L"\\", L".");
		if (m_CoordinateZ.Find(L"/") != -1)
			m_CoordinateZ.Replace(L"/", L".");

		if (channels_.size() > 1)
		{
			for (auto& element : channels_)
			{
				element->setCoordZ(boost::lexical_cast<double>((LPCTSTR)m_CoordinateZ));
			}
		}
		else
		{
			channel_->setCoordZ(boost::lexical_cast<double>((LPCTSTR)m_CoordinateZ));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_CoordinateZ.Compare(_T("-")))
			m_CoordinateZ = CoordZ;
	}

	UpdateData(FALSE);
	e->SetSel(ret);
}

void CChannelTab::OnCoordPChangeRequest()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	int nSel(m_coordPCombo.GetCurSel());

	if (channels_.size() > 1)
	{
		for (auto& element : channels_)
		{
			element->setCoordP(nSel);
		}
	}
	else
	{
		channel_->setCoordP(nSel);
	}

	
}

void CChannelTab::OnCoordPEdit()
{
	if (nullptr == channel_)
		return;
	UpdateData();

	m_coordPCombo.SetCurSel(channel_->getConstCoordP());
}

void CChannelTab::OnUnitSenseChangeRequest()  ////Меняет единицы измерения чувствительности // мкВ и В меняет

{
	if (nullptr == channel_)
		return;

	UpdateData();
	CString unitSense(m_UnitSense);
	CString sUnitSense(L"");
	auto curSel = m_UnitSenseCombo.GetCurSel();
	if (curSel >= 0)
		m_UnitSenseCombo.GetLBText(curSel, sUnitSense);
	else
	{
		m_UnitSenseCombo.GetWindowText(sUnitSense);
		sUnitSense = TranslateString(sUnitSense);
	}
	if (channels_.size() > 1)   // групповое изменение едениц измерения
	{

		for (auto& element : channels_)
		{

			element->setUnitSense((LPCTSTR)sUnitSense);

			double _sense(boost::lexical_cast<float>((LPCTSTR)m_sense));

			double f(1.);
			if (0 == zvGetConversionFactor(m_UnitSense, sUnitSense, f))
				_sense *= f;

			m_UnitSense = sUnitSense;

			if (_sense != 0)
			{
				m_sense.Format(_T("%g"), _sense);

				// пересчитаем чувствительность
				if (sUnitSense == g_sUNIT_MKV)
					element->setSense(_sense /= 1000000);
				else if (sUnitSense == g_sUNIT_MV)
					element->setSense(_sense /= 1000);
				else if (sUnitSense == g_sUNIT_KV)
					element->setSense(_sense *= 1000);
				else if (sUnitSense == g_sUNIT_V)
					element->setSense(_sense);
			}

		}
	}
	else
	{
		channel_->setUnitSense((LPCTSTR)sUnitSense);

		double _sense(boost::lexical_cast<float>((LPCTSTR)m_sense));

		double f(1.);
		if (0 == zvGetConversionFactor(m_UnitSense, sUnitSense, f))
			_sense *= f;

		m_UnitSense = sUnitSense;

		if (_sense != 0)
		{
			m_sense.Format(_T("%g"), _sense);

			// пересчитаем чувствительность
			if (sUnitSense == g_sUNIT_MKV)
				channel_->setSense(_sense /= 1000000);
			else if (sUnitSense == g_sUNIT_MV)
				channel_->setSense(_sense /= 1000);
			else if (sUnitSense == g_sUNIT_KV)
				channel_->setSense(_sense *= 1000);
			else if (sUnitSense == g_sUNIT_V)
				channel_->setSense(_sense);
		}
	}

	if (!sUnitSense.IsEmpty() && !m_unit.IsEmpty())
		GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + sUnitSense + L"/" + m_unit + L":");

	UpdateData(FALSE);
}

void CChannelTab::OnUnitSenseEdit()
{
	if (nullptr == channel_)
		return;
	UpdateData();
	// Пользователь может только выбирать из доступных ед измерения
	if (channels_.size() > 1)
	{

		for (auto& element : channels_)
		{
			m_UnitSenseCombo.SelectString(-1, element->getUnitSense().c_str());
		}
	}
	else
	{
		m_UnitSenseCombo.SelectString(-1, channel_->getUnitSense().c_str());
	}
}

void CChannelTab::OnSensorSelect()
{
	if (nullptr == channel_)
		return;

	int nSel(m_nameCombo.GetCurSel());
	zetlab::sensors::sensors_type v_sensors(sensors_.get());

	if (0 > nSel || nSel >= int(v_sensors.size()))
		return;
	m_name		= v_sensors[nSel]->name.c_str() + m_postfix;
	m_unit      = v_sensors[nSel]->unit.c_str();
	m_UnitSense = v_sensors[nSel]->unitSense.c_str();;
	m_sense.      Format( _T("%g"), v_sensors[nSel]->sense );
	m_reference.  Format( _T("%g"), v_sensors[nSel]->reference );
	m_shift.      Format( _T("%g"), v_sensors[nSel]->shift );
	m_amplify.    Format( _T("%g"), v_sensors[nSel]->amplify );
	m_icp       = v_sensors[nSel]->icp;
	m_icpCheck.SetCheck(m_icp);
	m_charge	= v_sensors[nSel]->charge;
	m_chargeCheck.SetCheck(m_charge);
	m_hpf		= v_sensors[nSel]->hpf;
	m_hpfCheck.SetCheck(m_hpf);
	m_tensoIcp  = 0;
	m_tensoIcpCheck.SetCheck(BST_UNCHECKED);
	m_tensoIcpSettingsButton.EnableWindow(FALSE);

	m_comment = L"⁣";
	m_CoordinateX = L"0";
	m_CoordinateY = L"0";
	m_CoordinateZ = L"0";
	m_CoordinateP = L"o";

	UpdateData(FALSE);

	if (m_icp)
	{
		OnChargeOnOff();
		OnTensoIcpOnOff();
		OnIcpOnOff();
	}
	else
	{
		OnIcpOnOff();
		OnTensoIcpOnOff();
		OnChargeOnOff();
	}

	channel_->setName		( v_sensors[nSel]->name );
	channel_->setUnit		( v_sensors[nSel]->unit );
	channel_->setUnitSense	( v_sensors[nSel]->unitSense );

	if (v_sensors[nSel]->unitSense.c_str() == g_sUNIT_MKV)
		channel_->setSense(v_sensors[nSel]->sense/1000000);
	else if (v_sensors[nSel]->unitSense.c_str() == g_sUNIT_MV)
		channel_->setSense(v_sensors[nSel]->sense /1000);
	else if (v_sensors[nSel]->unitSense.c_str() == g_sUNIT_KV)
		channel_->setSense(v_sensors[nSel]->sense * 1000);
	else if (v_sensors[nSel]->unitSense.c_str() == g_sUNIT_V)
		channel_->setSense(v_sensors[nSel]->sense);

	channel_->setReference	( v_sensors[nSel]->reference );
	channel_->setShift		( v_sensors[nSel]->shift );
	channel_->setHpf		( v_sensors[nSel]->hpf );
	channel_->setAmplify	( v_sensors[nSel]->amplify );
	channel_->setUnitSense  ( (LPCTSTR)m_UnitSense	);
	update_range();
	/*if (channel_->isSupportAmplify())
		m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
	else
		m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
	*/
	channel_->setICP( long(m_icp) );
	channel_->setCharge(long(m_charge));
	if (!m_icp)
		channel_->setTensoIcp(long(m_tensoIcp));

	channel_->setComment((LPCTSTR)m_comment);
	channel_->setCoordX(0);
	channel_->setCoordY(0);
	channel_->setCoordZ(0);
	channel_->setCoordP(0);
}

void CChannelTab::OnUnitChangeRequest()
{
	if (nullptr == channel_)
		return;

	UpdateData();
	if (channels_.size() > 1)
	{

		for (auto& element : channels_)
		{
			element->setUnit((LPCTSTR)m_unit);
			/*if (element->isSupportAmplify())
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", element->getMaxLevel(), m_unit, element->getRange(), element->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
			else
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", element->getMaxLevel(), m_unit, element->getRange()); //L"Диапазон: %g %s (до %g дБ)"
			*/
			update_range();
			if (!m_unit.IsEmpty())
			{
				auto sUnitSense = CString(element->getUnitSense().c_str());
				GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + sUnitSense + L"/" + m_unit + L":");
				GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L", " + m_unit + L":");
				GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
			}
	

		}
	}
	else
	{
		channel_->setUnit((LPCTSTR)m_unit);
		/*if (channel_->isSupportAmplify())
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
		else
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange()); //L"Диапазон: %g %s (до %g дБ)"
		*/
		update_range();
		if (!m_unit.IsEmpty())
		{
			auto sUnitSense = CString(channel_->getUnitSense().c_str());
			GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + sUnitSense + L"/" + m_unit + L":");
			GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L", " + m_unit + L":");
			GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
		}
	}
	UpdateData( FALSE );
}

void CChannelTab::OnUnitSelect()
{
	if (nullptr == channel_)
		return;
	UpdateData();
	CString unit(m_unit);
	auto curSel = m_unitCombo.GetCurSel();
	if (curSel >= 0)
		m_unitCombo.GetLBText(curSel, m_unit );
	else
		m_unitCombo.GetWindowText(m_unit);
	double f(1.);
	if (channels_.size() > 1)
	{

		for (auto& element : channels_)
		{
			if (0 == zvGetConversionFactor(m_unit, unit, f))
			{
				auto sense = element->Sense() * f;
				auto koefForserver = sense;

				CString sUnitSense(L"");
				curSel = m_UnitSenseCombo.GetCurSel();
				if (curSel >= 0)
					m_UnitSenseCombo.GetLBText(m_UnitSenseCombo.GetCurSel(), sUnitSense);
				else
				{
					m_UnitSenseCombo.GetWindowText(sUnitSense);
					sUnitSense = TranslateString(sUnitSense);
				}
				if (sUnitSense == g_sUNIT_MKV)
					sense *= 1000000;
				else if (sUnitSense == g_sUNIT_MV)
					sense *= 1000;
				else if (sUnitSense == g_sUNIT_KV)
					sense /= 1000;

				m_sense.Format(_T("%g"), sense);
				element->setSense(koefForserver);
			}
			element->setUnit((LPCTSTR)m_unit);
			/*if (element->isSupportAmplify())
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", element->getMaxLevel(), m_unit, element->getRange(), element->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
			else
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", element->getMaxLevel(), m_unit, element->getRange());//L"Диапазон: %g %s (до %g дБ)"
			*/
			update_range();
			// По просьбе умневича
			static const float pre_reference[] = { 0.00003f,		// g_sUNIT_G
													0.001f,			// g_sUNIT_MV
													0.000001f,		// g_sUNIT_V
													0.001f,			// g_sUNIT_MA
													0.000001f,		// g_sUNIT_A
													1.0f,			// g_sUNIT_H
													0.001f,			// g_sUNIT_KH
													0.00002f,		// g_sUNIT_PA
													0.00000002f,	// g_sUNIT_KPA
													0.00000000002f,	// g_sUNIT_MPA
													1.0f,			// g_sUNIT_MKM
													0.001f,			// g_sUNIT_MM
													1.f,			// g_sUNIT_MKMM
													0.00005f,		// g_sUNIT_MMPERS
													0.00000005f,	// g_sUNIT_MPERS
													0.0003f,		// g_sUNIT_MPERS2
													1.0f,			// g_sUNIT_ATM
													1.0f };			// g_sUNIT_CEL

			int nSel(m_unitCombo.GetCurSel());
			if (pre_reference[nSel] != 0.0)
			{
				m_reference.Format(_T("%g"), pre_reference[nSel]);
				element->setReference(pre_reference[nSel]);

			}


			if (!m_unit.IsEmpty())
			{
				GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + element->getUnitSense().c_str() + L"/" + m_unit + L":");
				GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L", " + m_unit + L":");
				GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
			}
		}
	}
	else
	{
		if (0 == zvGetConversionFactor(m_unit, unit, f))
		{
			auto sense = channel_->Sense() * f;
			auto koefForserver = sense;

			CString sUnitSense(L"");
			curSel = m_UnitSenseCombo.GetCurSel();
			if (curSel >= 0)
				m_UnitSenseCombo.GetLBText(m_UnitSenseCombo.GetCurSel(), sUnitSense);
			else
			{
				m_UnitSenseCombo.GetWindowText(sUnitSense);
				sUnitSense = TranslateString(sUnitSense);
			}
			if (sUnitSense == g_sUNIT_MKV)
				sense *= 1000000;
			else if (sUnitSense == g_sUNIT_MV)
				sense *= 1000;
			else if (sUnitSense == g_sUNIT_KV)
				sense /= 1000;

			m_sense.Format(_T("%g"), sense);
			channel_->setSense(koefForserver);
		}
		channel_->setUnit((LPCTSTR)m_unit);
		/*if (channel_->isSupportAmplify())
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
		else
			m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
		*/
		update_range();
		// По просьбе умневича
		static const float pre_reference[] = { 0.00003f,		// g_sUNIT_G
												0.001f,			// g_sUNIT_MV
												0.000001f,		// g_sUNIT_V
												0.001f,			// g_sUNIT_MA
												0.000001f,		// g_sUNIT_A
												1.0f,			// g_sUNIT_H
												0.001f,			// g_sUNIT_KH
												0.00002f,		// g_sUNIT_PA
												0.00000002f,	// g_sUNIT_KPA
												0.00000000002f,	// g_sUNIT_MPA
												1.0f,			// g_sUNIT_MKM
												0.001f,			// g_sUNIT_MM
												1.f,			// g_sUNIT_MKMM
												0.00005f,		// g_sUNIT_MMPERS
												0.00000005f,	// g_sUNIT_MPERS
												0.0003f,		// g_sUNIT_MPERS2
												1.0f,			// g_sUNIT_ATM
												1.0f };			// g_sUNIT_CEL

		int nSel(m_unitCombo.GetCurSel());
		if (pre_reference[nSel] != 0.0)
		{
			m_reference.Format(_T("%g"), pre_reference[nSel]);
			channel_->setReference(pre_reference[nSel]);

		}


		if (!m_unit.IsEmpty())
		{
			GetDlgItem(IDC_LABEL_SENSE)->SetWindowText(g_sChannelSensitivity + L", " + channel_->getUnitSense().c_str() + L"/" + m_unit + L":");
			GetDlgItem(IDC_LABEL_REFERENCE)->SetWindowText(g_sReferenceValueLevel + L", " + m_unit + L":");
			GetDlgItem(IDC_LABEL_SHIFT)->SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
		}
	}
	UpdateData( FALSE );
}

void CChannelTab::OnSenseChangeRequest()  //// Обрабатывает изменение чувствительности канала - ввод данных обработка
{
	if (nullptr == channel_)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_SENSE);

	CString sense = m_sense;

	UpdateData();

	DWORD ret = e->GetSel();
	
	try
	{
		if (m_sense.Find(L",") != -1)
			m_sense.Replace(L",", L".");
		if (m_sense.Find(L"б") != -1)
			m_sense.Replace(L"б", L".");
		if (m_sense.Find(L"ю") != -1)
			m_sense.Replace(L"ю", L".");
		if (m_sense.Find(L"\\") != -1)
			m_sense.Replace(L"\\", L".");
		if (m_sense.Find(L"\\") != -1)
			m_sense.Replace(L"\\", L".");
		if (m_sense.Find(L"/") != -1)
			m_sense.Replace(L"/", L".");

		CString sUnitSense(L"");
		auto curSel = m_UnitSenseCombo.GetCurSel();
		if (curSel >= 0)
			m_UnitSenseCombo.GetLBText(curSel, sUnitSense);
		else
		{
			m_UnitSenseCombo.GetWindowText(sUnitSense);
			sUnitSense = TranslateString(sUnitSense);
		}

		double _sense(boost::lexical_cast<float>((LPCTSTR)m_sense));
		if (_sense != 0)
		{
			if (channels_.size() > 1)
			{
				int numberChannel = 0;
				for (auto& element : channels_)
				{
					// пересчитаем чувствительность
					if (sUnitSense == g_sUNIT_MKV)
					{
						if (numberChannel == 0)
						{
							element->setSense(_sense /= 1000000);
						}
						else element->setSense(_sense );
					}
					else if (sUnitSense == g_sUNIT_MV)
					{
						if (numberChannel == 0)
						{
							element->setSense(_sense /= 1000);
						}
						else element->setSense(_sense );
					}
					else if (sUnitSense == g_sUNIT_KV)
					{
						if (numberChannel == 0)
						{
							element->setSense(_sense *= 1000);
						}
						else element->setSense(_sense );
					}
					else if (sUnitSense == g_sUNIT_V)
						element->setSense(_sense);
					else if (sUnitSense == g_sUNIT_OM)
					{
						_sense *= 0.0029 * 2 / 1e6;
						element->setSense(_sense);
					}
					++numberChannel;
				}
			}
			else
			{
				if (sUnitSense == g_sUNIT_MKV)
					channel_->setSense(_sense /= 1000000);
				else if (sUnitSense == g_sUNIT_MV)
					channel_->setSense(_sense /= 1000);
				else if (sUnitSense == g_sUNIT_KV)
					channel_->setSense(_sense *= 1000);
				else if (sUnitSense == g_sUNIT_V)
					channel_->setSense(_sense);
				else if (sUnitSense == g_sUNIT_OM)
				{
					_sense *= 0.0029 * 2 / 1e6;
					channel_->setSense(_sense);
				}
			}
		}

		/*if (channels_.size() > 1)
		{
			for (auto& element : channels_)
			{
				if (element->isSupportAmplify())
					m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", element->getMaxLevel(), m_unit, element->getRange(), element->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
				else
					m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", element->getMaxLevel(), m_unit, element->getRange());//L"Диапазон: %g %s (до %g дБ)"
			}
		}
		else
		{
			if (channel_->isSupportAmplify())
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L") " + g_sAmplifier + L" %g", channel_->getMaxLevel(), m_unit, channel_->getRange(), channel_->getAmplify());//L"Диапазон: %g %s (до %g дБ)"
			else
				m_range.Format(g_sRange + L": %g %s (" + g_sTo + L" %g " + g_sUNIT_dB + L")", channel_->getMaxLevel(), m_unit, channel_->getRange());//L"Диапазон: %g %s (до %g дБ)"
		}*/
		update_range();
	}
	catch (boost::bad_lexical_cast&)
	{		
		if (!m_sense.IsEmpty() && !!m_sense.Compare(_T("-")))
			m_sense = sense;
	}
	
	UpdateData( FALSE );
	e->SetSel(ret);
}

void CChannelTab::OnReferenceChangeRequest()
{
	if (nullptr == channel_)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_REFERENCE);

	CString reference = m_reference;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		if (m_reference.Find(L",") != -1)
			m_reference.Replace(L",", L".");
		if (m_reference.Find(L"б") != -1)
			m_reference.Replace(L"б", L".");
		if (m_reference.Find(L"ю") != -1)
			m_reference.Replace(L"ю", L".");
		if (m_reference.Find(L"\\") != -1)
			m_reference.Replace(L"\\", L".");
		if (m_reference.Find(L"\\") != -1)
			m_reference.Replace(L"\\", L".");
		if (m_reference.Find(L"/") != -1)
			m_reference.Replace(L"/", L".");
		if (channels_.size() > 1)
		{

			for (auto& element : channels_)
			{
				element->setReference(boost::lexical_cast<float>((LPCTSTR)m_reference));
			}
		}
		else
		{
			channel_->setReference(boost::lexical_cast<float>((LPCTSTR)m_reference));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_reference.IsEmpty())
			m_reference = reference;
	}

	UpdateData( FALSE );
	e->SetSel(ret);
}

void CChannelTab::OnShiftChangeRequest()
{
	if (nullptr == channel_)
		return;
	CEdit* e = (CEdit*)GetDlgItem(IDC_SHIFT);

	CString shift = m_shift;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_shift.Find(L",") != -1)
			m_shift.Replace(L",", L".");
		if (m_shift.Find(L"б") != -1)
			m_shift.Replace(L"б", L".");
		if (m_shift.Find(L"ю") != -1)
			m_shift.Replace(L"ю", L".");
		if (m_shift.Find(L"\\") != -1)
			m_shift.Replace(L"\\", L".");
		if (m_shift.Find(L"\\") != -1)
			m_shift.Replace(L"\\", L".");
		if (m_shift.Find(L"/") != -1)
			m_shift.Replace(L"/", L".");

		if (channels_.size() > 1)
		{

			for (auto& element : channels_)
			{
				element->setShift(boost::lexical_cast<float>((LPCTSTR)m_shift));
			}
		}
		else
		{
			channel_->setShift(boost::lexical_cast<float>((LPCTSTR)m_shift));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_shift.IsEmpty() && !!m_shift.Compare( _T("-") ))
			m_shift = shift;
	}

	UpdateData( FALSE );
	e->SetSel(ret);
}

void CChannelTab::OnAmplifyChangeRequest()
{
	if (nullptr == channel_)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_AMPLIFY);

	CString amplify = m_amplify;

	UpdateData();
	DWORD ret = e->GetSel();

	try
	{
		if (m_amplify.Find(L",") != -1)
			m_amplify.Replace(L",", L".");
		if (m_amplify.Find(L"б") != -1)
			m_amplify.Replace(L"б", L".");
		if (m_amplify.Find(L"ю") != -1)
			m_amplify.Replace(L"ю", L".");
		if (m_amplify.Find(L"\\") != -1)
			m_amplify.Replace(L"\\", L".");
		if (m_amplify.Find(L"\\") != -1)
			m_amplify.Replace(L"\\", L".");
		if (m_amplify.Find(L"/") != -1)
			m_amplify.Replace(L"/", L".");
		if (channels_.size() > 1)
		{

			for (auto& element : channels_)
			{
				element->setAmplify(boost::lexical_cast<float>((LPCTSTR)m_amplify));
			}
		}
		else
		{
			channel_->setAmplify(boost::lexical_cast<float>((LPCTSTR)m_amplify));
		}
		
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_amplify.IsEmpty() && !!m_amplify.Compare( _T("-") ))
			m_amplify = amplify;
	}

	UpdateData( FALSE );
	e->SetSel(ret);
}

void CChannelTab::OnRangeChangeRequestMoreChannel()   // меняем КУ на все каналы 
{
	if (nullptr == channel_)
		return;
	
	auto amplify = channel_->getAmplify();
	if (channels_.size() > 1)
	{
	    for (auto& element : channels_)
		{
			if (element->isFeedBackChannel())
				continue;
			element->setAmplifyGain(amplify);
			Sleep(100);
		}
		ZClose(channel_->getType(), channel_->getDSP());
	}
	SetTimer(2, 500, NULL);
}

void CChannelTab::OnRangeChangeRequest(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (nullptr == channel_)
		return;

	LPNMUPDOWN pNMUD = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (channels_.size() > 1)
	{
		long mask(0);
		for (auto& element : channels_)
		{
			if (element->isFeedBackChannel())
				continue;
			mask |= (1 << element->getDeviceID());
			if (element->is_zsp())
			{
				if (pNMUD->iDelta < 0)
					element->setNextRange(false);
				else
					element->setPrevRange(false);
			}
		}

		if (pNMUD->iDelta < 0)
			channels_.front()->setNextRange(mask);
		else
			channels_.front()->setPrevRange(mask);
	}
	else
	{
		if (pNMUD->iDelta < 0)
			channel_->setNextRange();
		else
			channel_->setPrevRange();
	}

	SetTimer(2, 500, NULL); // для обновления диапазона через некоторое время
}

void CChannelTab::OnIcpOnOff()       //////////////  Групповая настройка  ICP / галочка ICP  в меню групповые настройки
{
	if (nullptr == channel_)
		return;
	
	UpdateData();  //синхронизирует значения переменных с элементами управления.
	
	/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if (channels_.size() > 1)
	{
		
		std::map<long, long> OnOffChannels;
		
		for (auto& element : channels_) //   цикл по всем элементам контейнера channels_
		{
			if (element->getType() == 30 || element->getType() == 37)  // для каналов zsp 30 тип ус-тв
			{
				element->setICP(long(m_icp));
			}
			if (element->getType() == 14)
			{
				long mask(0);
				long module = element->getDevice()->getModule();

				long channelIndex = element->getDeviceID();
								
					if (OnOffChannels.count(module))
					{
						long inMapRezult = OnOffChannels[module];
						inMapRezult |= (1 << channelIndex);
						OnOffChannels[module] = inMapRezult;
					}
					else
					{
						OnOffChannels[module] = (mask |= (1 << channelIndex));
					}
			}
		}

		for (auto& [modul, mask] : OnOffChannels) {
			channels_[0]->setICPMoreChannel(modul, mask, m_icp);
		}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
		for (auto& element : channels_)
		{
			if (auto mode = element->getAdcInputMode(); mode != 1 && mode != 3)  //получает текущий режим входа для данного канала
			{
				m_hpfCheck.SetCheck(uint16_t(m_icp));
				m_hpf = m_icp;
			}

			m_chargeCheck.EnableWindow(m_state == 1 && 0 == element->getICP() && 0 == element->getTensoIcp());

			bool bEnable = m_state == 1 && !element->isSupportSensorStruct() && 0 == element->getCharge();
			if (bEnable)
			{
				if (element->getICP() && !element->getTensoIcp())
					bEnable = false;
			}
			m_tensoIcpCheck.EnableWindow(bEnable);
			m_tensoIcpSettingsButton.EnableWindow(bEnable && m_tensoIcp);

		}
	}
	else    // 1канал ICP в в леню пкм
	{
		
		channel_->setICP(long(m_icp)); 
		

		if (auto mode = channel_->getAdcInputMode(); mode != 1 && mode != 3)
		{
			m_hpfCheck.SetCheck(uint16_t(m_icp));
			m_hpf = m_icp;
		}

		m_chargeCheck.EnableWindow(m_state == 1 && 0 == channel_->getICP() && 0 == channel_->getTensoIcp());
		bool bEnable = m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getCharge();
		if (bEnable)
		{
			if (channel_->getICP() && !channel_->getTensoIcp())
				bEnable = false;
		}
		m_tensoIcpCheck.EnableWindow(bEnable);
		m_tensoIcpSettingsButton.EnableWindow(bEnable && m_tensoIcp);
	}

	OnHpfOnOff();


}

void CChannelTab::OnTensoIcpOnOff()
{
	if (nullptr == channel_)
		return;

	auto tensoIcp = m_tensoIcp;
	UpdateData();
	m_tensoIcpSettingsButton.EnableWindow(m_tensoIcp);
	if (tensoIcp == m_tensoIcp)
		return;

	channel_->setTensoIcp(long(m_tensoIcp));

	m_hpfCheck.SetCheck(uint16_t(m_tensoIcp));
	m_hpf = m_tensoIcp;

	m_chargeCheck.EnableWindow(m_state == 1 && 0 == channel_->getTensoIcp());
		
	if (m_tensoIcp)
	{
		m_UnitSenseCombo.SelectString(-1, g_sUNIT_V);
		m_UnitSenseCombo.EnableWindow(FALSE);
		m_unitCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_SENSE)->EnableWindow(FALSE);
		
		auto _sense = channel_->getTensoResistance() * 0.0029 * channel_->getTensoCoeff() / 1e6;

		m_UnitSense = g_sUNIT_V;
		m_sense.Format(_T("%g"), _sense);
		m_unit = g_sUNIT_MKMM;
		m_reference = L"1";
	}
	else
	{
		m_UnitSenseCombo.SelectString(-1, g_sUNIT_V);
		m_UnitSenseCombo.EnableWindow(TRUE);
		m_unitCombo.EnableWindow(TRUE);
		GetDlgItem(IDC_SENSE)->EnableWindow(TRUE);

		m_UnitSense = g_sUNIT_V;
		m_sense = L"0.001";
		m_unit = g_sUNIT_MV;
		m_reference = L"0.001";
	}
	m_icpCheck.EnableWindow(m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getCharge() && 0 == channel_->getTensoIcp());

	UpdateData(FALSE);

	OnReferenceChangeRequest();
	OnSenseChangeRequest();
	OnUnitChangeRequest();
	OnUnitSenseChangeRequest();
	OnHpfOnOff();
}

void CChannelTab::OnTensoIcpSettingsClick()
{
	std::unique_ptr<CDialog> pTensoIcpSettingsWindow(new CTensoIcpDlg(channel_));

	pTensoIcpSettingsWindow->DoModal();

	auto _sense = channel_->getTensoResistance() * 0.0029 * channel_->getTensoCoeff();
	CString sUnit = channel_->getUnit().c_str();
	if (sUnit == g_sUNIT_MKMM)
		_sense /= 1e6;
	else if (sUnit == g_sUNIT_PA)
		_sense /= channel_->getTensoYoungMod() * 1e9;
	else if (sUnit == g_sUNIT_KPA)
		_sense /= channel_->getTensoYoungMod() * 1e6;
	else if (sUnit == g_sUNIT_MPA)
		_sense /= channel_->getTensoYoungMod() * 1e3;
	else if (sUnit == g_sUNIT_KGSMM2)
		_sense /= channel_->getTensoYoungMod() * 101.9716212978;
	m_sense.Format(_T("%g"), _sense);
	m_unit = channel_->getUnit().c_str();
	m_reference.Format(_T("%g"), channel_->Reference());
	m_UnitSense = g_sUNIT_V;

	UpdateData(FALSE);

	OnReferenceChangeRequest();
	OnSenseChangeRequest();
	OnUnitSenseChangeRequest();
	OnUnitChangeRequest();
	OnUnitSenseChangeRequest();
}

void CChannelTab::OnChargeOnOff()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	channel_->setCharge(long(m_charge));

	m_hpfCheck.SetCheck(uint16_t(m_charge));
	m_hpf = m_charge;

	m_icpCheck.EnableWindow(m_state == 1 && 0 == channel_->getCharge() && 0 == channel_->getTensoIcp());
	bool bEnable = m_state == 1 && !channel_->isSupportSensorStruct() && 0 == channel_->getCharge();
	if (bEnable)
	{
		if (channel_->getICP() && !channel_->getTensoIcp())
			bEnable = false;
	}
	m_tensoIcpCheck.EnableWindow(bEnable);
	m_tensoIcpSettingsButton.EnableWindow(bEnable && m_tensoIcp);

	OnHpfOnOff();
}

void CChannelTab::OnTedsClick()
{
	if (nullptr == channel_ )
		return;

	if (channel_->GetBuiltinGeneratorEnabled() == false)
	{
		UpdateData();
		GetDlgItem(IDC_TEDS)->EnableWindow(FALSE);

		teds_.setChan(channel_->getDeviceID());
		teds_.setType(channel_->getType());
		teds_.setDSP(channel_->getDSP());
		teds_.SetParent(m_hWnd);
		teds_.setAdress(0x00);

		m_pThread = AfxBeginThread(readInfoTEDS, (LPVOID)(&teds_));
		CRect rect;
		GetDlgItem(IDC_TEDS)->GetWindowRect(&rect);
		GetDlgItem(IDC_TEDS)->GetParent()->ScreenToClient(&rect);
		m_progress.Create(PBM_SETMARQUEE | WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect, this, 1);
		m_progress.SetMarquee(TRUE, 20);
	}
	else
		MessageBox(g_sOffTEDS, L"Device Manager",MB_ICONEXCLAMATION);
}

LRESULT CChannelTab::OnStateTeds(WPARAM wParam, LPARAM lParam)
{
	if (m_pThread == nullptr ||
		WaitForSingleObject(m_pThread, 500) != WAIT_TIMEOUT)
	{
		long ret = teds_.getStatusRead();
		if (ret == 0)
		{
			m_progress.DestroyWindow();			
			if ( auto unit = teds_.getUnit(); !unit.IsEmpty() )
			{
				m_unit = unit;
				UpdateData(FALSE);
				if ( auto sense = teds_.getUnitSense(); !sense.IsEmpty() )
				{
					channel_->setUnitSense( sense.GetString() );
					m_UnitSenseCombo.SelectString( 0, sense );
				}

				OnUnitChangeRequest();	
			}
			
			if ( auto comment = teds_.getComment(); !comment.IsEmpty() )
			{
				m_comment = comment;
				channel_->setComment( ( LPCTSTR ) m_comment );
				UpdateData( FALSE );
			}

			if ( auto senc = teds_.getSensitivity(); !senc.IsEmpty() )
			{
				m_sense = senc;
				UpdateData(FALSE);
				OnSenseChangeRequest();
			}

			if ( m_icpCheck.GetCheck() == 0 )
			{
				m_icpCheck.SetCheck		( 1 );
				channel_->setICP		( 1 );
				m_chargeCheck.SetCheck	( 0 );
				channel_->setCharge		( 0 );

				UpdateData();
			}

			
		}
		else
		{
			m_progress.DestroyWindow();
			//m_progress.SetState(PBST_ERROR);
			CString error;
			switch (ret)
			{
			case  1: error = g_sSensorEmpty; break;
			case  2: error = g_sDataSensorIncorrectlyRecorded; break;
			case  4:
			case -3: error = g_sSensorNotSupported; break;
			case -1: error = g_sErrorReadingSensor; break;
			case -4:
			case -2: error = g_sSensorNotDetected; break;
			default:
				break;
			}
			TRACE(L" Ошибка чтения тедс = %d\n", ret);
			AfxMessageBox(error);
		}
	}
	else
	{
		m_progress.DestroyWindow();
		AfxMessageBox(g_sErrorReadingSensor);
	}
	
	GetDlgItem(IDC_TEDS)->EnableWindow(TRUE);
	return 0;
}

UINT CChannelTab::readInfoTEDS(LPVOID pParam)
{
	TEDS * teds = (TEDS*)pParam;
	teds->readData();
	return 0;
}

void CChannelTab::OnCancel()
{
	if (nullptr == channel_)
		return;
	
	long mask(0);
	for (auto& element : channels_)
	{
		if (element->isFeedBackChannel())
			continue;
		mask |= (1 << element->getDeviceID());
	}

	if (mementos.size() > 1)
		mementos.front()->restore_(mask);
	else
		mementos.front()->restore();
	
	CTab::OnCancel();
}

void CChannelTab::OnUserCancel() ////////////////////////////кнопка Отмена на групповом окне для возврата ICP и остальных каналов
{
	UpdateData();

	if (nullptr == channel_)
		return;

	std::vector<std::pair<long, long>> modulAndMask; // модуль и маска для включения
	std::vector<std::pair<long, long>> offICP; // для выключения ICP
	std::vector<zetlab::std::channel_memento_ptr> changeMomento; // mementos только 1 раз для 1 ус-ва, совместно с modulAndMask, добавление параллельно 
	
	if (mementos.size() > 1)
	{
		int countMomentos = 0; // проход по всем mementos
		for (auto& element : channels_) 
		{
			if (element->isFeedBackChannel())
				continue;
		
			long afterICP = mementos[countMomentos].get()->returnICP(); //проверка статуса ICP до открытия окна св-тв
			long afteramplifygain = mementos[countMomentos].get()->afteramplify();

			if (element->is_zsp())
			{
				element->setICP(afterICP);
				element->setAmplifyGain(afteramplifygain);
				continue;
			}
		
			long mask(0);
			long module = element->getDevice()->getModule();
			long channelIndex = element->getDeviceID();
			if (afterICP) { //для выключения ICP
				if (auto it = std::find_if(modulAndMask.begin(), modulAndMask.end(), //проверка на наличие модуля
					[&](const auto& p) { return p.first == module; });
					it != modulAndMask.end())
				{
					long inRezult = it->second;
					inRezult |= (1 << channelIndex);
					it->second = inRezult;
				}
				else
				{
					modulAndMask.push_back({ module, (mask |= (1 << channelIndex)) });
					changeMomento.push_back(mementos[countMomentos]);
				}
			}
			else   /// для выключения ICP 
			{
				if (auto it = std::find_if(offICP.begin(), offICP.end(),
					[&](const auto& p) { return p.first == module; });
					it != offICP.end())
				{
					long inRezult = it->second;
					inRezult |= ( 1<< channelIndex);
					it->second = inRezult;
				}
				else
				{
					offICP.push_back({ module, (mask |= (1 << channelIndex)) });
				}
			}
			++countMomentos;
		}

		for (int i = 0; i < changeMomento.size(); ++i)
		{
			long currentModul = modulAndMask[i].first;
			changeMomento[i]->restore_anyDevice(modulAndMask[i].first, modulAndMask[i].second);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));   //  стояло на тестах 300
			if (auto it = std::find_if(offICP.begin(), offICP.end(),
				[&](const auto& p) { return p.first == currentModul; });
				it != offICP.end())
			{
				this->channel_->setICPMoreChannel(it->first, it->second, 0);
				offICP.erase(it);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));  // //  стояло на тестах 300
		}
		if (!offICP.empty()) // для откл ICP на устройствах, когда не были включены изначально
		{
			for (auto &[modul, mask] : offICP)
			{
				this->channel_->setICPMoreChannel(modul, mask, 0);
			}
		}
	}
	else
	{
	mementos.front()->restore();
	}
	//UpdateData();
	//SendMessage(WM_USER_SAVE);
}

void CChannelTab::SetVisible(_In_ int Show)
{
	long m_tenso_mode; 
	
	GetDlgItem(IDC_LABEL_NAME)		->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_COMMENT)	->ShowWindow(Show);
	GetDlgItem(IDC_DIFF)			->ShowWindow(Show == SW_SHOW && channel_->isSupportDiff() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_SENSE)		->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_REFERENCE)	->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_SHIFT)		->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_AMPLIFY)	->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_LEVEL)		->ShowWindow(Show);
	GetDlgItem(IDC_ICP)				->ShowWindow(Show == SW_SHOW && channel_->isHaveICP() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_TENSO_ICP)		->ShowWindow(Show == SW_SHOW && channel_->isHaveTensoICP() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_TENSO_ICP_SETTINGS)->ShowWindow(Show == SW_SHOW && channel_->isHaveTensoICP() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CHARGE)			->ShowWindow(Show == SW_SHOW && channel_->isHaveCharge() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_TEDS)			->ShowWindow(Show == SW_SHOW && channel_->isHaveTEDS() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_TENSO_BUTTON)	->ShowWindow(Show == SW_SHOW && channel_->isHaveTenso(m_tenso_mode) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_NAME)            ->ShowWindow(m_b_metadata ? SW_HIDE : Show);
	GetDlgItem(IDC_LABEL_NAME_LOCATION)->ShowWindow(m_b_metadata ? Show : SW_HIDE);
	GetDlgItem(IDC_NAME_LOCATION)->ShowWindow(m_b_metadata ? Show : SW_HIDE);
	GetDlgItem(IDC_LABEL_NAME_CHANNEL)->ShowWindow(m_b_metadata ? Show : SW_HIDE);
	GetDlgItem(IDC_NAME_CHANNEL)->ShowWindow(m_b_metadata ? Show : SW_HIDE);
	GetDlgItem(IDC_COMMENT)			->ShowWindow(Show);
	GetDlgItem(IDC_COOR_X)			->ShowWindow(Show);
	GetDlgItem(IDC_COOR_Y)			->ShowWindow(Show);
	GetDlgItem(IDC_COOR_Z)			->ShowWindow(Show);
	GetDlgItem(IDC_COOR_P)			->ShowWindow(Show);
	GetDlgItem(IDC_LABEL_COORDINATE)->ShowWindow(Show);
	GetDlgItem(IDC_STATICY)		    ->ShowWindow(Show);
	GetDlgItem(IDC_STATICX)			->ShowWindow(Show);
	GetDlgItem(IDC_STATICZ)			->ShowWindow(Show);
	GetDlgItem(IDC_STATICP)			->ShowWindow(Show);
	GetDlgItem(IDC_UNIT)			->ShowWindow(Show);
	GetDlgItem(IDC_UNIT_SENSE)		->ShowWindow(Show);
	GetDlgItem(IDC_STATICSLASH)		->ShowWindow(Show);
	GetDlgItem(IDC_SENSE)			->ShowWindow(Show);
	GetDlgItem(IDC_REFERENCE)		->ShowWindow(Show);
	GetDlgItem(IDC_SHIFT)			->ShowWindow(Show);
	GetDlgItem(IDC_AMPLIFY)			->ShowWindow(Show);
	GetDlgItem(IDC_LEVEL)			->ShowWindow(Show);
	GetDlgItem(IDC_SEPARATOR)		->ShowWindow(Show);
	GetDlgItem(IDC_SPIN)			->ShowWindow(Show);
	GetDlgItem(IDC_RANGE)			->ShowWindow(Show);
	GetDlgItem(IDC_HPF)				->ShowWindow(Show == SW_SHOW && !channel_->isTensostation() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_REFERENCE) ->ShowWindow(Show == SW_SHOW && !channel_->isTensostation() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_REFERENCE)		->ShowWindow(Show == SW_SHOW && !channel_->isTensostation() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_INPUT_RESISTANCE)->ShowWindow(Show == SW_SHOW && channel_->isSupportInputModeAdc() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_INPUT_RESISTANCE_COMBO)->ShowWindow(Show == SW_SHOW && channel_->isSupportInputModeAdc() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_SELFTEST)		->ShowWindow(Show == SW_SHOW && channel_->isSupportInputModeAdc() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_NOISE)			->ShowWindow(Show == SW_SHOW && channel_->isSupportInputModeAdc() ? SW_SHOW : SW_HIDE);
	
	CRect Rect;
	// переносим контролы на нужные места
	if (Show == SW_SHOW)
	{
		CPoint Point;
		GetDlgItem(IDC_DIFF)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		Point.y = Rect.top;
		GetDlgItem(IDC_LABEL_STATE)->SetWindowPos(nullptr, Point.x, Point.y, 0, 0, SWP_NOSIZE);
	}
	else
	{
		GetDlgItem(IDC_LABEL_NAME)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		GetDlgItem(IDC_LABEL_STATE)->SetWindowPos(nullptr, Rect.left, Rect.top, 0, 0, SWP_NOSIZE);
		GetDlgItem(IDC_NAME)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		GetDlgItem(IDC_STATE)->SetWindowPos(nullptr, Rect.left, Rect.top, 0, 0, SWP_NOSIZE);
	}
}

void CChannelTab::SetVisibledemo()
{

	GetDlgItem(IDC_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMMENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COOR_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_COOR_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_COOR_Z)->EnableWindow(FALSE);
	GetDlgItem(IDC_COOR_P)->EnableWindow(FALSE);
	GetDlgItem(IDC_UNIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DIFF)->EnableWindow(FALSE);
	GetDlgItem(IDC_SENSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UNIT_SENSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_REFERENCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHIFT)->EnableWindow(FALSE);
	GetDlgItem(IDC_AMPLIFY)->EnableWindow(FALSE);
	GetDlgItem(IDC_LEVEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_ICP)->EnableWindow(FALSE);
	GetDlgItem(IDC_TENSO_ICP)->EnableWindow(FALSE);
	GetDlgItem(IDC_TENSO_ICP_SETTINGS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHARGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_HPF)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_TENSO_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_STATE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SELFTEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_NOISE)->EnableWindow(FALSE);
}

void CChannelTab::OnHpfOnOff()
{
	
	if (nullptr == channel_)
		return;

	UpdateData();
	if (channels_.size() > 1)
	{

		for (auto& element : channels_)
		{
			element->setHpf((uint16_t)m_hpf);

			m_selftestCheck.EnableWindow(m_state == 1 && element->isSupportInputModeAdc() && element->getAdcInputMode() != 2 && element->getAdcInputMode() != 3);
			m_noiseCheck.EnableWindow(m_state == 1 && element->isSupportInputModeAdc() && element->getAdcInputMode() != 2 && element->getAdcInputMode() != 1);
		}
	}
	else
	{

		channel_->setHpf((uint16_t)m_hpf);

		m_selftestCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 3);
		m_noiseCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 1);
	}
}

void CChannelTab::OnInputResistanceSelect()
{
	if (nullptr == channel_)
		return;
	if (channels_.size() > 1)
	{

		for (auto& element : channels_)
		{
			element->setInputResistance((uint16_t)m_inputResistanceCombo.GetCurSel());
		}
	}
	else
	{
		channel_->setInputResistance((uint16_t)m_inputResistanceCombo.GetCurSel());

	}
}

void CChannelTab::HideControls()
{
	GetDlgItem(IDC_STATICY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATICX)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATICZ)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATICP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COOR_X)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COOR_Y)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COOR_Z)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COOR_P)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_COMMENT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMMENT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_COORDINATE)->ShowWindow(SW_HIDE);
	CRect _RectDiff;
	GetDlgItem(IDC_LABEL_SENSE)->GetWindowRect(&_RectDiff);
	CRect rect;

	GetDlgItem(IDC_LABEL_SENSE)->GetWindowRect(&rect);
	GetDlgItem(IDC_LABEL_SENSE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height()*2;
	rect.bottom -= _RectDiff.Height()*2;
	GetDlgItem(IDC_LABEL_SENSE)->MoveWindow(rect);

	GetDlgItem(IDC_SENSE)->GetWindowRect(&rect);
	GetDlgItem(IDC_SENSE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_SENSE)->MoveWindow(rect);

	GetDlgItem(IDC_UNIT_SENSE)->GetWindowRect(&rect);
	GetDlgItem(IDC_UNIT_SENSE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_UNIT_SENSE)->MoveWindow(rect);

	GetDlgItem(IDC_STATICSLASH)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATICSLASH)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_STATICSLASH)->MoveWindow(rect);

	GetDlgItem(IDC_UNIT)->GetWindowRect(&rect);
	GetDlgItem(IDC_UNIT)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_UNIT)->MoveWindow(rect);

	GetDlgItem(IDC_LABEL_REFERENCE)->GetWindowRect(&rect);
	GetDlgItem(IDC_LABEL_REFERENCE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_LABEL_REFERENCE)->MoveWindow(rect);

	GetDlgItem(IDC_REFERENCE)->GetWindowRect(&rect);
	GetDlgItem(IDC_REFERENCE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_REFERENCE)->MoveWindow(rect);

	GetDlgItem(IDC_LABEL_SHIFT)->GetWindowRect(&rect);
	GetDlgItem(IDC_LABEL_SHIFT)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_LABEL_SHIFT)->MoveWindow(rect);

	GetDlgItem(IDC_SHIFT)->GetWindowRect(&rect);
	GetDlgItem(IDC_SHIFT)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_SHIFT)->MoveWindow(rect);

	GetDlgItem(IDC_LABEL_AMPLIFY)->GetWindowRect(&rect);
	GetDlgItem(IDC_LABEL_AMPLIFY)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_LABEL_AMPLIFY)->MoveWindow(rect);

	GetDlgItem(IDC_AMPLIFY)->GetWindowRect(&rect);
	GetDlgItem(IDC_AMPLIFY)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_AMPLIFY)->MoveWindow(rect);

	GetDlgItem(IDC_SEPARATOR)->GetWindowRect(&rect);
	GetDlgItem(IDC_SEPARATOR)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_SEPARATOR)->MoveWindow(rect);

	GetDlgItem(IDC_LABEL_LEVEL)->GetWindowRect(&rect);
	GetDlgItem(IDC_LABEL_LEVEL)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_LABEL_LEVEL)->MoveWindow(rect);

	GetDlgItem(IDC_LEVEL)->GetWindowRect(&rect);
	GetDlgItem(IDC_LEVEL)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_LEVEL)->MoveWindow(rect);

	GetDlgItem(IDC_RANGE)->GetWindowRect(&rect);
	GetDlgItem(IDC_RANGE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_RANGE)->MoveWindow(rect);

	GetDlgItem(IDC_SPIN)->GetWindowRect(&rect);
	GetDlgItem(IDC_SPIN)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_SPIN)->MoveWindow(rect);

	GetDlgItem(IDC_DIFF)->GetWindowRect(&rect);
	GetDlgItem(IDC_DIFF)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() *2;
	rect.bottom -= _RectDiff.Height()*2;
	GetDlgItem(IDC_DIFF)->MoveWindow(rect);

	GetDlgItem(IDC_HPF)->GetWindowRect(&rect);
	GetDlgItem(IDC_HPF)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_HPF)->MoveWindow(rect);

	GetDlgItem(IDC_SELFTEST)->GetWindowRect(&rect);
	GetDlgItem(IDC_SELFTEST)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_SELFTEST)->MoveWindow(rect);

	GetDlgItem(IDC_NOISE)->GetWindowRect(&rect);
	GetDlgItem(IDC_NOISE)->GetParent()->ScreenToClient(&rect);
	rect.top -= _RectDiff.Height() * 2;
	rect.bottom -= _RectDiff.Height() * 2;
	GetDlgItem(IDC_NOISE)->MoveWindow(rect);
}

void CChannelTab::OnTensoClick()
{
	std::unique_ptr<СTensoDlg> tens_window(new СTensoDlg(channels_));

	if (tens_window->DoModal() != IDOK)
		return;

	for (auto& element : channels_)
	{
		long state(0), tensoMode(-1), pot1(0), pot2(0);
		double freq(0.0), ampl(0.0), offset(0.0);
		if (element->GetBuiltinGeneratorSettings(state, freq, ampl, offset) &&
			element->isHaveTenso(tensoMode, &pot1, &pot2);
			state && offset > 0)
		{
			auto _sense = offset * element->getTensoCoeff();
			if (tensoMode == 1)
				_sense /= 2;
			else if (tensoMode == 2)
				_sense /= 4;

			CString sUnit = element->getUnit().c_str();
			if (sUnit == g_sUNIT_MKMM)
				_sense /= 1e6;
			else if (sUnit == g_sUNIT_PA)
				_sense /= element->getTensoYoungMod() * 1e9;
			else if (sUnit == g_sUNIT_KPA)
				_sense /= element->getTensoYoungMod() * 1e6;
			else if (sUnit == g_sUNIT_MPA)
				_sense /= element->getTensoYoungMod() * 1e3;
			else if (sUnit == g_sUNIT_KGSMM2)
				_sense /= element->getTensoYoungMod() * 101.9716212978;
			m_sense.Format(_T("%g"), _sense);
			m_unit = element->getUnit().c_str();
			m_reference.Format(_T("%g"), element->Reference());
			m_UnitSense = g_sUNIT_V;
			m_amplify.Format(_T("%g"), tens_window->m_ampl);

			if (tens_window->m_shift != 0.)
			{
				auto val = (tens_window->m_shift + element->Shift(true)) * element->Sense(true) * element->Amplify(true);
				auto dc = val / _sense / tens_window->m_ampl;
				m_shift.Format(_T("%g"), dc);
			}

			UpdateData(FALSE);

			OnAmplifyChangeRequest();
			OnShiftChangeRequest();
			OnReferenceChangeRequest();
			OnSenseChangeRequest();
			OnUnitSenseChangeRequest();
			OnUnitChangeRequest();
			OnUnitSenseChangeRequest();

		}
	}

	auto p = GetParent()->GetParent();
	if (p)
		::SendMessage(p->m_hWnd, WM_COMMAND, (BN_CLICKED << 16) | IDOK, (LPARAM)(p->m_hWnd));
}

void CChannelTab::OnSelftestOnOff()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	channel_->setAdcInputMode((uint16_t)m_selftest ? 1 : 0);

	bool bEnableHpf = m_state == 1 && !channel_->isSupportSensorStruct();
	if (bEnableHpf)
	{
		if (channel_->isSupportInputModeAdc())
		{
			auto mode = channel_->getAdcInputMode();
			if (mode == 1 || mode == 3)
				bEnableHpf = false;
		}
	}
	m_hpfCheck.EnableWindow(bEnableHpf);
	m_noiseCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 1);
}

void CChannelTab::OnNoiseOnOff()
{
	if (nullptr == channel_)
		return;

	UpdateData();

	channel_->setAdcInputMode((uint16_t)m_noise ? 3 : 0);

	bool bEnableHpf = m_state == 1 && !channel_->isSupportSensorStruct();
	if (bEnableHpf)
	{
		if (channel_->isSupportInputModeAdc())
		{
			auto mode = channel_->getAdcInputMode();
			if (mode == 1 || mode == 3)
				bEnableHpf = false;
		}
	}
	m_hpfCheck.EnableWindow(bEnableHpf);
	m_selftestCheck.EnableWindow(m_state == 1 && channel_->isSupportInputModeAdc() && channel_->getAdcInputMode() != 2 && channel_->getAdcInputMode() != 3);
}


