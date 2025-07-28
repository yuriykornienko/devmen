#pragma once
#ifndef crawler_std_hpp__
#define crawler_std_hpp__

#include "../../crawler/crawler_interface.hpp"
#include "../../factory/factory.hpp"
#include "../../zsp_factory/zsp_factory.h"
#include <ZETView_basic_class/CDSRV.h>

#include <boost/noncopyable.hpp>

namespace zetlab { namespace std {

namespace status {

	enum values {

		adc = 0x0,
		dac = 0x1,
		virtual_ = 0x2,
		digital = 0x3,
		lostchannel = 0x4,
		lostdevice = 0x5,
		_7xxx = 0x6,
		lost_7xxx = 0x7,
		fast_7xxx = 0x8,      // high freq ZET7xxx
		no_active_virtual = 0x9,

		adcR = 0x64,
		dacR = 0x65,
		virtualR_ = 0x66,
		digitalR = 0x67,
		lostchannelR = 0x68,
		lostdeviceR = 0x69,
		_7xxxR = 0x6A,
		lost_7xxxR = 0x6B,
		fast_7xxxR = 0x6C, 
		no_active_virtualR = 0x6D,
	};
}

class crawler :
	public crawler_interface,
	public boost::noncopyable
{
public:
	crawler( const factory_ptr& factory_ptr_, const zsp_factory_ptr& zsp_factory_ptr_, CDSRV* server, CWnd* pParent );
	~crawler();

public:
	enum {
		keep_alive_ms = 100,
	};

	virtual const long update();
	const long startServer();

	void keepAlive();

private:
	const tstring make_path( const tstring& group_name,
							 const tstring& elem_name,
							 const long& ph_id_ );

	void check_n_add( const tstring& full_path,
					  const long& module_,
					  const long& ph_id_,
					  const bool& virt_,
					  const long srv_id_ = -1,
					  const bool enabled_ = false, 
					  const bool zsp_ = false);

	void check_n_add_2( const tstring& full_path,
						const long& node_id,
						const long& srv_id_ );

	void check_n_add_SD(const tstring& full_path);

	const bool update_info( object* object_ );

	bool	   isClose;
private:
	factory_ptr factory_;
	zsp_factory_ptr zsp_factory_;
  
	long module;
	long ph_id;
	long srv_id;
	bool virt;
	bool enabled;
	bool zsp;
	CDSRV* server_;
};

}} // namespace zetlab::std

#endif // crawler_std_hpp__
