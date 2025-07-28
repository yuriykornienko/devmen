#pragma once

#include "../../zsp_common.hpp"
#include <pugixml/auto_link.h>

#include <functional>
#include <optional>

constexpr uint32_t g_bad_id = (::std::numeric_limits<uint32_t>::max)();

namespace zetlab {

using save_channel_callback = ::std::function<void(const uint32_t)>;
using change_common_parameter_callback = ::std::function<void(const uint32_t)>;

class zsp_channel
{
public:
	zsp_channel(pugi::xml_node& device_node, pugi::xml_node& channel_node);
	virtual ~zsp_channel() = default;

	zsp_channel(const zsp_channel& value);
	zsp_channel(zsp_channel&& value) noexcept;

	zsp_channel& operator=(const zsp_channel& value);
	zsp_channel& operator=(zsp_channel&& value) noexcept;

	void save();
	const int32_t compare(const zsp_channel& value) const;
	void notify_save_channel(save_channel_callback cb) { m_save_channel_cb = cb; }
	void notify_change_common_parameter(change_common_parameter_callback cb) { m_change_common_parameter_cb = cb; }

	const bool is_valid() const { return m_b_valid; }

	const zsp_device_type get_type() const { return m_type; }
	const zsp_device_modification get_modification() const { return m_modification; }
	
	const uint32_t get_id() const { return m_id; }
	const uint32_t get_slot_serial() const { return m_slot_serial.value_or(0); }
	const double get_resolution() const { return m_resolution.value_or(0.); }

	const bool get_enable() const { return m_enable.value_or(false); }
	void set_enable(const bool value);

	const bool has_charge() const { return m_charge.has_value(); }
	const bool get_charge() const { return m_charge.value_or(false); }
	bool set_charge(const bool value);

	const bool has_icp() const { return m_icp.has_value(); }
	const bool get_icp() const { return m_icp.value_or(false); }
	bool set_icp(const bool value);

	const bool has_diff_mode() const { return m_diff_mode.has_value(); }
	const bool get_diff_mode() const { return m_diff_mode.value_or(false); }
	bool set_diff_mode(const bool value);

	const ::std::wstring get_name() const { return m_name.value_or(L""); }
	void set_name(const ::std::wstring& value);

	const ::std::wstring get_comment() const { return m_comment.value_or(L""); }
	void set_comment(const ::std::wstring& value);
	
	const double get_sense() const { return m_sense.value_or(0.001); }
	void set_sense(const double value);

	const double get_reference() const { return m_reference.value_or(0.001); }
	void set_reference(const double value);

	const double get_shift() const { return m_shift.value_or(0.); }
	void set_shift(const double value);

	const ::std::wstring get_unit_sense() const { return m_unit_sense.value_or(L"Â"); }
	void set_unit_sense(const ::std::wstring value);

	const ::std::wstring get_unit() const { return m_unit.value_or(L"ìÂ"); }
	void set_unit(const ::std::wstring value);

	const double get_ext_amplify() const { return m_ext_amplify.value_or(1.); }
	void set_ext_amplify(const double value);

	const double get_amplify() const { return m_amplify.value_or(1.); }
	bool set_amplify(const double value);
	const ::std::vector<double> get_amplifies() const 
		{ return ::zetlab::get_amplifies_adc({ m_type, m_modification }); }

	const bool get_ac() const { return m_ac.value_or(false); }
	void set_ac(const bool value);

	const double get_x() const { return m_x.value_or(0.); }
	void set_x(const double value);

	const double get_y() const { return m_y.value_or(0.); }
	void set_y(const double value);

	const double get_z() const { return m_z.value_or(0.); }
	void set_z(const double value);

	const uint32_t get_p() const { return m_p.value_or(0); }
	void set_p(const uint32_t value);

	const zsp_tenso_scheme get_tenso_scheme() const { return m_tenso_scheme.value_or(zsp_tenso_scheme::unknown); }
	void set_tenso_scheme(const zsp_tenso_scheme value, bool b_notify);

	const double get_tenso_exc() const { return m_tenso_exc.value_or(0.); }
	void set_tenso_exc(const double value, bool b_notify);

	const double get_tenso_resistance() const { return m_tenso_resistance.value_or(0.); }
	void set_tenso_resistance(const double value);

	const zsp_tenso_sensor get_tenso_sensor() const { return m_tenso_sensor.value_or(zsp_tenso_sensor::unknown); }
	void set_tenso_sensor(const zsp_tenso_sensor value, bool b_notify);

	const double get_tenso_sensor_range() const { return m_tenso_sensor_range.value_or(0.); }
	void set_tenso_sensor_range(const double value);

	const double get_tenso_sensor_output() const { return m_tenso_sensor_output.value_or(0.); }
	void set_tenso_sensor_output(const double value);

	const double get_tenso_resistor_k() const { return m_tenso_resistor_k.value_or(0.); }
	void set_tenso_resistor_k(const double value);

	const zsp_tenso_resistor_mode get_tenso_resistor_mode() const { return m_tenso_resistor_mode.value_or(zsp_tenso_resistor_mode::unknown); }
	void set_tenso_resistor_mode(const zsp_tenso_resistor_mode value, bool b_notify);

	const double get_tenso_poisson_ratio() const { return m_tenso_poisson_ratio.value_or(0.); }
	void set_tenso_poisson_ratio(const double value);

	const double get_tenso_elastic_modulus() const { return m_tenso_elastic_modulus.value_or(0.); }
	void set_tenso_elastic_modulus(const double value);

	const double get_tenso_outer_diameter() const { return m_tenso_outer_diameter.value_or(0.); }
	void set_tenso_outer_diameter(const double value);

	const double get_tenso_inner_diameter() const { return m_tenso_inner_diameter.value_or(0.); }
	void set_tenso_inner_diameter(const double value);

	const zsp_tr_nsh get_tr_nsh() const { return m_tr_nsh.value_or(zsp_tr_nsh::unknown); }
	void set_tr_nsh(const zsp_tr_nsh value);

	const zsp_tc_type get_tc_type() const { return m_tc_type.value_or(zsp_tc_type::unknown); }
	void set_tc_type(const zsp_tc_type value);

protected:
	void load(pugi::xml_node& device_node, pugi::xml_node& channel_node);

	zsp_device_type m_type = zsp_device_type::unknown;
	zsp_device_modification m_modification = zsp_device_modification::unknown;
	uint32_t m_id = g_bad_id;
	save_channel_callback m_save_channel_cb = {};
	change_common_parameter_callback m_change_common_parameter_cb = {};
	
private:
	bool m_b_valid = false;

	::std::optional<double> m_resolution;
	::std::optional<bool> m_enable;
	::std::optional<::std::wstring> m_name;
	::std::optional<::std::wstring> m_comment;
	::std::optional<double> m_sense;
	::std::optional<double> m_reference;
	::std::optional<double> m_shift;
	::std::optional<::std::wstring> m_unit_sense;
	::std::optional<::std::wstring> m_unit;
	::std::optional<bool> m_icp;
	::std::optional<bool> m_charge;
	::std::optional<bool> m_diff_mode;
	::std::optional<double> m_amplify;
	::std::optional<double> m_ext_amplify;
	::std::optional<bool> m_ac;
	::std::optional<double> m_x;
	::std::optional<double> m_y;
	::std::optional<double> m_z;
	::std::optional<uint32_t> m_p;
	::std::optional<uint32_t> m_slot_serial;
	::std::optional<zsp_tenso_scheme> m_tenso_scheme;
	::std::optional<double> m_tenso_exc;
	::std::optional<double> m_tenso_resistance;
	::std::optional<zsp_tenso_sensor> m_tenso_sensor;
	::std::optional<double> m_tenso_sensor_range;
	::std::optional<double> m_tenso_sensor_output;
	::std::optional<double> m_tenso_resistor_k;
	::std::optional<zsp_tenso_resistor_mode> m_tenso_resistor_mode;
	::std::optional<double> m_tenso_poisson_ratio;
	::std::optional<double> m_tenso_elastic_modulus;
	::std::optional<double> m_tenso_outer_diameter;
	::std::optional<double> m_tenso_inner_diameter;
	::std::optional<zsp_tr_nsh> m_tr_nsh;
	::std::optional<zsp_tc_type> m_tc_type;
};

inline bool operator==(const zsp_channel& lhs, const zsp_channel& rhs)
{
	return 0 == lhs.compare(rhs);
}

inline bool operator!=(const zsp_channel& lhs, const zsp_channel& rhs)
{
	return 0 != lhs.compare(rhs);
}

};
