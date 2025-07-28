#include "zsp_config.h"

#include <boost/lexical_cast.hpp>

namespace zetlab {

using namespace pugi;

zsp_config::zsp_config(::std::shared_ptr<::std::wstring> path, const zsp_device_id& id)
	: m_path(path)
	, m_id(id)
{
	auto file_name = *m_path.get() + get_file_name(id);
	m_file_name = file_name + L".cfg";
	m_file_state_name = file_name + L"_state.cfg";
	m_file_metadata_name = file_name + L"_metadata.cfg";
	set_is_alive();

	load();
	load_state();
	load_metadata();
}

void zsp_config::update()
{
	load();
	load_state();
	load_metadata();
}

void zsp_config::save()
{
	if (!m_device)
		return;

	save_metadata();

	std::lock_guard lg(m_xml_mutex);

	auto config_node = m_xml.child(L"Config");
	if (!config_node)
		return;

	auto device_node = config_node.child(L"Device");
	if (!device_node)
		return;

	if (auto freq_node = device_node.child(L"Freq"))
	{
		freq_node.remove_children();
		freq_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(m_device->get_frequency_adc()).c_str());
	}

	if (auto freq_dac_node = device_node.child(L"FreqDAC"))
	{
		freq_dac_node.remove_children();
		freq_dac_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(m_device->get_frequency_dac()).c_str());
	}

	if (auto channel_node = device_node.child(L"Channel"))
	{
		uint64_t channel_mask(0);
		for (const auto& channel : m_channels)
		{
			if (channel.second->get_enable())
				channel_mask += (1ull << channel.first);
		}
		std::wstring s;
		if (auto count = m_channels.size(); count <= CHAR_BIT * sizeof(uint8_t))
			s = int_to_hex(static_cast<uint8_t>(channel_mask));
		else if (count <= CHAR_BIT * sizeof(uint16_t))
			s = int_to_hex(static_cast<uint16_t>(channel_mask));
		else if (count <= CHAR_BIT * sizeof(uint32_t))
			s = int_to_hex(static_cast<uint32_t>(channel_mask));
		else
			s = int_to_hex(channel_mask);
		channel_node.remove_children();
		channel_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	if (auto hcp_channel_node = device_node.child(L"HCPChannel"))
	{
		uint64_t icp_mask(0);
		for (const auto& channel : m_channels)
		{
			if (channel.second->get_icp())
				icp_mask += (1ull << channel.first);
		}
		std::wstring s;
		if (auto count = m_channels.size(); count <= CHAR_BIT * sizeof(uint8_t))
			s = int_to_hex(static_cast<uint8_t>(icp_mask));
		else if (count <= CHAR_BIT * sizeof(uint16_t))
			s = int_to_hex(static_cast<uint16_t>(icp_mask));
		else if (count <= CHAR_BIT * sizeof(uint32_t))
			s = int_to_hex(static_cast<uint32_t>(icp_mask));
		else
			s = int_to_hex(icp_mask);
		hcp_channel_node.remove_children();
		hcp_channel_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	if (auto channel_diff_node = device_node.child(L"ChannelDiff"))
	{
		uint64_t diff_mask(0);
		for (const auto& channel : m_channels)
		{
			if (channel.second->get_diff_mode())
				diff_mask += (1ull << channel.first);
		}
		std::wstring s;
		if (auto count = m_channels.size(); count <= CHAR_BIT * sizeof(uint8_t))
			s = int_to_hex(static_cast<uint8_t>(diff_mask));
		else if (count <= CHAR_BIT * sizeof(uint16_t))
			s = int_to_hex(static_cast<uint16_t>(diff_mask));
		else if (count <= CHAR_BIT * sizeof(uint32_t))
			s = int_to_hex(static_cast<uint32_t>(diff_mask));
		else
			s = int_to_hex(diff_mask);
		channel_diff_node.remove_children();
		channel_diff_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	if (auto kod_amplify_node = device_node.child(L"KodAmplify"))
	{
		std::wstring s;
		for (const auto& channel : m_channels)
		{
			s.append(std::to_wstring(
				get_gain({ m_id.first, channel.second->get_modification() }, channel.second->get_amplify())));
			s.append(L",");
		}
		if (!s.empty())
			s.pop_back();
		kod_amplify_node.remove_children();
		kod_amplify_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	if (auto record_minutes_node = device_node.child(L"RecordMinutes"))
	{
		record_minutes_node.remove_children();
		record_minutes_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_device->get_sd_record_minutes()).c_str());
		auto recorder_node = device_node.child(L"Recorder");
		if (!recorder_node)
			recorder_node = device_node.append_child(L"Recorder");
		auto start_attr = recorder_node.attribute(L"start");
		if (!start_attr)
			recorder_node.append_attribute(L"start");
		if (auto start_type = m_device->get_sd_start_record_type(); 0 == start_type)
			start_attr.set_value(L"button");
		else if (1 == start_type)
			start_attr.set_value(L"auto");
	}

	if (auto ethernet_node = device_node.child(L"Ethernet"))
	{
		if (auto method_attr = ethernet_node.attribute(L"method"))
		{
			if (auto b_static = m_device->get_ethernet_static_address())
				method_attr.set_value(L"static");
			else
				method_attr.set_value(L"dynamic");
		}
		if (auto addr_attr = ethernet_node.attribute(L"addr"))
			addr_attr.set_value(as_wide(m_device->get_ethernet_address()).c_str());
		if (auto ftp_attr = ethernet_node.attribute(L"ftp"))
		{
			if (auto ftp = m_device->get_ftp_enable())
				ftp_attr.set_value(L"yes");
			else
				ftp_attr.set_value(L"no");
		}
	}

	if (auto digital_output_node = device_node.child(L"DigitalOutput"))
	{
		std::wstring s;
		auto digital_output = m_device->get_digital_output();
		if (auto count = m_device->get_dig_bits_count(); count <= CHAR_BIT * sizeof(uint8_t))
			s = int_to_hex(static_cast<uint8_t>(digital_output));
		else if (count <= CHAR_BIT * sizeof(uint16_t))
			s = int_to_hex(static_cast<uint16_t>(digital_output));
		else if (count <= CHAR_BIT * sizeof(uint32_t))
			s = int_to_hex(static_cast<uint32_t>(digital_output));
		else
			s = int_to_hex(digital_output);
		digital_output_node.remove_children();
		digital_output_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	if (auto digital_out_enable_node = device_node.child(L"DigitalOutEnable"))
	{
		std::wstring s;
		auto digital_out_enable = m_device->get_digital_out_enable();
		if (auto count = m_device->get_dig_bits_count(); count <= CHAR_BIT * sizeof(uint8_t))
			s = int_to_hex(static_cast<uint8_t>(digital_out_enable));
		else if (count <= CHAR_BIT * sizeof(uint16_t))
			s = int_to_hex(static_cast<uint16_t>(digital_out_enable));
		else if (count <= CHAR_BIT * sizeof(uint32_t))
			s = int_to_hex(static_cast<uint32_t>(digital_out_enable));
		else
			s = int_to_hex(digital_out_enable);
		digital_out_enable_node.remove_children();
		digital_out_enable_node.append_child(pugi::node_pcdata).set_value(s.c_str());
	}

	uint32_t slot_index(0);
	while (true)
	{
		auto slot_tenso_node = device_node.find_child_by_attribute(L"SlotTenso", L"id", std::to_wstring(slot_index).c_str());
		if (!slot_tenso_node)
			break;

		std::wstring s_scheme;
		double exc(0.);
		try
		{
			auto& channel = m_channels.at(slot_index * 4);
			s_scheme = get_tenso_scheme(channel->get_tenso_scheme());
			exc = channel->get_tenso_exc();
		}
		catch (const std::out_of_range&)
		{
			s_scheme = L"full_4wire_i";
			exc = 0.;
		}

		/*for (uint32_t i = 0; i < 1; ++i)
		//for (uint32_t i = 0; i < 4; ++i)
		{
			auto j = slot_index * 4 + i;
			try
			{
				auto& channel = m_channels.at(j);
				s_scheme = get_tenso_scheme(channel->get_tenso_scheme());
				exc = channel->get_tenso_exc();
			}
			catch (const std::out_of_range&)
			{
				s_scheme = L"full_4wire_i";
				exc = 0.;
			}
		}*/

		std::wstring s_gain;
		for (uint32_t i = 0; i < 4; ++i)
		{
			auto j = slot_index * 4 + i;
			try
			{
				auto& channel = m_channels.at(j);
				s_gain.append(std::to_wstring(static_cast<uint32_t>(channel->get_amplify())));
				s_gain.append(L",");
			}
			catch (const std::out_of_range&)
			{
				s_gain.append(L"1,");
			}
		}
		if (!s_gain.empty())
			s_gain.pop_back();

		if (auto mode_node = slot_tenso_node.child(L"Mode"))
		{
			mode_node.remove_children();
			mode_node.append_child(pugi::node_pcdata).set_value(s_scheme.c_str());
		}
		if (auto exc_node = slot_tenso_node.child(L"Exc"))
		{
			exc_node.remove_children();
			exc_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(exc).c_str());
		}
		if (auto gain_node = slot_tenso_node.child(L"Gain"))
		{
			gain_node.remove_children();
			gain_node.append_child(pugi::node_pcdata).set_value(s_gain.c_str());
		}

		++slot_index;
	}

	if (auto channels_node = device_node.child(L"Channels"))
	{
		for (const auto& channel : m_channels)
		{
			auto channel_node = channels_node.find_child_by_attribute(
				L"Channel", L"id", std::to_wstring(channel.first).c_str());
			if (!channel_node)
				continue;

			auto name_attr = channel_node.attribute(L"name");
			if (!name_attr)
				name_attr = channel_node.append_attribute(L"name");

			auto name = channel.second->get_name();
			if (m_metadata)
				name =
					m_metadata->get_network_code() + L"_" +
					m_metadata->get_station_code() + L"_" +
					m_metadata->get_location(channel.first) + L"_" +
					m_metadata->get_channel(channel.first);
			name_attr.set_value(name.c_str());

			auto comment_attr = channel_node.attribute(L"comment");
			if (!comment_attr)
				comment_attr = channel_node.append_attribute(L"comment");
			comment_attr.set_value(channel.second->get_comment().c_str());

			auto units_attr = channel_node.attribute(L"units");
			if (!units_attr)
				units_attr = channel_node.append_attribute(L"units");
			units_attr.set_value(channel.second->get_unit().c_str());

			auto enabled_attr = channel_node.attribute(L"enabled");
			if (!enabled_attr)
				enabled_attr = channel_node.append_attribute(L"enabled");
			enabled_attr.set_value(channel.second->get_enable());

			auto unit_sense_attr = channel_node.attribute(L"unitsense");
			if (!unit_sense_attr)
				unit_sense_attr = channel_node.append_attribute(L"unitsense");
			unit_sense_attr.set_value(channel.second->get_unit_sense().c_str());

			auto sense_node = channel_node.child(L"Sense");
			if (!sense_node)
				sense_node = channel_node.append_child(L"Sense");
			sense_node.remove_children();
			sense_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_sense()).c_str());

			auto amplify_node = channel_node.child(L"Amplify");
			if (!amplify_node)
				amplify_node = channel_node.append_child(L"Amplify");
			amplify_node.remove_children();
			amplify_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_ext_amplify()).c_str());

			auto reference_node = channel_node.child(L"Reference");
			if (!reference_node)
				reference_node = channel_node.append_child(L"Reference");
			reference_node.remove_children();
			reference_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_reference()).c_str());

			auto shift_node = channel_node.child(L"Shift");
			if (!shift_node)
				shift_node = channel_node.append_child(L"Shift");
			shift_node.remove_children();
			shift_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_shift()).c_str());

			auto hpf_node = channel_node.child(L"HPF");
			if (!hpf_node)
				hpf_node = channel_node.append_child(L"HPF");
			hpf_node.remove_children();
			hpf_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(channel.second->get_ac()).c_str());

			auto coord_x_node = channel_node.child(L"CoordX");
			if (!coord_x_node)
				coord_x_node = channel_node.append_child(L"CoordX");
			coord_x_node.remove_children();
			coord_x_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_x()).c_str());

			auto coord_y_node = channel_node.child(L"CoordY");
			if (!coord_y_node)
				coord_y_node = channel_node.append_child(L"CoordY");
			coord_y_node.remove_children();
			coord_y_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_y()).c_str());

			auto coord_z_node = channel_node.child(L"CoordZ");
			if (!coord_z_node)
				coord_z_node = channel_node.append_child(L"CoordZ");
			coord_z_node.remove_children();
			coord_z_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_z()).c_str());

			auto coord_p_node = channel_node.child(L"CoordP");
			if (!coord_p_node)
				coord_p_node = channel_node.append_child(L"CoordP");
			coord_p_node.remove_children();
			coord_p_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(channel.second->get_p()).c_str());

			bool b_tenso =
				(m_id.first == zsp_device_type::zet037) &&
				(channel.second->get_modification() == zsp_device_modification::t);

			auto tenso_settings_node = channel_node.child(L"TensoSettings");
			if (!tenso_settings_node && b_tenso)
				tenso_settings_node.append_child(L"TensoSettings");
			if (tenso_settings_node)
			{
				auto resistance_node = tenso_settings_node.child(L"Resistance");
				if (!resistance_node)
					resistance_node = tenso_settings_node.append_child(L"Resistance");
				if (resistance_node)
				{
					resistance_node.remove_children();
					resistance_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_resistance()).c_str());
				}

				auto sensor_type_node = tenso_settings_node.child(L"SensorType");
				if (!sensor_type_node)
					sensor_type_node = tenso_settings_node.append_child(L"SensorType");
				if (sensor_type_node)
				{
					sensor_type_node.remove_children();
					sensor_type_node.append_child(pugi::node_pcdata).set_value(get_tenso_sensor(channel.second->get_tenso_sensor()).c_str());
				}

				auto sensor_settings_node = tenso_settings_node.child(L"SensorSettings");
				if (!sensor_settings_node)
					sensor_settings_node = tenso_settings_node.append_child(L"SensorSettings");

				auto range_node = sensor_settings_node.child(L"Range");
				if (!range_node)
					range_node = sensor_settings_node.append_child(L"Range");
				if (range_node)
				{
					range_node.remove_children();
					range_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_sensor_range()).c_str());
				}

				auto output_node = sensor_settings_node.child(L"Output");
				if (!output_node)
					output_node = sensor_settings_node.append_child(L"Output");
				if (output_node)
				{
					output_node.remove_children();
					output_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_sensor_output()).c_str());
				}

				auto resistor_settings_node = tenso_settings_node.child(L"ResistorSettings");
				if (!resistor_settings_node)
					resistor_settings_node = tenso_settings_node.append_child(L"ResistorSettings");

				auto k_node = resistor_settings_node.child(L"K");
				if (!k_node)
					k_node = resistor_settings_node.append_child(L"K");
				if (k_node)
				{
					k_node.remove_children();
					k_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_resistor_k()).c_str());
				}

				auto resistor_mode_node = resistor_settings_node.child(L"Mode");
				if (!resistor_mode_node)
					resistor_mode_node = resistor_settings_node.append_child(L"Mode");
				if (resistor_mode_node)
				{
					resistor_mode_node.remove_children();
					resistor_mode_node.append_child(pugi::node_pcdata).set_value(get_tenso_resistor_mode(channel.second->get_tenso_resistor_mode()).c_str());
				}

				auto poisson_ratio_node = resistor_settings_node.child(L"PoissonRatio");
				if (!poisson_ratio_node)
					poisson_ratio_node = resistor_settings_node.append_child(L"PoissonRatio");
				if (poisson_ratio_node)
				{
					poisson_ratio_node.remove_children();
					poisson_ratio_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_poisson_ratio()).c_str());
				}

				auto elastic_modulus_node = resistor_settings_node.child(L"ElasticModulus");
				if (!elastic_modulus_node)
					elastic_modulus_node = resistor_settings_node.append_child(L"ElasticModulus");
				if (elastic_modulus_node)
				{
					elastic_modulus_node.remove_children();
					elastic_modulus_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_elastic_modulus()).c_str());
				}

				auto outer_diameter_node = resistor_settings_node.child(L"OuterDiameter");
				if (!outer_diameter_node)
					outer_diameter_node = resistor_settings_node.append_child(L"OuterDiameter");
				if (outer_diameter_node)
				{
					outer_diameter_node.remove_children();
					outer_diameter_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_outer_diameter()).c_str());
				}

				auto inner_diameter_node = resistor_settings_node.child(L"InnerDiameter");
				if (!inner_diameter_node)
					inner_diameter_node = resistor_settings_node.append_child(L"InnerDiameter");
				if (inner_diameter_node)
				{
					inner_diameter_node.remove_children();
					inner_diameter_node.append_child(pugi::node_pcdata).set_value(boost::lexical_cast<std::wstring>(channel.second->get_tenso_inner_diameter()).c_str());
				}

				auto tr_settings_node = tenso_settings_node.child(L"TrSettings");
				if (!tr_settings_node)
					tr_settings_node = tenso_settings_node.append_child(L"TrSettings");

				auto nsh_node = tr_settings_node.child(L"NSH");
				if (!nsh_node)
					nsh_node = tr_settings_node.append_child(L"NSH");
				if (nsh_node)
				{
					nsh_node.remove_children();
					nsh_node.append_child(pugi::node_pcdata).set_value(get_tr_nsh(channel.second->get_tr_nsh()).c_str());
				}

				auto tc_settings_node = tenso_settings_node.child(L"TcSettings");
				if (!tc_settings_node)
					tc_settings_node = tenso_settings_node.append_child(L"TcSettings");

				auto type_node = tc_settings_node.child(L"Type");
				if (!type_node)
					type_node = tc_settings_node.append_child(L"Type");
				if (type_node)
				{
					type_node.remove_children();
					type_node.append_child(pugi::node_pcdata).set_value(get_tc_type(channel.second->get_tc_type()).c_str());
				}
			}
		}
	}

	auto config_time_node = device_node.child(L"ConfigTime");
	if (!config_time_node)
		config_time_node = device_node.append_child(L"ConfigTime");

	auto config_time = std::chrono::system_clock::now();
	wchar_t s[32] = { 0 };
	time_t t = std::chrono::system_clock::to_time_t(config_time);
#ifdef _WIN32
	if (tm tm_struct; 0 == gmtime_s(&tm_struct, &t) && 0 != wcsftime(s, 32, L"%d.%m.%Y %H:%M:%S", &tm_struct))
#else
	if (tm tm_struct; gmtime_r(&t, &tm_struct) && 0 != wcsftime(s, 32, L"%d.%m.%Y %H:%M:%S", &tm_struct))
#endif
	config_time_node.remove_children();
	config_time_node.append_child(pugi::node_pcdata).set_value(s);

	m_xml.save_file(m_file_name.c_str());
}

void zsp_config::save_state()
{
	std::lock_guard lg(m_xml_mutex);

	auto state_node = m_xml_state.child(L"State");
	if (!state_node)
		return;

	if (auto selftest_node = state_node.child(L"Selftest"))
	{
		selftest_node.remove_children();
		selftest_node.append_child(pugi::node_pcdata).set_value(m_state->get_selftest().c_str());
	}

	auto default_node = state_node.child(L"Default");
	if (!default_node)
		default_node = state_node.append_child(L"Default");
	default_node.remove_children();
	default_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_state->get_default()).c_str());

	auto service_node = state_node.child(L"Service");
	if (!service_node)
		service_node = state_node.append_child(L"Service");
	service_node.remove_children();
	service_node.append_child(pugi::node_pcdata).set_value(m_state->get_service() ? L"True" : L"False");

	m_xml_state.save_file(m_file_state_name.c_str());
}

void zsp_config::save_metadata()
{
	if (!m_metadata || !m_device)
		return;

	std::lock_guard lg(m_xml_mutex);

	if (m_xml_metadata.empty())
		return;

	auto fdsn_station_xml_node = m_xml_metadata.child(L"FDSNStationXML");
	if (!fdsn_station_xml_node)
		return;

	auto network_node = fdsn_station_xml_node.child(L"Network");
	if (!network_node)
		return;

	if (auto code_attribute = network_node.attribute(L"code"))
		code_attribute.set_value(m_metadata->get_network_code());

	auto station_node = network_node.child(L"Station");
	if (!station_node)
		return;

	if (auto code_attribute = station_node.attribute(L"code"))
		code_attribute.set_value(m_metadata->get_station_code());

	if (auto latitude_node = station_node.child(L"Latitude"))
	{
		latitude_node.remove_children();
		latitude_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_latitude()).c_str());
	}

	if (auto longitude_node = station_node.child(L"Longitude"))
	{
		longitude_node.remove_children();
		longitude_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_longitude()).c_str());
	}

	if (auto elevation_node = station_node.child(L"Elevation"))
	{
		elevation_node.remove_children();
		elevation_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_elevation()).c_str());
	}

	if (auto site_node = station_node.child(L"Site"))
	{
		if (auto name_node = site_node.child(L"Name"))
		{
			name_node.remove_children();
			name_node.append_child(pugi::node_pcdata).set_value(m_metadata->get_site_name().c_str());
		}
	}

	uint32_t i(0);
	auto channel_node = station_node.child(L"Channel");
	while (channel_node)
	{
		if (auto code_attribute = channel_node.attribute(L"code"))
			code_attribute.set_value(m_metadata->get_channel(i));

		if (auto location_code_attribute = channel_node.attribute(L"locationCode"))
			location_code_attribute.set_value(m_metadata->get_location(i));

		if (auto latitude_node = channel_node.child(L"Latitude"))
		{
			latitude_node.remove_children();
			latitude_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_latitude()).c_str());
		}

		if (auto longitude_node = channel_node.child(L"Longitude"))
		{
			longitude_node.remove_children();
			longitude_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_longitude()).c_str());
		}

		if (auto elevation_node = channel_node.child(L"Elevation"))
		{
			elevation_node.remove_children();
			elevation_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_metadata->get_elevation()).c_str());
		}

		if (auto response_node = channel_node.child(L"Response"))
		{
			if (auto sensitivity_node = response_node.child(L"InstrumentSensitivity"))
			{
				try
				{
					auto channel = m_channels.at(i);

					if (auto value_node = sensitivity_node.child(L"Value"))
					{
						auto value = channel->get_amplify() * channel->get_sense() / channel->get_resolution() / pow(2., 8.);
						wchar_t s[64];
						_snwprintf_s(s, 64, L"%g", value);
						std::wstring u(s);
						std::transform(u.begin(), u.end(), u.begin(), [](auto c) { return std::toupper(c); });

						value_node.remove_children();
						value_node.append_child(pugi::node_pcdata).set_value(u.c_str());
					}

					if (auto frequency_node = sensitivity_node.child(L"Frequency"))
					{
						frequency_node.remove_children();
						frequency_node.append_child(pugi::node_pcdata).set_value(std::to_wstring(m_device->get_frequency_adc()).c_str());
					}

					if (auto input_units_node = sensitivity_node.child(L"InputUnits"))
					{
						if (auto name_node = input_units_node.child(L"Name"))
						{
							name_node.remove_children();
							name_node.append_child(pugi::node_pcdata).set_value(channel->get_unit().c_str());
						}
					}
				}
				catch (const std::out_of_range&) {}
			}
		}

		++i;
		channel_node = channel_node.next_sibling(L"Channel");
	}

	m_xml_metadata.save_file(m_file_metadata_name.c_str());
}

void zsp_config::set_is_alive()
{
	m_t_alive = ::std::chrono::steady_clock::now();
}

const bool zsp_config::is_alive() const
{
	return ::std::chrono::steady_clock::now() - m_t_alive < m_alive_timeout;
}

::std::shared_ptr<zsp_state> zsp_config::get_state()
{
	return m_state;
}

::std::shared_ptr<zsp_metadata> zsp_config::get_metadata()
{
	return m_metadata;
}

::std::shared_ptr<zsp_device> zsp_config::get_device()
{
	return m_device;
}

::std::shared_ptr<zsp_channel> zsp_config::get_channel(const uint32_t id)
{
	try { return m_channels.at(id); }
	catch (const ::std::out_of_range&) {}

	return {};
}

void zsp_config::load()
{
	std::lock_guard lg(m_xml_mutex);

	if (::std::error_code ec = {}; !::std::filesystem::exists(m_file_name, ec) || ec)
	{
		unload();
		return;
	}
	
	::std::error_code ec = {};
	auto t = ::std::filesystem::last_write_time(m_file_name, ec);
	if (ec)
	{
		unload();
		return;
	}

	if (m_t_change == t)
		return;

	m_xml.reset();
	if (!m_xml.load_file(m_file_name.c_str()))
	{
		unload();
		return;
	}

	if (!parse())
	{
		unload();
		return;
	}

	m_t_change = t;
}

void zsp_config::unload()
{
	m_xml.reset();
	m_device.reset();
	m_t_change = {};
}

void zsp_config::load_state()
{
	std::lock_guard lg(m_xml_mutex);

	if (::std::error_code ec = {}; !::std::filesystem::exists(m_file_state_name, ec) || ec)
	{
		unload_state();
		return;
	}

	::std::error_code ec = {};
	auto t = ::std::filesystem::last_write_time(m_file_state_name, ec);
	if (ec)
	{
		unload_state();
		return;
	}

	if (m_t_state_change == t)
		return;

	m_xml_state.reset();
	if (!m_xml_state.load_file(m_file_state_name.c_str()))
	{
		unload_state();
		return;
	}

	if (!parse_state())
	{
		unload_state();
		return;
	}

	m_t_state_change = t;
}

void zsp_config::unload_state()
{
	m_xml_state.reset();
	m_state.reset();
	m_t_state_change = {};
}

void zsp_config::load_metadata()
{
	std::lock_guard lg(m_xml_mutex);

	if (::std::error_code ec = {}; !::std::filesystem::exists(m_file_metadata_name, ec) || ec)
	{
		unload_metadata();
		return;
	}

	::std::error_code ec = {};
	auto t = ::std::filesystem::last_write_time(m_file_metadata_name, ec);
	if (ec)
	{
		unload_metadata();
		return;
	}

	if (m_t_metadata_change == t)
		return;

	m_xml_metadata.reset();
	if (!m_xml_metadata.load_file(m_file_metadata_name.c_str()))
	{
		unload_metadata();
		return;
	}

	if (!parse_metadata())
	{
		unload_metadata();
		return;
	}

	m_t_metadata_change = t;
}

void zsp_config::unload_metadata()
{
	m_xml_metadata.reset();
	m_metadata.reset();
	m_t_metadata_change = {};
}

bool zsp_config::parse()
{
	if (m_device)
	{
		m_device->notify_save_device({});
		m_device.reset();
	}
	for (auto& channel : m_channels)
		channel.second->notify_save_channel({});
	m_channels.clear();

	auto config_node = m_xml.child(L"Config");
	if (!config_node)
		return false;

	double version = 0.;
	if (auto version_attr = config_node.attribute(L"version"))
		version = version_attr.as_double();
	if (version < 1.2)
		return false;

	auto device_node = config_node.child(L"Device");
	if (!device_node)
		return false;

	m_device = std::make_shared<zsp_device>(m_id, device_node);
	if (!m_device->is_valid())
		return false;

	m_device->notify_save_device(std::bind(&zsp_config::on_save_device, this));

	if (auto channels_node = device_node.child(L"Channels"))
	{
		auto channel_node = channels_node.child(L"Channel");
		while (channel_node)
		{
			if (auto channel = std::make_shared<zsp_channel>(device_node, channel_node); channel->is_valid())
			{
				channel->notify_save_channel(std::bind(&zsp_config::on_save_channel, this, std::placeholders::_1));
				channel->notify_change_common_parameter(
					std::bind(&zsp_config::on_change_common_parameter, this, std::placeholders::_1));
				auto id = channel->get_id();
				m_channels.emplace(id, std::move(channel));
				//m_channels.emplace(id, std::move(channel)).first->second->notify_save_channel(
				//	std::bind(&zsp_config::on_save_channel, this, std::placeholders::_1));

			}

			channel_node = channel_node.next_sibling(L"Channel");
		}
	}

	return true;
}

bool zsp_config::parse_state()
{
	if (m_state)
	{
		m_state->notify_save_state({});
		m_state.reset();
	}

	auto state_node = m_xml_state.child(L"State");
	if (!state_node)
		return false;

	double version = 0.;
	if (auto version_attr = state_node.attribute(L"version"))
		version = version_attr.as_double();
	if (version < 1.0)
		return false;

	m_state = std::make_shared<zsp_state>(m_id, state_node);
	if (!m_state->is_valid())
		return false;

	m_state->notify_save_state(std::bind(&zsp_config::on_save_state, this));

	return true;
}

bool zsp_config::parse_metadata()
{
	if (m_metadata)
		m_metadata.reset();
	
	auto fdsn_station_xml_node = m_xml_metadata.child(L"FDSNStationXML");
	if (!fdsn_station_xml_node)
		return false;

	m_metadata = std::make_shared<zsp_metadata>(m_id, fdsn_station_xml_node);
	if (!m_metadata->is_valid())
		return false;

	return true;
}

void zsp_config::on_save_channel(const uint32_t id)
{
	save();
}

void zsp_config::on_save_device()
{
	save();
}

void zsp_config::on_save_state()
{
	save_state();
}

void zsp_config::on_change_common_parameter(const uint32_t id)
{
	::std::shared_ptr<zsp_channel> channel = {};
	try {
		channel = m_channels.at(id);
	}
	catch (const std::out_of_range&) {}

	if (!channel)
		return;

	const auto slot_serial = channel->get_slot_serial();
	const auto tenso_exc = channel->get_tenso_exc();
	const auto tenso_scheme = channel->get_tenso_scheme();
	const auto tenso_sensor = channel->get_tenso_sensor();
	const auto tenso_resistor_mode = channel->get_tenso_resistor_mode();
	
	for (const auto chan : m_channels)
	{
		if (chan.second->get_slot_serial() == slot_serial && chan.second->get_id() != id)
		{
			chan.second->set_tenso_exc(tenso_exc, false);
			chan.second->set_tenso_scheme(tenso_scheme, false);
			chan.second->set_tenso_sensor(tenso_sensor, false);
			if (!is_compatible(tenso_resistor_mode, chan.second->get_tenso_resistor_mode()))
				chan.second->set_tenso_resistor_mode(tenso_resistor_mode, false);
		}
	}
}

};