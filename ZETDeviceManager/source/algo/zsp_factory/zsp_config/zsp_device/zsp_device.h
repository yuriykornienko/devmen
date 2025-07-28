#pragma once

#include "../../zsp_common.hpp"
#include <pugixml/auto_link.h>

#include <functional>
#include <optional>

namespace zetlab {

using save_device_callback = ::std::function<void(void)>;

class zsp_device
{
public:
	zsp_device(const zsp_device_id& id, pugi::xml_node& node);
	virtual ~zsp_device() = default;

	zsp_device(const zsp_device& value);
	zsp_device(zsp_device&& value) noexcept;
	
	zsp_device& operator=(const zsp_device& value);
	zsp_device& operator=(zsp_device&& value) noexcept;
	
	void save();
	const int32_t compare(const zsp_device& value) const;
	void notify_save_device(save_device_callback cb) { m_save_device_cb = cb; }

	const zsp_device_modification get_modification() const { return m_modification; }

	const bool is_valid() const { return m_b_valid; }

	const ::std::wstring get_name() const { return m_name; }
	const uint32_t get_frequency_adc() const { return m_freq_adc.value_or(0); }
	bool set_frequency_adc(const uint32_t value);
	const ::std::vector<uint32_t> get_frequencies_adc() const
		{ return ::zetlab::get_frequencies_adc({ m_id.first, m_modification} ); }
	const uint32_t get_frequency_dac() const { return m_freq_dac.value_or(0); }
	bool set_frequency_dac(const uint32_t value);
	const ::std::vector<uint32_t> get_frequencies_dac() const;
	
	const bool has_sd() const { return m_sd_record_minutes.has_value(); }
	const uint32_t get_sd_record_minutes() const { return m_sd_record_minutes.value_or(0); }
	void set_sd_record_minutes(const uint32_t minutes);
	const bool get_sd_record_continuous() const { return 0 == m_sd_record_minutes.value_or(0); }
	void set_sd_record_continuous(const bool continuous);
	const uint16_t get_sd_start_record_type() const { return m_sd_start_record_type.value_or(0); }
	void set_sd_start_record_type(const uint16_t type);

	const bool has_ethernet() const { return m_eth_address.has_value(); }
	const ::std::string get_ethernet_address() const { return m_eth_address.value_or(""); }
	void set_ethernet_address(const ::std::string address);
	const bool get_ethernet_static_address() const { return m_eth_static_address.value_or(true); }
	void set_ethernet_static_address(const bool enable);
	const bool has_ftp() const { return m_ftp_enable.has_value(); }
	const bool get_ftp_enable() const { return m_ftp_enable.value_or(false); }
	void set_ftp_enable(const bool enable);

	const uint16_t get_dig_bits_count() const { return ::zetlab::get_bits_digital_port(m_id.first); }
	const uint64_t get_digital_output() const { return m_digital_output.value_or(0); }
	void set_digital_output(const uint64_t digital_output);
	const uint64_t get_digital_out_enable() const { return m_digital_out_enable.value_or(0); }
	void set_digital_out_enable(const uint64_t digital_out_enable);

	const bool has_service_mode() const { return m_service_mode_enable.value_or(false); }
	const bool has_metadata() const { return m_metadata_enable.value_or(false); }

protected:
	void load(pugi::xml_node& node);

	zsp_device_id m_id = {};
	zsp_device_modification m_modification = zsp_device_modification::unknown;
	save_device_callback m_save_device_cb = {};

private:
	bool m_b_valid = false;

	::std::wstring m_name;
	::std::optional<uint32_t> m_freq_adc;
	::std::optional<uint32_t> m_freq_dac;
	::std::optional<uint32_t> m_sd_record_minutes;
	::std::optional<uint16_t> m_sd_start_record_type;
	::std::optional<::std::string> m_eth_address;
	::std::optional<bool> m_eth_static_address;
	::std::optional<bool> m_ftp_enable;
	::std::optional<uint64_t> m_digital_output;
	::std::optional<uint64_t> m_digital_out_enable;
	::std::optional<bool> m_service_mode_enable;
	::std::optional<bool> m_metadata_enable;
};

inline bool operator==(const zsp_device& lhs, const zsp_device& rhs)
{
	return 0 == lhs.compare(rhs);
}

inline bool operator!=(const zsp_device& lhs, const zsp_device& rhs)
{
	return 0 != lhs.compare(rhs);
}

};
