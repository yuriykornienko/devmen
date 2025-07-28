#pragma once
#ifndef crawler_hpp__
#define crawler_hpp__

#include "crawler_interface.hpp"
#include "../factory/factory.hpp"
#include "../zsp_factory/zsp_factory.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

namespace zetlab {

namespace std {
	class crawler;
	typedef boost::shared_ptr< crawler > crawler_ptr;
}

namespace _7xxx {
	class crawler;
	typedef boost::shared_ptr< crawler > crawler_ptr;
}

class crawler :
	public crawler_interface,
	public boost::noncopyable
{
public:
	crawler(
		const factory_ptr& factory_ptr_,
		const zsp_factory_ptr& zsp_factory_ptr_,
		CDSRV* server,
		CXMLWorker* _in_xml_worker_ptr,
		CWnd* pParent);
	~crawler();

public:
	void resume();
	void pause();

	bool isInited() const { return _inited; }

	void request_update();
	void SetWaitThread(bool _wait) { WaitModify =_wait; }
	bool GetWaitThread() { return WaitModify; }
private:
	enum {
		update_period_ms = 1000,
	};

	void update_thread();

	virtual const long update();

private:
	CDSRV*					 _server;
	bool                    _allowed;
	bool                    _inited;
	bool                    _update_requested;
	factory_ptr             _factory;
	zsp_factory_ptr         _zsp_factory;
	std::crawler_ptr        _crawler_std;
	_7xxx::crawler_ptr      _crawler_7xxx;
	boost::thread           _thread;
	CXMLWorker*				_xml_worker_ptr;
	bool WaitModify;
public:
	void onNeedInvalidate( const boost::function< void() >& f )
		{ _signal_inv.connect( f ); }

private:
	boost::signals2::signal< void() > _signal_inv;
};

typedef boost::shared_ptr< crawler > crawler_ptr;

} // namespace zetlab

#endif // crawler_hpp__
