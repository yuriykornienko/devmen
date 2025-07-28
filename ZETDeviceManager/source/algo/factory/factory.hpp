#pragma once
#ifndef factory_hpp__
#define factory_hpp__

#include "../../types.hpp"
#include "../object/object.hpp"
#include "../object/object_type.hpp"

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include <unordered_map>

namespace zetlab {

class factory :
	public boost::noncopyable
{
public:
	typedef boost::unordered_map< tstring, object_ptr > objects_type;
	typedef ::std::pair< tstring, object_ptr >          objects_pair;
	typedef boost::function<CString(void)> get_name_function_t;

public:
	factory();

public:
	object* const add( const tstring& full_path,
					   object_type::values type,
					   const boost::function< bool(object*) >& f );

	object* const get( const tstring& full_path ) const;

	object* const getRoot() const;

	void update( const tstring& full_path);

	void start_wait( const tstring& full_path );

	void stop_wait( const tstring& full_path );

	void update_wait( uint32_t elapsed_ms );

	const bool exists( const tstring& full_path ) const;

	void killAll();

	void removeDead();

	void setChanged( const bool changed = true );

	const bool isChanged() const;

	long GetWait() const;
	void SetWait(const long& wait);

private:
	objects_type _objects;
	object_ptr   _root;

	::std::unordered_map<tstring, int> _waiters;
	
public:
	void onAddListener   ( const boost::function< void(object*, bool) >& f )
		{ _signal_add.connect( f ); }

	void onRemoveListener( const boost::function< void(object*) >& f )
		{ _signal_rem.connect( f ); }

	void onUpdateListener( const boost::function< void(object*) >& f )
		{ _signal_upd.connect( f ); }

	void onFirstConnect( const boost::function< void(object*) >& f )
		{ _signal_fsc.connect( f ); }

	void onErrorListener( const boost::function< void(object*, bool) >& f )
		{ _signal_err.connect( f ); }

	void onWaitListener( const boost::function< void(bool) >& f )
		{ _signal_wait.connect( f ); }

	void onGetNameSelectedItemListener(const boost::function< CString(void) >& f)
	{
		funcGetNameSelectedItem =  f;
	}

private:
	boost::signals2::signal< void(object*, bool) > _signal_add;
	boost::signals2::signal< void(object*) > _signal_rem;
	boost::signals2::signal< void(object*) > _signal_upd;
	boost::signals2::signal< void(object*) > _signal_fsc;	

	boost::signals2::signal< void(bool) >          _signal_wait;
	boost::signals2::signal< void(object*, bool) > _signal_err;

	get_name_function_t funcGetNameSelectedItem;
	long needWait;
};

typedef boost::shared_ptr< factory > factory_ptr;

} // namespace zetlab

#endif // factory_hpp__
