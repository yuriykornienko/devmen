#pragma once
#ifndef md5_hpp__
#define md5_hpp__

#include <md5/md5.h>

#include <string>

namespace zetlab { namespace utils {

	std::string md5b(const md5_byte_t* bytes, int size_in_bytes)
	{
		std::string s(16, '0');

		md5_state_t state;

		// Initialize md5 hash
		md5_init(&state);

		// Calculate hash
		md5_append(&state, bytes, size_in_bytes);

		// Finish
		md5_finish(&state, (md5_byte_t*)(s.data()));

		return s;
	}
}
} // namespace zetlab::utils

#endif // md5_hpp__