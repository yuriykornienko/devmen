#pragma once
#ifndef crawler_7xxx_hpp__
#define crawler_7xxx_hpp__

#include "../../../../types.hpp"
#include "../../../crawler/crawler_interface.hpp"
#include "../../../factory/factory.hpp"

#include <boost/noncopyable.hpp>
#define BOOST_USE_WINDOWS_H
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace zetlab { namespace _7xxx {

class crawler :
	public crawler_interface,
	public boost::noncopyable
{
public:
	crawler(const factory_ptr& factory_ptr_, CXMLWorker* in_xml_worker_ptr, CWnd* pParent);

public:
	virtual const long update();

private:
	const long read_controllers_count( void* void_shared_struct );

	void* open_shared( const ::std::string& shared_mem );

	inline const tstring make_path( void* void_shared_struct, const long& index ) const;

	void check_n_add( const tstring& full_path, const long& ftdi_id, const unsigned long long serialNum );

	const bool check_info();

	const bool update_info( object* object_ );

	bool CheckFileAccess(CString fileName);

private:
	factory_ptr _factory;
	bool _shared;
	boost::interprocess::windows_shared_memory _7xxxShared;
	boost::interprocess::mapped_region _7xxxData;

	long _ftdi_id;

	CXMLWorker* _xml_worker_ptr;
	WIN32_FILE_ATTRIBUTE_DATA _attrib;
	FILETIME _lastWriteTime;
	bool _isFistUpdate;

	CWnd* _parent_wnd;
	bool _notified_detect; // флаг, что сообщение было отправлено
};

}} // namespace zetlab::_7xxx

#endif // crawler_7xxx_hpp__
