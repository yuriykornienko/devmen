#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../../types.hpp"

namespace zetlab {
	namespace std {

class sd : public boost::noncopyable
{
public:
	sd();
	~sd();
	
	void launchSilentConverter(const tstring &path);
};

typedef boost::shared_ptr< sd > sd_ptr;
	}
} // namespsace zetlab::std