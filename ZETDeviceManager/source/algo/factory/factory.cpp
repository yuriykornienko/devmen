#include "stdafx.h"

#include "factory.hpp"

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <pugixml/auto_link.h>

namespace zetlab {

enum { kWaitTimeMs = 6000 };

factory::factory() :
	_root(boost::make_shared< object >(object_type::undefined, _T(""))),
	needWait(0)
{
}

object* const factory::add( const tstring& full_path, 
							object_type::values type,
							const boost::function< bool(object*) >& f )
{
	if (exists( full_path ))
		return _objects.find( full_path )->second.get();

	size_t     pos        = full_path.rfind( _T(";") );
	object*    parent     = nullptr;
	object_ptr new_object;
	bool isSelect(false);
	
	if (tstring::npos == pos)
		parent = _root.get();
	else
		parent = add( full_path.substr( 0, pos ), type++, f );

	new_object = boost::make_shared< object >(type, full_path, parent);
	parent->addChild( new_object );
	_objects.insert( objects_pair(full_path, new_object) );

	while  (!f( new_object.get() ))
		boost::this_thread::sleep( boost::posix_time::microseconds(20) );

	CString name = funcGetNameSelectedItem();
	if (full_path.c_str() == name)
		isSelect = true;

	_signal_add( new_object.get(), isSelect );
	_signal_upd( new_object.get() );

	return new_object.get();
}

object* const factory::get( const tstring& full_path ) const
{
	auto it = _objects.find( full_path );

	if (_objects.end() != it)
		return it->second.get();

	return nullptr;
}

void factory::update( const tstring& full_path )
{
	auto it = _objects.find( full_path );

	if (_objects.end() != it)
		_signal_upd( it->second.get() );
}

void factory::start_wait( const tstring& full_path )
{
	if (_waiters.empty())
		_signal_wait(true);

	auto it = _waiters.find( full_path );
	if (it == _waiters.end())
		_waiters.insert(::std::make_pair( full_path, 0 ));
}

void factory::stop_wait( const tstring& full_path )
{
	auto it = _waiters.find( full_path );
	if (it != _waiters.end())
	{
		if (it->second < 0)
		{
			auto object_ = get( full_path );
			if (object_)
				_signal_err( object_, false );
		}

		_waiters.erase(it);
		if (_waiters.empty())
			_signal_wait(false);
	}
}

void factory::update_wait( uint32_t elapsed_ms )
{
	for (auto& it : _waiters)
	{
		if (it.second < 0)
			continue;

		it.second += elapsed_ms;
		if (it.second < kWaitTimeMs)
			continue;

		it.second = -1;
		auto object_ = get( it.first );
		if (object_)
			_signal_err( object_, true );
	}
}

object* const factory::getRoot() const
{
	return _root.get();
}

const bool factory::exists( const tstring& full_path ) const
{
	return (_objects.end() != _objects.find( full_path ));
}

void factory::killAll()
{
	_root->setAlive( false );
}

void factory::removeDead()
{
	if (needWait == 0)
	{
		needWait = 1;
		for (auto it = _objects.begin(), end = _objects.end(); it != end; )
		{
			if (!it->second->isAlive())
			{
				auto wait_it = _waiters.find(it->second->getFullPath());
				if (wait_it != _waiters.end())
				{
					if (wait_it->second < 0)
						_signal_err(it->second.get(), false);
					_waiters.erase(wait_it);
					if (_waiters.empty())
						_signal_wait(false);
				}


				_signal_rem(it->second.get());//сигнал представлению удалить элемент
				it->second->removeThis();
				it = _objects.erase(it);
			}
			else
			{
				++it;
			}
		}
		needWait = 0;
	}
}

void factory::setChanged( const bool changed )
{
	_root->setChanged( changed );
}

const bool factory::isChanged() const
{
	return _root->isChanged();
}

// Работает либо удаление объектов, либо их создание
long factory::GetWait() const
{
	return needWait;
}

// Работает либо удаление объектов, либо их создание
void factory::SetWait(const long& wait)
{
	needWait = wait;
}


} // namespace zetlab
