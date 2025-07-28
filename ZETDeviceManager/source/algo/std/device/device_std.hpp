#pragma once
#ifndef device_std_hpp__
#define device_std_hpp__

#include "../../../types.hpp"
#include <ZETView_basic_class/CDSRV.h>
#include "../../memento/memento.hpp"
#include "../../zsp_factory/zsp_config/zsp_device/zsp_device.h"
#include "../../zsp_factory/zsp_config/zsp_state/zsp_state.h"
#include "../../zsp_factory/zsp_config/zsp_metadata/zsp_metadata.h"

#include <adcinfo.h>
#include <ADC_INFO_USB.h>
#include <Zadc.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <optional>
#include <vector>

namespace zetlab { namespace std {

typedef struct
{
	short int day	: 5;
	short int month	: 4;
	short int year	: 7;
} DATE_BITFIELD;

typedef union
{
	short int date_word;
	DATE_BITFIELD date;
} DIR_ENT_DATE;

typedef struct
{
	short int x2sec	: 5;
	short int min	: 6;
	short int hour	: 5;
} TIME_BITFIELD;

typedef union
{
	short int time_word;
	TIME_BITFIELD time;
} DIR_ENT_TIME;

typedef struct
{
	unsigned int sec	: 6;
	unsigned int min	: 6;
	unsigned int hour	: 5;
} TIME_BITFIELD_LONG;

typedef union
{
	long time_int;
	TIME_BITFIELD_LONG time;
} LONG_TIME;

typedef struct
{
	char filename[16];
	long sizebyte;
	DIR_ENT_TIME crt_time;
	DIR_ENT_DATE crt_date;

} DIR_FILE;

typedef struct
{
	unsigned long sizeSelf;
	char Signature[16];
	short ICP;
	short TestCode[3];
	char name[128];
	char conv[16];
	float sense;
	float DC;
	float refer;
	float ExtAmp;

} SENSOR;

enum connection_type {

	undefined = 0x0,
	pci,
	usb,
	ethernet,

	demo = 0xA

};

enum class start_record_t
{
	undefined = 0,
	button,
	power,
};

namespace devices {

	const tstring undefined = _T("Не определен");
	const tstring analyzer  = _T("Анализатор спектра");
	const tstring tenzo		= _T("Тензостанция");
	const tstring vcs		= _T("Контроллер СУВ");
	const tstring adc_dac   = _T("Модуль АЦП/ЦАП");
	const tstring vibrometer= _T("Виброметр/шумомер");
	const tstring osc       = _T("Осциллограф");
	const tstring seismo    = _T("Сейсморегистратор");

	static const tstring tenzo_keywords[] = { L"T8", L"058" };
	static const tstring vcs_keywords[] = { L"022", L"024", L"028" };
}

class device :
	public boost::noncopyable
{
public:
	typedef struct 
	{
		float adc_freq;
		float dac_freq;
		bool v_output[14];
		bool v_out_enable[14];
		tstring selftest;
	} clipboard;

private:
	class blinker;
	typedef boost::shared_ptr< blinker > blinker_ptr;

public:
	device();
	~device();

private:
	friend memento< device >::memento( device* );
	friend void memento< device >::restore();

	memento< device >::snapshot createMemento() const;

	void restoreMemento( const memento< device >::snapshot& snapshot_ );

public:
	void setServer( CDSRV* const server__ );
	bool set_zsp_config(
		::std::shared_ptr<zsp_device> zsp_device__, ::std::shared_ptr<zsp_state> zsp_state__, ::std::shared_ptr<zsp_metadata> zsp_metadata__);
	void save_zsp_config();
	void save_zsp_state();

	void setModule( const long& module_ );

	const long getModule() const;

	void setType( const long& type_ );

	const long getType() const;

	CString getTypeStr() const;

	void setDSP( const long& DSP_ );

	const long getDSP() const;

	void setSerial( const long& serial_ );
	void setSerial64(const uint64_t& serial_);

	const long getSerial() const;
	const uint64_t getSerial64() const;

	const tstring getVersion() const;

	const tstring getNameDevice() const;

	const connection_type getConnectionType();

	const float setAdcFrequency( const float& freq );

	const float getAdcFrequency() const;

	const float getAdcFrequency( ::std::vector< float >& v_freq ) const;

	const float setDacFrequency( const float& freq );

	const float getDacFrequency() const;

	const float getDacFrequency( ::std::vector< float >& v_freq ) const;

	const long numChannels();

	const long numDigitalPorts() const;

	const long getDigitalInputs( ::std::vector< bool >& v_status ) const;

	const long getDigitalOutputs( ::std::vector< bool >& v_status ) const;

	const long getDigitalOutEnable( ::std::vector< bool >& v_status ) const;

	void setDigitalOutput( const unsigned& port, const bool set );

	void setDigitalOutput( const ::std::vector< bool >& v_status );

	void setDigitalOutEnable( const unsigned& port, const bool set );

	void setDigitalOutEnable( const ::std::vector< bool >& v_status );

	const bool readEthSettings( unsigned char* ip,
								unsigned char* mask,
								unsigned char* gateway,
								unsigned short& port,
								unsigned char* mac,
								unsigned short& timeout,
								unsigned char& duplex,
								unsigned char& speed );

	long GetWordsADC();

	void writeEthSettings( unsigned char* ip,
						   unsigned char* mask,
						   unsigned char* gateway,
						   unsigned short timeout,
						   unsigned char duplex,
						   unsigned char speed );

	const bool isHaveEth();

	void readDigSyncSettings( bool& enableMaster,
							  bool& enableSlave );

	void writeDigSyncSettings( const bool& enableMaster,
							   const bool& enableSlave );

	const bool isHaveDigSync() const;

	bool readEthIP(BYTE* ip1, BYTE* ip2, BYTE* ip3, BYTE* ip4) const;

	bool readDescription(tstring& description);
	void writeDescription(const tstring& description);

	const bool isHaveDescription() const;

	const bool readGpsInfo( __time64_t& time,
							bool& synced,
							unsigned char& pps,
							unsigned char& sats );

	const bool isHaveGps();
	const bool isHaveSd();
	const unsigned long GetUptimeDevice();
	const unsigned long GetUptimeDriver();

	const bool readSyncSettings( bool& enableMaster,
								 bool& enableSlave,
								 unsigned char& domain,
								 unsigned char& absolutePriority,
								 unsigned char& relativePriority );

	const bool writeSyncSettings( const bool enableMaster,
								  const bool enableSlave,
								  const unsigned char domain,
								  const unsigned char absolutePriority,
								  const unsigned char relativePriority );

	const bool readBatteryState(unsigned char& batteryCharge,
								char& boardTemperature,
								unsigned short& batteryVoltage );

	const bool readSdState(unsigned long long& sdSize,
						   unsigned long long& busySize,
						   unsigned long long& availableSize,
						   ::std::vector< DIR_FILE >& v_files,
						   bool from_exclusive_mode = false);

	const bool readSdAvailableSize(unsigned long long& availableSize);
	
	const bool readMaxLevelDAC(double  &MaxDACLevel);

	const bool formatSd();
	const bool deleteFilesFromSd(::std::vector< tstring >& v_files);
	
	const bool isHaveSync();

	void rawAdcInfo( ADC_INFO* adc_info,
					 double* ref_freq,
					 long* master_sync,
					 long* enableBiGen,
					 long* activeSineBiGen,
					 double* freqSineBiGen,
					 double* amplSineBiGen,
					 double* offsetSineBiGen,
					 unsigned long* chargeChannel);

	void GetRawDataFromDevice(ADC_INFO* adc_info);

	void startBlinking();

	void stopBlinking();

	const bool isBlinking() const;

	const bool isHaveICP() const;

	const bool isHaveTensoICP() const;

	const bool isHaveTenso(long& new_bridge_mode, long& new_pot1, long& new_pot2, long device_id);

	void setTenso(long new_bridge_mode, long device_id, long Pot1, long Pot2);
	void setTenso(long new_bridge_mode, long device_id, long Pot1, long Pot2, bool group);

	void saveTenso();

	const bool isHaveCharge() const;

	const bool isHaveTEDS() const;

	const bool isSupportDiff() const;

	const bool isSupportAmplify() const;

	const bool isSupportSensorStruct() const;

	const bool isHaveAdjustmentLevel() const;

	const bool isHaveActuator() const;

	const bool isTensostation() const;

	const bool isVCS() const;

	UINT GetAdjustmentLevel();

	const double getMaxLevelDAC();

	void setDefaultCfg();

	const long GetIndexGenerator();
#pragma region Работа с внутренним ЦАП
	const bool isHaveBuiltinGenDC() const;

	const bool isHaveBuiltinGenerator() const;

	const bool GetBuiltinGeneratorEnabled();

	const bool GetExternalGeneratorEnabled();

	const bool SetBuiltinGeneratorEnabled(long Enable);

	const bool GetBuiltinGeneratorSettings(long &Enable,
										   double &freq,
										   double &level,
										   double &offset);

	const bool SetBuiltinGeneratorSettings(long Enable,
										   double freq,
										   double level,
										   double offset);

	const bool Get1PPSBuiltinGeneratorSettings(long &Enable);

	const bool Set1PPSBuiltinGeneratorSettings(long Enable);

	const bool GetCalibrationDevice();

	const start_record_t get_sd_start_record() const;
	void set_sd_start_record(const start_record_t t);
	const uint32_t get_sd_record_minutes() const;
	void set_sd_record_minutes(const uint32_t minutes);
	const bool get_sd_record_continuous() const;
	void set_sd_record_continuous(const bool continuous);

	const bool get_dhcp() const;
	void set_dhcp(const bool dhcp);

	const bool has_ftp() const;
	const bool get_ftp() const;
	void set_ftp(const bool ftp);

	const bool has_selftest() const;
	const tstring get_selftest() const;
	void set_selftest(const tstring& selftest);
	const ::std::list<::std::pair<::std::wstring, ::std::wstring>> get_selftest_list() const;

	const bool has_service_mode() const;
	const bool get_service_mode() const;
	void set_service_mode(const bool service_mode);

	const bool has_mseed_metadata() const;
	const tstring get_network() const;
	void set_network(const tstring& network);
	const tstring get_station() const;
	void set_station(const tstring& station);
	const double get_latitude() const;
	void set_latitude(const double& latitude);
	const double get_longitude() const;
	void set_longitude(const double& longitude);
	const double get_elevation() const;
	void set_elevation(const double& elevation);
	const tstring get_site_name() const;
	void set_site_name(const tstring& site_name);

#pragma endregion Работа с внутренним ЦАП
private:
	const long getDigitalIOE( const boost::function< long(long) >& f,
							  ::std::vector< bool >& v_status ) const;	

	const long getDigitalIOE2(const boost::function< long(unsigned long, unsigned long long) >& f,
							  ::std::vector< bool >& v_status) const;

private:
	CDSRV* server_;

	long module;
	long type;
	long DSP;
	long serial;
	uint64_t serial64;

	::std::shared_ptr<zsp_device> zsp_device_;
	::std::shared_ptr<zsp_state> zsp_state_;
	::std::shared_ptr<zsp_metadata> zsp_metadata_;

	blinker_ptr blinker_ptr_;
};

typedef boost::shared_ptr< device >         device_ptr;
typedef memento< device >                   device_memento;
typedef boost::shared_ptr< device_memento > device_memento_ptr;

}} // namespace zetlab::std

#endif // device_std_hpp__
