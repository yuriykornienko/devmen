#pragma once
#ifndef device_7xxx_hpp__
#define device_7xxx_hpp__

#include "../../../../types.hpp"

#include <boost/noncopyable.hpp>

#define BOOST_USE_WINDOWS_H
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <Zet7xxxDeviceTypes.h>
#include "CFGWorker.h"

namespace zetlab { namespace _7xxx {

enum class flagViewLable : int
{
	flagDefault = 0,
	flagWait,
	flagWarning
};

enum device_type {

	undefined = 0x0,
	controller,
	sensor,

};

enum device_state {
	other = 0,
	connecting = 1,
	connected = 2,
	accepted = 4,
	busy = 5,
	conflict = 6,
};

namespace devices {

	const tstring undefined  = _T("Не определен");
	const tstring controller = _T("Контроллер");
	const tstring sensor     = _T("Интеллектуальный датчик");

}

class device :
	public boost::noncopyable
{
public:
	class config;

	struct info
	{
		tstring label;
		tstring ip_str;
		uint32_t state;
		uint32_t flags;
		uint32_t modes;
		tstring peer_ipv4;

		info() : state(0), flags(0), modes(0) {}
	};

public:
	device();
	~device();

public:
	const device_type getType() const;

	tstring getTypeStr() const;

	void setFtdiId( const long& ftdi_id );

	const long getFtdiId() const;

	void setNodeId( const long& node_id );

	const long getNodeId() const;

	const uint32_t getChanNum() const;

	void setDeviceSerialNType(const long& serial);

	void setDeviceSerial(const unsigned long long& serial);

	void setDeviceType(const ZET7XXX_DEVICE_TYPE& type);

	const uint64_t& getDeviceSerial() const;

	const ZET7XXX_DEVICE_TYPE& getDeviceType() const;

	tstring getDeviceName() const;

	void readSettings( const boost::function< void(config) >& f, CCFGWorker* pCfgWorker, bool isRefresh );

	void stopReadSettings();

	void saveSettings(bool isAll = false);
	void noSaveSettings();
	void removeFileConfiguration();

	bool isEthernet() const;

	const bool canEnableOrDisable() const;
	void setEnabled( const bool& enabled_ );
	const bool isEnabled() const;
	const bool isConnected() const;

	bool isAcceptable() const;
	void enableAccept(bool enabled);
	bool isAcceptEnabled() const;
	bool isAccepted() const;

	bool isConnecting() const;

	bool getLabel(tstring& label) const;
	bool getPeerIpv4(tstring& peer_ipv4) const;
	bool getIpStr(tstring& ip_str) const;

	device_state getState() const;

	bool update_info();

	void SetCfgWorkerPtr(CCFGWorker* inCfgWorkerPtr);

	void setOffChannel(flagViewLable isFlag);
	flagViewLable getOffChannel();

	bool CheckSettings();
private:
	void read_thread(bool isRefresh);

	const bool isConnectable() const;
//	bool getStruct7xxxForConfig(_structCfg7xxx *pStruct7xxx);
//	bool sendStruct7xxxForConfig(_structCfg7xxx *pStruct7xxx);

private:
	long _ftdi_id;
	long _node_id;
	uint64_t _serial;
	uint32_t _chan_num;
	ZET7XXX_DEVICE_TYPE _type;
	_structCfg7xxx* pStruct;
	boost::thread                              _thread;
	boost::signals2::signal< void(config) >    _sig_ready;
	boost::signals2::connection                _connection;
	boost::interprocess::windows_shared_memory _7xxxShared;
	boost::interprocess::mapped_region         _7xxxData;

	info _info;
	bool _mode_changed;

	CCFGWorker* _cfgWorker;
	flagViewLable _viewChannel;
};

typedef boost::shared_ptr< device > device_ptr;

}} // namespace zetlab::_7xxx

#endif // device_7xxx_hpp__
