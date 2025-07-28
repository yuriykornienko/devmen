#include "stdafx.h"

#include "crawler.hpp"

#include <boost/make_shared.hpp>

#include "../std/crawler/crawler_std.hpp"
#include "../7xxx/7xxx/crawler/crawler_7xxx.hpp"

namespace zetlab {

	crawler::crawler(
		const factory_ptr& factory_ptr_, 
		const zsp_factory_ptr& zsp_factory_ptr_, 
		CDSRV* server,
		CXMLWorker* _in_xml_worker_ptr,
		CWnd* pParent) :
	_allowed         (true),
	_inited          (false),
	_update_requested(false),
	_factory         (factory_ptr_),
	_zsp_factory	 (zsp_factory_ptr_),
	_xml_worker_ptr	 (_in_xml_worker_ptr),
	_crawler_7xxx	 (boost::make_shared< _7xxx::crawler >(factory_ptr_, _in_xml_worker_ptr, pParent)),
	_crawler_std     (boost::make_shared< std::crawler >  (factory_ptr_, zsp_factory_ptr_, server, pParent)),
	_thread          (boost::thread( &crawler::update_thread, this )),
	WaitModify(false),
	_server(server)
{
}

crawler::~crawler()
{
	pause();
	_thread.interrupt();
//     thread_.join();
}

void crawler::resume()
{
	_allowed = true;
}

void crawler::pause()
{
	_allowed = false;
}

void crawler::update_thread()
{
	long tick     = std::crawler::keep_alive_ms;
	auto duration = boost::posix_time::milliseconds(tick);
	long count    = tick < update_period_ms ? (update_period_ms / tick) : 1;
	
	try
	{
		while (true)
		{
			if (WaitModify || (_server != nullptr && _server->isReadCfgFile()))
			{
				boost::this_thread::sleep(duration);
				continue;
			}
			
			long ticks = 0;
			for (; ticks < count; ++ticks)
			{
				boost::this_thread::sleep( duration );
				_crawler_std->keepAlive();
				if (_update_requested)
					break;
			}

			_factory->update_wait(ticks * std::crawler::keep_alive_ms);

			if (_allowed)
				update();
			_update_requested = false;

			_inited = true;
		}
	}
	catch (boost::thread_interrupted&)
	{
	}
}

void crawler::request_update()
{
	_update_requested = true;
}

const long crawler::update()
{
	if (_factory->GetWait() == 0)
	{
		_factory->SetWait(1);
		_factory->setChanged(false);
		_factory->killAll();

		_zsp_factory->update();

		// Order of updates changed for true icons
		long count = _crawler_7xxx->update();
		count += _crawler_std->update();

		_factory->SetWait(0);
	}

	_factory->removeDead();

	if (_factory->isChanged())
		_signal_inv();

	return 0;
}

} // namespace zetlab
