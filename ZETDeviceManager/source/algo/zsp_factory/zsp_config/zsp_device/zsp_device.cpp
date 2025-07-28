#include "zsp_device.h"

namespace zetlab {

using namespace pugi;

zsp_device::zsp_device(const zsp_device_id& id, xml_node& node)
	: m_id(id)
{
	load(node);
}

zsp_device::zsp_device(const zsp_device& value)
	: m_b_valid(value.m_b_valid)
	, m_id(value.m_id)
	, m_name(value.m_name)
	, m_freq_adc(value.m_freq_adc)
	, m_freq_dac(value.m_freq_dac)
	, m_sd_record_minutes(value.m_sd_record_minutes)
	, m_sd_start_record_type(value.m_sd_start_record_type)
	, m_eth_address(value.m_eth_address)
	, m_eth_static_address(value.m_eth_static_address)
	, m_ftp_enable(value.m_ftp_enable)
	, m_digital_output(value.m_digital_output)
	, m_digital_out_enable(value.m_digital_out_enable)
	, m_service_mode_enable(value.m_service_mode_enable)
	, m_metadata_enable(value.m_metadata_enable)
{}

zsp_device::zsp_device(zsp_device&& value) noexcept
	: m_b_valid(std::exchange(value.m_b_valid, false))
	, m_id(std::move(value.m_id))
	, m_name(std::move(value.m_name))
	, m_freq_adc(std::move(value.m_freq_adc))
	, m_freq_dac(std::move(value.m_freq_dac))
	, m_sd_record_minutes(std::move(value.m_sd_record_minutes))
	, m_sd_start_record_type(std::move(value.m_sd_start_record_type))
	, m_eth_address(std::move(value.m_eth_address))
	, m_eth_static_address(std::move(value.m_eth_static_address))
	, m_ftp_enable(std::move(value.m_ftp_enable))
	, m_digital_output(std::move(value.m_digital_output))
	, m_digital_out_enable(std::move(value.m_digital_out_enable))
	, m_service_mode_enable(std::move(value.m_service_mode_enable))
	, m_metadata_enable(std::move(value.m_metadata_enable))
{}

zsp_device& zsp_device::operator=(const zsp_device& value)
{
	if (this != &value)
	{
		m_b_valid = value.m_b_valid;
		m_id = value.m_id;
		m_name = value.m_name;
		m_freq_adc = value.m_freq_adc;
		m_freq_dac = value.m_freq_dac;
		m_sd_record_minutes = value.m_sd_record_minutes;
		m_sd_start_record_type = value.m_sd_start_record_type;
		m_eth_address = value.m_eth_address;
		m_eth_static_address = value.m_eth_static_address;
		m_ftp_enable = value.m_ftp_enable;
		m_digital_output = value.m_digital_output;
		m_digital_out_enable = value.m_digital_out_enable;
		m_service_mode_enable = value.m_service_mode_enable;
		m_metadata_enable = value.m_metadata_enable;
	}

	return *this;
}

zsp_device& zsp_device::operator=(zsp_device&& value) noexcept
{
	if (this != &value)
	{
		m_b_valid = std::exchange(value.m_b_valid, false);
		m_id = std::move(value.m_id);
		m_name = std::move(value.m_name);
		m_freq_adc = std::move(value.m_freq_adc);
		m_freq_dac = std::move(value.m_freq_dac);
		m_sd_record_minutes = std::move(value.m_sd_record_minutes);
		m_sd_start_record_type = std::move(value.m_sd_start_record_type);
		m_eth_address = std::move(value.m_eth_address);
		m_eth_static_address = std::move(value.m_eth_static_address);
		m_ftp_enable = std::move(value.m_ftp_enable);
		m_digital_output = std::move(value.m_digital_output);
		m_digital_out_enable = std::move(value.m_digital_out_enable);
		m_service_mode_enable = std::move(value.m_service_mode_enable);
		m_metadata_enable = std::move(value.m_metadata_enable);
	}

	return *this;
}

void zsp_device::save()
{
	if (m_save_device_cb)
		m_save_device_cb();
}

const int32_t zsp_device::compare(const zsp_device& value) const
{
	if (m_b_valid != value.m_b_valid)
		return m_b_valid < value.m_b_valid ? -1 : 1;

	if (m_id != value.m_id)
		return m_id < value.m_id ? -1 : 1;

	if (auto result = m_name.compare(value.m_name); 0 != result)
		return result;

	if (m_freq_adc != value.m_freq_adc)
		return m_freq_adc < value.m_freq_adc ? -1 : 1;

	if (m_freq_adc != value.m_freq_adc)
		return m_freq_adc < value.m_freq_adc ? -1 : 1;

	if (m_sd_record_minutes != value.m_sd_record_minutes)
		return m_sd_record_minutes < value.m_sd_record_minutes ? -1 : 1;

	if (m_sd_start_record_type != value.m_sd_start_record_type)
		return m_sd_start_record_type < value.m_sd_start_record_type ? -1 : 1;

	if (m_eth_address != value.m_eth_address)
		return m_eth_address < value.m_eth_address ? -1 : 1;

	if (m_eth_static_address != value.m_eth_static_address)
		return m_eth_static_address < value.m_eth_static_address ? -1 : 1;

	if (m_ftp_enable != value.m_ftp_enable)
		return m_ftp_enable < value.m_ftp_enable ? -1 : 1;

	if (m_digital_output != value.m_digital_output)
		return m_digital_output < value.m_digital_output ? -1 : 1;

	if (m_digital_out_enable != value.m_digital_out_enable)
		return m_digital_out_enable < value.m_digital_out_enable ? -1 : 1;

	if (m_service_mode_enable != value.m_service_mode_enable)
		return m_service_mode_enable < value.m_service_mode_enable ? -1 : 1;
	
	if (m_metadata_enable != value.m_metadata_enable)
		return m_metadata_enable < value.m_metadata_enable ? -1 : 1;

	return 0;
}

bool zsp_device::set_frequency_adc(const uint32_t value)
{
	auto freq_adc = m_freq_adc;
	if (m_freq_adc.has_value())
		m_freq_adc = value;
	return m_freq_adc != freq_adc;
}

bool zsp_device::set_frequency_dac(const uint32_t value)
{
	auto freq_dac = m_freq_dac;
	if (m_freq_dac.has_value())
		m_freq_dac = value;
	return m_freq_dac != freq_dac;
}

const::std::vector<uint32_t> zsp_device::get_frequencies_dac() const
{
	if (m_freq_dac.has_value())
		return ::zetlab::get_frequencies_dac(m_id.first);

	return {};
}

void zsp_device::set_sd_record_minutes(const uint32_t minutes)
{
	m_sd_record_minutes = minutes;
}

void zsp_device::set_sd_record_continuous(const bool continuous)
{
	m_sd_record_minutes = continuous ? 0 : 10;
}

void zsp_device::set_sd_start_record_type(const uint16_t type)
{
	m_sd_start_record_type = type;
}

void zsp_device::set_ethernet_address(const std::string address)
{
	m_eth_address = address;
}

void zsp_device::set_ethernet_static_address(const bool enable)
{
	m_eth_static_address = enable;
}

void zsp_device::set_ftp_enable(const bool enable)
{
	m_ftp_enable = enable;
}

void zsp_device::set_digital_output(const uint64_t digital_output)
{
	m_digital_output = digital_output;
}

void zsp_device::set_digital_out_enable(const uint64_t digital_out_enable)
{
	m_digital_out_enable = digital_out_enable;
}

void zsp_device::load(pugi::xml_node& node)
{
	m_b_valid = false;

	uint32_t type(0);
	if (auto type_attr = node.attribute(L"type"))
		type = type_attr.as_uint();
	if (m_id.first != get_device_type(type))
		return;

	if (m_id.first == zsp_device_type::zet037)
		m_modification = node.child(L"SlotTenso") ? zsp_device_modification::t : zsp_device_modification::a;

	if (m_id.first == zsp_device_type::zet048)
		m_metadata_enable = true;

	uint64_t serial(0);
	if (auto serial_attr = node.attribute(L"serial"))
		serial = serial_attr.as_ullong();
	if (m_id.second != serial)
		return;

	m_name.clear();
	if (auto name_attr = node.attribute(L"name"))
		m_name = name_attr.as_string();
	if (m_name.empty())
		m_name = get_device_name(m_id.first);
	
	if (auto freq_node = node.child(L"Freq"))
		m_freq_adc = freq_node.text().as_uint(0);

	if (auto freq_dac_node = node.child(L"FreqDAC"))
		m_freq_dac = freq_dac_node.text().as_uint(0);

	if (auto record_minutes_node = node.child(L"RecordMinutes"))
		m_sd_record_minutes = record_minutes_node.text().as_uint(10);

	if (auto recorder_node = node.child(L"Recorder"))
	{
		if (auto start_attr = recorder_node.attribute(L"start"))
		{
			std::wstring record_type = start_attr.as_string();
			if (0 == record_type.compare(L"button"))
				m_sd_start_record_type = 0;
			else if (0 == record_type.compare(L"auto"))
				m_sd_start_record_type = 1;
		}
	}
	if (!m_sd_start_record_type.has_value())
		m_sd_start_record_type = 0;
		
	if (auto ethernet_node = node.child(L"Ethernet"))
	{
		if (auto method_attr = ethernet_node.attribute(L"method"))
		{
			std::wstring method = method_attr.as_string();
			if (0 == method.compare(L"static"))
				m_eth_static_address = true;
			else if (0 == method.compare(L"dynamic"))
				m_eth_static_address = false;
		}
		
		if (auto addr_attr = ethernet_node.attribute(L"addr"))
			m_eth_address = as_utf8(addr_attr.as_string());

		if (auto ftp_attr = ethernet_node.attribute(L"ftp"))
		{
			std::wstring ftp = ftp_attr.as_string();
			if (0 == ftp.compare(L"no"))
				m_ftp_enable = false;
			else if (0 == ftp.compare(L"yes"))
				m_ftp_enable = true;
		}

		if (auto digital_output_node = node.child(L"DigitalOutput"))
			m_digital_output = digital_output_node.text().as_ullong(0);

		if (auto digital_out_enable_node = node.child(L"DigitalOutEnable"))
			m_digital_out_enable = digital_out_enable_node.text().as_ullong(0);

		if (auto slot_tenso_node = node.child(L"SlotTenso"))
			m_service_mode_enable = true;
	}
	
	m_b_valid = true;
}

};