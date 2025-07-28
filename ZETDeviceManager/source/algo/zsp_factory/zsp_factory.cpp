#include "zsp_factory.h"

#include <set>

namespace zetlab {

zsp_factory::zsp_factory(::std::shared_ptr<::std::wstring> path)
	: m_path(path)
{}

void zsp_factory::check(const zsp_device_id& id)
{
	if (auto empl_result = m_config.try_emplace(id); empl_result.second)
		empl_result.first->second = std::make_shared<zsp_config>(m_path, id);
	else
		empl_result.first->second->set_is_alive();
}

void zsp_factory::update()
{
	::std::set<zsp_device_id> dead_devices;
	for (auto& config : m_config)
	{
		config.second->update();
		if (!config.second->is_alive())
			dead_devices.emplace(config.first);
	}

	for (const auto& dead_device : dead_devices)
		m_config.erase(dead_device);
}

::std::shared_ptr<zsp_metadata> zsp_factory::get_metadata(const zsp_device_id& id)
{
	try {
		return m_config.at(id)->get_metadata();
	}
	catch (const ::std::out_of_range&) {}

	return {};
}

::std::shared_ptr<zsp_state> zsp_factory::get_state(const zsp_device_id& id)
{
	try {
		return m_config.at(id)->get_state();
	}
	catch (const ::std::out_of_range&) {}

	return {};
}

::std::shared_ptr<zsp_device> zsp_factory::get_device(const zsp_device_id& id)
{
	try {
		return m_config.at(id)->get_device();
	}
	catch (const ::std::out_of_range&) {}

	return {};
}

::std::shared_ptr<zsp_channel> zsp_factory::get_channel(const zsp_device_id& id, const uint32_t n)
{
	try {
		return m_config.at(id)->get_channel(n);
	}
	catch (const ::std::out_of_range&) {}

	return {};
}

};