#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "../../../algo/utils/clipboard/clipboard.hpp"
#include "../../../algo/path/path.h"
#include "../../../algo/zsp_factory/zsp_common.hpp"
#include "zet037_tenso_channel_tab.h"

#include <Dialog_ZET/ConnectToZetViewResult.h>
#include <Dialog_ZET/ConnectToZetPath.h>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/lexical_cast.hpp>

#include "resource/Phrases.h"

extern CString g_sNameOrientation[7];

std::map<CString, double> g_tenso_resistor_unit_k =
{
	{ g_sUNIT_MKMM,		1.e-3 },
	{ g_sUNIT_MMPERM,	1.e0 },
	{ g_sUNIT_MPERM,	1.e3 },
	{ g_sUNIT_PERCENT,	1.e1 },
	{ g_sUNIT_PA,		1.e3 },
	{ g_sUNIT_KPA,		1.e6 },
	{ g_sUNIT_MPA,		1.e9 },
	{ g_sUNIT_KGSMM2,	0.980665e10 },
	{ g_sUNIT_HM,		1.e3 },
	{ g_sUNIT_KHM,		1.e6 },
};
std::list<CString> g_tenso_resistor_units =
{
	g_sUNIT_MKMM,
	g_sUNIT_MMPERM,
	g_sUNIT_MPERM,
	g_sUNIT_PERCENT,
	g_sUNIT_PA,
	g_sUNIT_KPA,
	g_sUNIT_MPA,
	g_sUNIT_KGSMM2,
	g_sUNIT_HM,
	g_sUNIT_KHM,
};

zet037_tenso_channel_tab::zet037_tenso_channel_tab( CWnd* pParent )
	: CTab(zet037_tenso_channel_tab::IDD, pParent )
	, m_channel(nullptr)
{}

void zet037_tenso_channel_tab::DoDataExchange( CDataExchange* pDX )
{
	CTab::DoDataExchange(pDX);

	DDX_CBStringExact(pDX, IDC_NAME_ZET037, m_name);
	DDX_Control(pDX, IDC_NAME_ZET037, m_name_combo);
	DDX_Text(pDX, IDC_COMMENT_ZET037, m_comment);
	DDX_Text(pDX, IDC_COOR_X_ZET037, m_coordinate_x);
	DDX_Control(pDX, IDC_COOR_X_ZET037, m_coordinate_x_edit);
	DDX_Text(pDX, IDC_COOR_Y_ZET037, m_coordinate_y);
	DDX_Control(pDX, IDC_COOR_Y_ZET037, m_coordinate_y_edit);
	DDX_Text(pDX, IDC_COOR_Z_ZET037, m_coordinate_z);
	DDX_Control(pDX, IDC_COOR_Z_ZET037, m_coordinate_z_edit);
	DDX_CBStringExact(pDX, IDC_COOR_P_ZET037, m_coordinate_p);
	DDX_Control(pDX, IDC_COOR_P_ZET037, m_coordinate_p_combo);
	DDX_Control(pDX, IDC_SENSOR_TYPE_ZET037, m_sensor_type_combo);
	DDX_Control(pDX, IDC_SCHEME_ZET037, m_scheme_combo);
	DDX_Text(pDX, IDC_EXC_ZET037, m_exc);
	DDX_Control(pDX, IDC_EXC_ZET037, m_exc_edit);
	DDX_Text(pDX, IDC_RESISTANCE_ZET037, m_resistance);
	DDX_Control(pDX, IDC_RESISTANCE_ZET037, m_resistance_edit);
	DDX_Control(pDX, IDC_NSH_ZET037, m_nsh_combo);
	DDX_Control(pDX, IDC_TC_TYPE_ZET037, m_tc_type_combo);
	DDX_Text(pDX, IDC_SENSE_ZET037, m_sense);
	DDX_Control(pDX, IDC_SENSE_ZET037, m_sense_edit);
	DDX_CBStringExact(pDX, IDC_UNIT_SENSE_ZET037, m_unit_sense);
	DDX_Control(pDX, IDC_UNIT_SENSE_ZET037, m_unit_sense_combo);
	DDX_CBStringExact(pDX, IDC_UNIT_ZET037, m_unit);
	DDX_Control(pDX, IDC_UNIT_ZET037, m_unit_combo);
	DDX_Control(pDX, IDC_UNIT_EX_ZET037, m_unit_combo_ex);
	DDX_Text(pDX, IDC_SHIFT_ZET037, m_shift);
	DDX_Control(pDX, IDC_SHIFT_ZET037, m_shift_edit);
	DDX_Text(pDX, IDC_SENSOR_RANGE_ZET037, m_sensor_range);
	DDX_Control(pDX, IDC_SENSOR_RANGE_ZET037, m_sensor_range_edit);
	DDX_Text(pDX, IDC_SENSOR_OUTPUT_ZET037, m_sensor_output);
	DDX_Control(pDX, IDC_SENSOR_OUTPUT_ZET037, m_sensor_output_edit);
	DDX_Text(pDX, IDC_RESISTOR_K_ZET037, m_resistor_k);
	DDX_Control(pDX, IDC_RESISTOR_K_ZET037, m_resistor_k_edit);
	DDX_Control(pDX, IDC_RESISTOR_MODE_ZET037, m_resistor_mode_combo);
	DDX_Text(pDX, IDC_POISSON_RATIO_ZET037, m_poisson_ratio);
	DDX_Control(pDX, IDC_POISSON_RATIO_ZET037, m_poisson_ratio_edit);
	DDX_Text(pDX, IDC_ELASTIC_MODULUS_ZET037, m_elastic_modulus);
	DDX_Control(pDX, IDC_ELASTIC_MODULUS_ZET037, m_elastic_modulus_edit);
	DDX_Text(pDX, IDC_OUTER_DIAMETER_ZET037, m_outer_diameter);
	DDX_Control(pDX, IDC_OUTER_DIAMETER_ZET037, m_outer_diameter_edit);
	DDX_Text(pDX, IDC_INNER_DIAMETER_ZET037, m_inner_diameter);
	DDX_Control(pDX, IDC_INNER_DIAMETER_ZET037, m_inner_diameter_edit);
	DDX_Control(pDX, IDC_LEVEL_ZET037, m_level);
	DDX_Control(pDX, IDC_SPIN_ZET037, m_spin);
	DDX_Text(pDX, IDC_RANGE_ZET037, m_range);
	DDX_Control(pDX, IDC_RANGE_ZET037, m_range_static);
	DDX_Control(pDX, IDC_SCHEME_STICK_PICTURE_ZET037, m_scheme_stick_picture);
	DDX_Control(pDX, IDC_SCHEME_CONN_PICTURE_ZET037, m_scheme_conn_picture);
}

BEGIN_MESSAGE_MAP(zet037_tenso_channel_tab, CTab )
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_CBN_EDITCHANGE(IDC_NAME_ZET037, OnNameChangeRequest)
	ON_CBN_SELCHANGE(IDC_NAME_ZET037, OnSensorSelect)
	ON_EN_CHANGE(IDC_COMMENT_ZET037, OnCommentChangeRequest)
	ON_EN_KILLFOCUS(IDC_COOR_X_ZET037, OnCoordXKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_COOR_Y_ZET037, OnCoordYKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_COOR_Z_ZET037, OnCoordZKillfocusRequest)
	ON_CBN_SELCHANGE(IDC_COOR_P_ZET037, OnCoordPChangeRequest)
	ON_CBN_EDITCHANGE(IDC_COOR_P_ZET037, OnCoordPEdit)
	ON_CBN_SELCHANGE(IDC_SENSOR_TYPE_ZET037, OnSensorTypeSelect)
	ON_CBN_SELCHANGE(IDC_SCHEME_ZET037, OnSchemeSelect)
	ON_EN_KILLFOCUS(IDC_EXC_ZET037, OnExcKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_RESISTANCE_ZET037, OnResistanceKillfocusRequest)
	ON_CBN_SELCHANGE(IDC_NSH_ZET037, OnNshSelect)
	ON_CBN_SELCHANGE(IDC_TC_TYPE_ZET037, OnTcTypeSelect)
	ON_EN_KILLFOCUS(IDC_SENSE_ZET037, OnSenseKillfocusRequest)
	ON_CBN_SELCHANGE(IDC_UNIT_SENSE_ZET037, OnUnitSenseChangeRequest)
	ON_CBN_EDITCHANGE(IDC_UNIT_SENSE_ZET037, OnUnitSenseEdit)
	ON_CBN_EDITCHANGE(IDC_UNIT_ZET037, OnUnitChangeRequest)
	ON_CBN_SELCHANGE(IDC_UNIT_ZET037, OnUnitSelect)
	ON_CBN_SELCHANGE(IDC_UNIT_EX_ZET037, OnUnitExSelect)
	ON_EN_KILLFOCUS(IDC_SHIFT_ZET037, OnShiftKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_SENSOR_RANGE_ZET037, OnSensorRangeKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_SENSOR_OUTPUT_ZET037, OnSensorOutputKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_RESISTOR_K_ZET037, OnResistorKKillfocusRequest)
	ON_CBN_SELCHANGE(IDC_RESISTOR_MODE_ZET037, OnResistorModeSelect)
	ON_EN_KILLFOCUS(IDC_POISSON_RATIO_ZET037, OnPoissonRatioKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_ELASTIC_MODULUS_ZET037, OnElasticModulusKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_OUTER_DIAMETER_ZET037, OnOuterDiameterKillfocusRequest)
	ON_EN_KILLFOCUS(IDC_INNER_DIAMETER_ZET037, OnInnerDiameterKillfocusRequest)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ZET037, OnRangeChangeRequest)
END_MESSAGE_MAP()

BOOL zet037_tenso_channel_tab::OnInitDialog()
{
	CTab::OnInitDialog();

	m_scheme_path = zpGetInstallLocation().c_str();
	m_scheme_path.Append(L"template\\tenso\\");

	GetDlgItem(IDC_LABEL_NAME_ZET037)->SetWindowText(g_sName + L":");
	GetDlgItem(IDC_LABEL_COMMENT_ZET037)->SetWindowText(g_sComment + L":");
	GetDlgItem(IDC_LABEL_COORDINATE_ZET037)->SetWindowTextW(g_sCoordinate + L":");
	GetDlgItem(IDC_LABEL_SCHEME_ZET037)->SetWindowTextW(g_sTensoScheme + L":");
	GetDlgItem(IDC_LABEL_EXC_ZET037)->SetWindowTextW(g_sTensoExc + L":");
	GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->SetWindowTextW(g_sTensoResistance + L":");
	GetDlgItem(IDC_LABEL_NSH_ZET037)->SetWindowText(g_sNsh + L":");
	GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->SetWindowText(g_sTcType + L":");
	GetDlgItem(IDC_LABEL_SENSOR_TYPE_ZET037)->SetWindowTextW(g_sTensoSensorType + L":");
	GetDlgItem(IDC_LABEL_LEVEL_ZET037)->SetWindowText(g_sIntegratedLevelSignal + L":");
	GetDlgItem(IDC_LABEL_SENSE_ZET037)->SetWindowText(g_sChannelSensitivity + L":");
	GetDlgItem(IDC_LABEL_SHIFT_ZET037)->SetWindowText(g_sOffsetDC + L":");
	GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->SetWindowText(g_sSensorRange + L":");
	GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->SetWindowText(g_sSensorOutput + L":");
	GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->SetWindowText(g_sTensoCoeff + L":");
	GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->SetWindowText(g_sTensoResistorMode + L":");
	GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->SetWindowText(g_sPoissonRatio + L":");
	GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->SetWindowText(g_sElasticModulus + L":");
	GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->SetWindowText(g_sOuterDiameter + L":");
	GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->SetWindowText(g_sInnerDiameter + L":");

	init_controls();
	
	SetTimer(m_timer_update_range_id, m_timer_update_range_elapse, nullptr);

	return TRUE;
}

void zet037_tenso_channel_tab::init_controls()
{
	for (const auto& name_orientation : g_sNameOrientation)
		m_coordinate_p_combo.AddString(name_orientation);

	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sTensoResistor), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::resistor));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sTensoSensor), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::sensor));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sLvdt), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::lvdt));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sTr), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::tr));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sTc), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::tc));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sDiff), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::diff_u));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sSingle), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::single_u));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sCurrent), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::current));
	m_sensor_type_combo.SetItemData(
		m_sensor_type_combo.AddString(g_sTensoUser), static_cast<DWORD_PTR>(zetlab::zsp_tenso_sensor::user));
	
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_100p), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_100p));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_50p), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_50p));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_pt1000), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_pt1000));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_pt100), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_pt100));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_pt50), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_pt50));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_100m), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_100m));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_50m), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_50m));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_cu100), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_cu100));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_cu50), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_cu50));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_100n), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_100n));
	m_nsh_combo.SetItemData(
		m_nsh_combo.AddString(g_sNsh_50n), static_cast<DWORD_PTR>(zetlab::zsp_tr_nsh::nsh_50n));

	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" R"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_r));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" S"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_s));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" B"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_b));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" J"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_j));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" T"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_t));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" E"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_e));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" K"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_k));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" N"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_n));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" A1"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_a1));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" A2"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_a2));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" A3"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_a3));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" L"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_l));
	m_tc_type_combo.SetItemData(
		m_tc_type_combo.AddString(g_sType + L" M"), static_cast<DWORD_PTR>(zetlab::zsp_tc_type::type_m));
	
	m_unit_sense_combo.AddString(g_sUNIT_MVV);
	m_unit_sense_combo.EnableWindow(FALSE);

	m_unit_combo.AddString(g_sUNIT_MVV);
	m_unit_combo.AddString(g_sUNIT_MKMM);
	m_unit_combo.AddString(g_sUNIT_H);
	m_unit_combo.AddString(g_sUNIT_KH);
	m_unit_combo.AddString(g_sUNIT_MH);
	m_unit_combo.AddString(g_sUNIT_HM);
	m_unit_combo.AddString(g_sUNIT_KHM);
	m_unit_combo.AddString(g_sUNIT_MM);
	m_unit_combo.AddString(g_sUNIT_KG);
	m_unit_combo.AddString(g_sUNIT_T);
	m_unit_combo.AddString(g_sUNIT_M);
	m_unit_combo.AddString(g_sUNIT_PA);
	m_unit_combo.AddString(g_sUNIT_KPA);
	m_unit_combo.AddString(g_sUNIT_MPA);
	m_unit_combo.AddString(g_sUNIT_ATM);
	m_unit_combo.AddString(g_sUNIT_G);
	m_unit_combo.AddString(g_sUNIT_MPERS2);
	m_unit_combo.AddString(g_sUNIT_MV);
	m_unit_combo.AddString(g_sUNIT_V);
	m_unit_combo.AddString(g_sUNIT_MA);
	m_unit_combo.AddString(g_sUNIT_A);
	m_unit_combo.AddString(g_sUNIT_DEG);

	m_scheme_stick_picture.GetWindowRect(m_scheme_stick_picture_rect);
	ScreenToClient(m_scheme_stick_picture_rect);
	m_scheme_conn_picture.GetWindowRect(m_scheme_conn_picture_rect);
	ScreenToClient(m_scheme_conn_picture_rect);
}

void zet037_tenso_channel_tab::update_visibility()
{
	auto tenso_sensor = static_cast<zetlab::zsp_tenso_sensor>(m_channel->get_tenso_sensor());
	if (tenso_sensor == zetlab::zsp_tenso_sensor::resistor)
	{
		GetDlgItem(IDC_LABEL_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->EnableWindow(FALSE);
		GetDlgItem(IDC_UNIT_SENSE_ZET037)->ShowWindow(SW_SHOW);
		m_unit_sense_combo.ResetContent();
		m_unit_sense_combo.AddString(g_sUNIT_MVV);
		m_unit_sense_combo.SetCurSel(0);
		GetDlgItem(IDC_STATICSLASH_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNIT_EX_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESISTOR_K_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESISTOR_MODE_ZET037)->ShowWindow(SW_SHOW);
				
		auto mode = static_cast<zetlab::zsp_tenso_resistor_mode>(m_channel->get_tenso_resistor_mode());
		if (auto mode = static_cast<zetlab::zsp_tenso_resistor_mode>(m_channel->get_tenso_resistor_mode());
			mode == zetlab::zsp_tenso_resistor_mode::half3_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_8 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_8)
		{
			GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		}
		else if (mode == zetlab::zsp_tenso_resistor_mode::half3_torsion ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_torsion || 
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_1 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_1 || 
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_2)
		{
			if (m_unit == g_sUNIT_HM || m_unit == g_sUNIT_KHM)
			{
				GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_SHOW);
			}
			else
			{
				GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			}
		}
		else
		{
			GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		}
		if (m_unit == g_sUNIT_PA || m_unit == g_sUNIT_KPA || m_unit == g_sUNIT_MPA || m_unit == g_sUNIT_KGSMM2 || 
			m_unit == g_sUNIT_HM || m_unit == g_sUNIT_KHM)
		{
			GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		}
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::sensor ||
		tenso_sensor == zetlab::zsp_tenso_sensor::lvdt)
	{
		GetDlgItem(IDC_LABEL_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EXC_ZET037)->ShowWindow(SW_SHOW);
		if (tenso_sensor == zetlab::zsp_tenso_sensor::lvdt)
		{
			GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
		}
		GetDlgItem(IDC_LABEL_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->EnableWindow(FALSE);
		GetDlgItem(IDC_UNIT_SENSE_ZET037)->ShowWindow(SW_SHOW);
		m_unit_sense_combo.ResetContent();
		m_unit_sense_combo.AddString(g_sUNIT_MVV);
		m_unit_sense_combo.SetCurSel(0);
		GetDlgItem(IDC_STATICSLASH_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_EX_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSOR_RANGE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);		
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::tr || tenso_sensor == zetlab::zsp_tenso_sensor::tc)
	{
		GetDlgItem(IDC_LABEL_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_NSH_ZET037)->ShowWindow(tenso_sensor == zetlab::zsp_tenso_sensor::tr ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_NSH_ZET037)->ShowWindow(tenso_sensor == zetlab::zsp_tenso_sensor::tr ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->ShowWindow(tenso_sensor == zetlab::zsp_tenso_sensor::tc ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_TC_TYPE_ZET037)->ShowWindow(tenso_sensor == zetlab::zsp_tenso_sensor::tc ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNIT_SENSE_ZET037)->ShowWindow(SW_HIDE);
		m_unit_sense_combo.ResetContent();
		GetDlgItem(IDC_STATICSLASH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNIT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNIT_EX_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);		
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::diff_u ||
			tenso_sensor == zetlab::zsp_tenso_sensor::single_u ||
			tenso_sensor == zetlab::zsp_tenso_sensor::current)
	{
		GetDlgItem(IDC_LABEL_EXC_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EXC_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->EnableWindow(TRUE);
		GetDlgItem(IDC_UNIT_SENSE_ZET037)->ShowWindow(SW_SHOW);
		m_unit_sense_combo.ResetContent();
		m_unit_sense_combo.AddString(tenso_sensor == zetlab::zsp_tenso_sensor::current ? g_sUNIT_MA : g_sUNIT_V);
		m_unit_sense_combo.SetCurSel(0);
		GetDlgItem(IDC_STATICSLASH_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_EX_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::user)
	{
		GetDlgItem(IDC_LABEL_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EXC_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESISTANCE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LABEL_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NSH_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TC_TYPE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SENSE_ZET037)->EnableWindow(TRUE);
		GetDlgItem(IDC_UNIT_SENSE_ZET037)->ShowWindow(SW_SHOW);
		m_unit_sense_combo.ResetContent();
		m_unit_sense_combo.AddString(g_sUNIT_MVV);
		m_unit_sense_combo.SetCurSel(0);
		GetDlgItem(IDC_STATICSLASH_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_ZET037)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNIT_EX_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_RANGE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SENSOR_OUTPUT_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_K_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESISTOR_MODE_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POISSON_RATIO_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ELASTIC_MODULUS_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OUTER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INNER_DIAMETER_ZET037)->ShowWindow(SW_HIDE);		
	}
}

void zet037_tenso_channel_tab::SetAdapter( zetlab::std::channel* channel__, zetlab::std::device* device__ )
{
	m_channel = channel__;
	m_device = device__;
		
	if (!m_memento) 
		m_memento = boost::make_shared< zetlab::std::channel_memento >( channel__ );
	
	m_name = m_channel->getName(true).c_str();
	m_name.Replace(L";", L"");
	m_comment = m_channel->getComment(true).c_str();
	m_comment.Replace(L";", L"");
	m_coordinate_x.Format(_T("%g"), m_channel->CoordX(true));
	m_coordinate_y.Format(_T("%g"), m_channel->CoordY(true));
	m_coordinate_z.Format(_T("%g"), m_channel->CoordZ(true));
	auto coordinate_p(m_channel->CoordP(true));
	if (coordinate_p < 0 || coordinate_p > 6)
		coordinate_p = 0;
	if (m_coordinate_p_combo.SetCurSel(coordinate_p) >= 0)
		m_coordinate_p_combo.GetLBText(m_coordinate_p_combo.GetCurSel(), m_coordinate_p);
	m_exc.Format(_T("%.1f"), m_channel->get_tenso_exc());
	m_resistance.Format(_T("%.1f"), m_channel->getTensoResistance(true));
	m_sense.Format(_T("%g"), m_channel->Sense(true));
	m_unit_sense = m_channel->getUnitSense(true).c_str();
	m_unit = m_channel->getUnit(true).c_str();
	m_shift.Format(_T("%g"), m_channel->Shift(true));
	m_sensor_range.Format(_T("%g"), m_channel->get_tenso_sensor_range());
	m_sensor_output.Format(_T("%g"), m_channel->get_tenso_sensor_output());
	m_resistor_k.Format(_T("%g"), m_channel->get_tenso_resistor_k());
	m_poisson_ratio.Format(_T("%g"), m_channel->get_tenso_poisson_ratio());
	m_elastic_modulus.Format(_T("%g"), m_channel->get_tenso_elastic_modulus());
	m_outer_diameter.Format(_T("%g"), m_channel->get_tenso_outer_diameter());
	m_inner_diameter.Format(_T("%g"), m_channel->get_tenso_inner_diameter());
			
	m_level.SetFocus();

	SetTimer(m_timer_update_level_id, m_timer_update_level_elapse, nullptr);

	UpdateData(FALSE);

	update_sensor_type_combo();
	update_nsh_combo();
	update_tc_type_combo();
	update_tenso_scheme_combo();
	update_tenso_resistor_mode_combo();
	update_unit_combo();
	update_pictures();
	update_sense();
	update_sensitivity();
	update_range();
	update_visibility();
}

void zet037_tenso_channel_tab::CopyToClipboard()
{
	zetlab::utils::copy_to_clipboard<zetlab::std::channel, zetlab::std::channel::clipboard>(
		_T("CF_ZET_CHANNEL"), m_channel);
}

void zet037_tenso_channel_tab::PasteFromClipboard()
{
	if (zetlab::utils::paste_from_clipboard<zetlab::std::channel, zetlab::std::channel::clipboard>(
		_T("CF_ZET_CHANNEL"), m_channel))
		SetAdapter(m_channel, const_cast<zetlab::std::device*>(m_channel->getDevice()));
}

BOOL zet037_tenso_channel_tab::IsClipboardAvailable()
{
	return zetlab::utils::is_clipboard_format_available( _T("CF_ZET_CHANNEL") );
}

HBRUSH zet037_tenso_channel_tab::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	if (IDC_SEPARATOR == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkMode   ( TRANSPARENT );
		pDC->SetTextColor( GetSysColor( COLOR_BTNSHADOW ) );
	}

	return CTab::OnCtlColor( pDC, pWnd, nCtlColor );
}

void zet037_tenso_channel_tab::OnTimer( UINT_PTR nIDEvent )
{
	CTab::OnTimer(nIDEvent);

	if (m_timer_update_level_id == nIDEvent)
	 	m_level.SetAmplitude(m_channel->getLevel());
	else if (m_timer_update_range_id == nIDEvent)
		update_range();
}

void zet037_tenso_channel_tab::OnNameChangeRequest()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	m_name.Replace(L";", L"");
	zetlab::tstring name = m_name;
	m_channel->setName(name);
}

void zet037_tenso_channel_tab::OnCommentChangeRequest()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	if (m_comment.GetLength() > 250)
		m_comment.Left(250);
	m_comment.Replace(L";", L"");
	zetlab::tstring comment = m_comment;
	m_channel->setComment(comment);
}

void zet037_tenso_channel_tab::OnCoordXKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString coordinate_x = m_coordinate_x;

	UpdateData();
	
	//DWORD sel = m_coordinate_x_edit.GetSel();

	process_string_as_float(m_coordinate_x);

	try
	{
		zetlab::tstring coord_x = m_coordinate_x;
		m_channel->setCoordX(boost::lexical_cast<double>(coord_x));
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_coordinate_x.Compare(_T("-")))
			m_coordinate_x = coordinate_x;
	}

	UpdateData(FALSE);

	//m_coordinate_x_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnCoordYKillfocusRequest()
{
	if (nullptr == m_channel)
		return;
	
	CString coordinate_y = m_coordinate_y;

	UpdateData();
	
	//DWORD sel = m_coordinate_y_edit.GetSel();

	process_string_as_float(m_coordinate_y);

	try
	{
		zetlab::tstring coord_y = m_coordinate_y;
		m_channel->setCoordY(boost::lexical_cast<double>(coord_y));
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_coordinate_y.Compare(_T("-")))
			m_coordinate_y = coordinate_y;
	}

	UpdateData(FALSE);

	//m_coordinate_y_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnCoordZKillfocusRequest()
{
	if (nullptr == m_channel)
		return;
	
	CString coordinate_z = m_coordinate_z;

	UpdateData();

	//DWORD sel = m_coordinate_z_edit.GetSel();

	process_string_as_float(m_coordinate_z);

	try
	{
		zetlab::tstring coord_z = m_coordinate_z;
		m_channel->setCoordZ(boost::lexical_cast<double>(coord_z));
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_coordinate_z.Compare(_T("-")))
			m_coordinate_z = coordinate_z;
	}

	UpdateData(FALSE);

	//m_coordinate_z_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnCoordPChangeRequest()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	m_channel->setCoordP(m_coordinate_p_combo.GetCurSel());
}

void zet037_tenso_channel_tab::OnCoordPEdit()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	m_coordinate_p_combo.SetCurSel(m_channel->getConstCoordP());
}

void zet037_tenso_channel_tab::OnSensorSelect()
{
	if (nullptr == m_channel)
		return;
}

void zet037_tenso_channel_tab::OnSensorTypeSelect()
{
	if (nullptr == m_channel)
		return;

	m_channel->set_tenso_sensor(static_cast<uint16_t>(m_sensor_type_combo.GetItemData(m_sensor_type_combo.GetCurSel())), true);

	on_sensor_type_changed();
}

void zet037_tenso_channel_tab::OnSchemeSelect()
{
	if (nullptr == m_channel)
		return;

	m_channel->set_tenso_scheme(static_cast<uint16_t>(m_scheme_combo.GetItemData(m_scheme_combo.GetCurSel())), true);

	on_scheme_changed();
}

void zet037_tenso_channel_tab::OnExcKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString exc = m_exc;

	UpdateData();
	
	//DWORD sel = m_exc_edit.GetSel();

	process_string_as_float(m_exc);

	try
	{
		zetlab::tstring e = m_exc;
		auto exc_value = boost::lexical_cast<double>(e);

		if (exc_value < 0.1)
		{
			if (m_device->get_service_mode())
			{
				if (exc_value < 0.)
				{
					exc_value = 0.;
					m_exc = L"0.";
				}
			}
			else
			{
				exc_value = 0.1;
				m_exc = L"0.1";
			}
		}
		else if (exc_value > 30.)
		{
			exc_value = 30.;
			m_exc = L"30.0";
		}
		m_channel->set_tenso_exc(exc_value, true);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_exc.Compare(_T("-")))
			m_exc = exc;
	}

	UpdateData(FALSE);

	//m_exc_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnResistanceKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString resistance = m_resistance;

	UpdateData();

	//DWORD sel = m_resistance_edit.GetSel();

	process_string_as_float(m_resistance);

	try
	{
		zetlab::tstring resist = m_resistance;
		auto resistance_value = boost::lexical_cast<double>(resist);
		if (resistance_value < 1.)
		{
			resistance_value = 1.;
			m_resistance = L"1.0";
		}
		else if (resistance_value > 10000.)
		{
			resistance_value = 10000.;
			m_resistance = L"10000.0";
		}

		m_channel->setTensoResistance(resistance_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_resistance.Compare(_T("-")))
			m_resistance = resistance;
	}

	UpdateData(FALSE);

	//m_resistance_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnNshSelect()
{
	if (nullptr == m_channel)
		return;

	m_channel->set_tr_nsh(static_cast<uint16_t>(m_nsh_combo.GetItemData(m_nsh_combo.GetCurSel())));
}

void zet037_tenso_channel_tab::OnTcTypeSelect()
{
	if (nullptr == m_channel)
		return;

	m_channel->set_tc_type(static_cast<uint16_t>(m_tc_type_combo.GetItemData(m_tc_type_combo.GetCurSel())));
}

void zet037_tenso_channel_tab::OnSenseKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString sense = m_sense;

	UpdateData();

	//DWORD sel = m_sense_edit.GetSel();

	process_string_as_float(m_sense);

	try
	{
		zetlab::tstring s = m_sense;
		auto sense_value = boost::lexical_cast<double>(s);
		if (sense_value != 0)
			m_channel->setSense(sense_value);
		
		update_range();
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_sense.IsEmpty() && !!m_sense.Compare(_T("-")))
			m_sense = sense;
	}

	UpdateData(FALSE);

	//m_sense_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnUnitSenseChangeRequest()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	CString unit_sense(L"");
	if (auto sel = m_unit_sense_combo.GetCurSel(); sel >= 0)
		m_unit_sense_combo.GetLBText(sel, unit_sense);
	else
	{
		m_unit_sense_combo.GetWindowText(unit_sense);
		unit_sense = TranslateString(unit_sense);
	}

	zetlab::tstring us = unit_sense;
	m_channel->setUnitSense(us);

	UpdateData(FALSE);

	update_sensitivity();
}

void zet037_tenso_channel_tab::OnUnitSenseEdit()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	m_unit_sense_combo.SelectString(-1, m_channel->getUnitSense().c_str());
}

void zet037_tenso_channel_tab::OnUnitChangeRequest()
{
	if (nullptr == m_channel)
		return;

	UpdateData();

	zetlab::tstring u = m_unit;
	m_channel->setUnit(u);

	update_range();

	UpdateData(FALSE);

	update_sensitivity();
}

void zet037_tenso_channel_tab::OnUnitSelect()
{
	if (nullptr == m_channel)
		return;

	UpdateData();
	
	CString unit(m_unit);
	if (auto sel = m_unit_combo.GetCurSel(); sel >= 0)
		m_unit_combo.GetLBText(sel, m_unit);
	else
		m_unit_combo.GetWindowText(m_unit);

	if (double f(1.); 0 == zvGetConversionFactor(m_unit, unit, f))
	{
		auto sense = m_channel->Sense() * f;
		m_sense.Format(_T("%g"), sense);
		m_channel->setSense(sense);
	}
	
	zetlab::tstring u = m_unit;
	m_channel->setUnit(u);

	update_range();

	UpdateData(FALSE);

	update_sensitivity();
}

void zet037_tenso_channel_tab::OnUnitExSelect()
{
	if (nullptr == m_channel)
		return;

	CString unit(m_unit);
	if (auto sel = m_unit_combo_ex.GetCurSel(); sel >= 0)
		m_unit_combo_ex.GetLBText(sel, m_unit);
	else
		m_unit_combo_ex.GetWindowText(m_unit);

	zetlab::tstring u = m_unit;
	m_channel->setUnit(u);

	update_range();

	UpdateData(FALSE);

	update_sense();
	update_sensitivity();
	update_visibility();
}

void zet037_tenso_channel_tab::OnShiftKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString shift = m_shift;

	UpdateData();

	//DWORD sel = m_shift_edit.GetSel();

	process_string_as_float(m_shift);

	try
	{
		zetlab::tstring s = m_shift;
		auto shift_value = boost::lexical_cast<double>(s);
		m_channel->setShift(shift_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_shift.IsEmpty() && !!m_shift.Compare(_T("-")))
			m_shift = shift;
	}

	UpdateData(FALSE);

	//m_shift_edit.SetSel(sel);
}

void zet037_tenso_channel_tab::OnSensorRangeKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString sensor_range = m_sensor_range;

	UpdateData();

	//DWORD sel = m_sensor_range_edit.GetSel();

	process_string_as_float(m_sensor_range);

	try
	{
		zetlab::tstring s = m_sensor_range;
		auto sensor_range_value = boost::lexical_cast<double>(s);
		if (sensor_range_value != 0)
			m_channel->set_tenso_sensor_range(sensor_range_value);

		update_range();
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_sensor_range.IsEmpty() && !!m_sensor_range.Compare(_T("-")))
			m_sensor_range = sensor_range;
	}

	UpdateData(FALSE);

	//m_sensor_range_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnSensorOutputKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString sensor_output = m_sensor_output;

	UpdateData();

	//DWORD sel = m_sensor_output_edit.GetSel();

	process_string_as_float(m_sensor_output);

	try
	{
		zetlab::tstring s = m_sensor_output;
		auto sensor_output_value = boost::lexical_cast<double>(s);
		if (sensor_output_value != 0)
			m_channel->set_tenso_sensor_output(sensor_output_value);

		update_range();
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_sensor_output.IsEmpty() && !!m_sensor_output.Compare(_T("-")))
			m_sensor_output = sensor_output;
	}

	UpdateData(FALSE);

	//m_sensor_output_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnResistorKKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString resistor_k = m_resistor_k;

	UpdateData();

	//DWORD sel = m_resistor_k_edit.GetSel();

	process_string_as_float(m_resistor_k);

	try
	{
		zetlab::tstring s = m_resistor_k;
		auto resistor_k_value = boost::lexical_cast<double>(s);
		if (resistor_k_value != 0)
			m_channel->set_tenso_resistor_k(resistor_k_value);

		update_range();
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_resistor_k.IsEmpty() && !!m_resistor_k.Compare(_T("-")))
			m_resistor_k = resistor_k;
	}

	UpdateData(FALSE);

	//m_resistor_k_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnResistorModeSelect()
{
	if (nullptr == m_channel)
		return;

	m_channel->set_tenso_resistor_mode(
		static_cast<uint16_t>(m_resistor_mode_combo.GetItemData(m_resistor_mode_combo.GetCurSel())), true);

	update_unit_combo();
	update_sense();
	update_pictures();
	update_visibility();
}

void zet037_tenso_channel_tab::OnPoissonRatioKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString poisson_ratio = m_poisson_ratio;

	UpdateData();

	//DWORD sel = m_poisson_ratio_edit.GetSel();

	process_string_as_float(m_poisson_ratio);

	try
	{
		zetlab::tstring ratio = m_poisson_ratio;
		auto poisson_ratio_value = boost::lexical_cast<double>(ratio);
		if (poisson_ratio_value < 0.)
		{
			poisson_ratio_value = 0.;
			m_poisson_ratio = L"0.0";
		}
		else if (poisson_ratio_value > 1.)
		{
			poisson_ratio_value = 1.;
			m_poisson_ratio = L"1.0";
		}

		m_channel->set_tenso_poisson_ratio(poisson_ratio_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_poisson_ratio.Compare(_T("-")))
			m_poisson_ratio = poisson_ratio;
	}

	UpdateData(FALSE);

	//m_poisson_ratio_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnElasticModulusKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	CString elastic_modulus = m_elastic_modulus;

	UpdateData();

	//DWORD sel = m_elastic_modulus_edit.GetSel();

	process_string_as_float(m_elastic_modulus);

	try
	{
		zetlab::tstring modulus = m_elastic_modulus;
		auto elastic_modulus_value = boost::lexical_cast<double>(modulus);
		if (elastic_modulus_value < 1.)
		{
			elastic_modulus_value = 1.;
			m_elastic_modulus = L"1.0";
		}
		else if (elastic_modulus_value > 1000000.)
		{
			elastic_modulus_value = 1000000.;
			m_elastic_modulus = L"1000000.0";
		}

		m_channel->set_tenso_elastic_modulus(elastic_modulus_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_elastic_modulus.Compare(_T("-")))
			m_elastic_modulus = elastic_modulus;
	}

	UpdateData(FALSE);

	//m_elastic_modulus_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnOuterDiameterKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	auto inner_diameter = m_channel->get_tenso_inner_diameter();
	CString outer_diameter = m_outer_diameter;

	UpdateData();

	//DWORD sel = m_outer_diameter_edit.GetSel();

	process_string_as_float(m_outer_diameter);

	try
	{
		zetlab::tstring diameter = m_outer_diameter;
		auto outer_diameter_value = boost::lexical_cast<double>(diameter);
		if (outer_diameter_value < inner_diameter)
		{
			outer_diameter_value = inner_diameter + 0.001;
			m_outer_diameter.Format(_T("%g"), outer_diameter_value);
		}
		else if (outer_diameter_value > 10.)
		{
			outer_diameter_value = 10.;
			m_outer_diameter = L"10.0";
		}

		m_channel->set_tenso_outer_diameter(outer_diameter_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_outer_diameter.Compare(_T("-")))
			m_outer_diameter = outer_diameter;
	}

	UpdateData(FALSE);

	//m_outer_diameter_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnInnerDiameterKillfocusRequest()
{
	if (nullptr == m_channel)
		return;

	auto outer_diameter = m_channel->get_tenso_outer_diameter();
	CString inner_diameter = m_inner_diameter;

	UpdateData();

	//DWORD sel = m_inner_diameter_edit.GetSel();

	process_string_as_float(m_inner_diameter);

	try
	{
		zetlab::tstring diameter = m_inner_diameter;
		auto inner_diameter_value = boost::lexical_cast<double>(diameter);
		if (inner_diameter_value < 0.)
		{
			inner_diameter_value = 0.;
			m_inner_diameter = L"0.0";
		}
		else if (inner_diameter_value > outer_diameter)
		{
			inner_diameter_value = outer_diameter - 0.001;
			m_inner_diameter.Format(_T("%g"), inner_diameter_value);
		}

		m_channel->set_tenso_inner_diameter(inner_diameter_value);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!!m_inner_diameter.Compare(_T("-")))
			m_inner_diameter = inner_diameter;
	}

	UpdateData(FALSE);

	//m_inner_diameter_edit.SetSel(sel);

	update_sense();
}

void zet037_tenso_channel_tab::OnRangeChangeRequest(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (nullptr == m_channel)
		return;

	LPNMUPDOWN pNMUD = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (pNMUD->iDelta < 0)
		m_channel->setNextRange();
	else
		m_channel->setPrevRange();
}

void zet037_tenso_channel_tab::OnUserCancel()
{
	if (nullptr == m_channel)
		return;

	m_memento->restore();
}

void zet037_tenso_channel_tab::OnCancel()
{
	if (nullptr == m_channel)
		return;

	m_memento->restore();

	CTab::OnCancel();
}

void zet037_tenso_channel_tab::update_sensor_type_combo()
{
	bool b_sensor(false);
	auto sensor_count = m_sensor_type_combo.GetCount();
	for (int i = 0; i < sensor_count; ++i)
	{
		if (auto item_data = static_cast<uint16_t>(m_sensor_type_combo.GetItemData(i)); item_data == m_channel->get_tenso_sensor())
		{
			m_sensor_type_combo.SetCurSel(i);
			b_sensor = true;
			break;
		}
	}
	if (!b_sensor)
	{
		m_sensor_type_combo.SetCurSel(0);
		m_channel->set_tenso_sensor(static_cast<uint16_t>(m_sensor_type_combo.GetItemData(m_sensor_type_combo.GetCurSel())), true);
		on_sensor_type_changed();
	}	
}

void zet037_tenso_channel_tab::on_sensor_type_changed()
{
	update_tenso_scheme_combo();
	update_tenso_resistor_mode_combo();
	update_sensitivity();
	update_sense();
	update_unit_combo();
	update_pictures();
	update_visibility();
}

void zet037_tenso_channel_tab::update_tenso_scheme_combo()
{
	m_scheme_combo.ResetContent();

	auto tenso_sensor = static_cast<zetlab::zsp_tenso_sensor>(m_channel->get_tenso_sensor());
	if (tenso_sensor == zetlab::zsp_tenso_sensor::lvdt)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sLvdtScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::lvdt_u));
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::tr)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sTScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::tr_i));
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::tc)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sTScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::tc_u));
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::diff_u)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sUScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::diff_t));
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::single_u)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sUScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::single_t));
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::current)
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sIScheme), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::current_t));
	else
	{
		if (m_device->get_service_mode())
		{
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sFullBridge4wireI), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::full_4w_i));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sFullBridge4wireU), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::full_4w_u));
		}
		else
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sFullBridge4wire), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::full_4w_i));
		m_scheme_combo.SetItemData(
			m_scheme_combo.AddString(g_sFullBridge6wire), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::full_6w_u));
		if (tenso_sensor != zetlab::zsp_tenso_sensor::sensor)
		{
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sHalfBridge3wire), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::half_3w_i));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sHalfBridge5wire), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::half_5w_u));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sQuaterBridgeR1), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::quarter_r1_i));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sQuaterBridgeR2), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::quarter_r2_i));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sQuaterBridgeR3), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::quarter_r3_i));
			m_scheme_combo.SetItemData(
				m_scheme_combo.AddString(g_sQuaterBridgeR4), static_cast<DWORD_PTR>(zetlab::zsp_tenso_scheme::quarter_r4_i));
		}
	}

	bool b_scheme(false);
	auto scheme_count = m_scheme_combo.GetCount();
	for (int i = 0; i < scheme_count; ++i)
	{
		if (auto item_data = static_cast<uint16_t>(m_scheme_combo.GetItemData(i)); item_data == m_channel->get_tenso_scheme())
		{
			m_scheme_combo.SetCurSel(i);
			b_scheme = true;
			break;
		}
	}
	if (!b_scheme)
	{
		m_scheme_combo.SetCurSel(0);
		m_channel->set_tenso_scheme(static_cast<uint16_t>(m_scheme_combo.GetItemData(m_scheme_combo.GetCurSel())), true);
		on_scheme_changed();
	}	
}

void zet037_tenso_channel_tab::on_scheme_changed()
{
	if (auto scheme = m_channel->get_tenso_scheme();
		static_cast<uint16_t>(zetlab::zsp_tenso_scheme::diff_t) == scheme ||
		static_cast<uint16_t>(zetlab::zsp_tenso_scheme::single_t) == scheme)
		m_unit_sense = g_sUNIT_V;
	else if (static_cast<uint16_t>(zetlab::zsp_tenso_scheme::current_t) == scheme)
		m_unit_sense = g_sUNIT_MA;
	else
		m_unit_sense = g_sUNIT_MVV;
	UpdateData(FALSE);
	zetlab::tstring us = m_unit_sense.GetString();
	m_channel->setUnitSense(us);
	
	update_tenso_resistor_mode_combo();
	update_sensitivity();
	update_sense();
	update_unit_combo();
	update_pictures();
	update_visibility();
}

void zet037_tenso_channel_tab::update_sensitivity()
{
	if (!m_unit.IsEmpty())
	{
		if (!m_unit_sense.IsEmpty())
		{
			auto unit_sense(m_unit_sense);
			process_unit_on_brackets(unit_sense);
			auto unit(m_unit);
			process_unit_on_brackets(unit);
			GetDlgItem(IDC_LABEL_SENSE_ZET037)->
				SetWindowText(g_sChannelSensitivity + L", " + unit_sense + L"/" + unit + L":");
		}
		GetDlgItem(IDC_LABEL_SHIFT_ZET037)->
			SetWindowText(g_sOffsetDC + L", " + m_unit + L":");
		GetDlgItem(IDC_LABEL_SENSOR_RANGE_ZET037)->
			SetWindowText(g_sSensorRange + L", " + m_unit + L":");
	}
}

void zet037_tenso_channel_tab::update_sense()
{
	CString s;

	if (auto tenso_sensor = static_cast<zetlab::zsp_tenso_sensor>(m_channel->get_tenso_sensor());
		tenso_sensor == zetlab::zsp_tenso_sensor::resistor)
	{
		auto sense = m_channel->get_tenso_resistor_k() / 4.;
		auto mode = static_cast<zetlab::zsp_tenso_resistor_mode>(m_channel->get_tenso_resistor_mode());
		if (mode == zetlab::zsp_tenso_resistor_mode::half3_tension_compression_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_tension_compression_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::half3_bending_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_bending_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::half3_torsion ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_torsion ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_6 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_6 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_7 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_7 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_8 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_8 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_bending_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_bending_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_1 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_1)
			sense *= 2.;

		if (mode == zetlab::zsp_tenso_resistor_mode::full4_bending_3 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_bending_3 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_2)
			sense *= 4.;
		
		if (mode == zetlab::zsp_tenso_resistor_mode::half3_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_4 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_5 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_tension_compression_8 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_tension_compression_8 ||
			m_unit == g_sUNIT_HM || m_unit == g_sUNIT_KHM)
			sense *= (1. + m_channel->get_tenso_poisson_ratio());
	
		if (auto it_unit = g_tenso_resistor_unit_k.find(m_unit); it_unit != g_tenso_resistor_unit_k.end())
			sense *= it_unit->second;

		if (m_unit == g_sUNIT_PA || m_unit == g_sUNIT_KPA || m_unit == g_sUNIT_MPA || m_unit == g_sUNIT_KGSMM2 ||
			m_unit == g_sUNIT_HM || m_unit == g_sUNIT_KHM)
			sense /= m_channel->get_tenso_elastic_modulus() * 1.e6;

		if (m_unit == g_sUNIT_HM || m_unit == g_sUNIT_KHM)
		{
			auto D = m_channel->get_tenso_outer_diameter();
			sense = sense * 16. / IPP_PI / D / D / D;
			auto d = m_channel->get_tenso_inner_diameter();
			if (d != 0.)
			{
				d *= d;
				d *= d;
				D *= D;
				D *= D;
				sense /= (1. - d / D);
			}
		}
		
		s.Format(_T("%g"), sense);
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::sensor ||
		tenso_sensor == zetlab::zsp_tenso_sensor::lvdt ||
		tenso_sensor == zetlab::zsp_tenso_sensor::tr ||
		tenso_sensor == zetlab::zsp_tenso_sensor::tc)
		s.Format(_T("%g"), m_channel->get_tenso_sensor_output() / m_channel->get_tenso_sensor_range());
	else
		s.Format(_T("%g"), m_channel->Sense(true));

	m_sense_edit.SetWindowText(s);
	OnSenseKillfocusRequest();
}

void zet037_tenso_channel_tab::update_tenso_resistor_mode_combo()
{
	m_resistor_mode_combo.ResetContent();

	if (auto tenso_scheme = static_cast<zetlab::zsp_tenso_scheme>(m_channel->get_tenso_scheme());
		is_tenso_scheme_quarter(tenso_scheme))
	{
		m_resistor_mode_combo.SetItemData(
			m_resistor_mode_combo.AddString(g_sTensionCompression + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_1));
		m_resistor_mode_combo.SetItemData(
			m_resistor_mode_combo.AddString(g_sTensionCompression + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_2));
		m_resistor_mode_combo.SetItemData(
			m_resistor_mode_combo.AddString(g_sTensionCompression + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_3));
		m_resistor_mode_combo.SetItemData(
			m_resistor_mode_combo.AddString(g_sBending), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::quarter_bending));
	}
	else if (is_tenso_scheme_half(tenso_scheme))
	{
		if (tenso_scheme == zetlab::zsp_tenso_scheme::half_5w_u)
		{
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_tension_compression_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_tension_compression_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_tension_compression_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 4"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_tension_compression_4));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_bending_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_bending_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half5_torsion));
		}
		else
		{
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_tension_compression_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_tension_compression_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_tension_compression_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 4"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_tension_compression_4));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_bending_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_bending_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::half3_torsion));
		}
	}
	else if (is_tenso_scheme_full(tenso_scheme))
	{
		if (tenso_scheme == zetlab::zsp_tenso_scheme::full_6w_u)
		{
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 4"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_4));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 5"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_5));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 6"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_6));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 7"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_7));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 8"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_tension_compression_8));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_bending_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_bending_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_bending_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_torsion_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full6_torsion_2));
		}
		else
		{
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 4"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_4));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 5"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_5));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 6"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_6));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 7"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_7));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTensionCompression + L" 8"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_tension_compression_8));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_bending_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_bending_2));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sBending + L" 3"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_bending_3));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion + L" 1"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_torsion_1));
			m_resistor_mode_combo.SetItemData(
				m_resistor_mode_combo.AddString(g_sTorsion + L" 2"), static_cast<DWORD_PTR>(zetlab::zsp_tenso_resistor_mode::full4_torsion_2));
		}
	}

	bool b_mode(false);
	auto mode_count = m_resistor_mode_combo.GetCount();
	for (int i = 0; i < mode_count; ++i)
	{
		if (auto item_data = static_cast<uint16_t>(m_resistor_mode_combo.GetItemData(i));
			item_data == m_channel->get_tenso_resistor_mode())
		{
			m_resistor_mode_combo.SetCurSel(i);
			b_mode = true;
			break;
		}
	}
	if (!b_mode)
	{
		m_resistor_mode_combo.SetCurSel(0);
		m_channel->set_tenso_resistor_mode(
			static_cast<uint16_t>(m_resistor_mode_combo.GetItemData(m_resistor_mode_combo.GetCurSel())), true);
	}

	update_sense();
	update_unit_combo();
	update_pictures();
	update_visibility();
}

void zet037_tenso_channel_tab::update_unit_combo()
{
	if (auto tenso_sensor = static_cast<zetlab::zsp_tenso_sensor>(m_channel->get_tenso_sensor());
		tenso_sensor == zetlab::zsp_tenso_sensor::resistor)
	{
		m_unit_combo_ex.ResetContent();
		auto current_unit = m_channel->getUnit();
		bool b_moment(false);
		if (auto mode = static_cast<zetlab::zsp_tenso_resistor_mode>(m_channel->get_tenso_resistor_mode());
			mode == zetlab::zsp_tenso_resistor_mode::half3_torsion ||
			mode == zetlab::zsp_tenso_resistor_mode::half5_torsion ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_1 ||
			mode == zetlab::zsp_tenso_resistor_mode::full4_torsion_2 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_1 ||
			mode == zetlab::zsp_tenso_resistor_mode::full6_torsion_2)
			b_moment = true;

		bool b_unit(false);
		for (const auto& unit : g_tenso_resistor_units)
		{
			if (!b_moment && (0 == unit.Compare(g_sUNIT_HM) || 0 == unit.Compare(g_sUNIT_KHM)))
				continue;

			auto i = m_unit_combo_ex.AddString(unit);
			if (0 == unit.Compare(current_unit.c_str()))
			{
				b_unit = true;
				m_unit_combo_ex.SetCurSel(i);
			}
		}
		
		if (!b_unit)
		{
			m_unit_combo_ex.SetCurSel(0);
			m_unit = g_tenso_resistor_units.front();
			zetlab::tstring u = m_unit;
			m_channel->setUnit(u);
			UpdateData(FALSE);
		}
	}
	else if (tenso_sensor == zetlab::zsp_tenso_sensor::tr || tenso_sensor == zetlab::zsp_tenso_sensor::tc)
	{
		m_unit = g_sUNIT_CEL;
		zetlab::tstring u = m_unit;
		m_channel->setUnit(u);
	}
}

void zet037_tenso_channel_tab::update_nsh_combo()
{
	bool b_nsh(false);
	auto nsh_count = m_nsh_combo.GetCount();
	for (int i = 0; i < nsh_count; ++i)
	{
		if (auto item_data = static_cast<uint16_t>(m_nsh_combo.GetItemData(i)); item_data == m_channel->get_tr_nsh())
		{
			m_nsh_combo.SetCurSel(i);
			b_nsh = true;
			break;
		}
	}
	if (!b_nsh)
	{
		m_nsh_combo.SetCurSel(0);
		m_channel->set_tr_nsh(static_cast<uint16_t>(m_nsh_combo.GetItemData(m_nsh_combo.GetCurSel())));
	}
}

void zet037_tenso_channel_tab::update_tc_type_combo()
{
	bool b_tc_type(false);
	auto tc_type_count = m_tc_type_combo.GetCount();
	for (int i = 0; i < tc_type_count; ++i)
	{
		if (auto item_data = static_cast<uint16_t>(m_tc_type_combo.GetItemData(i)); item_data == m_channel->get_tc_type())
		{
			m_tc_type_combo.SetCurSel(i);
			b_tc_type = true;
			break;
		}
	}
	if (!b_tc_type)
	{
		m_tc_type_combo.SetCurSel(0);
		m_channel->set_tc_type(static_cast<uint16_t>(m_tc_type_combo.GetItemData(m_tc_type_combo.GetCurSel())));
	}
}

void zet037_tenso_channel_tab::update_pictures()
{
	CString stick(L""), conn(L"");
	const auto scheme = static_cast<zetlab::zsp_tenso_scheme>(m_channel->get_tenso_scheme());
	const auto sensor = static_cast<zetlab::zsp_tenso_sensor>(m_channel->get_tenso_sensor());
	const auto mode = static_cast<zetlab::zsp_tenso_resistor_mode>(m_channel->get_tenso_resistor_mode());
	get_scheme_pictures(scheme, sensor, mode, stick, conn);
	update_picture(m_scheme_stick_picture, m_scheme_stick_picture_rect, stick);
	update_picture(m_scheme_conn_picture, m_scheme_conn_picture_rect, conn);
}

void zet037_tenso_channel_tab::update_picture(CStatic& element, CRect rect, CString& file_name)
{
	CImage image;
	if (FAILED(image.Load(file_name)))
	{
		element.SetBitmap(NULL);
		return;
	}

	auto dim_y = image.GetHeight();
	auto dim_x = image.GetWidth();

	rect.right -= 2;
	rect.left += 2;

	resize_picture(dim_x, dim_y, rect);

	rect.right += 2;
	rect.left -= 2;

	element.MoveWindow(rect);

	CDC* screen_dc = GetDC();
	CDC dc;
	dc.CreateCompatibleDC(screen_dc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(screen_dc, dim_x, dim_y);

	CBitmap* object = dc.SelectObject(&bitmap);
	dc.SetStretchBltMode(HALFTONE);
	image.StretchBlt(dc.m_hDC, 0, 0, dim_x, dim_y, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
	dc.SelectObject(object);

	element.SetBitmap((HBITMAP)bitmap.Detach());
	ReleaseDC(screen_dc);
	element.Invalidate();
}

void zet037_tenso_channel_tab::resize_picture(int& cx, int& cy, CRect& rect)
{
	double proportion = (double)cx / (double)cy;
	double proportion_cx = (double)rect.Width() / (double)cx;
	double proportion_cy = (double)rect.Height() / (double)cy;

	if (proportion_cx < 1.)
	{
		cx = rect.Width();
		cy = int(cx / proportion);
		proportion_cy = (double)rect.Height() / (double)cy;
	}

	if (proportion_cy < 1.)
	{
		cy = rect.Height();
		cx = int(cy * proportion);
		proportion_cx = (double)rect.Width() / (double)cx;
	}

	if (proportion_cx >= 0 && proportion_cy >= 0)
	{
		if (proportion_cx > proportion_cy)
		{
			cy = rect.Height();
			cx = int(cy * proportion);
		}
		else
		{
			cx = rect.Width();
			cy = int(cx / proportion);
		}
	}

	POINT center;
	center.x = rect.left + (rect.Width() / 2);
	center.y = rect.top + (rect.Height() / 2);

	rect.left = center.x - cx / 2;
	rect.right = center.x + cx / 2;

	rect.top = center.y - cy / 2;
	rect.bottom = center.y + cy / 2;
}

void zet037_tenso_channel_tab::get_scheme_pictures(
	const zetlab::zsp_tenso_scheme scheme,
	const zetlab::zsp_tenso_sensor sensor,
	const zetlab::zsp_tenso_resistor_mode mode,
	CString& stick,
	CString& conn)
{
	stick.Empty();
	conn.Empty();

	CString file_name_s(L""), file_name_c(L"");
	if (zetlab::zsp_tenso_sensor::resistor == sensor)
	{
		if (zetlab::zsp_tenso_scheme::full_4w_i == scheme || 
			zetlab::zsp_tenso_scheme::full_4w_u == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_1 == mode)
			{ 
				file_name_s = L"full_tension_compression_1";
				file_name_c = L"full4_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_2 == mode)
			{
				file_name_s = L"full_tension_compression_2";
				file_name_c = L"full4_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_3 == mode)
			{
				file_name_s = L"full_tension_compression_3";
				file_name_c = L"full4_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_4 == mode)
			{
				file_name_s = L"full_tension_compression_4";
				file_name_c = L"full4_tension_compression_4";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_5 == mode)
			{
				file_name_s = L"full_tension_compression_5";
				file_name_c = L"full4_tension_compression_5";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_6 == mode)
			{
				file_name_s = L"full_tension_compression_6";
				file_name_c = L"full4_tension_compression_6";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_7 == mode)
			{
				file_name_s = L"full_tension_compression_7";
				file_name_c = L"full4_tension_compression_7";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_tension_compression_8 == mode)
			{
				file_name_s = L"full_tension_compression_8";
				file_name_c = L"full4_tension_compression_8";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_bending_1 == mode)
			{
				file_name_s = L"full_bending_1";
				file_name_c = L"full4_bending_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_bending_2 == mode)
			{
				file_name_s = L"full_bending_2";
				file_name_c = L"full4_bending_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_bending_3 == mode)
			{
				file_name_s = L"full_bending_3";
				file_name_c = L"full4_bending_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_torsion_1 == mode)
			{
				file_name_s = L"full_torsion_1";
				file_name_c = L"full4_torsion_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full4_torsion_2 == mode)
			{
				file_name_s = L"full_torsion_2";
				file_name_c = L"full4_torsion_2";
			}
		}
		else if (zetlab::zsp_tenso_scheme::full_6w_u == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_1 == mode)
			{
				file_name_s = L"full_tension_compression_1";
				file_name_c = L"full6_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_2 == mode)
			{
				file_name_s = L"full_tension_compression_2";
				file_name_c = L"full6_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_3 == mode)
			{
				file_name_s = L"full_tension_compression_3";
				file_name_c = L"full6_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_4 == mode)
			{
				file_name_s = L"full_tension_compression_4";
				file_name_c = L"full6_tension_compression_4";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_5 == mode)
			{
				file_name_s = L"full_tension_compression_5";
				file_name_c = L"full6_tension_compression_5";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_6 == mode)
			{
				file_name_s = L"full_tension_compression_6";
				file_name_c = L"full6_tension_compression_6";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_7 == mode)
			{
				file_name_s = L"full_tension_compression_7";
				file_name_c = L"full6_tension_compression_7";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_tension_compression_8 == mode)
			{
				file_name_s = L"full_tension_compression_8";
				file_name_c = L"full6_tension_compression_8";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_bending_1 == mode)
			{
				file_name_s = L"full_bending_1";
				file_name_c = L"full6_bending_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_bending_2 == mode)
			{
				file_name_s = L"full_bending_2";
				file_name_c = L"full6_bending_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_bending_3 == mode)
			{
				file_name_s = L"full_bending_3";
				file_name_c = L"full6_bending_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_torsion_1 == mode)
			{
				file_name_s = L"full_torsion_1";
				file_name_c = L"full6_torsion_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::full6_torsion_2 == mode)
			{
				file_name_s = L"full_torsion_2";
				file_name_c = L"full6_torsion_2";
			}
		}
		else if (zetlab::zsp_tenso_scheme::half_3w_i == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::half3_tension_compression_1 == mode)
			{
				file_name_s = L"half_tension_compression_1";
				file_name_c = L"half3_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_tension_compression_2 == mode)
			{
				file_name_s = L"half_tension_compression_2";
				file_name_c = L"half3_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_tension_compression_3 == mode)
			{
				file_name_s = L"half_tension_compression_3";
				file_name_c = L"half3_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_tension_compression_4 == mode)
			{
				file_name_s = L"half_tension_compression_4";
				file_name_c = L"half3_tension_compression_4";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_bending_1 == mode)
			{
				file_name_s = L"half_bending_1";
				file_name_c = L"half3_bending_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_bending_2 == mode)
			{
				file_name_s = L"half_bending_2";
				file_name_c = L"half3_bending_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half3_torsion == mode)
			{
				file_name_s = L"half_torsion";
				file_name_c = L"half3_torsion";
			}
		}
		else if (zetlab::zsp_tenso_scheme::half_5w_u == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::half5_tension_compression_1 == mode)
			{
				file_name_s = L"half_tension_compression_1";
				file_name_c = L"half5_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_tension_compression_2 == mode)
			{
				file_name_s = L"half_tension_compression_2";
				file_name_c = L"half5_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_tension_compression_3 == mode)
			{
				file_name_s = L"half_tension_compression_3";
				file_name_c = L"half5_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_tension_compression_4 == mode)
			{
				file_name_s = L"half_tension_compression_4";
				file_name_c = L"half5_tension_compression_4";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_bending_1 == mode)
			{
				file_name_s = L"half_bending_1";
				file_name_c = L"half5_bending_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_bending_2 == mode)
			{
				file_name_s = L"half_bending_2";
				file_name_c = L"half5_bending_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::half5_torsion == mode)
			{
				file_name_s = L"half_torsion";
				file_name_c = L"half5_torsion";
			}
		}
		else if (zetlab::zsp_tenso_scheme::quarter_r1_i == scheme || zetlab::zsp_tenso_scheme::quarter_r2_i == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_1 == mode)
			{
				file_name_s = L"quarter_tension_compression_1";
				file_name_c = L"quarter_r1_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_2 == mode)
			{
				file_name_s = L"quarter_tension_compression_2";
				file_name_c = L"quarter_r1_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_3 == mode)
			{
				file_name_s = L"quarter_tension_compression_3";
				file_name_c = L"quarter_r1_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_bending == mode)
			{
				file_name_s = L"quarter_bending";
				file_name_c = L"quarter_r1_bending";
			}
		}
		else if (zetlab::zsp_tenso_scheme::quarter_r3_i == scheme || zetlab::zsp_tenso_scheme::quarter_r4_i == scheme)
		{
			if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_1 == mode)
			{
				file_name_s = L"quarter_tension_compression_1";
				file_name_c = L"quarter_r2_tension_compression_1";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_2 == mode)
			{
				file_name_s = L"quarter_tension_compression_2";
				file_name_c = L"quarter_r2_tension_compression_2";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_tension_compression_3 == mode)
			{
				file_name_s = L"quarter_tension_compression_3";
				file_name_c = L"quarter_r2_tension_compression_3";
			}
			else if (zetlab::zsp_tenso_resistor_mode::quarter_bending == mode)
			{
				file_name_s = L"quarter_bending";
				file_name_c = L"quarter_r2_bending";
			}
		}
	}
	else if (zetlab::zsp_tenso_sensor::sensor == sensor)
	{
		if (zetlab::zsp_tenso_scheme::full_4w_i == scheme ||
			zetlab::zsp_tenso_scheme::full_4w_u == scheme)
			file_name_c = L"sensor4";
		else if (zetlab::zsp_tenso_scheme::full_6w_u == scheme)
			file_name_c = L"sensor6";
	}
	else if (zetlab::zsp_tenso_sensor::lvdt == sensor)
		file_name_c = L"lvdt";
	else if (zetlab::zsp_tenso_sensor::tr == sensor)
		file_name_c = L"tr";
	else if (zetlab::zsp_tenso_sensor::tc == sensor)
		file_name_c = L"tc";
	else if (zetlab::zsp_tenso_sensor::diff_u == sensor)
		file_name_c = L"diff_u";
	else if (zetlab::zsp_tenso_sensor::single_u == sensor)
		file_name_c = L"single_u";
	else if (zetlab::zsp_tenso_sensor::current == sensor)
		file_name_c = L"current";
	else if (zetlab::zsp_tenso_sensor::user == sensor)
	{
		if (zetlab::zsp_tenso_scheme::full_4w_i == scheme ||
			zetlab::zsp_tenso_scheme::full_4w_u == scheme)
			file_name_c = L"full4_user";
		else if (zetlab::zsp_tenso_scheme::full_6w_u == scheme)
			file_name_c = L"full6_user";
		else if (zetlab::zsp_tenso_scheme::half_3w_i == scheme)
			file_name_c = L"half3_user";
		else if (zetlab::zsp_tenso_scheme::half_5w_u == scheme)
			file_name_c = L"half5_user";
		else if (zetlab::zsp_tenso_scheme::quarter_r1_i == scheme || zetlab::zsp_tenso_scheme::quarter_r2_i == scheme)
			file_name_c = L"quarter_r1_user";
		else if (zetlab::zsp_tenso_scheme::quarter_r3_i == scheme || zetlab::zsp_tenso_scheme::quarter_r4_i == scheme)
			file_name_c = L"quarter_r2_user";
	}
	
	if (!file_name_s.IsEmpty())
		stick = m_scheme_path + file_name_s + L".png";
	if (!file_name_c.IsEmpty())
		conn = m_scheme_path + file_name_c + L".png";
}

void zet037_tenso_channel_tab::update_range()
{
	CString range(g_sRange);
	range.AppendFormat(L": %g %s (", m_channel->getMaxLevel(), m_unit.GetString());
	range.Append(g_sAmplifier);
	range.AppendFormat(L" %g)", m_channel->getAmplify());
	if (m_range != range)
	{
		m_range = range;
		m_range_static.SetWindowText(m_range);
	}
}

void zet037_tenso_channel_tab::process_string_as_float(CString& value)
{
	if (value.Find(L",") != -1)
		value.Replace(L",", L".");
	if (value.Find(L"б") != -1)
		value.Replace(L"б", L".");
	if (value.Find(L"ю") != -1)
		value.Replace(L"ю", L".");
	if (value.Find(L"\\") != -1)
		value.Replace(L"\\", L".");
	if (value.Find(L"\\") != -1)
		value.Replace(L"\\", L".");
	if (value.Find(L"/") != -1)
		value.Replace(L"/", L".");
}

void zet037_tenso_channel_tab::process_unit_on_brackets(CString& value)
{
	if (-1 != value.Find(L"/") || -1 != value.Find(L"·") || -1 != value.Find(L"*"))
	{
		value.Insert(0, L"(");
		value.Append(L")");
	}
}
