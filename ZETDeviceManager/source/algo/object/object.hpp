#pragma once
#ifndef object_hpp__
#define object_hpp__

#include "../../types.hpp"
#include "object_type.hpp"
#include "../std/device/device_std.hpp"
#include "../std/sd/sd_std.hpp"

#include "../std/channel/channel.hpp"
#include "../7xxx/7xxx/device/device_7xxx.hpp"

#include "ZetDeviceSort.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

#include <unordered_set>
#include <vector>

namespace zetlab {

class object;
typedef boost::shared_ptr< object > object_ptr;

class object :
	public boost::noncopyable,
	public ZetDeviceSorting
{
public:
	typedef boost::unordered_map< tstring, object_ptr > children_type;
	typedef ::std::pair< tstring, object_ptr >          children_pair;

	enum image {
		pci,
		usb,
		ethernet,
		channel_enabled,
		channel_icp,
		channel_disabled,
		_7xxx,
		blinked_on,
		blinked_off,
		demo,
		ethernet_locked,
		ethernet_error,
		channel_charge,
		channel_service,
		generator,
		sdcard,
		connecting1,
		connecting2,
		connecting3,
		connecting4,
		//ethernet_connect,
		//ethernet_accepted,
		ethernet_warning,
		usb_warning,
		count
	};

public:
	object( const object_type::values& object_type_,
			const tstring& full_path,
			object* const parent = nullptr );

public:
	void addChild( const object_ptr& object_ptr_ );

	void removeThis();

	void browse( ::std::vector< object_ptr >& v_objects,
				 const object_type::values& filter ) const;

	const object_type::values& getType() const;

	const image getImage() const;

	const tstring& getFullPath() const;

	void setFullPath(tstring& newFullPath);

	const tstring& getName() const;

	CString getNameForShow() const; //для вывода в таблицах, описание(если есть) или наименование

	//CString getNameForSort() const; //для сортировки: описание, IP, наименование, серийный номер

	void SetObjectSerialNum(const long& serial_);

	void setName(tstring& newName);

	object* const getParent() const;

	const bool isRoot() const;

	const bool isHaveChild() const;

	void setAlive( bool alive_ = true );

	const bool isAlive() const;

	void setChanged( bool changed_ = true );

	const bool isChanged() const;

	void setModify(bool modify_, bool isParent = false);

	const bool isModify() const;

	bool isNeedChangeNameParent();

	tstring GetNameModify(tstring& name, bool isModify);

	void SetNameChildren(const boost::function< int(object*) >& fFind, const boost::function< void(int) >& fChange);
	
	bool hasChildren() const;
	int getLevel() const; //текщий уровень вложенности
	int colLevel() const; //количество уровеней вложенности
	void increaseLevel();

public:
	std::device* const        getAsDevice() const;

	std::channel* const       getAsChannel() const;

	_7xxx::device* const      getAs7xxx() const;
	std::channel* const       get7xxxAsChannel() const;

	std::sd* const			  getAsSd() const;

	int                       compare(const object &other) const;

private:
	object_type::values    _object_type;
	std::device_ptr        _device_ptr;
	std::channel_ptr       _channel_ptr;
	std::sd_ptr			   _sd_ptr;
	_7xxx::device_ptr      _7xxx_ptr;

	tstring                _full_path;
	tstring                _short_name;
	object*                _parent;
	children_type          _children;
	bool                   _alive;
	bool                   _changed;
	bool				   _modify;
	int					   _colLevels; //количество уровеней вложенности
};

} // namespace zetlab

#endif // object_hpp__
