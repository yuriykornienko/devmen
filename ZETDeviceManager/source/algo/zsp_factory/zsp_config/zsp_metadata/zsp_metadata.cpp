#include "zsp_metadata.h"

namespace zetlab {

using namespace pugi;

zsp_metadata::zsp_metadata(const zsp_device_id& id, xml_node& node)
	: m_id(id)
{
	load(node);
}

zsp_metadata::zsp_metadata(const zsp_metadata& value)
	: m_b_valid(value.m_b_valid)
	, m_id(value.m_id)
	, m_network_code(value.m_network_code)
	, m_station_code(value.m_station_code)
	, m_latitude(value.m_latitude)
	, m_longitude(value.m_longitude)
	, m_elevation(value.m_elevation)
	, m_site_name(value.m_site_name)
{}

zsp_metadata::zsp_metadata(zsp_metadata&& value) noexcept
	: m_b_valid(std::exchange(value.m_b_valid, false))
	, m_id(std::move(value.m_id))
	, m_network_code(std::move(value.m_network_code))
	, m_station_code(std::move(value.m_station_code))
	, m_latitude(std::move(value.m_latitude))
	, m_longitude(std::move(value.m_longitude))
	, m_elevation(std::move(value.m_elevation))
	, m_site_name(std::move(value.m_site_name))
{}

zsp_metadata& zsp_metadata::operator=(const zsp_metadata& value)
{
	if (this != &value)
	{
		m_b_valid = value.m_b_valid;
		m_id = value.m_id;
		m_network_code = value.m_network_code;
		m_station_code = value.m_station_code;
		m_latitude = value.m_latitude;
		m_longitude = value.m_longitude;
		m_elevation = value.m_elevation;
		m_site_name = value.m_site_name;
	}

	return *this;
}

zsp_metadata& zsp_metadata::operator=(zsp_metadata&& value) noexcept
{
	if (this != &value)
	{
		m_b_valid = std::exchange(value.m_b_valid, false);
		m_id = std::move(value.m_id);
		m_network_code = std::move(value.m_network_code);
		m_station_code = std::move(value.m_station_code);
		m_latitude = std::move(value.m_latitude);
		m_longitude = std::move(value.m_longitude);
		m_elevation = std::move(value.m_elevation);
		m_site_name = std::move(value.m_site_name);
	}

	return *this;
}

const int32_t zsp_metadata::compare(const zsp_metadata& value) const
{
	if (m_b_valid != value.m_b_valid)
		return m_b_valid < value.m_b_valid ? -1 : 1;

	if (m_id != value.m_id)
		return m_id < value.m_id ? -1 : 1;

	if (m_network_code != value.m_network_code)
		return m_network_code < value.m_network_code ? -1 : 1;

	if (m_station_code != value.m_station_code)
		return m_station_code < value.m_station_code ? -1 : 1;

	if (m_latitude != value.m_latitude)
		return m_latitude < value.m_latitude ? -1 : 1;

	if (m_longitude != value.m_longitude)
		return m_longitude < value.m_longitude ? -1 : 1;

	if (m_elevation != value.m_elevation)
		return m_elevation < value.m_elevation ? -1 : 1;

	if (m_site_name != value.m_site_name)
		return m_site_name < value.m_site_name ? -1 : 1;

	return 0;
}

const::std::wstring zsp_metadata::get_location(const uint32_t id)
{
	try
	{
		return m_channels.at(id).first;
	}
	catch (const std::out_of_range&) {}

	return L"00";
}

void zsp_metadata::set_location(const uint32_t id, const::std::wstring value)
{
	try
	{
		m_channels.at(id).first = value;
	}
	catch (const std::out_of_range&) {}	
}

const::std::wstring zsp_metadata::get_channel(const uint32_t id)
{
	try
	{
		return m_channels.at(id).second;
	}
	catch (const std::out_of_range&) {}

	return L"EH1";
}

void zsp_metadata::set_channel(const uint32_t id, const::std::wstring value)
{
	try
	{
		m_channels.at(id).second = value;
	}
	catch (const std::out_of_range&) {}
}

void zsp_metadata::load(pugi::xml_node& node)
{
	m_b_valid = false;
	m_channels.clear();

	auto network_node = node.child(L"Network");
	if (!network_node)
		return;

	m_network_code = network_node.attribute(L"code").value();

	auto station_node = network_node.child(L"Station");
	if (!station_node)
		return;

	m_station_code = station_node.attribute(L"code").value();
	
	if (auto latitude_node = station_node.child(L"Latitude"))
		m_latitude = latitude_node.text().as_double();

	if (auto longitude_node = station_node.child(L"Longitude"))
		m_longitude = longitude_node.text().as_double();

	if (auto elevation_node = station_node.child(L"Elevation"))
		m_elevation = elevation_node.text().as_double();

	if (auto site_node = station_node.child(L"Site"))
	{
		if (auto name_node = site_node.child(L"Name"))
			m_site_name = name_node.text().as_string();
	}

	uint32_t i(0);
	auto channel_node = station_node.child(L"Channel");
	while (channel_node)
	{
		::std::wstring loc = channel_node.attribute(L"locationCode").value();
		::std::wstring cha = channel_node.attribute(L"code").value();
		m_channels.emplace(i, ::std::make_pair(loc, cha));
		++i;

		channel_node = channel_node.next_sibling(L"Channel");
	}

	m_b_valid = true;
}

};
