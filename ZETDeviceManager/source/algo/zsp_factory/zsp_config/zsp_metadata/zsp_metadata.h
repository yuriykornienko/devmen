#pragma once

#include "../../zsp_common.hpp"
#include <pugixml/auto_link.h>

#include <optional>

namespace zetlab {

class zsp_metadata
{
public:
	zsp_metadata(const zsp_device_id& id, pugi::xml_node& node);
	virtual ~zsp_metadata() = default;

	zsp_metadata(const zsp_metadata& value);
	zsp_metadata(zsp_metadata&& value) noexcept;

	zsp_metadata& operator=(const zsp_metadata& value);
	zsp_metadata& operator=(zsp_metadata&& value) noexcept;

	const int32_t compare(const zsp_metadata& value) const;

	const bool is_valid() const { return m_b_valid; }

	const ::std::wstring get_network_code() { return m_network_code.value_or(L"XX"); }
	void set_network_code(const ::std::wstring value) { m_network_code = value; }

	const ::std::wstring get_station_code() { return m_station_code.value_or(L"00000"); }
	void set_station_code(const ::std::wstring value) { m_station_code = value; }

	const double get_latitude() { return m_latitude.value_or(0.); }
	void set_latitude(const double value) { m_latitude = value; }

	const double get_longitude() { return m_longitude.value_or(0.); }
	void set_longitude(const double value) { m_longitude = value; }

	const double get_elevation() { return m_elevation.value_or(0.); }
	void set_elevation(const double value) { m_elevation = value; }

	const ::std::wstring get_site_name() { return m_site_name.value_or(L""); }
	void set_site_name(const ::std::wstring value) { m_site_name = value; }

	const ::std::wstring get_location(const uint32_t id);
	void set_location(const uint32_t id, const ::std::wstring value);

	const ::std::wstring get_channel(const uint32_t id);
	void set_channel(const uint32_t id, const ::std::wstring value);

protected:
	void load(pugi::xml_node& node);

	zsp_device_id m_id = {};

private:
	bool m_b_valid = false;

	::std::optional<::std::wstring> m_network_code;
	::std::optional<::std::wstring> m_station_code;
	::std::optional<double> m_latitude;
	::std::optional<double> m_longitude;
	::std::optional<double> m_elevation;
	::std::optional<::std::wstring> m_site_name;
	::std::map<uint32_t, ::std::pair<::std::wstring, ::std::wstring>> m_channels;
};

inline bool operator==(const zsp_metadata& lhs, const zsp_metadata& rhs)
{
	return 0 == lhs.compare(rhs);
}

inline bool operator!=(const zsp_metadata& lhs, const zsp_metadata& rhs)
{
	return 0 != lhs.compare(rhs);
}

};
