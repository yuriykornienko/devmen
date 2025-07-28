#include "stdafx.h"

#include "device_std.hpp"

#include "blinker/blinker.hpp"

#include "../../zsp_factory/zsp_common.hpp"

#include <boost/algorithm/string.hpp>

bool isAnyBlinked(false);

namespace zetlab { namespace std {

	using namespace boost::placeholders;

device::device() :
	server_(nullptr),
	module(-1),
	type(-1),
	DSP(-1),
	serial(-1),
	serial64(-1)
{
// 	devices::analyzer	= TranslateString(_T("Анализатор"));
// 	devices::adc_dac	= TranslateString(_T("Модуль АЦП/ЦАП"));
// 	devices::vibrometer = TranslateString(_T("Виброметр/шумомер"));
// 	devices::osc		= TranslateString(_T("Осциллограф"));
// 	devices::seismo		= TranslateString(_T("Сейсмостанция"));
}

device::~device()
{
	stopBlinking();
}

memento< device >::snapshot device::createMemento() const
{
	clipboard* snapshot_(nullptr);
	snapshot_ = new clipboard;

	::std::vector< bool > v;

	snapshot_->adc_freq = getAdcFrequency();
	snapshot_->dac_freq = getDacFrequency();
	getDigitalOutputs  ( v ); for (size_t i = 0, s = v.size(); i < s; ++i) snapshot_->v_output[i] = v[i];
	getDigitalOutEnable( v ); for (size_t i = 0, s = v.size(); i < s; ++i) snapshot_->v_out_enable[i] = v[i];
	snapshot_->selftest = get_selftest();

	return memento< device >::snapshot(snapshot_);
}

void device::restoreMemento( const memento< device >::snapshot& snapshot_ )
{
	setAdcFrequency    ( snapshot_->adc_freq );
	setDacFrequency    ( snapshot_->dac_freq );
	setDigitalOutput   ( ::std::vector< bool >(snapshot_->v_output, snapshot_->v_output + 14) );
	setDigitalOutEnable( ::std::vector< bool >(snapshot_->v_out_enable, snapshot_->v_out_enable + 14) );
	set_selftest	   ( snapshot_->selftest );
}

void device::setServer( CDSRV* const server__ )
{
	server_ = server__;
}

bool device::set_zsp_config(
	::std::shared_ptr<zsp_device> zsp_device__,
	::std::shared_ptr<zsp_state> zsp_state__,
	::std::shared_ptr<zsp_metadata> zsp_metadata__)
{
	bool b_result(false);

	if (zsp_device_ != zsp_device__)
	{
		zsp_device_ = zsp_device__;
		b_result = true;
	}

	if (zsp_device_ && zsp_device__)
	{
		if (*zsp_device_.get() != *zsp_device__.get())
		{
			*zsp_device_.get() = *zsp_device__.get();
			b_result = true;
		}
	}

	if (zsp_state_ != zsp_state__)
	{
		zsp_state_ = zsp_state__;
		b_result = true;
	}

	if (zsp_state_ && zsp_state__)
	{
		if (*zsp_state_.get() != *zsp_state__.get())
		{
			*zsp_state_.get() = *zsp_state__.get();
			b_result = true;
		}
	}

	if (zsp_metadata_ != zsp_metadata__)
		zsp_metadata_ = zsp_metadata__;

	if (zsp_metadata_ && zsp_metadata__)
	{
		if (*zsp_metadata_.get() != *zsp_metadata__.get())
			*zsp_metadata_.get() = *zsp_metadata__.get();
	}

	return b_result;
}

void device::save_zsp_config()
{
	if (zsp_device_)
		zsp_device_->save();
}

void device::save_zsp_state()
{
	if (zsp_state_)
		zsp_state_->save();
}

void device::setModule( const long& module_ )
{
	module = module_;
}

const long device::getModule() const
{
	return module;
}

void device::setType( const long& type_ )
{
	if (type_ >= 0)
		type = type_;
}

const long device::getType() const
{
	return type;
}

CString device::getTypeStr() const
{
	auto name = getNameDevice();

	switch (type)
	{
	case KD1610:
	case KD216:
	case KD500:
	case KD500P:
	case KD1432:
	case KDUACPB:
	case KDU2500:
	case ZADC_ZDS:
	case 30:
	case 37:
		return TranslateString(devices::analyzer.c_str());

	case KDU8500:
		if (::std::any_of(::std::begin( devices::tenzo_keywords ), ::std::end( devices::tenzo_keywords ),
			[&]( const tstring& keyword ) { return name.find( keyword ) != tstring::npos; } ))
			return TranslateString( devices::tenzo.c_str() );
		else if (::std::any_of( ::std::begin( devices::vcs_keywords ), ::std::end( devices::vcs_keywords ),
			[&]( const tstring& keyword ) { return name.find( keyword ) != tstring::npos; } ))
			return TranslateString( devices::vcs.c_str() );
		else
			return TranslateString( devices::analyzer.c_str() );

	case KD816:
	case KD1002:
	case KDU216:
	case KD24:
	case KDU1616:
	case KDUPD14:
	case KDU1624:
	case KDU0424:
	case 211:
	case 221:
		return TranslateString(devices::adc_dac.c_str());

	case KDU0824:
	case 48:
		return TranslateString(devices::seismo.c_str());

	case KDUVN:
		return TranslateString(devices::vibrometer.c_str());

	case KDUOSC:
		return TranslateString(devices::osc.c_str());
	}

	return TranslateString(devices::undefined.c_str());
}

void device::setDSP( const long& DSP_ )
{
	if (DSP_ >= 0)
		DSP = DSP_;
}

const long device::getDSP() const
{
	return DSP;
}

void device::setSerial( const long& serial_ )
{
	if (serial_ >= 0)
		serial = serial_;
}

void device::setSerial64(const uint64_t& serial_)
{
	if (serial_ >= 0)
		serial64 = serial_;
}

const long device::getSerial() const
{
	return serial;
}

const uint64_t device::getSerial64() const
{
	return serial64;
}

const tstring device::getVersion() const
{
	tstring version(_T(""));

	if (zsp_state_)
		version = zsp_state_->get_version();
	else if (0 == ZOpen(type, DSP))
	{
		char verDSP[64] = { 0 }, verDRV[64] = { 0 }, verLIB[64] = { 0 };
		if (0 == ZGetVersion(type, DSP, verDSP, verDRV, verLIB))
			version = CA2W(verDSP);
		ZClose(type, DSP);
	}

	return version;
}

const tstring device::getNameDevice() const
{
	tstring name(_T(""));

	if (zsp_device_)
		name = zsp_device_->get_name();
	else if (0 == ZOpen(type, DSP))
	{
		char cName[16] = { 0 };
		if (0 == ZGetNameDevice(type, DSP, cName, 16))
			name = CA2W(cName);
		ZClose(type, DSP);
	}

	return name;
}

const connection_type device::getConnectionType()
{
	connection_type ctype(undefined);

	if (zsp_state_)
	{
		if (auto iface = zsp_state_->get_interface(); 0 == iface.compare(L"usb"))
			ctype = usb;
		else if (0 == iface.compare(L"ethernet"))
			ctype = ethernet;
	}
	else
	{
		if (0 == ZOpen(type, DSP))
		{
			if (long t(0); 0 == ZGetTypeConnection(type, DSP, &t))
				ctype = static_cast<connection_type>(t);

			ZClose(type, DSP);
		}
		else if (0 == serial)
			ctype = demo;
	}
	
	return ctype;
}

const float device::setAdcFrequency( const float& freq )
{
	if (zsp_device_)
	{
		if (zsp_device_->set_frequency_adc(static_cast<uint32_t>(freq)))
			zsp_device_->save();
	}
	else if (server_ && IsWindow(server_->GetSafeHwnd()))
		return server_->SetFrequency( module, freq );

	return float(0);
}

const float device::getAdcFrequency() const
{
	float freq_adc(0.f);

	if (zsp_device_)
		freq_adc = static_cast<float>(zsp_device_->get_frequency_adc());
	else if (server_ && IsWindow(server_->GetSafeHwnd()))
	{
		if (auto result = server_->GetFrequency(module); result >= 0)
			freq_adc = result;
	}
	
	return freq_adc;
}

const float device::getAdcFrequency( ::std::vector< float >& v_freq ) const
{
	v_freq.clear();

	if (zsp_device_)
	{
		auto frequencies = zsp_device_->get_frequencies_adc();
		::std::transform(frequencies.begin(), frequencies.end(), ::std::back_inserter(v_freq),
			[](const auto& f) { return static_cast<float>(f); });
	}
	else if (server_ && IsWindow(server_->GetSafeHwnd()))
	{
		long n(0);
		float f(0.f);
		while ((f = server_->GetNextFreq(module, n++)) && n <= 100)
			v_freq.emplace_back(f);		
	}

	return getAdcFrequency();
}

const float device::setDacFrequency( const float& freq )
{
	if (zsp_device_)
	{
		if (zsp_device_->set_frequency_dac(static_cast<uint32_t>(freq)))
			zsp_device_->save();
	}
	else if (nullptr != server_)
		return server_->SetFrequency( -module - 1, freq );

	return float(0);
}

const float device::getDacFrequency() const
{
	float freq_dac(0.f);

	if (zsp_device_)
		freq_dac = static_cast<float>(zsp_device_->get_frequency_dac());
	else if (server_ && IsWindow(server_->GetSafeHwnd()))
	{
		if (auto result = server_->GetFrequency(-module - 1); result >= 0)
			freq_dac = result;
	}

	return freq_dac;
}

const float device::getDacFrequency( ::std::vector< float >& v_freq ) const
{
	v_freq.clear();

	if (zsp_device_)
	{
		auto frequencies = zsp_device_->get_frequencies_dac();
		::std::transform(frequencies.begin(), frequencies.end(), ::std::back_inserter(v_freq),
			[](const auto& f) { return static_cast<float>(f); });
	}
	else if (server_ && IsWindow(server_->GetSafeHwnd()))
	{
		long n(0);
		float f(0.f);
		while ((f = server_->GetNextFreq(-module - 1, n++)) && n <= 100)
			v_freq.emplace_back(f);
	}

	return getDacFrequency();
}

const long device::numChannels()
{
	long n_channels(0);
	
	if (!ZOpen( type, DSP ))
	{
		ZGetNumberInputADC( type, DSP, &n_channels );
		ZClose( type, DSP );
	}

	return n_channels;
}

const long device::numDigitalPorts() const
{
	long result(0);

	if (nullptr != server_)
		result = server_->GetDigBits(type, serial );

	if (result >= 0)
		return result;

	return long(0);
}

const long device::getDigitalIOE( const boost::function< long(long) >& f,
								  ::std::vector< bool >& v_status ) const
{
	v_status.clear();

	if (nullptr != server_)
	{
		long nPorts(numDigitalPorts());
		long mask  (f( module ));

		for (long i = 0; i < nPorts; ++i)
			v_status.push_back( bool((mask >> i) & 1) );

		return nPorts;
	}

	return long(0);
}

const long device::getDigitalIOE2(const boost::function< long(unsigned long, unsigned long long) >& f,
	::std::vector< bool >& v_status) const
{
	v_status.clear();

	if (nullptr != server_)
	{
		long nPorts(numDigitalPorts());
		long mask(f(type, serial));

		for (long i = 0; i < nPorts; ++i)
			v_status.push_back(bool((mask >> i) & 1));

		return nPorts;
	}

	return long(0);
}

const start_record_t device::get_sd_start_record() const
{
	start_record_t record_type = start_record_t::undefined;
	if (zsp_device_)
	{
		if (auto t = zsp_device_->get_sd_start_record_type(); 0 == t)
			record_type = start_record_t::button;
		else if (1 == t)
			record_type = start_record_t::power;
	}
	return record_type;
}

void device::set_sd_start_record(const start_record_t t)
{
	if (zsp_device_)
		zsp_device_->set_sd_start_record_type(static_cast<uint16_t>(t) - 1);
}

const uint32_t device::get_sd_record_minutes() const
{
	uint32_t record_minutes = 0;
	if (zsp_device_)
	{
		record_minutes = zsp_device_->get_sd_record_minutes();
		if (0 == record_minutes)
			record_minutes = 10;
	}
	return record_minutes;
}

void device::set_sd_record_minutes(const uint32_t minutes)
{
	if (zsp_device_)
		zsp_device_->set_sd_record_minutes(minutes);
}

const bool device::get_sd_record_continuous() const
{
	if (zsp_device_)
		return zsp_device_->get_sd_record_continuous();
	return false;
}

void device::set_sd_record_continuous(const bool continuous)
{
	if (zsp_device_)
		zsp_device_->set_sd_record_continuous(continuous);
}

const bool device::get_dhcp() const
{
	if (zsp_device_)
		return !zsp_device_->get_ethernet_static_address();
	
	return false;
}

void device::set_dhcp(const bool dhcp)
{
	if (zsp_device_)
		zsp_device_->set_ethernet_static_address(!dhcp);
}

const bool device::has_ftp() const
{
	if (zsp_state_)
		return zsp_device_->has_ftp();

	return false;
}

const bool device::get_ftp() const
{
	if (zsp_device_)
		return zsp_device_->get_ftp_enable();

	return false;
}

void device::set_ftp(const bool ftp)
{
	if (zsp_device_)
		zsp_device_->set_ftp_enable(ftp);
}

const bool device::has_selftest() const
{
	if (zsp_state_)
		return !get_selftest_list().empty();

	return false;
}

const tstring device::get_selftest() const
{
	if (zsp_state_)
		return zsp_state_->get_selftest();

	return tstring();
}

void device::set_selftest(const tstring& selftest)
{
	if (zsp_state_)
	{
		zsp_state_->set_selftest(selftest);
		zsp_state_->save();
	}
}

const ::std::list<::std::pair<::std::wstring, ::std::wstring>> device::get_selftest_list() const
{
	if (zsp_device_)
		return zetlab::get_selftest_list({ zetlab::get_device_type(type), zsp_device_->get_modification() });
	
	return {};
}

const bool device::has_service_mode() const
{
	if (zsp_device_)
		return zsp_device_->has_service_mode();

	return false;
}

const bool device::get_service_mode() const
{
	if (zsp_state_)
		return zsp_state_->get_service();

	return false;
}

void device::set_service_mode(const bool service_mode)
{
	if (zsp_state_)
	{
		zsp_state_->set_service(service_mode);
		zsp_state_->save();
	}
}

const bool device::has_mseed_metadata() const
{
	if (zsp_device_)
		return zsp_device_->has_metadata();

	return false;
}

const tstring device::get_network() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_network_code();

	return {};
}

void device::set_network(const tstring& network)
{
	if (zsp_metadata_)
		zsp_metadata_->set_network_code(network);
}

const tstring device::get_station() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_station_code();

	return {};
}

void device::set_station(const tstring& station)
{
	if (zsp_metadata_)
		zsp_metadata_->set_station_code(station);
}

const double device::get_latitude() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_latitude();

	return {};
}

void device::set_latitude(const double& latitude)
{
	if (zsp_metadata_)
		zsp_metadata_->set_latitude(latitude);
}

const double device::get_longitude() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_longitude();

	return {};
}

void device::set_longitude(const double& longitude)
{
	if (zsp_metadata_)
		zsp_metadata_->set_longitude(longitude);
}

const double device::get_elevation() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_elevation();

	return {};
}

void device::set_elevation(const double& elevation)
{
	if (zsp_metadata_)
		zsp_metadata_->set_elevation(elevation);
}

const tstring device::get_site_name() const
{
	if (zsp_metadata_)
		return zsp_metadata_->get_site_name();

	return {};
}

void device::set_site_name(const tstring& site_name)
{
	if (zsp_metadata_)
		zsp_metadata_->set_site_name(site_name);
}

const long device::getDigitalInputs( ::std::vector< bool >& v_status ) const
{
	return getDigitalIOE2( boost::bind( &CDSRV::GetDigInput, server_, _1, _2 ), v_status );
}

const long device::getDigitalOutputs( ::std::vector< bool >& v_status ) const
{
	return getDigitalIOE2( boost::bind( &CDSRV::GetDigOutput, server_, _1, _2 ), v_status );
}

const long device::getDigitalOutEnable( ::std::vector< bool >& v_status ) const
{
	return getDigitalIOE2( boost::bind( &CDSRV::GetDigOutEnable, server_, _1, _2 ), v_status );
}

void device::setDigitalOutput( const unsigned& port, const bool set )
{
	::std::vector< bool > v_status;

	getDigitalOutputs( v_status );

	if (0 <= port && port < v_status.size())
		v_status[port] = set;

	setDigitalOutput( v_status );
}

void device::setDigitalOutput( const ::std::vector< bool >& v_status )
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return;

	long mask(0);
	for (int i = 0, s = v_status.size(); i < s; ++i)
		mask |= (int(v_status[i]) << i);

	server_->SetDigOutput( type, serial, mask );
	if (zsp_device_)
		zsp_device_->set_digital_output(mask);
}

void device::setDigitalOutEnable( const unsigned& port, const bool set )
{
	::std::vector< bool > v_status;

	getDigitalOutEnable( v_status );

	if (0 <= port && port < v_status.size())
		v_status[port] = set;

	setDigitalOutEnable( v_status );
}

void device::setDigitalOutEnable( const ::std::vector< bool >& v_status )
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return;

	long mask(0);
	for (int i = 0, s = v_status.size(); i < s; ++i)
		mask |= (int(v_status[i]) << i);

	server_->SetDigOutEnable( type, serial, mask );
	if (zsp_device_)
		zsp_device_->set_digital_out_enable(mask);
}

long device::GetWordsADC()
{
	long NumberWords(1);

	if (!ZOpen(type, DSP))
	{
		ZGetWordsADC(type, DSP, &NumberWords);	
		ZClose(type, DSP);
	}

	return NumberWords;
}

#include <WS2tcpip.h>
#pragma comment (lib, "Iphlpapi.lib")
const bool device::readEthSettings( unsigned char* ip,
									unsigned char* mask,
									unsigned char* gateway,
									unsigned short& port,
									unsigned char* mac,
									unsigned short& timeout,
									unsigned char& duplex,
									unsigned char& speed )
{
	bool result(false);

	if (zsp_device_)
	{
		port = 1832;
		auto addr = zsp_device_->get_ethernet_address();
		auto address = addr;
		uint32_t prefix(0);
		if (auto pos = addr.find("/"); pos != ::std::string::npos)
		{
			address = addr.substr(0, pos);
			addr.erase(0, pos + 1);
			if (!addr.empty())
				prefix = atoi(addr.c_str());
		}
		
		inet_pton(AF_INET, address.c_str(), ip);
		ConvertLengthToIpv4Mask(prefix, reinterpret_cast<PULONG>(mask));
	}
	else if (!ZOpen( type, DSP ))
	{
		unsigned short reserved;

		if (!ZReadSettingsEthernet( type,
									DSP,
									ip,
									&port,
									mask,
									gateway,
									mac,
									&duplex,
									&speed,
									&timeout,
									&reserved ))
		{
			result = true;
		}

		ZClose( type, DSP );
	}

	return result;
}

void device::writeEthSettings( unsigned char* ip,
							   unsigned char* mask,
							   unsigned char* gateway,
							   unsigned short timeout,
							   unsigned char duplex,
							   unsigned char speed )
{
	if (zsp_device_)
	{
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, ip, str, INET_ADDRSTRLEN);
		uint8_t prefix(0);
		ConvertIpv4MaskToLength(*reinterpret_cast<PULONG>(mask), &prefix);
		::std::string address = str;
		address.append("/");
		address.append(::std::to_string(prefix));
		zsp_device_->set_ethernet_address(address);
	}
	else if (!ZOpen( type, DSP ))
	{
		unsigned char ip0[4];
		unsigned char mask0[4];
		unsigned char gateway0[4];
		unsigned short port0;
		unsigned char mac0[6];
		unsigned short timeout0;
		unsigned char duplex0;
		unsigned char speed0;
		unsigned short reserved = 0;

		ZReadSettingsEthernet( type,
							   DSP,
							   ip0,
							   &port0,
							   mask0,
							   gateway0,
							   mac0,
							   &duplex0,
							   &speed0,
							   &timeout0,
							   &reserved );

		ZSaveSettingsEthernet( type,
							   DSP,
							   ip,
							   &port0,
							   mask,
							   gateway,
							   mac0,
							   &duplex,
							   &speed,
							   timeout,
							   reserved );

		ZClose( type, DSP );
	}
}

const bool device::isHaveEth()
{
	if (zsp_device_)
		return zsp_device_->has_ethernet();

	unsigned char ip[4];
	unsigned char mask[4];
	unsigned char gateway[4];
	unsigned short port;
	unsigned char mac[6];
	unsigned short timeout;
	unsigned char duplex;
	unsigned char speed;

	return readEthSettings( ip, mask, gateway, port, mac, timeout, duplex, speed );
}

void device::readDigSyncSettings( bool& enableMaster,
								  bool& enableSlave )
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return;

	long  long_;
	float float_;

	server_->Triger( module, -1, &long_, &float_ ),      enableSlave = (1 == long_);
	server_->Triger( module, -2, &long_, &float_ ),      enableSlave = ((1 == long_) && enableSlave);
	server_->Triger( module, -3, &long_, &float_ );
	server_->Triger( module, -4, &long_, &float_ );      enableMaster= (1 == long_);
	server_->Triger( -module - 1, -1, &long_, &float_ ), enableSlave = ((1 == long_) && enableSlave);
	server_->Triger( -module - 1, -2, &long_, &float_ ), enableSlave = ((1 == long_) && enableSlave);
	server_->Triger( -module - 1, -3, &long_, &float_ );

	enableSlave = enableMaster ? false : enableSlave;
}

void device::writeDigSyncSettings( const bool& enableMaster,
								   const bool& enableSlave )
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return;

	long  long_;
	float float_adc;
	float float_dac;

	ADC_INFO adc_info;
	double   float_;
	long long2_;
	unsigned long long3_;
	rawAdcInfo(&adc_info, &float_, &long_, &long2_, &long2_, &float_, &float_, &float_, &long3_);

	switch (type)
	{
	case KDU0424:
		if ((adc_info.ModaADC & 0xFF) < 2) float_adc = float_dac = 12800000;
		else                               float_adc = float_dac = 12288000;
		break;

	case KDU8500:
		float_adc = 500000;
		float_dac = 100000;
		break;
	
	case KDU2500:
		if ((adc_info.ModaADC & 0xFF) == 0) float_adc = 1000000;
		else                                float_adc = 500000;
		float_dac = 100000;
		break;
	}

	long_ = long(enableMaster ? true : enableSlave), server_->Triger( module, 1, &long_, &float_adc );
	long_ = long(enableMaster ? true : enableSlave), server_->Triger( module, 2, &long_, &float_adc );
	long_ = long(enableMaster ? true : enableSlave), server_->Triger( module, 3, &long_, &float_adc );
	long_ = long(enableMaster),                      server_->Triger( module, 4, &long_, &float_adc );
	long_ = long(enableMaster ? true : enableSlave), server_->Triger( -module - 1, 1, &long_, &float_dac );
	long_ = long(enableMaster ? true : enableSlave), server_->Triger( -module - 1, 2, &long_, &float_dac );
	long_ = long(enableMaster ? true : enableSlave), server_->Triger( -module - 1, 3, &long_, &float_dac );
}

const bool device::isHaveDescription() const
{
	BSTR desc;
	bool result = (server_->GetDeviceLabel(module, &desc) == 0);
	if (result)
		SysFreeString(desc);

	return result;
}


bool device::readEthIP(BYTE* ip1, BYTE* ip2, BYTE* ip3, BYTE* ip4) const
{
	if (server_ == nullptr)
		return false;

	if (server_->GetEthIP(module, ip1, ip2, ip3, ip4) == 0)
	{
		return true;
	}
	return false;
}

bool device::readDescription(tstring& description)
{
	if (server_ == nullptr)
		return false;
	BSTR desc;
	if (server_->GetDeviceLabel(module, &desc) == 0)
	{
		description = desc;
		SysFreeString(desc);
	}
	return !description.empty();
}


void device::writeDescription(const tstring& description)
{
	CString sDesc = description.c_str();
	BSTR desc = sDesc.AllocSysString();
	server_->SetDeviceLabel(module, desc);
	SysFreeString(desc);
}

const bool device::isHaveDigSync() const
{
	switch (type)
	{
	case KDU0424:
		return true;
	}

	return false;
}

const bool device::readGpsInfo( __time64_t& time,
								bool& synced,
								unsigned char& pps,
								unsigned char& sats )
{
	bool result(false);

	if (!ZOpen( type, DSP ))
	{
		ADC_INFO     adc_info;
		ADC_INFO_EXT adc_info_ext;

		if (!ZGetInfo   ( type, DSP, &adc_info ) &&
			!ZGetDataExt( type, DSP, &adc_info_ext ))
		{
			time   = adc_info_ext.GPSTime;
			synced = !!adc_info_ext.SyncFlag;

			if (adc_info.EnableExtDevice & 0x200)
			{
				pps    = adc_info_ext.TestCode[5] & 0xFF;
				sats   = adc_info_ext.SatsInUse;

				result = true;
			}
		}

		ZClose( type, DSP );
	}

	return result;
}

const bool device::isHaveGps()
{
	if (zsp_device_ || zsp_state_)
		return false;

	__time64_t    time;
	bool          synced;
	unsigned char pps;
	unsigned char sats;

	return readGpsInfo( time, synced, pps, sats );
}

const bool device::isHaveSd()
{
	bool has_sd(false);
	if (zsp_device_ || zsp_state_)
		has_sd = zsp_device_->has_sd();
	else if (0 == ZOpen(type, DSP))
	{
		if (ADC_INFO adc_info = { 0 }; 0 == ZGetData(type, DSP, &adc_info))
			has_sd = adc_info.EnableExtDevice & 0x8;
		ZClose(type, DSP);
	}
	
	return has_sd;
}

const unsigned long device::GetUptimeDevice()
{
	unsigned long up_time(0);
	if (zsp_device_ || zsp_state_)
		up_time = 0;
	else if (0 == ZOpen(type, DSP))
	{
		if (ADC_INFO_EXT adc_info_ext = { 0 };  0 == ZGetDataExt(type, DSP, &adc_info_ext))
			up_time = adc_info_ext.DeviceUptime;
		ZClose(type, DSP);
	}

	return up_time;
}

const unsigned long device::GetUptimeDriver()
{
	unsigned long up_time(0);
	if (zsp_device_ || zsp_state_)
		up_time = 0;
	else if (0 == ZOpen(type, DSP))
	{
		if (ADC_INFO_EXT adc_info_ext = { 0 };  0 == ZGetDataExt(type, DSP, &adc_info_ext))
			up_time = adc_info_ext.DriverUptime;
		ZClose(type, DSP);
	}

	return up_time;
}

const bool device::readSyncSettings( bool& enableMaster,
									 bool& enableSlave,
									 unsigned char& domain,
									 unsigned char& absolutePriority,
									 unsigned char& relativePriority )
{
	bool result(false);

	if (!ZOpen( type, DSP ))
	{
		unsigned char enaMaster(0);
		unsigned char enaSlave(0);

		if (!ZReadSettingsPTP( type,
							   DSP,
							   &enaMaster,
							   &enaSlave,
							   &domain,
							   &absolutePriority,
							   &relativePriority ))
		{
			enableMaster = !!enaMaster;
			enableSlave  = !!enaSlave;
			result = true;
		}

		ZClose( type, DSP );
	}

	return result;
}

const bool device::writeSyncSettings( const bool enableMaster,
									  const bool enableSlave,
									  const unsigned char domain,
									  const unsigned char absolutePriority,
									  const unsigned char relativePriority )
{
	bool result(false);

	if (!ZOpen( type, DSP ))
	{
		if (!ZSaveSettingsPTP( type,
							   DSP,
							   (enableMaster ? 1 : 0),
							   (enableSlave ? 1 : 0),
							   domain,
							   absolutePriority,
							   relativePriority ))
		{
			result = true;
		}

		ZClose( type, DSP );
	}

	return result;
}

const bool device::readBatteryState(unsigned char& batteryCharge, char& boardTemperature, unsigned short& batteryVoltage)
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		ADC_INFO adcInfo = { 0 };
		ADC_INFO_EXT adcInfoExt = { 0 };

		if (!ZGetInfo(type, DSP, &adcInfo) && !ZGetDataExt(type, DSP, &adcInfoExt))
		{
			// BoardTemperature и BatteryVoltage могут быть нулевыми, так как старый драйвер USB (18.11.2016) эти поля не передает
			if (adcInfoExt.BatteryCharge > 0 && adcInfoExt.BatteryCharge <= 100)
			{
				batteryCharge = adcInfoExt.BatteryCharge;
				boardTemperature = adcInfoExt.BoardTemperature;
				batteryVoltage = adcInfoExt.BatteryVoltage;
				result = true;
			}
		}
		ZClose(type, DSP);
	}
	return result;
}

const bool device::readSdState(unsigned long long& sdSize, unsigned long long& busySize,
	unsigned long long& availableSize, ::std::vector< DIR_FILE >& v_files, bool from_exclusive_mode)
{
	long outForWhile(0);

	if (!from_exclusive_mode)
	{
		auto error = ZExclusive1Open(type, DSP);
		if (0 != error)
		{
			while (ZExclusive1Open(type, DSP) != 0)
			{
				Sleep(1000);
				if (outForWhile == 10)
					break;

				outForWhile++;
			}
		}
		if (outForWhile==10)
			return false;
	}

	COMMAND_INFO_USB commandInfoUsb;

	long size = sizeof(commandInfoUsb);
	commandInfoUsb.Command = 0x105;
	
	auto error = ZSendCommand(type, DSP, &commandInfoUsb, &size);
	if (0 != error)
	{
		ZExclusive1Close(type, DSP);
		return false;
	}

	commandInfoUsb.Size = 0;
	size = sizeof(commandInfoUsb);
	error = ZReceiveCommand(type, DSP, &commandInfoUsb, &size);
	if (0 != error || /*0x105 != commandInfoUsb.Command ||*/ 0 != commandInfoUsb.Error)
	{
		ZExclusive1Close(type, DSP);
		return false;
	}
	
	sdSize = (reinterpret_cast<long*>(commandInfoUsb.Data)[0] - 128) / 2;
	sdSize *= 1024;
	
	auto numUsbPackets = (commandInfoUsb.Size + 504 - 1) / 504;
	auto numFiles = commandInfoUsb.Size / sizeof(zetlab::std::DIR_FILE);
	auto count = 504 / sizeof(zetlab::std::DIR_FILE);

	busySize = 0;
	for (unsigned long i = 0; i < numUsbPackets; ++i)
	{
		size = sizeof(commandInfoUsb);
		error = ZReceiveCommand(type, DSP, &commandInfoUsb, &size);
		if (0 != error)
		{
			ZExclusive1Close(type, DSP);
			return false;
		}
		zetlab::std::DIR_FILE* pFile = reinterpret_cast<zetlab::std::DIR_FILE*>(commandInfoUsb.Data);
		for (unsigned int j = 0; j < count; ++j)
		{
			if (numFiles > v_files.size())
			{
				v_files.push_back(pFile[0]);
				busySize += pFile[0].sizebyte;
			}
			else
				break;
			++pFile;
		}
	}
	if (busySize > sdSize)
		busySize = sdSize;
	availableSize = sdSize - busySize;

	if (!from_exclusive_mode)
		ZExclusive1Close(type, DSP);

	return true;
}

const bool device::readSdAvailableSize(unsigned long long& availableSize)
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		ADC_INFO_EXT adcInfoExt = { 0 };

		if (!ZGetInfoExt(type, DSP, &adcInfoExt))
		{
			if (adcInfoExt.SDFreeSectors == 0)
			{
				ZClose(type, DSP);
				unsigned long long sdSize(0), busySize(0);
				::std::vector<zetlab::std::DIR_FILE> files;
				return readSdState(sdSize, busySize, availableSize, files);
			}
			else if (adcInfoExt.SDFreeSectors < 0)
			{
				availableSize = 0;
				result = false;
			}
			else
			{
				availableSize = adcInfoExt.SDFreeSectors;
				availableSize *= 512;
				result = true;
			}
		}
		ZClose(type, DSP);
	}
	return result;
}

const bool device::readMaxLevelDAC(double &MaxDACLevel)
{
	bool retVal(false);
	long err(0);
	err = ZOpen(type, DSP);
	if (err == 0)
	{ 
		double Resolution(0.0);
		long BitsCount(0);
		if ((ZGetDigitalResolutionDAC(type, DSP, &Resolution) == 0) && (ZGetBitsDAC(type, DSP, &BitsCount) == 0))
		{
			MaxDACLevel = pow(2, BitsCount - 1) * Resolution;
			retVal = true;
		}
		ZClose(type, DSP);
	}

	return retVal;
}

const bool device::formatSd()
{
	long outForWhile(0);
	auto error = ZExclusive1Open(type, DSP);
	if (0 != error)
	{
		while (ZExclusive1Open(type, DSP) != 0)
		{
			Sleep(1000);
			if (outForWhile == 10)
				break;

			outForWhile++;
		}
	}

	if (outForWhile == 10)
		return false;
	
	COMMAND_INFO_USB commandInfoUsb;

	long size = sizeof(commandInfoUsb);
	commandInfoUsb.Command = 0x122;
	error = ZSendCommand(type, DSP, &commandInfoUsb, &size);
	if (0 != error)
	{
		ZExclusive1Close(type, DSP);
		return false;
	}

	error = !error;
	uint32_t counter(0);
	while (0 != error)
	{
		Sleep(1000);
		commandInfoUsb.Command = 0;
		size = sizeof(commandInfoUsb);
		error = ZSendCommand(type, DSP, &commandInfoUsb, &size);
		size = sizeof(commandInfoUsb);
		error = ZReceiveCommand(type, DSP, &commandInfoUsb, &size);
		
		if (++counter >= 60)
			break;
	}
	
	/*commandInfoUsb.Command = 0x120;
	size = sizeof(commandInfoUsb);
	error = ZSendCommand(type, DSP, &commandInfoUsb, &size);
	if (0 != error)
	{
		ZExclusive1Close(type, DSP);
		return false;
	}
	
	size = sizeof(commandInfoUsb);
	error = ZReceiveCommand(type, DSP, &commandInfoUsb, &size);
	if (0 != error)
	{
		ZExclusive1Close(type, DSP);
		return false;
	}*/

	ZExclusive1Close(type, DSP);

	server_->WriteCfg();

	return true;
}

const bool device::deleteFilesFromSd(::std::vector< tstring >& v_files)
{
	long outForWhile(0);
	auto error = ZExclusive1Open(type, DSP);
	if (0 != error)
	{
		while (ZExclusive1Open(type, DSP) != 0)
		{
			Sleep(1000);
			if (outForWhile == 10)
				break;

			outForWhile++;
		}
	}
	if (outForWhile == 10)
		return false;

	for (auto file : v_files)
	{
		boost::algorithm::to_lower(file);
		if (0 != file.compare(_T("info.adc")) && 
			0 != file.compare(_T("config.par")) && 
			0 != file.compare(_T("devices.cfg")))
		{
			char name[504] = "0";
// 			if (nullptr != name)
// 			{
// 				WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, file.c_str(), file.length(), name, 504, "\0", NULL);

			if (WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, file.c_str(), file.length(), name, 504, "\0", NULL) > 0)
			{
				COMMAND_INFO_USB commandInfoUsb;

				long size = sizeof(commandInfoUsb);
				commandInfoUsb.Command = 0x114;
				strcpy_s(reinterpret_cast<char*>(commandInfoUsb.Data), 504, name);
				error = ZSendCommand(type, DSP, &commandInfoUsb, &size);
				if (0 != error)
				{
					ZExclusive1Close(type, DSP);
					return false;
				}

				size = sizeof(commandInfoUsb);
				error = ZReceiveCommand(type, DSP, &commandInfoUsb, &size);
				if (0 != error)
				{
					ZExclusive1Close(type, DSP);
					return false;
				}

			}
		
		}
	}

	ZExclusive1Close(type, DSP);

	return true;
}

const bool device::isHaveSync()
{
	bool          master(false);
	bool          slave(false);
	unsigned char domain(0);
	unsigned char priority1(0);
	unsigned char priority2(0);

	return readSyncSettings( master, slave, domain, priority1, priority2 );
}

void device::rawAdcInfo( ADC_INFO* adc_info,
						 double* ref_freq,
						 long* master_sync,
						 long* enableBiGen, 
						 long* activeSineBiGen,
						 double* freqSineBiGen,
						 double* amplSineBiGen,
						 double* offsetSineBiGen,
						 unsigned long* chargeChannel)
{
	if (!ZOpen( type, DSP ))
	{
		ZGetData        ( type, DSP, adc_info );
		ZGetExtFreqDAC  ( type, DSP, ref_freq );
		ZGetMasterSynchr( type, DSP, master_sync );
		ZGetEnableBuiltinGenerator(type, DSP, enableBiGen);
		ZGetSineBuiltinGenerator(type, DSP, activeSineBiGen, freqSineBiGen, amplSineBiGen, offsetSineBiGen);
		*chargeChannel = 0;
		for (int i = 0; i < (int)(adc_info->QuantityChannelADC - adc_info->QuantityChannelVirtual); ++i)
		{
			long enable(0);
			if (0 == ZGetChargeADC(type, DSP, i, &enable))
			{
				if (enable != 0)
					*chargeChannel += (1 << i);
			}
		}

		ZClose( type, DSP );
	}
}


void device::GetRawDataFromDevice(ADC_INFO* adc_info) {
	if (!ZOpen(type, DSP))
	{
		ZGetData(type, DSP, adc_info);
		ZClose(type, DSP);
	}
}

void device::startBlinking()
{
	if (nullptr != server_)
	{
		server_->Stop();

		blinker_ptr_.reset(new blinker());
		blinker_ptr_->start( type, DSP );

		isAnyBlinked = true;
	}
}

void device::stopBlinking()
{
	if (nullptr != server_)
	{
		if (blinker_ptr_)
		{
			blinker_ptr_.reset();
			try { server_->Start(); } catch (...) {}

			isAnyBlinked = false;
		}
	}
}

const bool device::isBlinking() const
{
	return !!blinker_ptr_;
}

const bool device::isHaveICP() const
{
	bool result(false);

	if (!ZOpen( type, DSP ))
	{
		long icp(0);
		if (!ZFindHCPADC( type, DSP, &icp ))
			result = (1 == icp);

		ZClose( type, DSP );
	}

	return result;
}

const bool device::isHaveTensoICP() const
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		long icp(0);
		if (!ZFindHCPADC(type, DSP, &icp))
			result = (1 == icp);
		if (result)
		{
			long numberBits(0);
			if (!ZGetBitsADC(type, DSP, &numberBits))
				result = (16 < numberBits);
		}

		ZClose(type, DSP);
	}

	return result;
}


const bool device::isHaveTenso(long& new_bridge_mode, long& new_pot1, long& new_pot2, long device_id)
{
	bool result = false;

	if (!ZOpen( type, DSP)) 
	{
		//type = 14
		//DSP = 67
		long bridge_mode;
		long pot1, pot2;
		long ret = ZGetSettingsTenso(type, DSP, device_id, &bridge_mode, &pot1, &pot2);
		new_bridge_mode = bridge_mode;
		
		new_pot1 = pot1;
		new_pot2 = pot2;
		
		result = (ret == 0);

		ZClose(type, DSP);
	}
	return result;
}

void device::setTenso(long new_bridge_mode, long device_id, long Pot1, long Pot2) {
	if (!ZOpen(type, DSP))
	{
		long bridge_mode;
		long pot1, pot2;
		if (!ZGetSettingsTenso(type, DSP, device_id, &bridge_mode, &pot1, &pot2)) {
			if (Pot1 >= 0 && Pot2 >= 0) {
				pot1 = Pot1;
				pot2 = Pot2;
			}
			if (new_bridge_mode == 0 || new_bridge_mode == 1 || new_bridge_mode == 2 || new_bridge_mode == 3)
				ZSetSettingsTenso(type, DSP, device_id, new_bridge_mode, pot1, pot2);
			else
				ZSetSettingsTenso(type, DSP, device_id, bridge_mode, pot1, pot2);
		}
		ZClose(type, DSP);
	}
}
void device::setTenso(long new_bridge_mode, long device_id, long Pot1, long Pot2, bool group)
{
	if (!ZOpen(type, DSP))
	{

		for (long i = 0; i <(numChannels() -1); ++i)
		{
			long bridge_mode;
			long pot1, pot2;
			if (!ZGetSettingsTenso(type, DSP, i, &bridge_mode, &pot1, &pot2)) {
				if (Pot1 >= 0 && Pot2 >= 0) {
					pot1 = Pot1;
					pot2 = Pot2;
				}
				if (new_bridge_mode == 0 || new_bridge_mode == 1 || new_bridge_mode == 2 || new_bridge_mode == 3)
					ZSetSettingsTenso(type, DSP, i, new_bridge_mode, pot1, pot2);
				else
					ZSetSettingsTenso(type, DSP, i, bridge_mode, pot1, pot2);
			}
		}
		ZClose(type, DSP);
	}
}

void device::saveTenso() {
	if (!ZOpen(type, DSP))
	{
		ZSaveSettingsTenso(type, DSP);
		ZClose(type, DSP);
	}
}

const bool device::isHaveCharge() const
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		long charge(0);
		if (!ZFindChargeADC(type, DSP, &charge))
			result = (1 == charge);

		ZClose(type, DSP);
	}

	return result;
}

const bool device::isHaveTEDS() const
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		long present(0);
		if (!ZFindTEDS( type, DSP, &present ))
			result = (1 == present);

		ZClose(type, DSP);
	}

	return result;
}

const bool device::isSupportDiff() const
{
	switch (type)
	{
	case KDU1616:
	case KDU1624:
		return true;
	}

	return false;
}

const bool device::isSupportAmplify() const
{
	switch (type)
	{
	case KDUACPB:
	case KDUVN:
	case KDU8500:
	case KDU2500:
	case ZADC_ZDS:
	case 30:
	case 37:
	case 48:
		return true;
	}

	return false;
}

const bool device::isSupportSensorStruct() const
{
	switch (type)
	{
	case KDUVN:
		return true;
	}

	return false;
}

const bool device::isHaveAdjustmentLevel() const
{
	ADC_INFO_EXT MyInfo = { 0 };
	server_->GetModulelInfoExt(module, &MyInfo);
	return MyInfo.SyncAdjustment != 0;
}

const bool device::isHaveActuator() const
{
	bool result(false);

	if (!ZOpen(type, DSP))
	{
		long mask(0);
		if (!ZGetActuator(type, DSP, &mask))
			result = true;

		ZClose(type, DSP);
	}

	return result;
}

const bool device::isTensostation() const
{
	if (KDU8500 == type)
	{
		auto name = getNameDevice();
		if (::std::any_of(::std::begin(devices::tenzo_keywords), ::std::end(devices::tenzo_keywords),
						  [&](const tstring& keyword)
		{
			return name.find(keyword) != tstring::npos;
		}))
			return true;
		else
			return false;
	}
	else
		return false;
}

const bool device::isVCS() const
{
	if (KDU8500 == type)
	{
		auto name = getNameDevice();
		if (::std::any_of(::std::begin(devices::vcs_keywords), ::std::end(devices::vcs_keywords),
			[&](const tstring& keyword)
			{
				return name.find(keyword) != tstring::npos;
			}))
			return true;
		else
			return false;
	}
	else
		return false;
}

UINT device::GetAdjustmentLevel()
{
	ADC_INFO_EXT MyInfo = { 0 };
	server_->GetModulelInfoExt(module, &MyInfo);
	return MyInfo.SyncAdjustment;
}

const double device::getMaxLevelDAC()
{
	double Res(0.);
	long err(0);
	err = ZOpen(type, DSP);
	if (err == 0)
	{
		double Resolution(0.0);
		long BitsCount(0);
		if ((ZGetDigitalResolutionDAC(type, DSP, &Resolution) == 0) &&
			(ZGetBitsDAC(type, DSP, &BitsCount) == 0))
			Res = pow(2, BitsCount - 1) * Resolution;

		ZClose(type, DSP);
	}

	return Res;
}

void device::setDefaultCfg()
{
	if (zsp_state_)
		zsp_state_->set_default();
	else
		server_->DefaultCfg(module);
}

const bool device::isHaveBuiltinGenDC() const
{
	bool bRes(false);
	if (isHaveBuiltinGenerator())
	{
		// проверяем есть ли встроенный генератор
		ADC_INFO_EXT MyInfo = { 0 };
		server_->GetModulelInfoExt(module, &MyInfo);
		// количество виртуальных каналов должно быть равно 0
		bRes = MyInfo.QuantityChannelVirtual == 0;		
	}

	return bRes;
}

const bool device::isHaveBuiltinGenerator() const
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return false;
	return server_->IsHaveSineBuiltinGen(module) == 1;
}

const bool device::GetBuiltinGeneratorEnabled()
{
	long Enable(0);
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return false;
	server_->GetEnableBuiltinGenerator(module, &Enable);
	return Enable != 0;
}

const bool device::GetExternalGeneratorEnabled()
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return false;

	for (long i = 0; i < server_->QuanChan(); ++i)
	{
		if (server_->NumModule(i) == module)
		{// генератор сторонней программы
			if (server_->IsFeedBackChan(i) && !GetBuiltinGeneratorEnabled())
				return true;
		}
	}
	return false;
}

const long device::GetIndexGenerator()
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return -1;

	for (long i = 0; i < server_->QuanChan(); ++i)
	{
		if (server_->NumModule(i) == module)
		{
			if (server_->IsFeedBackChan(i))
				return i;
		}
	}
	return -1;
}

const bool device::SetBuiltinGeneratorEnabled(long Enable)
{
	return server_->SetEnableBuiltinGenerator(module, Enable) == 0;
}

const bool device::GetBuiltinGeneratorSettings(long &Enable, double &freq, double &level, double &offset)
{
	return server_->GetSineBuiltinGenParam(module, &Enable, &freq, &level, &offset) == 0;
}

const bool device::SetBuiltinGeneratorSettings(long Enable, double freq, double level, double offset)
{
	return server_->SetSineBuiltinGenParam(module, Enable, freq, level, offset) == 0;
}

const bool device::Get1PPSBuiltinGeneratorSettings(long &Enable)
{
	return server_->Get1PPSBuiltinGen(module, &Enable) == 0;
}

const bool device::Set1PPSBuiltinGeneratorSettings(long Enable)
{
	return server_->Set1PPSBuiltinGen(module, Enable) == 0;
}

const bool device::GetCalibrationDevice()
{
	if (zsp_device_)
		return true;

	return server_->GetCalibrationDevices(module) == 1;
}

}} // namespace zetlab::std
