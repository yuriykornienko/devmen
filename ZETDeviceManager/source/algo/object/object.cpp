#include "stdafx.h"

#include "object.hpp"

#include <boost/make_shared.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>
#include "../../algo/utils/hex/hex.hpp"

//порядок сортировки каналов/устройств:
//каналы сортируются по номеру из zserver
//устройства сортируются:
//для устройств в которых есть descriprion(наименование введенное  пользователем - предусмотрен в устройстве и не пустой):
//description + IP + Name + SerialNum
//для устройств без descriprion(наименование введенное  пользователем - не предусмотрен в устройстве или пустой):
//Name + IP + SerialNum

namespace zetlab {

object::object( const object_type::values& object_type_,
				const tstring& full_path,
				object* const parent ) :
	_object_type(object_type_),
	_full_path  (full_path),
	_parent     (parent),
	_alive      (true),
	_changed    (false),
	_modify		(false),
	_colLevels	(1)
{
	size_t spos(_full_path.rfind( _T(";") ));
	size_t epos(_full_path.rfind( _T("|") ));

	if (tstring::npos == spos)
		spos = size_t(0);
	else
		spos += 1;
	if (tstring::npos == epos)
		epos = size_t(_full_path.length());

	zetlab::tstring temp = _full_path.substr(spos, epos - spos);
	setName(temp);
	//_short_name = _full_path.substr( spos, epos - spos );

	switch (object_type_)
	{
		case object_type::device:
			_device_ptr = boost::make_shared< std::device >();
			break;

		case object_type::channel:
			_channel_ptr = boost::make_shared< std::channel >();
			break;

		case object_type::_7xxx_controller:
			_7xxx_ptr = boost::make_shared< _7xxx::device >();
			break;

		case object_type::_7xxx_is:
			_7xxx_ptr = boost::make_shared< _7xxx::device >();
			_channel_ptr = boost::make_shared< std::channel >();
			break;

		case object_type::sd:
			_sd_ptr = boost::make_shared< std::sd >();
			break;
	}
}

void object::addChild( const object_ptr& object_ptr_ )
{
	_children.insert( children_pair( object_ptr_->getFullPath(), object_ptr_ ) );

	setChanged();

	//
	if (_children.size() == 1)
	{
		increaseLevel();
		object* parent = getParent();
		while (parent)
		{
			if (parent->_children.size() == 1) 
			{
				parent->increaseLevel();
				parent = parent->getParent();
			}
			else
				break;
		}
	}
	
}

void object::removeThis()
{
	if (_parent)
	{
		using namespace boost;

		range::for_each( _children | adaptors::map_values, [&]( object_ptr& obj ) {
			obj->_parent = nullptr;
		});

		_parent->setChanged();
		_parent->_children.erase( _full_path );
	}
}

void object::browse( ::std::vector< object_ptr >& v_objects,
					 const object_type::values& filter ) const
{
	v_objects.clear();

	using namespace boost;
	
	range::for_each( _children | adaptors::map_values, [&]( object_ptr obj ) {
		if (filter == obj->getType())
			v_objects.push_back( obj );
	});
}

const object_type::values& object::getType() const
{
	return _object_type;
}

const object::image object::getImage() const
{
	long bridge_mode = 0;
	switch (getType())
	{
	case zetlab::object_type::device:
		switch (getAsDevice()->getConnectionType())
		{
		case std::connection_type::pci:
			return image::pci;
		case std::connection_type::usb:
			return getAsDevice()->GetCalibrationDevice() ? image::usb : image::usb_warning;
		case std::connection_type::ethernet:
			return getAsDevice()->GetCalibrationDevice() ? image::ethernet : image::ethernet_warning;
		}
		return image::demo;

	case zetlab::object_type::sd:
		return image::sdcard;

	case zetlab::object_type::channel:
		if (!getAsChannel()->isEnabled())
			return image::channel_disabled;
		else if (getAsChannel()->getICP())
			return getAsChannel()->GetVersionDeviceNewVersion() ? image::generator : image::channel_icp;
		else if (getAsChannel()->getCharge())
			return image::channel_charge;
		else if (getAsChannel()->isFeedBackChannel() && 
				 getAsChannel()->GetBuiltinGeneratorEnabled() && 
				 getAsChannel()->GetVersionDeviceNewVersion())
			return image::generator;
		
		if (getAsChannel()->isHaveTenso(bridge_mode))
		{
			if (bridge_mode == 1 || bridge_mode == 2)
				return image::generator;
		}
		
		if (auto mode = getAsChannel()->getAdcInputMode(); mode != 0 && mode != 2)
			return image::channel_service;

		return image::channel_enabled;

	case zetlab::object_type::_7xxx_controller:
		if (getAs7xxx()->isEthernet())
		{
			switch (getAs7xxx()->getState())
			{
			case zetlab::_7xxx::device_state::connected:
				return image::ethernet;
			case zetlab::_7xxx::device_state::accepted:
				return image::ethernet;
			case zetlab::_7xxx::device_state::busy:
				return image::ethernet_locked;
			case zetlab::_7xxx::device_state::conflict:
				return image::ethernet_error;
			}
			return image::ethernet;
		}
		return image::usb;

	case zetlab::object_type::_7xxx_is:
		return image::_7xxx;
	}

	return image::demo;
}

const tstring& object::getFullPath() const
{
	return _full_path;
}

void object::setFullPath(tstring& newFullPath)
{
	_full_path = newFullPath;
}

const tstring& object::getName() const
{
	return _short_name;
}

CString object::getNameForShow() const //для вывода в таблицах, описание(если есть) или наименование
{
	tstring name(L"");
	CString nameC(L"");
	tstring ip_str(L"");
	std::device* device = getAsDevice();
	_7xxx::device* device7xxx = getAs7xxx();
	if (device) 
	{
		if (!fLabel.IsEmpty())
			nameC = fLabel;
		else
			nameC = getName().c_str();
		return nameC;

	}else if (device7xxx)
	{
		if (device7xxx->getLabel(name)) 
			nameC = name.c_str();
		else
			nameC = getName().c_str();
		return nameC;
	}

	nameC = getName().c_str();
	return nameC;

}

//CString object::getNameForSort() const //для сортировки: описание, IP, наименование, серийный номер
//{
//	tstring descript(L"");
//	tstring name = getName();
//	tstring ip_str(L"");
//	CString ip_strC(L"");
//	tstring serNum_str(L"");
//	bool isHaveDescript(false);
//	std::device* device = getAsDevice();
//	if (device)
//	{
//		if (device->readDescription(descript))
//			isHaveDescript = true;
//		
//		serNum_str = zetlab::utils::hex(device->getSerial());
//
//		unsigned char ip[4], mask[4], gateway[4], mac[6], duplex, speed;
//		unsigned short port, timeout;
//		if (device->readEthSettings(ip, mask, gateway, port, mac, timeout, duplex, speed))
//			ip_strC.Format(L"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
//	}
//
//	_7xxx::device* device7xxx = getAs7xxx();
//	if (device7xxx)
//	{
//		if (device7xxx->getLabel(descript))
//			isHaveDescript = true;
//
//		serNum_str = zetlab::utils::hex(device7xxx->getDeviceSerial());
//
//		device7xxx->getIpStr(ip_str);
//		ip_strC = ip_str.c_str();
//	}
//
//	CString sortName(L"");
//	if (isHaveDescript) 
//	{
//		sortName.Format(L"%s%s%s%s", descript.c_str(), ip_strC, name.c_str(), serNum_str.c_str());
//	}
//	else 
//	{
//		sortName.Format(L"%s%s%s", name.c_str(), ip_strC, serNum_str.c_str());
//	}
//
//	return sortName;
//}

void object::SetObjectSerialNum(const long& serial_)
{
	tstring serNum_str = zetlab::utils::hex(serial_);
	CString str = serNum_str.c_str();
	ZetDeviceSorting::SetSerialNum(str);
}

void object::setName(tstring& newName)
{
	_short_name = newName;

	SetGroupName(_short_name.c_str());
}

object* const object::getParent() const
{
	return _parent;
}

const bool object::isRoot() const
{
	return _full_path.empty();
}

const bool object::isHaveChild() const
{
	return !_children.empty();
}

void object::setAlive( bool alive )
{
	_alive = alive;

	if (_alive)
	{
		if (_parent)
			_parent->setAlive();
	}
	else
	{
		using namespace boost;

		for (auto it = _children.begin(); it != _children.end(); it++)
		{
			it->second->setAlive(false);
		}
		//range::for_each( _children | adaptors::map_values, [&]( object_ptr& obj ) {
		//	obj->setAlive( false );
		//});
	}
}

const bool object::isAlive() const
{
	return _alive;
}

void object::setChanged( bool changed )
{
	_changed = changed;

	if (_changed)
	{
		if (_parent)
			_parent->setChanged();
	}
	else
	{
		using namespace boost;

		range::for_each( _children | adaptors::map_values, [&]( object_ptr& obj ) {
			obj->setChanged( false );
		});			
	}
}

const bool object::isChanged() const
{
	return _changed;
}

std::device* const object::getAsDevice() const
{
	if (object_type::device == _object_type)
		return _device_ptr.get();

	return nullptr;
}


std::sd* const object::getAsSd() const
{
	if (object_type::sd == _object_type)
		return _sd_ptr.get();

	return nullptr;
}

std::channel* const object::getAsChannel() const
{
	if (object_type::channel == _object_type)
		return _channel_ptr.get();

	return nullptr;
}

_7xxx::device* const object::getAs7xxx() const
{
	if (object_type::_7xxx_controller == _object_type ||
		object_type::_7xxx_is  == _object_type)
		return _7xxx_ptr.get();

	return nullptr;
}

std::channel* const object::get7xxxAsChannel() const
{
	if (object_type::_7xxx_is == _object_type)
		return _channel_ptr.get();

	return nullptr;
}

int object::compare(const object &other) const
{
	if (this->getParent() != other.getParent())
		return this->getParent() ? +1 : -1;

	if (this->getLevel() != other.getLevel())
		return this->getLevel() > other.getLevel() ? +1 : -1;

	if (this->hasChildren())//сортировка устройств
		return Compare(other);

	// сортировка каналов
	auto ch1 = this->getAsChannel();
	if (!ch1)
		ch1 = this->get7xxxAsChannel();

	auto ch2 = other.getAsChannel();
	if (!ch2)
		ch2 = other.get7xxxAsChannel();

	if (ch1 && ch2)
	{
		if (ch1->getDeviceID() != ch2->getDeviceID())
			return ch1->getDeviceID() > ch2->getDeviceID() ? +1 : -1;
	}

	return ch1 ? +1 : -1;
}

void object::setModify(bool modify_, bool isParent/* = false*/)
{
	_modify = modify_;

	if (isParent)
	{
		using namespace boost;

		range::for_each(_children | adaptors::map_values, [&](object_ptr& obj) {
			obj->setModify(_modify);
		});
	}
		
// 	if (_parent)
// 		_parent->setModify(modify_);
	
}

const bool object::isModify() const
{
	return _modify;
}

bool object::isNeedChangeNameParent()
{
	int count(0);
	using namespace boost;

	range::for_each(_children | adaptors::map_values, [&](object_ptr& obj) {
		if (obj->isModify())
			count++;
	});

	return count >= 1 ? false : true;
}

tstring object::GetNameModify(tstring& name, bool isModify)
{
	tstring sResult(name);
	tstring sFind(L"(");

	auto eraseFindFunc = [&](tstring sFind){
		tstring::size_type tempPos(name.rfind(sFind));
		if (tempPos != tstring::npos)
			sResult.erase(tempPos, 1);
	};

	tstring::size_type pos = name.rfind(sFind);

	if (pos != tstring::npos)
	{
		if (isModify)
			sResult.insert(pos - 1, _T("*"));
		else
			eraseFindFunc(_T("*"));
	}
	else
	{
		if (isModify)
			sResult.push_back(_T('*'));
		else
			eraseFindFunc(_T("*"));
	}
	return sResult;
}

void object::SetNameChildren(const boost::function< int(object*) >& fFind, const boost::function< void(int) >& fChange)
{
	using namespace boost;

	range::for_each(_children | adaptors::map_values, [&](object_ptr& obj) {
		int iItem(fFind(obj.get()));
		if (iItem >= 0)
			fChange(iItem);
	});
}

bool object::hasChildren() const
{
	return !_children.empty();
}
	
int object::getLevel() const
{
	int lvl = 0;
	object* parent = getParent();
	while(parent)
	{
		lvl++;
		parent = parent->getParent();
	}

	return lvl;
}

int object::colLevel() const
{
	return _colLevels;
}

void object::increaseLevel()
{
	_colLevels++;
}

} // namespace zetlab
