#include "stdafx.h"

#include "device_7xxx.hpp"
#include "config/config_7xxx.hpp"

#include <ZETFile\include\ZETFile.h>
#include <boost/algorithm/string.hpp>

#include <Algorithms/CRC.h>
#include "resource\Phrases.h"


namespace zetlab { namespace _7xxx {

device::device() :
	_ftdi_id(-1),
	_node_id(-1),
	_serial(0),
	_type(UNKNOWN),
	_chan_num(0),
	_mode_changed(false),
	_viewChannel(flagViewLable::flagDefault)
{
	using namespace boost::interprocess;

	try
	{
		_7xxxShared = windows_shared_memory(open_only, "MyData7000", read_write);
		_7xxxData   = mapped_region(_7xxxShared, read_write);
	}
	catch (...)
	{
	}
}

device::~device()
{
	_thread.interrupt();
	_thread.join();
}

const device_type device::getType() const
{
	if (-1 != _ftdi_id)
		return controller;

	if (-1 != _node_id)
		return sensor;

	return undefined;
}

tstring device::getTypeStr() const
{
	tstring typeStr = devices::undefined;

	if (-1 != _ftdi_id)
		typeStr = devices::controller;
	else if (-1 != _node_id)
		typeStr = devices::sensor;

	tstring param = L"ZETDeviceManager.exe";
	utils::translate(typeStr, param);

	return typeStr;
}

void device::setFtdiId( const long& ftdi_id )
{
	_ftdi_id = ftdi_id;
}

const long device::getFtdiId() const
{
	return _ftdi_id;
}

void device::setNodeId( const long& node_id )
{
	_node_id = node_id;
}

const long device::getNodeId() const
{
	return _node_id;
}

const uint32_t device::getChanNum() const
{
	return _chan_num;
}

void device::readSettings(const boost::function< void(device::config) >& f, CCFGWorker* pCfgWorker, bool isRefresh)
{
	_connection = _sig_ready.connect(f);
	_cfgWorker = pCfgWorker;
	_thread = boost::thread(&device::read_thread, this, isRefresh);
}

#ifdef USING_CFG_FILE_FOR_CREATE_TAB
void device::read_thread(bool isRefresh)
{
	try
	{
//		pStruct = new _structCfg7xxx();
//		getStruct7xxxForConfig(pStruct);
		_structCfg7xxx* pStruct(nullptr);
		pStruct = _cfgWorker->getStructCfg7xxx(getDeviceSerial(), isRefresh);		

		if (pStruct != nullptr)
		{
			utils::user_type user = utils::user_unknown;

			if (user == utils::user_unknown)
				user = utils::getUserType();

			_sig_ready(config(pStruct, user));
		}
		_connection.disconnect();
	}
	catch (boost::thread_interrupted&)
	{
	}
}
#else
void device::read_thread(bool isRefresh)
{
	try
	{
		auto shared_struct = static_cast<STRUCT7000EX*>(_7xxxData.get_address());
		if (!shared_struct)
			return;
		
		shared_struct->virtchan = _node_id;
		shared_struct->ftdindex = _ftdi_id;
		shared_struct->node = -1;
		shared_struct->IsReady = 0;
		shared_struct->stepcomm = 1;

		utils::user_type user = utils::user_unknown;

		while (!shared_struct->IsReady || !!shared_struct->stepcomm)
		{
			if (user == utils::user_unknown)
				user = utils::getUserType();

			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}

		_sig_ready(config(shared_struct, user));
		_connection.disconnect();
	}
	catch (boost::thread_interrupted&)
	{
	}
}
#endif // USING_CFG_FILE_FOR_CREATE_TAB


void device::stopReadSettings()
{
	_connection.disconnect();

	_thread.interrupt();

	_thread.join();	
}

#ifdef USING_CFG_FILE_FOR_CREATE_TAB
void device::saveSettings(bool isAll/* = false*/)
{
//	sendStruct7xxxForConfig(pStruct);	
	_cfgWorker->writeStructsToFile(getDeviceSerial(), isAll);	
}
#else
void device::saveSettings()
{
	auto shared_struct = static_cast<STRUCT7000EX*>(_7xxxData.get_address());
	if (!shared_struct)
		return;

	shared_struct->virtchan = _node_id;
	shared_struct->ftdindex = _ftdi_id;
	shared_struct->node = -1;
	shared_struct->IsReady = 0;
	shared_struct->stepcomm = -1;
}
#endif //USING_CFG_FILE_FOR_CREATE_TAB


bool device::isEthernet() const
{
	return !!( _info.flags & LINE_INFO_EXT_FLAG_ZET76 );
}

const bool device::isConnectable() const
{
	return ( (_info.flags & LINE_INFO_EXT_FLAG_CONNECTABLE) || (_info.modes & LINE_INFO_EXT_MODE_CONNECT) );
}

const bool device::canEnableOrDisable() const
{
	return false;
}

void device::setEnabled( const bool& enabled )
{
	if (!canEnableOrDisable())
		return;

	if (enabled)
	{
		_info.modes &= ~LINE_INFO_EXT_MODE_ACCEPT;
		_info.modes |= LINE_INFO_EXT_MODE_CONNECT;
	}
	else
		_info.modes &= ~LINE_INFO_EXT_MODE_CONNECT;
	_mode_changed = true;
}

const bool device::isEnabled() const
{
	if (!isEthernet())
		return true;

	if (!isConnectable())
		return false;

	return !!( _info.modes & LINE_INFO_EXT_MODE_CONNECT );
}

const bool device::isConnected() const
{
	if (!isEthernet())
		return true;

	if (!isConnectable())
		return false;

	return ( _info.state == LINE_INFO_EXT_STATE_CONNECTED );
}

bool device::isAcceptable() const
{
	return false;

	// Если надо разрешить управление подключениями в потоковом режиме
	//return ( (_info.flags & LINE_INFO_EXT_FLAG_ACCEPTABLE) || (_info.modes & LINE_INFO_EXT_MODE_ACCEPT) );
}

void device::enableAccept(bool enabled)
{
	if (!isAcceptable())
		return;

	if (enabled)
		_info.modes |= LINE_INFO_EXT_MODE_ACCEPT;
	else
		_info.modes &= ~LINE_INFO_EXT_MODE_ACCEPT;
	_mode_changed = true;
}

bool device::isAcceptEnabled() const
{
	if (!isEthernet())
		return false;

	return !!( _info.modes & LINE_INFO_EXT_MODE_ACCEPT );
}

bool device::isAccepted() const
{
	if (!isEthernet())
		return false;

	return ( _info.state == LINE_INFO_EXT_STATE_ACCEPTED );
}

bool device::getLabel(tstring& label) const
{
	label = _info.label;
	return !label.empty();
}

bool device::getPeerIpv4(tstring& peer_ipv4) const
{
	peer_ipv4 = _info.peer_ipv4;
	return !peer_ipv4.empty();
}

bool device::getIpStr(tstring& ip_str) const
{
	ip_str = _info.ip_str;
	return !ip_str.empty();
}

device_state device::getState() const
{
	return static_cast< device_state >(_info.state);
}

void device::setDeviceSerialNType(const long& serial)
{
	STRUCT7000EX* shared_struct = static_cast<STRUCT7000EX*>(_7xxxData.get_address());

	if (!shared_struct)
		return;

	unsigned short crc = short((serial >> 16) & 0xffff);
	unsigned short alt_crc = ((crc & 0xff) << 8) + ((crc >> 8) & 0xff);
	for (uint32_t i = 0; i < NUMCELLS; ++i)
	{
		unsigned short serialCRC = 0xffff;
		uint64_t deviceSerial = shared_struct->devinfo[i].serial_num;
		serialCRC = CRC16(serialCRC, &deviceSerial, sizeof(uint64_t));
		if (serialCRC == crc || serialCRC == alt_crc)
		{
			_serial = shared_struct->devinfo[i].serial_num;
			_type = ZET7XXX_DEVICE_TYPE(shared_struct->devinfo[i].type);
			_chan_num = (uint32_t)(serial & 0xff);
			break;
		}
	}
}

const uint64_t& device::getDeviceSerial() const
{
	return _serial;
}

const ZET7XXX_DEVICE_TYPE& device::getDeviceType() const
{
	return _type;
}

tstring device::getDeviceName() const
{
	if (0 <= _type && _type < ZET7XXX_DEVICE_TYPE_COUNT)
		return tstring( device_names[_type] );
	return tstring();
}

bool device::update_info()
{
	long long serial_ = _serial;
	ZET7XXX_DEVICE_TYPE type_ = _type;
	info info_;

	// Считываем информацию от MODBUSZETLAB, но при этом управляем modes
	if (_ftdi_id >= 0 && _7xxxData.get_size() >= sizeof(STRUCT7000EX))
	{
		auto shared_struct = static_cast<const STRUCT7000EX*>(_7xxxData.get_address());
		if (shared_struct)
		{
			if (_ftdi_id < NUMLINES)
			{
				auto& linfo = shared_struct->linfo[_ftdi_id];
				auto& linfo_ext = shared_struct->linfo_ext[_ftdi_id];

				type_ = static_cast<ZET7XXX_DEVICE_TYPE>(linfo_ext.device_type);

				// 			if (type_ == ZET_7174)
				// 				serial_ = shared_struct->devinfo->serial_num;
				// 			else
				// 				serial_ = linfo.SerialNumber64;

				if (type_ > ZET7XXX_DEVICE_TYPE_COUNT)
					type_ = _type;

				if (linfo_ext.label[0] != L'\0')
					info_.label.assign(&linfo_ext.label[0], &linfo_ext.label[63]);
				if (linfo_ext.ipv4)
					info_.ip_str.assign(&linfo_ext.ipv4[0], &linfo_ext.ipv4[15]);

				if (linfo_ext.peer_ipv4)
					info_.peer_ipv4.assign(&linfo_ext.peer_ipv4[0], &linfo_ext.peer_ipv4[15]);

				info_.state = linfo_ext.state;
				info_.flags = linfo_ext.flags;

				if (_mode_changed && linfo_ext.modes != _info.modes)
				{
					// Делаем const_cast, так как linfo_ext константная
					auto editable_struct = const_cast<STRUCT7000EX*>(shared_struct);
					editable_struct->linfo_ext[_ftdi_id].modes = _info.modes;
				}

				info_.modes = linfo_ext.modes;
			}
			else
			{
				for (const auto& info : shared_struct->devinfo)
				{
					if (_serial != 0 && _serial == info.serial_num)
					{
						type_ = ZET7XXX_DEVICE_TYPE(info.type);
						break;
					}
				}
			}
		}
	}

	if (_mode_changed ||
		_info.state  != info_.state ||
		_info.flags  != info_.flags ||
		_info.modes  != info_.modes ||
		_info.label  != info_.label ||
		_info.ip_str != info_.ip_str ||
		_info.peer_ipv4 != info_.peer_ipv4 ||
//		_serial      != serial_ ||
		_type        != type_)
	{
		_type         = type_;
//		_serial       = serial_;
		_info         = info_;
		_mode_changed = false;
		return true;
	}

	return false;
}

bool device::isConnecting() const
{
	if (!isEthernet())
		return false;

	if (!(_info.modes & (LINE_INFO_EXT_MODE_CONNECT | LINE_INFO_EXT_MODE_ACCEPT)))
		return false;

	switch (_info.state)
	{
	case LINE_INFO_EXT_STATE_CONNECTED:
	case LINE_INFO_EXT_STATE_ACCEPTED:
		return false;
	}

	return true;
}

void device::noSaveSettings()
{
	_cfgWorker->removeStructFromMap(getDeviceSerial());
}

void device::removeFileConfiguration()
{
	_cfgWorker->removeFileConfiguration(getDeviceSerial());
}

void device::SetCfgWorkerPtr(CCFGWorker* inCfgWorkerPtr)
{
	_cfgWorker = inCfgWorkerPtr;
}

void device::setDeviceSerial(const unsigned long long& serial)
{
	_serial = serial;
}

void device::setDeviceType(const ZET7XXX_DEVICE_TYPE& type)
{
	_type = type;
}

void device::setOffChannel(flagViewLable isFlag)
{
	_viewChannel = isFlag;
}

flagViewLable device::getOffChannel()
{
	return _viewChannel;
}

// Проверка изменений настроек
bool device::CheckSettings()
{
	return _cfgWorker->CheckSettings();
}

}} // namespace zetlab::_7xxx
