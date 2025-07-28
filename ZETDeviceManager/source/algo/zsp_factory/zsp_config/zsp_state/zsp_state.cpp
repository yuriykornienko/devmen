#include "zsp_state.h"

namespace zetlab {

using namespace pugi;

zsp_state::zsp_state(const zsp_device_id& id, xml_node& node)
	: m_id(id)
{
	load(node);
}

zsp_state::zsp_state(const zsp_state& value)
	: m_b_valid(value.m_b_valid)
	, m_id(value.m_id)
	, m_interface(value.m_interface)
	, m_selftest(value.m_selftest)
	, m_version(value.m_version)
	, m_default(value.m_default)
	, m_service(value.m_service)
{}

zsp_state::zsp_state(zsp_state&& value) noexcept
	: m_b_valid(std::exchange(value.m_b_valid, false))
	, m_id(std::move(value.m_id))
	, m_interface(std::move(value.m_interface))
	, m_selftest(std::move(value.m_selftest))
	, m_version(std::move(value.m_version))
	, m_default(std::move(value.m_default))
	, m_service(std::move(value.m_service))
{}

zsp_state& zsp_state::operator=(const zsp_state& value)
{
	if (this != &value)
	{
		m_b_valid = value.m_b_valid;
		m_id = value.m_id;
		m_interface = value.m_interface;
		m_selftest = value.m_selftest;
		m_version = value.m_version;
		m_default = value.m_default;
		m_service = value.m_service;
	}

	return *this;
}

zsp_state& zsp_state::operator=(zsp_state&& value) noexcept
{
	if (this != &value)
	{
		m_b_valid = std::exchange(value.m_b_valid, false);
		m_id = std::move(value.m_id);
		m_interface = std::move(value.m_interface);
		m_selftest = std::move(value.m_selftest);
		m_version = std::move(value.m_version);
		m_default = std::move(value.m_default);
		m_service = std::move(value.m_service);
	}

	return *this;
}

void zsp_state::save()
{
	if (m_save_state_cb)
		m_save_state_cb();
}

const int32_t zsp_state::compare(const zsp_state& value) const
{
	if (m_b_valid != value.m_b_valid)
		return m_b_valid < value.m_b_valid ? -1 : 1;

	if (m_id != value.m_id)
		return m_id < value.m_id ? -1 : 1;

	if (m_interface != value.m_interface)
		return m_interface < value.m_interface ? -1 : 1;

	if (m_selftest != value.m_selftest)
		return m_selftest < value.m_selftest ? -1 : 1;

	if (m_version != value.m_version)
		return m_version < value.m_version ? -1 : 1;

	if (m_default != value.m_default)
		return m_default < value.m_default ? -1 : 1;

	if (m_service != value.m_service)
		return m_service < value.m_service ? -1 : 1;

	return 0;
}

void zsp_state::set_selftest(const::std::wstring& selftest)
{
	m_selftest = selftest;
}

void zsp_state::set_default()
{
	m_default = true;
}

void zsp_state::set_service(const bool& service)
{
	m_service = service;
}

void zsp_state::load(pugi::xml_node& node)
{
	m_b_valid = false;

	if (auto interface_node = node.child(L"Interface"))
		m_interface = interface_node.text().as_string();

	if (auto selftest_node = node.child(L"Selftest"))
		m_selftest = selftest_node.text().as_string();

	if (auto version_node = node.child(L"Version"))
		m_version = version_node.text().as_string();

	if (auto default_node = node.child(L"Default"))
		m_default = !!default_node.text().as_uint();

	if (auto service_node = node.child(L"Service"))
		m_service = service_node.text().as_bool();

	m_b_valid = true;
}

};
