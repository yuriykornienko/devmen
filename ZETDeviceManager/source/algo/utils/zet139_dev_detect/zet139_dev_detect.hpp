#pragma once

#include <Zet7xxxDeviceTypes.h>

#include <guiddef.h>
#include <cstdint>

inline bool is_zet139_type(const uint16_t type)
{
	return type == ZET_139 || type == ZET_139A || type == ZET_139B || type == ZET_139C || type == ZET_139D || 
		type == ZET_139F;
}

inline bool is_zet139_reg(const uint16_t reg)
{
	return reg == 0xC004 + 128 * 40 || reg == 0xC004 + 128 * 41 || reg == 0xC004 + 128 * 42;
}

inline bool is_zet139(const GUID& guid)
{
	return 
		is_zet139_type(*reinterpret_cast<const uint16_t*>(
			(reinterpret_cast<const char*>(&guid) + sizeof(GUID) - 3 * sizeof(uint16_t)))) &&
		is_zet139_reg(*reinterpret_cast<const uint16_t*>(
			(reinterpret_cast<const char*>(&guid) + sizeof(GUID) - sizeof(uint16_t))));
}
