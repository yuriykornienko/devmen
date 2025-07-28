#pragma once

#include "../zsp_common.hpp"
#include "zsp_state/zsp_state.h"
#include "zsp_metadata/zsp_metadata.h"
#include "zsp_device/zsp_device.h"
#include "zsp_channel/zsp_channel.h"

#include <pugixml/auto_link.h>

#include <filesystem>
#include <map>
#include <mutex>
#include <optional>

namespace zetlab {

class zsp_config
{
public:
	zsp_config(::std::shared_ptr<::std::wstring> path, const zsp_device_id& id);

	void update();
	void save();
	void save_state();
	void save_metadata();

	void set_is_alive();
	const bool is_alive() const;

	::std::shared_ptr<zsp_state> get_state();
	::std::shared_ptr<zsp_metadata> get_metadata();
	::std::shared_ptr<zsp_device> get_device();
	::std::shared_ptr<zsp_channel> get_channel(const uint32_t id);

protected:
	::std::shared_ptr<::std::wstring> m_path;
	zsp_device_id m_id = {};
	
	::std::shared_ptr<zsp_metadata> m_metadata;
	::std::shared_ptr<zsp_state> m_state;
	::std::shared_ptr<zsp_device> m_device;
	::std::map<uint32_t, ::std::shared_ptr<zsp_channel>> m_channels;

private:
	void load();
	void unload();

	void load_state();
	void unload_state();

	void load_metadata();
	void unload_metadata();

	bool parse();
	bool parse_state();
	bool parse_metadata();

	void on_save_channel(const uint32_t id);
	void on_save_device();
	void on_save_state();

	void on_change_common_parameter(const uint32_t id);

	::std::filesystem::path m_file_name = {};
	::std::filesystem::file_time_type m_t_change = {};

	::std::filesystem::path m_file_state_name = {};
	::std::filesystem::file_time_type m_t_state_change = {};

	::std::filesystem::path m_file_metadata_name = {};
	::std::filesystem::file_time_type m_t_metadata_change = {};

	::std::chrono::steady_clock::time_point m_t_alive = {};
	const ::std::chrono::steady_clock::duration m_alive_timeout = ::std::chrono::seconds(10);

	pugi::xml_document m_xml;
	pugi::xml_document m_xml_state;
	pugi::xml_document m_xml_metadata;

	::std::mutex m_xml_mutex;
};

};
