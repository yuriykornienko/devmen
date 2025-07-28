#pragma once

#include "../default_tab.h"

#include "../../../algo/std/channel/channel.hpp"
#include "../../../algo/std/device/device_std.hpp"
#include "../../../algo/sensors/sensors.hpp"
#include "../../horscale/horscale.h"

class zet037_tenso_channel_tab : public CTab
{
public:
	zet037_tenso_channel_tab( CWnd* pParent = nullptr );
	virtual ~zet037_tenso_channel_tab() = default;
	enum { IDD = IDD_ZET037_TENSO_CHANNEL_TAB };
	enum { IDS = IDS_IDD_ZET037_TENSO_CHANNEL_TAB };

public:
	void SetAdapter( zetlab::std::channel* channel__,
					 zetlab::std::device* device__ );
	
	void Activate() override {}
	void Disactivate() override {}
	
	void CopyToClipboard();
	void PasteFromClipboard();
	BOOL IsClipboardAvailable();

protected:
	void DoDataExchange( CDataExchange* pDX ) override;
	BOOL OnInitDialog() override;

	afx_msg HBRUSH  OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnCancel() override;
	afx_msg void OnUserCancel();
	afx_msg void OnNameChangeRequest();
	afx_msg void OnSensorSelect();
	afx_msg void OnCommentChangeRequest();
	afx_msg void OnCoordXKillfocusRequest();
	afx_msg void OnCoordYKillfocusRequest();
	afx_msg void OnCoordZKillfocusRequest();
	afx_msg void OnCoordPChangeRequest();
	afx_msg void OnCoordPEdit();
	afx_msg void OnSensorTypeSelect();
	afx_msg void OnSchemeSelect();
	afx_msg void OnExcKillfocusRequest();
	afx_msg void OnResistanceKillfocusRequest();
	afx_msg void OnNshSelect();
	afx_msg void OnTcTypeSelect();
	afx_msg void OnSenseKillfocusRequest();
	afx_msg void OnUnitSenseChangeRequest();
	afx_msg void OnUnitSenseEdit();
	afx_msg void OnUnitChangeRequest();
	afx_msg void OnUnitSelect();
	afx_msg void OnUnitExSelect();
	afx_msg void OnShiftKillfocusRequest();
	afx_msg void OnSensorRangeKillfocusRequest();
	afx_msg void OnSensorOutputKillfocusRequest();
	afx_msg void OnResistorKKillfocusRequest();
	afx_msg void OnResistorModeSelect();
	afx_msg void OnPoissonRatioKillfocusRequest();
	afx_msg void OnElasticModulusKillfocusRequest();
	afx_msg void OnOuterDiameterKillfocusRequest();
	afx_msg void OnInnerDiameterKillfocusRequest();
	afx_msg void OnRangeChangeRequest(NMHDR* pNMHDR, LRESULT* pResult);
	
	DECLARE_MESSAGE_MAP()

	CString m_name;
	CComboBox m_name_combo;
	CString m_comment;
	CString m_coordinate_x;
	CEdit m_coordinate_x_edit;
	CString m_coordinate_y;
	CEdit m_coordinate_y_edit;
	CString m_coordinate_z;
	CEdit m_coordinate_z_edit;
	CString m_coordinate_p;
	CComboBox m_coordinate_p_combo;
	CComboBox m_scheme_combo;
	CString m_exc;
	CEdit m_exc_edit;
	CString m_resistance;
	CEdit m_resistance_edit;
	CComboBox m_sensor_type_combo;
	CComboBox m_nsh_combo;
	CComboBox m_tc_type_combo;
	CString m_sense;
	CEdit m_sense_edit;
	CString m_unit_sense;
	CComboBox m_unit_sense_combo;
	CString m_unit;
	CComboBox m_unit_combo;
	CComboBox m_unit_combo_ex;
	CEdit m_shift_edit;
	CString m_shift;
	CEdit m_sensor_range_edit;
	CString m_sensor_range;
	CEdit m_sensor_output_edit;
	CString m_sensor_output;
	CEdit m_resistor_k_edit;
	CString m_resistor_k;
	CComboBox m_resistor_mode_combo;
	CString m_poisson_ratio;
	CEdit m_poisson_ratio_edit;
	CString m_elastic_modulus;
	CEdit m_elastic_modulus_edit;
	CString m_outer_diameter;
	CEdit m_outer_diameter_edit;
	CString m_inner_diameter;
	CEdit m_inner_diameter_edit;
	CHorScale m_level;
	CSpinButtonCtrl m_spin;
	CString m_range;
	CStatic m_range_static;
	CStatic m_scheme_stick_picture;
	CStatic m_scheme_conn_picture;

	CString m_scheme_path;
	CRect m_scheme_stick_picture_rect;
	CRect m_scheme_conn_picture_rect;

private:
	void init_controls();
	void update_visibility();

	void update_sensor_type_combo();
	void on_sensor_type_changed();
	
	void update_tenso_scheme_combo();
	void on_scheme_changed();

	void update_nsh_combo();
	void update_tc_type_combo();

	void update_range();

	void update_sensitivity();
	void process_string_as_float(CString& value);
	void process_unit_on_brackets(CString& value);
	void update_sense();
	
	void update_tenso_resistor_mode_combo();
	void update_unit_combo();
	
	void update_pictures();
	void update_picture(CStatic& element, CRect rect, CString& file_name);
	void resize_picture(int& cx, int& cy, CRect& rect);
	void get_scheme_pictures(
		const zetlab::zsp_tenso_scheme scheme,
		const zetlab::zsp_tenso_sensor sensor,
		const zetlab::zsp_tenso_resistor_mode mode,
		CString& stick,
		CString& conn);

	zetlab::std::channel* m_channel;
	zetlab::std::channel_memento_ptr m_memento;

	zetlab::std::device* m_device;

	const UINT_PTR m_timer_update_level_id = 1;
	const UINT m_timer_update_level_elapse = 500;
	const UINT_PTR m_timer_update_range_id = 2;
	const UINT m_timer_update_range_elapse = 500;
public:
	
};
