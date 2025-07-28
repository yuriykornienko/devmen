#include "stdafx.h"

#include "config_7xxx.hpp"
#include "../../../../utils/bin/bin.hpp"
#include "../../../../utils/hex/hex.hpp"
#include "../../../../utils/time_conv/time_conv.hpp"
#include "../../../../utils/mac_conv/mac_conv.hpp"
#include "../../../../utils/base64/base64.hpp"
#include "../../../../utils/md5/md5.hpp"

#include <ZET7xxx/include/base/ZET7xxxDefines.h>

#include <boost/make_shared.hpp>

static const wchar_t TIME_FORMAT[] = _T("%d.%m.%Y %H:%M:%S");

template< typename Target, typename Source >
Target my_lexical_cast(const Source &arg)
{
	TCHAR str[32] = { 0 };
	swprintf_s(str, _T("%g"), static_cast<double>(arg));
	return (Target)str;
}

namespace zetlab { namespace _7xxx {


#ifdef USING_CFG_FILE_FOR_CREATE_TAB
	device::config::config(_structCfg7xxx* shared_struct_,
		const utils::user_type& user_) :
		user(user_),
		sensorName(L""),
		serial(0)
	{
		Configuration config(shared_struct_->_sizeDev / 2);
		memcpy_s(config.data(), config.size() * sizeof(Configuration::size_type),
			shared_struct_->_vDevStruct.data(), shared_struct_->_sizeDev);

		CConfigurationWorkHelper* pConfigurationWorkHelper(nullptr);
		try
		{
			pConfigurationWorkHelper = new CConfigurationWorkHelper(config);			
		}
		catch (std::exception&)
		{
			pConfigurationWorkHelper = nullptr;
		}
		if (nullptr != pConfigurationWorkHelper)
		{
			auto itDeviceName = g_devices.find(ZET7XXX_DEVICE_TYPE(pConfigurationWorkHelper->getTypeDevicePar()));
			if (itDeviceName != g_devices.end())
			{
				if (UNKNOWN != itDeviceName->first)
					sensorName = itDeviceName->second;
				else
					sensorName = L"MODBUSZETLAB.exe";
			}
			serial = pConfigurationWorkHelper->getSerialDevicePar();
			delete pConfigurationWorkHelper;
		}

		parse(shared_struct_);

		while (user == utils::user_unknown)
		{
			user = utils::getUserType();
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}
#else
	device::config::config(STRUCT7000EX* shared_struct_,
		const utils::user_type& user_) :
		user(user_),
		sensorName(L"")
	{
		Configuration config(shared_struct_->size_dev_struct / 2);
		memcpy_s(config.data(), config.size() * sizeof(Configuration::size_type),
			shared_struct_->dev_struct, shared_struct_->size_dev_struct);

		CConfigurationWorkHelper* pConfigurationWorkHelper(nullptr);
		try
		{
			pConfigurationWorkHelper = new CConfigurationWorkHelper(config);			
		}
		catch (std::exception&)
		{
			pConfigurationWorkHelper = nullptr;
		}
		if (nullptr != pConfigurationWorkHelper)
		{
			auto itDeviceName = g_devices.find(ZET7XXX_DEVICE_TYPE(pConfigurationWorkHelper->getTypeDevicePar()));
			if (itDeviceName != g_devices.end())
			{
				if (UNKNOWN != itDeviceName->first)
					sensorName = itDeviceName->second;
				else
					sensorName = L"MODBUSZETLAB.exe";
			}
			delete pConfigurationWorkHelper;
		}

		parse(shared_struct_);

		while (user == utils::user_unknown)
		{
			user = utils::getUserType();
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}
#endif //USING_CFG_FILE_FOR_CREATE_TAB

device::config::pages_type::iterator device::config::begin()
{
	return pages.begin();
}

device::config::pages_type::iterator device::config::end()
{
	return pages.end();
}

device::config::pages_type::size_type device::config::size() const
{
	return pages.size();
}

const utils::user_type& device::config::getUserType() const
{
	return user;
}

#ifdef USING_CFG_FILE_FOR_CREATE_TAB
void device::config::parse(_structCfg7xxx* shared_struct)
{
	_structCfg7xxx*      s(nullptr);
	s = shared_struct; // alias
	long               def_pos(0);
	long               dev_pos(0);
	int                empty_counter(0);
	utils::access_type access(utils::access_operator);

	while (def_pos < s->_sizeDefine && dev_pos < s->_sizeDev)
	{
		// Prevent hang up
		boost::this_thread::interruption_point();

		STRUCT_HEAD* head_def(nullptr);
		head_def = reinterpret_cast<STRUCT_HEAD*>(s->_vDefineStruct.data() + def_pos);
		STRUCT_HEAD* head_dev(nullptr);
		head_dev = reinterpret_cast<STRUCT_HEAD*>(s->_vDevStruct.data() + dev_pos);

		// Skip if types of structs differs
		if (head_def->struct_type != head_dev->struct_type)
		{
			dev_pos += head_dev->size ? head_dev->size : sizeof(STRUCT_HEAD);
			continue;
		}

		if (head_def->size <= 0)
		{
			def_pos += sizeof(STRUCT_HEAD);
			dev_pos += sizeof(STRUCT_HEAD);

			// We have reach the last structure
			if (++empty_counter >= 3)
				break;

			switch (empty_counter)
			{
			case 1:
				access = utils::access_metrology;
				break;

			case 2:
				access = utils::access_dispatcher;
				break;
			}

			continue;
		}

		addPage(head_def, head_dev, access, STRUCT_TYPE(head_dev->struct_type), sensorName, serial);

		def_pos += head_def->size;
		dev_pos += head_dev->size;
	}
}
#else
void device::config::parse(STRUCT7000EX* shared_struct)
{
	STRUCT7000EX*      s(nullptr);
	s = shared_struct; // alias
	long               def_pos(0);
	long               dev_pos(0);
	int                empty_counter(0);
	utils::access_type access(utils::access_operator);

	while (def_pos < s->size_define && dev_pos < s->size_dev_struct)
	{
		// Prevent hang up
		boost::this_thread::interruption_point();

		STRUCT_HEAD* head_def(nullptr);
		head_def = reinterpret_cast<STRUCT_HEAD*>(&(s->define_struct[def_pos]));
		STRUCT_HEAD* head_dev(nullptr);
		head_dev = reinterpret_cast<STRUCT_HEAD*>(&(s->dev_struct[dev_pos]));

		// Skip if types of structs differs
		if (head_def->struct_type != head_dev->struct_type)
		{
			dev_pos += head_dev->size ? head_dev->size : sizeof(STRUCT_HEAD);
			continue;
		}

		if (head_def->size <= 0)
		{
			def_pos += sizeof(STRUCT_HEAD);
			dev_pos += sizeof(STRUCT_HEAD);

			// We have reach the last structure
			if (++empty_counter >= 3)
				break;

			switch (empty_counter)
			{
			case 1:
				access = utils::access_metrology;
				break;

			case 2:
				access = utils::access_dispatcher;
				break;
			}

			continue;
		}

		addPage(head_def, head_dev, access, STRUCT_TYPE(head_dev->struct_type), sensorName, serial);
		
		def_pos += head_def->size;
		dev_pos += head_dev->size;
	}
}
#endif //USING_CFG_FILE_FOR_CREATE_TAB

void device::config::addPage( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev,
							  utils::access_type access_, const STRUCT_TYPE& type_,
							  const tstring& sensorType_, const uint64_t& serial_)
{
	pages.push_back( boost::make_shared< page >(head_def, head_dev, access_, type_, sensorType_, serial_) );
}

device::config::page::page( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev,
							utils::access_type access_, const STRUCT_TYPE& type_,
							const tstring& sensorName_, const uint64_t& serial_) :
	name(zetlab::utils::cp1251_to_string(&reinterpret_cast< _DEF_STRUCT* >(head_def)->name[0])),
	desc(zetlab::utils::cp1251_to_string(&reinterpret_cast< _DEF_STRUCT* >(head_def)->desc[0])),
	access(access_),
	type(type_),
	sensorName(sensorName_),
	serial(serial_)
{
	size_t maxlen;

	maxlen = sizeof(reinterpret_cast< _DEF_STRUCT* >(head_def)->name);
	if (name.length() >= maxlen)
		name[maxlen] = _T('\0');
	utils::translate(name, sensorName);

	maxlen = sizeof(reinterpret_cast< _DEF_STRUCT* >(head_def)->desc);
	if (desc.length() >= maxlen)
		desc[maxlen] = _T('\0');
	utils::translate(desc, sensorName);

	parse( head_def, head_dev );
}

const tstring& device::config::page::getName() const
{
	return name;
}

const tstring& device::config::page::getDesc() const
{
	return desc;
}

const STRUCT_TYPE& device::config::page::getType() const
{
	return type;
}

const utils::access_type& device::config::page::getAccessType() const
{
	return access;
}

device::config::page::params_type::iterator device::config::page::begin()
{
	return params.begin();
}

device::config::page::params_type::iterator device::config::page::end()
{
	return params.end();
}

device::config::page::params_type::size_type device::config::page::size() const
{
	return params.size();
}

void device::config::page::parse( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev )
{
	unsigned int def_pos(sizeof(DEF_STRUCT));
	unsigned int dev_pos(sizeof(STRUCT_HEAD));

	BYTE* def = reinterpret_cast< BYTE* >(head_def);
	BYTE* dev = reinterpret_cast< BYTE* >(head_dev);

	while (def_pos < head_def->size && dev_pos < head_dev->size)
	{
		// Prevent hang up
		boost::this_thread::interruption_point();

		STRUCT_ARGV* arg = reinterpret_cast< STRUCT_ARGV* >(&(def[def_pos]));

		if (arg->size <= 0)
			break;

		auto param_ = addParam( arg->name,
								type_of_arg(arg->type),
								&dev[dev_pos],
								&def[def_pos],
								arg->size,
								!!arg->editable,
								sensorName,
								serial,
								type);

		def_pos += param_->size1();
		dev_pos += param_->size2();
	}
}

device::config::page::param* device::config::page::addParam( char* name_,
															 const type_of_arg& type_,
															 void* addr_,
															 void* addr2_,
															 const size_t& size_,
															 const bool& editable_,
															 const tstring& sensorType_,
															 const uint64_t& serial_,
															 const STRUCT_TYPE& structType_)
{
	param_ptr param_ptr_ = boost::make_shared< param >( utils::cp1251_to_string(name_),
														type_,
														addr_,
														addr2_,
														size_,
														editable_,
														sensorType_,
														serial_,
														structType_);
	params.push_back( param_ptr_ );

	return param_ptr_.get();
}

device::config::page::param::param( const tstring& name_,
									const type_of_arg& type_,
									void* addr_,
									void* addr2_,
									const size_t& size_,
									const bool& editable_,
									const tstring& sensorName_,
									const uint64_t& serial_,
									const STRUCT_TYPE& structType_) :
	name(name_), type(type_), addr(addr_), addr2(addr2_), size(size_),
	editable(editable_), modified(false), sensorName(sensorName_), serial(serial_), structType(structType_)
{
	utils::translate(name, sensorName);

	if (editable)
	{
		::std::vector< tstring > extra;
		default_value = getValue(extra);
		switch (type)
		{
		case t_list_char_index:
			for (size_t i = 0; i < extra.size(); i++)
			{
				if (default_value == extra[i])
				{
					default_value = boost::lexical_cast< tstring >( i );
					break;
				}
			}
			break;
		case t_calibration:
			{
				union
				{
					double coded;
					struct
					{
						float valueX;
						float valueY;
					};
				} ch;

				ch.valueX = * (reinterpret_cast< float* >(addr));
				ch.valueY = * (reinterpret_cast< float* >(addr) + 1);
				default_value = boost::lexical_cast< tstring >( ch.coded );
			}
			break;
		}
	}
}

const tstring& device::config::page::param::getName() const
{
	return name;
}

const tstring& device::config::page::param::getSensorName() const
{
	return sensorName;
}

const type_of_arg& device::config::page::param::getType() const
{
	return type;
}

const STRUCT_TYPE& device::config::page::param::getStructType() const
{
	return structType;
}

const uint64_t& device::config::page::param::getSerial() const
{
	return serial;
}

const size_t device::config::page::param::size1() const
{
	size_t extra_size(0);
	long   len(*reinterpret_cast< long* >(reinterpret_cast< BYTE* >(addr2) + sizeof(STRUCT_ARGV)));

	switch (type)
	{
	case t_list_float:
		extra_size = sizeof(long) + sizeof(float) * len;
		break;

	case t_list_long:
		extra_size = sizeof(long) + sizeof(long) * len;
		break;
 
	case t_list_char:
	case t_list_char_index:
		extra_size = sizeof(long) + sizeof(char) * 16 * len;
		break;
	}

	return sizeof(STRUCT_ARGV) + extra_size;
}

const size_t device::config::page::param::size2() const
{
	size_t size_t_(0);

	switch (type)
	{
	case t_char:
	case t_list_char:
	case t_password_hash:
		size_t_ = sizeof(char);
		break;

	case t_short:
	case t_unshort:   
		size_t_ = sizeof(short);
		break;

	case t_long:
	case t_unlong:
	case t_list_long:
	case t_list_char_index:
	case t_longhex:
	case t_ip:
	case t_type_sensor:
	case t_bin_2:
	case t_bin_4:
	case t_bin_8:
	case t_bin_16:
	case t_bin_32:
		size_t_ = sizeof(long);
		break;

	case t_float:
	case t_list_float:
		size_t_ = sizeof(float);
		break;

	case t_time:
		size_t_ = sizeof(__time32_t);
		break;

	case t_longlong:
	case t_longlonghex:
		size_t_ = sizeof(long long);
		break;

	case t_mac:
		size_t_ = 6 * sizeof(unsigned char);
		break;

	case t_calibration:
		size_t_ = 2 * sizeof(float);
		break;
	}

	return size * size_t_;
}

const tstring device::config::page::param::getValue( ::std::vector< tstring >& extra )
{
	extra.clear();

	try
	{
		switch (type)
		{
		case t_char:
		case t_password_hash:
			return utils::cp1251_to_string(reinterpret_cast< char* >(addr));

		case t_short:
			return boost::lexical_cast< tstring >( *reinterpret_cast< short* >(addr) );

		case t_long:
			return boost::lexical_cast< tstring >( *reinterpret_cast< long* >(addr) );

		case t_unshort:
			return boost::lexical_cast< tstring >( *reinterpret_cast< unsigned short* >(addr) );

		case t_unlong:
			return boost::lexical_cast< tstring >( *reinterpret_cast< unsigned long* >(addr) );

		case t_float:
			if (structType == DIAGNOST_STRUCT && !editable)
			{
				switch ( *reinterpret_cast< uint32_t* >(addr) )
				{
				case 0x00000000:
					return tstring(L"—");
				case 0x00800000:
				case 0x00000001:
					return my_lexical_cast< tstring >( 0.f );
				}
			}
			return my_lexical_cast< tstring >( *reinterpret_cast< float* >(addr) );

		case t_list_float:
			return list< float >( extra );

		case t_list_long:
			return list< long >( extra );

		case t_list_char:
			return list_ret_char( extra );

		case t_list_char_index:
			return list_ret_index( extra );

		case t_time:
			return utils::time_to_string( *reinterpret_cast< __time32_t* >(addr), TIME_FORMAT );

		case t_longhex:
			return utils::hex( *reinterpret_cast< long* >(addr) );

		case t_longlong:
			return boost::lexical_cast< tstring >( *reinterpret_cast< long long* >(addr) );

		case t_longlonghex:
			return utils::hex( *reinterpret_cast< long long* >(addr) );

		case t_ip:
			return boost::lexical_cast< tstring >( *reinterpret_cast< unsigned long* >(addr) );

		case t_mac:
			return utils::mac_to_string( reinterpret_cast< unsigned char* >(addr) );

		case t_type_sensor:
			if (long at = *reinterpret_cast<long*>(addr))
			{
				if (0 <= at && at < ZET7XXX_DEVICE_TYPE_COUNT)
					return tstring( device_names[at] );
				else
					return boost::lexical_cast< tstring >( at );
			}
			break;

		case t_calibration:
			extra.push_back( my_lexical_cast< tstring >( * reinterpret_cast< float* >(addr) ) );
			extra.push_back( my_lexical_cast< tstring >( *(reinterpret_cast< float* >(addr) + 1) ) );
			break;

		case t_bin_2:
			return utils::bin2(*reinterpret_cast< unsigned long* >(addr));
		case t_bin_4:
			return utils::bin4(*reinterpret_cast< unsigned long* >(addr));
		case t_bin_8:
			return utils::bin(*reinterpret_cast< unsigned long* >(addr));
		case t_bin_16:
			return utils::bin(*reinterpret_cast< unsigned long* >(addr));
		case t_bin_32:
			return utils::bin(*reinterpret_cast< unsigned long* >(addr));
		}
	}
	catch (boost::bad_lexical_cast&)
	{
	}

	return tstring();
}

void device::config::page::param::setValue( const tstring& value )
{
	if (!editable)
		return;

	modified = (value != default_value);

	try
	{
		switch (type)
		{
		case t_char:
			saveValue< char* >( value );
			break;

		case t_short:
			saveValue< short >( value );
			break;

		case t_long:
			saveValue< long >( value );
			break;

		case t_unshort:
			saveValue< unsigned short >( value );
			break;

		case t_unlong:
			saveValue< unsigned long >( value );
			break;

		case t_float:
			saveValue< float >( value );
			break;

		case t_list_float:
			saveValue< float >( value );
			break;

		case t_list_long:
			saveValue< long >( value );
			break;

		case t_list_char:
			saveValue< char* >( value );
			break;

		case t_list_char_index:
			saveValue< long >( value );
			break;

		case t_time:
			// Запрещаем запись времени в формате t_time
			//saveValue< __time32_t >( boost::lexical_cast< tstring >( utils::string_to_time< __time32_t >( value, TIME_FORMAT ) ) );
			break;

		case t_longhex:
			saveValue< long >( boost::lexical_cast< tstring >( _tcstoul( value.c_str(), NULL, 16 ) ) );
			break;

		case t_longlong:
			saveValue< long long >( value );
			break;

		case t_longlonghex:
			saveValue< long long >( boost::lexical_cast< tstring >( _tcstoui64( value.c_str(), NULL, 16 ) ) );
			break;

		case t_ip:
			saveValue< unsigned long >( value );
			break;

		case t_mac:
			// TODO: пока не актуально
			break;

		case t_calibration:
			saveValue< double >( value );
			break;

		case t_bin_2:
		case t_bin_4:
		case t_bin_8:
		case t_bin_16:
		case t_bin_32:
			saveValue< unsigned long >(boost::lexical_cast< tstring >( _tcstoul( value.c_str(), NULL, 2) ) );
			break;

		case t_password_hash:
		{
			if (value.empty())
			{
				saveValue< char* >(value);
				break;
			}

			auto s = utils::string_to_cp1251(value);
			std::vector<uint8_t> data(sizeof(serial) + s.size());
			memcpy_s(data.data(), data.size(), &serial, sizeof(serial));
			memcpy_s(data.data() + sizeof(serial), data.size() - sizeof(serial), s.data(), s.size());
			saveValue< char* >(utils::cp1251_to_string(base64::to_base64(utils::md5b(data.data(), data.size())).c_str()));
			break;
		}
		}
	}
	catch (boost::bad_lexical_cast&)
	{
	}
}

bool device::config::page::param::restoreDefaultValue()
{
	if (!editable || !modified)
		return false;

	setValue( default_value );
	modified = false;
	return true;
}

const bool& device::config::page::param::isEditable() const
{
	return editable;
}

const bool& device::config::page::param::isModified() const
{
	return modified;
}

void* device::config::page::param::getAddr() const
{
	return addr;	
}

void device::config::page::param::setAddr(BYTE* inAddr)
{
	addr = inAddr;
}

void* device::config::page::param::getAddr2() const
{
	return addr2;
}

void device::config::page::param::setAddr2(BYTE* inAddr2)
{
	addr2 = inAddr2;
}

size_t device::config::page::param::getSize() const
{
	return size;
}

}} // namespace zetlab::_7xxx
