#pragma once
#ifndef channel_hpp__
#define channel_hpp__

#include "../../../types.hpp"
#include <ZETView_basic_class/CDSRV.h>

#include "../../utils/clipboard/clipboard.hpp"
#include "../../memento/memento.hpp"
#include "../../zsp_factory/zsp_config/zsp_channel/zsp_channel.h"
#include "../../zsp_factory/zsp_config/zsp_metadata/zsp_metadata.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <optional>

namespace zetlab { namespace std {

class device;

class channel :
	public boost::noncopyable
{
public:
	bool TensoChangeFlag;
	bool PotsChangeFlag;
	bool group;
	typedef struct  
	{
		bool  enabled;
		TCHAR name[64 + 1];
		TCHAR comment[254 + 1];
		TCHAR unit[64 + 1];
		double sense;
		TCHAR unitSense[64 + 1];
		double reference;
		double shift;
		uint16_t hpf;
		double amplify;
		double amplifygain; ///////////////
		bool diff;
		long icp;
		long charge;
		
		double coorX;
		double coorY;
		double coorZ;
		long coorP;

		long tensoIcp;
		double tenso_resistance;
		double tenso_coeff;
		double tenso_young_mod;

		uint16_t inputResistance;
		uint16_t adcInputMode;

		uint16_t tenso_scheme;
		double tenso_exc;
		uint16_t tenso_sensor;

		double tenso_sensor_range;
		double tenso_sensor_output;

		double tenso_resistor_k;
		uint16_t tenso_resistor_mode;
		double tenso_poisson_ratio;
		double tenso_elastic_modulus;
		double tenso_outer_diameter;
		double tenso_inner_diameter;

	} clipboard;	

public:
	channel();

private:
	friend void ::zetlab::utils::copy_to_clipboard< channel, channel::clipboard >( LPCTSTR, channel* );
	friend const bool ::zetlab::utils::paste_from_clipboard< channel, channel::clipboard >( LPCTSTR, channel* );
	friend memento< channel >::memento( channel* );
	friend void memento< channel >::restore();
	friend void memento< channel >::restore_(long mask);
	//friend void memento< channel >::restoreMemento_anyDevice(long modul, long mask, const memento< channel >::snapshot& snapshot_);

	void backup( clipboard& cb ) const;

	void restore( const clipboard& cb);

	void restore_(const clipboard& cb, long mask);

	void restore_anydevice(long modul, long mask, const clipboard& cb);  ///////// ///////
	
	clipboard toClipboard() const;
	
	void fromClipboard( const clipboard& cb);
	
	memento< channel >::snapshot createMemento() const;
	
	void restoreMemento( const memento< channel >::snapshot& snapshot_);
	
	void restoreMemento_(const memento< channel >::snapshot& snapshot_, long mask);

	
public:

	void restoreMemento_anyDevice(long modul, long mask, const memento< channel >::snapshot& snapshot_);///////////////////////

	void setServer( CDSRV* const server__ );

	void setDevice( device* const device__ );

	bool set_zsp_config(::std::shared_ptr<zsp_channel> zsp_channel__, ::std::shared_ptr<zsp_metadata> zsp_metadata__);
	void save_zsp_config();

	const device* getDevice() const;

	void setDeviceID( const long device_id_ );

	const long getDeviceID() const;

	const long getType() const;

	const long getDSP() const;

	void setServerID( const long& server_id_ );

	const long getServerID() const;

	void setEnabled( const bool& enabled_ );

	const bool isEnabled() const;

	void setEnabledPh( const bool& enabled_ );

	void setEnabledPhZspMoreChannel(const bool& enabled_); // откл - вкл для zsp каналов

	const bool canEnableOrDisable();

	const bool isAlone();

	void setName( const tstring& name_ );

	void setComment(const tstring& comment_);

	void setCoordX(const double& coordX_);

	void setCoordY(const double& coordY_);

	void setCoordZ(const double& coordZ_);

	void setCoordP(const long& coordP_);

	const tstring& getName( bool from_server = false );

	const tstring& getComment(bool from_server = false);

	const double CoordX(bool from_server = false);

	const double getConstCoordX() const;

	const double CoordY(bool from_server = false);

	const double getConstCoordY() const;

	const double CoordZ(bool from_server = false);

	const double getConstCoordZ() const;

	const long CoordP(bool from_server = false);

	const long getConstCoordP() const;

	void setUnit( const tstring& unit_ );

	const tstring& getUnit( bool from_server = false );

	const tstring& getUnitFor7xxx();

	void setSense( const double& sense_ );

	const double Sense( bool from_server = false );

	void setTensoResistance(const double& tenso_resistance_);

	const double getTensoResistance(bool from_server = false);

	void setTensoCoeff(const double& tenso_coeff_);

	const double getTensoCoeff(bool from_server = false);

	void setTensoYoungMod(const double& tenso_young_mod_);

	const double getTensoYoungMod(bool from_server = false);

	void setTensoIcp(const long& tensoIcp_);

	const long getTensoIcp(bool from_server = false);

	void setUnitSense(const tstring& unitsense);

	const tstring& getUnitSense(bool from_server = false);

	void SetFreq(const float& freq_);

	const float GetFreq();

	void setReference( const double& reference_ );

	const double Reference( bool from_server = false );

	void setShift( const double& shift_ );

	const double Shift( bool from_server = false );

	void setHpf(const uint16_t& hpf_);

	const uint16_t& HPF(bool from_server = false);

	void setVirt(const bool& virt_);

	const bool isVirt() const;

	void set_zsp(const bool& zsp_);

	const bool is_zsp() const;

	// канал с обратной связью
	const bool isFeedBackChannel() const;

	// при закрытии менеджера функция  isHaveBuiltinGenerator падала
	const bool isHaveBuiltinGeneratorWithCheck() const;

	const bool GetVersionDeviceNewVersion() const;

	const long GetGenIndex();

#pragma region Работа с внутренним ЦАП
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

	// проверка на то, что канал генератора не используется другими программами
	const bool IsBuiltinGenChanFree();
#pragma endregion Работа с внутренним ЦАП

	const long Preamplifier();

	const float getLevel();

	const double getMaxLevel();

	const double getMaxLevelDAC();

	const double getRange();

	void setAmplify( const double& amplify_ );

	void setAmplifyGain(const double& amplify_);  //для установки КУ на все каналы

	const double getAmplify();

	double getAmplifyzsp();  // для получения КУ на zsp в backup

	const double Amplify( bool from_server = false );

	const tstring AFCH() { return _T(""); }

	void setNextRange(bool save = true);
	void setNextRange(const long& mask);

	void setPrevRange(bool save = true);
	void setPrevRange(const long& mask);

	void setDiff( bool diff = true, bool save = true );
	void setDiff(const long& mask, bool diff = true);

	const long getDiff() const;

	void setICP( const long& icp_, bool save = true);
	void setICP(const long& icp_, const long& mask);
	void setICPZsp(const long& icp_);  // установка ICP для zsp на 1 прибор 037 отработка от канала
	void saveICPZsp();  //сохранение zsp

	void setMoreChannel(const long& , const long& , const long& ); // для множества каналов на разных уст-вах
	void setICPMoreChannel(const long&, const long&, const long&);// для множества каналов на разных уст-вах ICP
	void setICPMoreZsp(bool OnOff); // для выделенных zsp вкл/откл
	const long getICP() const;

	//функции для проверки, задачи и получения tenso
	const bool isHaveTenso(long& new_bridge_mode, long* __pot1 = NULL, long* __pot2 = NULL);//, long);

	void setTenso(bool group, const long& tenso_mode_, long Pot1 = -1, long Pot2 = -1);

	void setTensoMode(const long& tenso_mode_);

	void saveTenso();

	const long Tenso(bool from_server = false);

	long Pot1();

	long Pot2();

	const bool isHaveICP() const;

	const bool isHaveTensoICP() const;

	const bool isTensostation() const;

	void setCharge(const long& charge_);

	const long getCharge() const;

	void setInputResistance(const uint16_t& inputResistance_);

	const uint16_t getInputResistance() const;

	const long InputResistance(bool from_server = false);

	void setAdcInputMode(const uint16_t& adcInputMode_);

	const uint16_t getAdcInputMode() const;

	const long AdcInputMode(bool from_server = false);

	const bool isHaveCharge() const;

	// функции для проверки наличия TEDS
	const bool isHaveTEDS() const;

	const bool isSupportDiff() const;

	const bool isSupportAmplify() const;

	const bool isSupportSensorStruct() const;

	const bool isSupportInputModeAdc() const;

	const bool isDemo() const;

	CDSRV* getServer() const;

	const uint32_t get_slot_serial() const;
	const bool is_zet037_tenso() const;

	const uint16_t get_tenso_scheme() const;
	void set_tenso_scheme(const uint16_t scheme, bool b_notify);

	const double get_tenso_exc() const;
	void set_tenso_exc(const double exc, bool b_notify);

	const uint16_t get_tenso_sensor() const;
	void set_tenso_sensor(const uint16_t sensor, bool b_notify);

	const double get_tenso_sensor_range() const;
	void set_tenso_sensor_range(const double sensor_range);

	const double get_tenso_sensor_output() const;
	void set_tenso_sensor_output(const double sensor_output);

	const double get_tenso_resistor_k() const;
	void set_tenso_resistor_k(const double resistor_k);

	const uint16_t get_tenso_resistor_mode() const;
	void set_tenso_resistor_mode(const uint16_t resistor_mode, bool b_notify);
		
	const double get_tenso_poisson_ratio() const;
	void set_tenso_poisson_ratio(const double poisson_ratio);
			
	const double get_tenso_elastic_modulus() const;
	void set_tenso_elastic_modulus(const double poisson_ratio);	

	const double get_tenso_outer_diameter() const;
	void set_tenso_outer_diameter(const double outer_diameter);

	const double get_tenso_inner_diameter() const;
	void set_tenso_inner_diameter(const double inner_diameter);

	const tstring get_location() const;
	void set_location(const tstring& location);

	const tstring get_channel() const;
	void set_channel(const tstring& channel);

	const uint16_t get_tr_nsh() const;
	void set_tr_nsh(const uint16_t tr_nsh);
		
	const uint16_t get_tc_type() const;
	void set_tc_type(const uint16_t tc_type);
		
private:
	CDSRV* server_;
	device* parent_;
	::std::shared_ptr<zsp_channel> zsp_channel_;
	::std::shared_ptr<zsp_metadata> zsp_metadata_;

	long    device_id;
	long    server_id;
	bool    enabled;
	tstring name;
	tstring comment;
	double cX;
	double cY;
	double cZ;
	long cP;
	tstring unit;
	double  sense;
	tstring UnitSense;
	long tenso_mode;
	long Pot1_val, Pot2_val;
	double  reference;
	double  shift;
	uint16_t hpf;
	double  amplify;
	double amplifygain;//////////////
	bool    virt;
	bool    zsp;
	float m_frequency;
	uint16_t tenso_icp;
	double tenso_resistance;
	double tenso_coeff;
	double tenso_young_mod;
	uint16_t inputResistance;
	uint16_t adcInputMode;

};

typedef boost::shared_ptr< channel >         channel_ptr;
typedef memento< channel >                   channel_memento;
typedef boost::shared_ptr< channel_memento > channel_memento_ptr;

}} // namespace zetlab::std

#endif // channel_hpp__
