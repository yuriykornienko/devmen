#pragma once

#include <guiddef.h>
#include <cstdint>

inline bool is_zsp(const uint16_t type)
{
	return type == 30 || type == 37 || type == 48 || type == 211 || type == 221;
}

inline bool is_zsp(const long type)
{
	return is_zsp(static_cast<uint16_t>(type));
}

inline bool is_zsp(const GUID& guid)
{
	return is_zsp(*reinterpret_cast<const uint16_t*>(
		(reinterpret_cast<const char*>(&guid) + sizeof(GUID) - sizeof(uint16_t))));
}

