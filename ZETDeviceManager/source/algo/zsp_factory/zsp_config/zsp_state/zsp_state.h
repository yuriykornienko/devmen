#pragma once

#include "../../zsp_common.hpp"
#include <pugixml/auto_link.h>

#include <functional>
#include <optional>

namespace zetlab {

using save_state_callback = ::std::function<void(void)>;

class zsp_state
{
public:
	zsp_state(const zsp_device_id& id, pugi::xml_node& node);
	virtual ~zsp_state() = default;

	zsp_state(const zsp_state& value);
	zsp_state(zsp_state&& value) noexcept;

	zsp_state& operator=(const zsp_state& value);
	zsp_state& operator=(zsp_state&& value) noexcept;

	void save();
	const int32_t compare(const zsp_state& value) const;
	void notify_save_state(save_state_callback cb) { m_save_state_cb = cb; }

	const bool is_valid() const { return m_b_valid; }

	const ::std::wstring get_interface() const { return m_interface.value_or(L""); }
	const ::std::wstring get_selftest() const { return m_selftest.value_or(L"off"); }
	void set_selftest(const ::std::wstring& selftest);
	const ::std::wstring get_version() const { return m_version.value_or(L""); }
	const bool get_default() const { return m_default.value_or(false); }
	void set_default();
	const bool get_service() const { return m_service.value_or(false); }
	void set_service(const bool& service);

protected:
	void load(pugi::xml_node & node);

	zsp_device_id m_id = {};
	save_state_callback m_save_state_cb = {};

private:
	bool m_b_valid = false;

	::std::optional<::std::wstring> m_interface;
	::std::optional<::std::wstring> m_selftest;
	::std::optional<::std::wstring> m_version;
	::std::optional<bool> m_default;
	::std::optional<bool> m_service;
};

inline bool operator==(const zsp_state& lhs, const zsp_state& rhs)
{
	return 0 == lhs.compare(rhs);
}

inline bool operator!=(const zsp_state& lhs, const zsp_state& rhs)
{
	return 0 != lhs.compare(rhs);
}

};
