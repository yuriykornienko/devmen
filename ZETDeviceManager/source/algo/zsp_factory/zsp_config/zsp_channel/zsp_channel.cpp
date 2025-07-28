#include "zsp_channel.h"

#include <vector>

namespace zetlab {

zsp_channel::zsp_channel(pugi::xml_node& device_node, pugi::xml_node& channel_node)
{
	load(device_node, channel_node);
}

zsp_channel::zsp_channel(const zsp_channel& value)
	: m_b_valid(value.m_b_valid)
	, m_type(value.m_type)
	, m_modification(value.m_modification)
	, m_id(value.m_id)
	, m_save_channel_cb(value.m_save_channel_cb)
	, m_change_common_parameter_cb(value.m_change_common_parameter_cb)
	, m_resolution(value.m_resolution)
	, m_enable(value.m_enable)
	, m_name(value.m_name)
	, m_comment(value.m_comment)
	, m_sense(value.m_sense)
	, m_reference(value.m_reference)
	, m_shift(value.m_shift)
	, m_unit_sense(value.m_unit_sense)
	, m_unit(value.m_unit)
	, m_icp(value.m_icp)
	, m_charge(value.m_charge)
	, m_diff_mode(value.m_diff_mode)
	, m_amplify(value.m_amplify)
	, m_ext_amplify(value.m_ext_amplify)
	, m_ac(value.m_ac)
	, m_x(value.m_x)
	, m_y(value.m_y)
	, m_z(value.m_z)
	, m_p(value.m_p)
	, m_tenso_scheme(value.m_tenso_scheme)
	, m_tenso_exc(value.m_tenso_exc)
	, m_tenso_resistance(value.m_tenso_resistance)
	, m_tenso_sensor(value.m_tenso_sensor)
	, m_tenso_sensor_range(value.m_tenso_sensor_range)
	, m_tenso_sensor_output(value.m_tenso_sensor_output)
	, m_tenso_resistor_k(value.m_tenso_resistor_k)
	, m_tenso_resistor_mode(value.m_tenso_resistor_mode)
	, m_tenso_poisson_ratio(value.m_tenso_poisson_ratio)
	, m_tenso_elastic_modulus(value.m_tenso_elastic_modulus)
	, m_tenso_outer_diameter(value.m_tenso_outer_diameter)
	, m_tenso_inner_diameter(value.m_tenso_inner_diameter)
	, m_tr_nsh(value.m_tr_nsh)
	, m_tc_type(value.m_tc_type)
{}

zsp_channel::zsp_channel(zsp_channel&& value) noexcept
	: m_b_valid(std::exchange(value.m_b_valid, false))
	, m_type(std::exchange(value.m_type, zsp_device_type::unknown))
	, m_modification(std::exchange(value.m_modification, zsp_device_modification::unknown))
	, m_id(std::exchange(value.m_id, g_bad_id))
	, m_save_channel_cb(std::move(value.m_save_channel_cb))
	, m_change_common_parameter_cb(std::move(value.m_change_common_parameter_cb))
	, m_resolution(std::move(value.m_resolution))
	, m_enable(std::move(value.m_enable))
	, m_name(std::move(value.m_name))
	, m_comment(std::move(value.m_comment))
	, m_sense(std::move(value.m_sense))
	, m_reference(std::move(value.m_reference))
	, m_shift(std::move(value.m_shift))
	, m_unit_sense(std::move(value.m_unit_sense))
	, m_unit(std::move(value.m_unit))
	, m_icp(std::move(value.m_icp))
	, m_charge(std::move(value.m_charge))
	, m_diff_mode(std::move(value.m_diff_mode))
	, m_amplify(std::move(value.m_amplify))
	, m_ext_amplify(std::move(value.m_ext_amplify))
	, m_ac(std::move(value.m_ac))
	, m_x(std::move(value.m_x))
	, m_y(std::move(value.m_y))
	, m_z(std::move(value.m_z))
	, m_p(std::move(value.m_p))
	, m_tenso_scheme(std::move(value.m_tenso_scheme))
	, m_tenso_exc(std::move(value.m_tenso_exc))
	, m_tenso_resistance(std::move(value.m_tenso_resistance))
	, m_tenso_sensor(std::move(value.m_tenso_sensor))
	, m_tenso_sensor_range(std::move(value.m_tenso_sensor_range))
	, m_tenso_sensor_output(std::move(value.m_tenso_sensor_output))
	, m_tenso_resistor_k(std::move(value.m_tenso_resistor_k))
	, m_tenso_resistor_mode(std::move(value.m_tenso_resistor_mode))
	, m_tenso_poisson_ratio(std::move(value.m_tenso_poisson_ratio))
	, m_tenso_elastic_modulus(std::move(value.m_tenso_elastic_modulus))
	, m_tenso_outer_diameter(std::move(value.m_tenso_outer_diameter))
	, m_tenso_inner_diameter(std::move(value.m_tenso_inner_diameter))
	, m_tr_nsh(std::move(value.m_tr_nsh))
	, m_tc_type(std::move(value.m_tc_type))
{}

zsp_channel& zsp_channel::operator=(const zsp_channel& value)
{
	if (this != &value)
	{
		m_b_valid = value.m_b_valid;
		m_type = value.m_type;
		m_modification = value.m_modification;
		m_id = value.m_id;
		m_save_channel_cb = value.m_save_channel_cb;
		m_change_common_parameter_cb = value.m_change_common_parameter_cb;
		m_resolution = value.m_resolution;
		m_enable = value.m_enable;
		m_name = value.m_name;
		m_comment = value.m_comment;
		m_sense = value.m_sense;
		m_reference = value.m_reference;
		m_shift = value.m_shift;
		m_unit_sense = value.m_unit_sense;
		m_unit = value.m_unit;
		m_icp = value.m_icp;
		m_charge = value.m_charge;
		m_diff_mode = value.m_diff_mode;
		m_amplify = value.m_amplify;
		m_ext_amplify = value.m_ext_amplify;
		m_ac = value.m_ac;
		m_x = value.m_x;
		m_y = value.m_y;
		m_z = value.m_z;
		m_p = value.m_p;
		m_tenso_scheme = value.m_tenso_scheme;
		m_tenso_exc = value.m_tenso_exc;
		m_tenso_resistance = value.m_tenso_resistance;
		m_tenso_sensor = value.m_tenso_sensor;
		m_tenso_sensor_range = value.m_tenso_sensor_range;
		m_tenso_sensor_output = value.m_tenso_sensor_output;
		m_tenso_resistor_k = value.m_tenso_resistor_k;
		m_tenso_resistor_mode = value.m_tenso_resistor_mode;
		m_tenso_poisson_ratio = value.m_tenso_poisson_ratio;
		m_tenso_elastic_modulus = value.m_tenso_elastic_modulus;
		m_tenso_outer_diameter = value.m_tenso_outer_diameter;
		m_tenso_inner_diameter = value.m_tenso_inner_diameter;
		m_tr_nsh = value.m_tr_nsh;
		m_tc_type = value.m_tc_type;
	}

	return *this;
}

zsp_channel& zsp_channel::operator=(zsp_channel&& value) noexcept
{
	if (this != &value)
	{
		m_b_valid = std::exchange(value.m_b_valid, false);
		m_type = std::exchange(value.m_type, zsp_device_type::unknown);
		m_modification = std::exchange(value.m_modification, zsp_device_modification::unknown);
		m_id = std::exchange(value.m_id, g_bad_id);
		m_save_channel_cb = std::move(value.m_save_channel_cb);
		m_change_common_parameter_cb = std::move(value.m_change_common_parameter_cb);
		m_resolution = std::move(value.m_resolution);
		m_enable = std::move(value.m_enable);
		m_name = std::move(value.m_name);
		m_comment = std::move(value.m_comment);
		m_sense = std::move(value.m_sense);
		m_reference = std::move(value.m_reference);
		m_shift = std::move(value.m_shift);
		m_unit_sense = std::move(value.m_unit_sense);
		m_unit = std::move(value.m_unit);
		m_icp = std::move(value.m_icp);
		m_charge = std::move(value.m_charge);
		m_diff_mode = std::move(value.m_diff_mode);
		m_amplify = std::move(value.m_amplify);
		m_ext_amplify = std::move(value.m_ext_amplify);
		m_ac = std::move(value.m_ac);
		m_x = std::move(value.m_x);
		m_y = std::move(value.m_y);
		m_z = std::move(value.m_z);
		m_p = std::move(value.m_p);
		m_tenso_scheme = std::move(value.m_tenso_scheme);
		m_tenso_exc = std::move(value.m_tenso_exc);
		m_tenso_resistance = std::move(value.m_tenso_resistance);
		m_tenso_sensor = std::move(value.m_tenso_sensor);
		m_tenso_sensor_range = std::move(value.m_tenso_sensor_range);
		m_tenso_sensor_output = std::move(value.m_tenso_sensor_output);
		m_tenso_resistor_k = std::move(value.m_tenso_resistor_k);
		m_tenso_resistor_mode = std::move(value.m_tenso_resistor_mode);
		m_tenso_poisson_ratio = std::move(value.m_tenso_poisson_ratio);
		m_tenso_elastic_modulus = std::move(value.m_tenso_elastic_modulus);
		m_tenso_outer_diameter = std::move(value.m_tenso_outer_diameter);
		m_tenso_inner_diameter = std::move(value.m_tenso_inner_diameter);
		m_tr_nsh = std::move(value.m_tr_nsh);
		m_tc_type = std::move(value.m_tc_type);
	}

	return *this;
}

void zsp_channel::save()
{
	if (m_save_channel_cb)
		m_save_channel_cb(m_id);
}

const int32_t zsp_channel::compare(const zsp_channel& value) const
{
	if (m_b_valid != value.m_b_valid)
		return m_b_valid < value.m_b_valid ? -1 : 1;

	if (m_type != value.m_type)
		return m_type < value.m_type ? -1 : 1;
	
	if (m_modification != value.m_modification)
		return m_modification < value.m_modification ? -1 : 1;

	if (m_id != value.m_id)
		return m_id < value.m_id ? -1 : 1;

	if (m_resolution != value.m_resolution)
		return m_resolution < value.m_resolution ? -1 : 1;

	if (m_enable != value.m_enable)
		return m_enable < value.m_enable ? -1 : 1;

	if (m_name != value.m_name)
		return m_name < value.m_name ? -1 : 1;

	if (m_comment != value.m_comment)
		return m_comment < value.m_comment ? -1 : 1;

	if (m_sense != value.m_sense)
		return m_sense < value.m_sense ? -1 : 1;

	if (m_reference != value.m_reference)
		return m_reference < value.m_reference ? -1 : 1;

	if (m_shift != value.m_shift)
		return m_shift < value.m_shift ? -1 : 1;

	if (m_unit_sense != value.m_unit_sense)
		return m_unit_sense < value.m_unit_sense ? -1 : 1;

	if (m_unit != value.m_unit)
		return m_unit < value.m_unit ? -1 : 1;

	if (m_icp != value.m_icp)
		return m_icp < value.m_icp ? -1 : 1;

	if (m_charge != value.m_charge)
		return m_charge < value.m_charge ? -1 : 1;

	if (m_diff_mode != value.m_diff_mode)
		return m_diff_mode < value.m_diff_mode ? -1 : 1;
	
	if (m_amplify != value.m_amplify)
		return m_amplify < value.m_amplify ? -1 : 1;

	if (m_ext_amplify != value.m_ext_amplify)
		return m_ext_amplify < value.m_ext_amplify ? -1 : 1;

	if (m_ac != value.m_ac)
		return m_ac < value.m_ac ? -1 : 1;

	if (m_x != value.m_x)
		return m_x < value.m_x ? -1 : 1;

	if (m_y != value.m_y)
		return m_y < value.m_y ? -1 : 1;

	if (m_z != value.m_z)
		return m_z < value.m_z ? -1 : 1;

	if (m_p != value.m_p)
		return m_p < value.m_p ? -1 : 1;

	if (m_tenso_scheme != value.m_tenso_scheme)
		return m_tenso_scheme < value.m_tenso_scheme ? -1 : 1;

	if (m_tenso_exc != value.m_tenso_exc)
		return m_tenso_exc < value.m_tenso_exc ? -1 : 1;
	
	if (m_tenso_resistance != value.m_tenso_resistance)
		return m_tenso_resistance < value.m_tenso_resistance ? -1 : 1;
	
	if (m_tenso_sensor != value.m_tenso_sensor)
		return m_tenso_sensor < value.m_tenso_sensor ? -1 : 1;
	
	if (m_tenso_sensor_range != value.m_tenso_sensor_range)
		return m_tenso_sensor_range < value.m_tenso_sensor_range ? -1 : 1;
	
	if (m_tenso_sensor_output != value.m_tenso_sensor_output)
		return m_tenso_sensor_output < value.m_tenso_sensor_output ? -1 : 1;
	
	if (m_tenso_resistor_k != value.m_tenso_resistor_k)
		return m_tenso_resistor_k < value.m_tenso_resistor_k ? -1 : 1;

	if (m_tenso_resistor_mode != value.m_tenso_resistor_mode)
		return m_tenso_resistor_mode < value.m_tenso_resistor_mode ? -1 : 1;
	
	if (m_tenso_poisson_ratio != value.m_tenso_poisson_ratio)
		return m_tenso_poisson_ratio < value.m_tenso_poisson_ratio ? -1 : 1;
	
	if (m_tenso_elastic_modulus != value.m_tenso_elastic_modulus)
		return m_tenso_elastic_modulus < value.m_tenso_elastic_modulus ? -1 : 1;
	
	if (m_tenso_outer_diameter != value.m_tenso_outer_diameter)
		return m_tenso_outer_diameter < value.m_tenso_outer_diameter ? -1 : 1;

	if (m_tenso_inner_diameter != value.m_tenso_inner_diameter)
		return m_tenso_inner_diameter < value.m_tenso_inner_diameter ? -1 : 1;
	
	if (m_tr_nsh != value.m_tr_nsh)
		return m_tr_nsh < value.m_tr_nsh ? -1 : 1;
	
	if (m_tc_type != value.m_tc_type)
		return m_tc_type < value.m_tc_type ? -1 : 1;

	return 0;
}

void zsp_channel::set_enable(const bool value)
{
	if (m_enable.has_value())
		m_enable = value;
}

bool zsp_channel::set_charge(const bool value)
{
	auto charge = m_charge;
	if (m_charge.has_value())
		m_charge = value;
	return m_charge != charge;
}

bool zsp_channel::set_icp(const bool value)
{
	auto icp = m_icp;
	if (m_icp.has_value())
		m_icp = value;
	return m_icp != icp;
}

bool zsp_channel::set_diff_mode(const bool value)
{
	auto diff_mode = m_diff_mode;
	if (m_diff_mode.has_value())
		m_diff_mode = value;
	return m_diff_mode != diff_mode;
}

void zsp_channel::set_name(const::std::wstring& value)
{
	m_name = value;
}

void zsp_channel::set_comment(const::std::wstring& value)
{
	m_comment = value;
}

void zsp_channel::set_sense(const double value)
{
	m_sense = value;
}

void zsp_channel::set_reference(const double value)
{
	m_reference = value;
}

void zsp_channel::set_shift(const double value)
{
	m_shift = value;
}

void zsp_channel::set_unit_sense(const::std::wstring value)
{
	m_unit_sense = value;
}

void zsp_channel::set_unit(const::std::wstring value)
{
	m_unit = value;
}

void zsp_channel::set_ext_amplify(const double value)
{
	m_ext_amplify = value;
}

bool zsp_channel::set_amplify(const double value)
{
	auto amplify = m_amplify;
	if (m_amplify.has_value())
		m_amplify = value;
	return m_amplify != amplify;
}

void zsp_channel::set_ac(const bool value)
{
	m_ac = value;
}

void zsp_channel::set_x(const double value)
{
	m_x = value;
}

void zsp_channel::set_y(const double value)
{
	m_y = value;
}

void zsp_channel::set_z(const double value)
{
	m_z = value;
}

void zsp_channel::set_p(const uint32_t value)
{
	m_p = value;
}

void zsp_channel::set_tenso_scheme(const zsp_tenso_scheme value, bool b_notify)
{
	m_tenso_scheme = value;
	if (b_notify && m_change_common_parameter_cb)
		m_change_common_parameter_cb(m_id);
}

void zsp_channel::set_tenso_exc(const double value, bool b_notify)
{
	m_tenso_exc = value;
	if (b_notify && m_change_common_parameter_cb)
		m_change_common_parameter_cb(m_id);
}

void zsp_channel::set_tenso_resistance(const double value)
{
	m_tenso_resistance = value;
}

void zsp_channel::set_tenso_sensor(const zsp_tenso_sensor value, bool b_notify)
{
	m_tenso_sensor = value;
	if (b_notify && m_change_common_parameter_cb)
		m_change_common_parameter_cb(m_id);
}

void zsp_channel::set_tenso_sensor_range(const double value)
{
	m_tenso_sensor_range = value;
}

void zsp_channel::set_tenso_sensor_output(const double value)
{
	m_tenso_sensor_output = value;
}

void zsp_channel::set_tenso_resistor_k(const double value)
{
	m_tenso_resistor_k = value;
}

void zsp_channel::set_tenso_resistor_mode(const zsp_tenso_resistor_mode value, bool b_notify)
{
	m_tenso_resistor_mode = value;
	if (b_notify && m_change_common_parameter_cb)
		m_change_common_parameter_cb(m_id);
}

void zsp_channel::set_tenso_poisson_ratio(const double value)
{
	m_tenso_poisson_ratio = value;
}

void zsp_channel::set_tenso_elastic_modulus(const double value)
{
	m_tenso_elastic_modulus = value;
}

void zsp_channel::set_tenso_outer_diameter(const double value)
{
	m_tenso_outer_diameter = value;
}

void zsp_channel::set_tenso_inner_diameter(const double value)
{
	m_tenso_inner_diameter = value;
}

void zsp_channel::set_tr_nsh(const zsp_tr_nsh value)
{
	m_tr_nsh = value;
}

void zsp_channel::set_tc_type(const zsp_tc_type value)
{
	m_tc_type = value;
}

void zsp_channel::load(pugi::xml_node& device_node, pugi::xml_node& channel_node)
{
	m_b_valid = false;

	if (auto type_attr = device_node.attribute(L"type"))
		m_type = get_device_type(type_attr.as_uint());

	if (m_type == zsp_device_type::unknown)
		return;

	if (auto id_attr = channel_node.attribute(L"id"))
		m_id = id_attr.as_uint();

	if (g_bad_id == m_id)
		return;

	auto slot_index = m_id / 4;
	auto slot_tenso_node = device_node.find_child_by_attribute(L"SlotTenso", L"id", std::to_wstring(slot_index).c_str());
	if (m_type == zsp_device_type::zet037)
		m_modification = slot_tenso_node ? zsp_device_modification::t : zsp_device_modification::a;

	if (auto name_attr = channel_node.attribute(L"name"))
		m_name = name_attr.as_string();

	if (auto comment_attr = channel_node.attribute(L"comment"))
		m_comment = comment_attr.as_string();

	if (auto unit_sense_attr = channel_node.attribute(L"unitsense"))
		m_unit_sense = unit_sense_attr.as_string();

	if (auto units_attr = channel_node.attribute(L"units"))
		m_unit = units_attr.as_string();

	if (auto sense_node = channel_node.child(L"Sense"))
		m_sense = sense_node.text().as_double();

	if (auto reference_node = channel_node.child(L"Reference"))
		m_reference = reference_node.text().as_double();

	if (auto shift_node = channel_node.child(L"Shift"))
		m_shift = shift_node.text().as_double();

	if (auto amplify_node = channel_node.child(L"Amplify"))
		m_ext_amplify = amplify_node.text().as_double();

	if (auto hpf_node = channel_node.child(L"HPF"))
		m_ac = !!hpf_node.text().as_uint();

	if (auto coord_x_node = channel_node.child(L"CoordX"))
		m_x = coord_x_node.text().as_double();

	if (auto coord_y_node = channel_node.child(L"CoordY"))
		m_y = coord_y_node.text().as_double();

	if (auto coord_z_node = channel_node.child(L"CoordZ"))
		m_z = coord_z_node.text().as_double();

	if (auto coord_p_node = channel_node.child(L"CoordP"))
		m_p = coord_p_node.text().as_uint();

	if (auto tenso_settings_node = channel_node.child(L"TensoSettings"))
	{
		if (auto resistance_node = tenso_settings_node.child(L"Resistance"))
			m_tenso_resistance = resistance_node.text().as_double();

		if (auto sensor_type_node = tenso_settings_node.child(L"SensorType"))
			m_tenso_sensor = ::zetlab::get_tenso_sensor(sensor_type_node.text().as_string());

		if (auto sensor_settings_node = tenso_settings_node.child(L"SensorSettings"))
		{
			if (auto range_node = sensor_settings_node.child(L"Range"))
				m_tenso_sensor_range = range_node.text().as_double();

			if (auto output_node = sensor_settings_node.child(L"Output"))
				m_tenso_sensor_output = output_node.text().as_double();
		}

		if (auto resistor_settings_node = tenso_settings_node.child(L"ResistorSettings"))
		{
			if (auto k_node = resistor_settings_node.child(L"K"))
				m_tenso_resistor_k = k_node.text().as_double();

			if (auto resistor_mode_node = resistor_settings_node.child(L"Mode"))
				m_tenso_resistor_mode = ::zetlab::get_tenso_resistor_mode(resistor_mode_node.text().as_string());

			if (auto resistor_poisson_ratio_node = resistor_settings_node.child(L"PoissonRatio"))
				m_tenso_poisson_ratio = resistor_poisson_ratio_node.text().as_double();

			if (auto resistor_elastic_modulus_node = resistor_settings_node.child(L"ElasticModulus"))
				m_tenso_elastic_modulus = resistor_elastic_modulus_node.text().as_double();

			if (auto resistor_outer_diameter_node = resistor_settings_node.child(L"OuterDiameter"))
				m_tenso_outer_diameter = resistor_outer_diameter_node.text().as_double();

			if (auto resistor_inner_diameter_node = resistor_settings_node.child(L"InnerDiameter"))
				m_tenso_inner_diameter = resistor_inner_diameter_node.text().as_double();
		}

		if (auto tr_settings_node = tenso_settings_node.child(L"TrSettings"))
		{
			if (auto nsh_node = tr_settings_node.child(L"NSH"))
				m_tr_nsh = ::zetlab::get_tr_nsh(nsh_node.text().as_string());
		}

		if (auto tc_settings_node = tenso_settings_node.child(L"TcSettings"))
		{
			if (auto type_node = tc_settings_node.child(L"Type"))
				m_tc_type = ::zetlab::get_tc_type(type_node.text().as_string());
		}
	}

	if (auto channel_node = device_node.child(L"Channel"))
	{
		uint64_t channel_mask = channel_node.text().as_ullong();
		m_enable = channel_mask & (1ull << m_id);
	}

	if (auto hcp_channel_node = device_node.child(L"HCPChannel"))
	{
		uint64_t icp_mask = hcp_channel_node.text().as_ullong();
		m_icp = icp_mask & (1ull << m_id);
	}

	if (auto channel_diff_node = device_node.child(L"ChannelDiff"))
	{
		uint64_t diff_mask = channel_diff_node.text().as_ullong();
		m_diff_mode = diff_mask & (1ull << m_id);
	}

	if (auto resolution_node = device_node.child(L"DigitalResolChanADC"))
	{
		::std::wstring resolution_str = resolution_node.text().as_string();
		::std::vector<double> resolutions;
		while (true)
		{
			if (resolution_str.empty())
				break;

			std::wstring s;
			if (auto i = resolution_str.find(L","); i == ::std::wstring::npos)
			{
				s = resolution_str;
				resolution_str.clear();
			}
			else
			{
				s = resolution_str.substr(0, i);
				resolution_str.erase(0, i + 1);
			}
			if (!s.empty())
				resolutions.emplace_back(wcstod(s.c_str(), nullptr));
		}

		if (resolutions.size() > m_id)
			m_resolution = resolutions[m_id];
	}

	if (auto kod_amplify_node = device_node.child(L"KodAmplify"))
	{
		::std::wstring kod_amplify_str = kod_amplify_node.text().as_string();
		::std::vector<uint32_t> kods;
		while (true)
		{
			if (kod_amplify_str.empty())
				break;

			std::wstring s;
			if (auto i = kod_amplify_str.find(L","); i == ::std::wstring::npos)
			{
				s = kod_amplify_str;
				kod_amplify_str.clear();
			}
			else
			{
				s = kod_amplify_str.substr(0, i);
				kod_amplify_str.erase(0, i + 1);
			}
			if (!s.empty())
				kods.emplace_back(wcstoul(s.c_str(), nullptr, 0));
		}

		if (kods.size() > m_id)
			m_amplify = get_gain({ m_type, m_modification}, kods[m_id]);
	}

	m_slot_serial.reset();
	if (auto slot_serial_node = device_node.child(L"SlotSerial"))
	{
		::std::wstring slot_serial_str = slot_serial_node.text().as_string();
		::std::vector<uint32_t> slot_serials;
		while (true)
		{
			if (slot_serial_str.empty())
				break;

			std::wstring s;
			if (auto i = slot_serial_str.find(L","); i == ::std::wstring::npos)
			{
				s = slot_serial_str;
				slot_serial_str.clear();
			}
			else
			{
				s = slot_serial_str.substr(0, i);
				slot_serial_str.erase(0, i + 1);
			}
			if (!s.empty())
				slot_serials.emplace_back(wcstoul(s.c_str(), nullptr, 0));
		}
		
		if (slot_serials.size() > slot_index)
			m_slot_serial = slot_serials[slot_index];
	}

	if (slot_tenso_node)
	{
		if (auto mode_node = slot_tenso_node.child(L"Mode"))
			m_tenso_scheme = ::zetlab::get_tenso_scheme(mode_node.text().as_string());

		if (auto exc_node = slot_tenso_node.child(L"Exc"))
			m_tenso_exc = exc_node.text().as_double();

		if (auto gain_node = slot_tenso_node.child(L"Gain"))
		{
			auto id = m_id % 4;

			::std::wstring gain_str = gain_node.text().as_string();
			::std::vector<uint32_t> gains;
			while (true)
			{
				if (gain_str.empty())
					break;

				std::wstring s;
				if (auto i = gain_str.find(L","); i == ::std::wstring::npos)
				{
					s = gain_str;
					gain_str.clear();
				}
				else
				{
					s = gain_str.substr(0, i);
					gain_str.erase(0, i + 1);
				}
				if (!s.empty())
					gains.emplace_back(wcstoul(s.c_str(), nullptr, 0));
			}

			if (gains.size() > id)
				m_amplify = gains[id];
		}
	}

	m_b_valid = true;
}

};