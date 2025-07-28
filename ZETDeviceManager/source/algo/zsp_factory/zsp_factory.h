#pragma once

#include "zsp_common.hpp"
#include "zsp_config/zsp_config.h"
#include "../utils/pair_hash/pair_hash.hpp"

#include <boost/noncopyable.hpp>

#include <memory>
#include <unordered_map>

namespace zetlab {

class zsp_factory : public boost::noncopyable
{
public:
	zsp_factory(::std::shared_ptr<::std::wstring> path);

	void check(const zsp_device_id& id);
	void update();

	::std::shared_ptr<zsp_metadata> get_metadata(const zsp_device_id& id);
	::std::shared_ptr<zsp_state> get_state(const zsp_device_id& id);
	::std::shared_ptr<zsp_device> get_device(const zsp_device_id& id);
	::std::shared_ptr<zsp_channel> get_channel(const zsp_device_id& id, const uint32_t n);

protected:
	::std::shared_ptr<::std::wstring> m_path;
	::std::unordered_map<zsp_device_id, ::std::shared_ptr<zsp_config>, utils::pair_hash> m_config;

private:

};

using zsp_factory_ptr = ::std::shared_ptr<zsp_factory>;

};
