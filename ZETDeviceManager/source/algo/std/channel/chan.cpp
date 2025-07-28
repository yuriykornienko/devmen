#include "stdafx.h"

#include "channel.hpp"

#include "../device/device_std.hpp"
#include "../../utils/align_value/align_value.hpp"
#include "resource/Phrases.h"

#include <adcinfo.h>
#include <Zadc.h>

namespace zetlab { namespace std {

channel::channel() :
	server_(nullptr),
	parent_(nullptr),
	server_id(-1),
	device_id(-1),
	enabled(false),
	sense(float(0xffff)),
	reference(float(0xffff)),
	shift(float(0xffff)),
	hpf(0xffff),
	amplify(float(0xffff)),
	amplifygain(1), //////////////////////////
	virt(false),
	tenso_mode(0),
	Pot1_val(0),
	Pot2_val(0),
	TensoChangeFlag(false),
	PotsChangeFlag(false),
	cX(double(0xffff)), cY(double(0xffff)), cZ(double(0xffff)), cP(long(-1)),
	m_frequency(0),
	tenso_icp(0xffff),
	tenso_resistance(float(0xffff)),
	tenso_coeff(float(0xffff)),
	tenso_young_mod(float(0xffff)),
	inputResistance(0xffff),
	adcInputMode(0xffff)
{
}

void channel::backup( clipboard& cb ) const
{
	cb.enabled   = enabled;
	_tcscpy_s( cb.name, sizeof(cb.name)/sizeof(cb.name[0]), name.c_str());
	_tcscpy_s( cb.comment, sizeof(cb.comment) / sizeof(cb.comment[0]), comment.c_str());
	_tcscpy_s( cb.unit, sizeof(cb.unit)/sizeof(cb.unit[0]), unit.c_str());
	_tcscpy_s( cb.unitSense, sizeof(cb.unitSense) / sizeof(cb.unitSense[0]), UnitSense.c_str());
	cb.sense     = sense;
	cb.reference = reference;
	cb.shift     = shift;
	cb.hpf		 = hpf;
	cb.amplify   = amplify;

	if(zsp_channel_)
	cb.amplifygain = zsp_channel_->get_amplify(); //для zsp KY

	cb.diff      = (1 == getDiff());
	cb.icp       = getICP();
	cb.charge	 = getCharge();

	cb.tensoIcp = tenso_icp;
	cb.tenso_resistance = tenso_resistance;
	cb.tenso_coeff = tenso_coeff;
	cb.tenso_young_mod = tenso_young_mod;
	
	cb.coorX = getConstCoordX();
	cb.coorY = getConstCoordY();
	cb.coorZ = getConstCoordZ();
	cb.coorP = getConstCoordP();

	cb.inputResistance = getInputResistance();
	cb.adcInputMode = getAdcInputMode();

	cb.tenso_scheme = get_tenso_scheme();
	cb.tenso_exc = get_tenso_exc();
	cb.tenso_sensor = get_tenso_sensor();
	cb.tenso_sensor_range = get_tenso_sensor_range();
	cb.tenso_sensor_output = get_tenso_sensor_output();
	cb.tenso_resistor_k = get_tenso_resistor_k();
	cb.tenso_resistor_mode = get_tenso_resistor_mode();
	cb.tenso_poisson_ratio = get_tenso_poisson_ratio();
	cb.tenso_elastic_modulus = get_tenso_elastic_modulus();
	cb.tenso_outer_diameter = get_tenso_outer_diameter();
	cb.tenso_inner_diameter = get_tenso_inner_diameter();
}

void channel::restore( const clipboard& cb )
{
	setEnabledPh( cb.enabled );
	setName     ( cb.name );
	setComment	( cb.comment);
	setUnit     ( cb.unit );
	setSense    ( cb.sense );
	setReference( cb.reference );
	setShift    ( cb.shift );
	setHpf		( cb.hpf );
	setAmplify  ( cb.amplify );

	if (zsp_channel_)
	setAmplifyGain (cb.amplifygain); //////////////////////

	setDiff		( cb.diff );
	setICP      ( cb.icp );
	setCharge	( cb.charge );
	setTensoIcp ( cb.tensoIcp );
	setCoordX	( cb.coorX);
	setCoordY	( cb.coorY );
	setCoordZ	( cb.coorZ );
	setCoordP	( cb.coorP );
	setUnitSense( cb.unitSense );
	setTensoResistance( cb.tenso_resistance );
	setTensoCoeff(cb.tenso_coeff);
	setTensoYoungMod(cb.tenso_young_mod);
	setInputResistance(cb.inputResistance);
	setAdcInputMode(cb.adcInputMode);
	set_tenso_scheme(cb.tenso_scheme, false);
	set_tenso_exc(cb.tenso_exc, false);
	set_tenso_sensor(cb.tenso_sensor, false);
	set_tenso_sensor_range(cb.tenso_sensor_range);
	set_tenso_sensor_output(cb.tenso_sensor_output);
	set_tenso_resistor_k(cb.tenso_resistor_k);
	set_tenso_resistor_mode(cb.tenso_resistor_mode, false);
	set_tenso_poisson_ratio(cb.tenso_poisson_ratio);
	set_tenso_elastic_modulus(cb.tenso_elastic_modulus);
	set_tenso_outer_diameter(cb.tenso_outer_diameter);
	set_tenso_inner_diameter(cb.tenso_inner_diameter);
}

void channel::restore_(const clipboard& cb, long mask)
{
	setEnabledPh(cb.enabled);
	setName(cb.name);
	setComment(cb.comment);
	setUnit(cb.unit);
	setSense(cb.sense);
	setReference(cb.reference);
	setShift(cb.shift);
	setHpf(cb.hpf);
	setAmplify(cb.amplify);

    if (zsp_channel_)
	setAmplifyGain(cb.amplifygain); //////////////

	setDiff(mask, cb.diff);
	setICP(cb.icp, mask);
	//setICPMoreChannel(modul, mask, cb.icp);
	setCharge(cb.charge);
	setTensoIcp(cb.tensoIcp);
	setCoordX(cb.coorX);
	setCoordY(cb.coorY);
	setCoordZ(cb.coorZ);
	setCoordP(cb.coorP);
	setUnitSense(cb.unitSense);
	setTensoResistance(cb.tenso_resistance);
	setTensoCoeff(cb.tenso_coeff);
	setTensoYoungMod(cb.tenso_young_mod);
	setInputResistance(cb.inputResistance);
	setAdcInputMode(cb.adcInputMode);
	set_tenso_scheme(cb.tenso_scheme, false);
	set_tenso_exc(cb.tenso_exc, false);
	set_tenso_sensor(cb.tenso_sensor, false);
	set_tenso_sensor_range(cb.tenso_sensor_range);
	set_tenso_sensor_output(cb.tenso_sensor_output);
	set_tenso_resistor_k(cb.tenso_resistor_k);
	set_tenso_resistor_mode(cb.tenso_resistor_mode, false);
	set_tenso_poisson_ratio(cb.tenso_poisson_ratio);
	set_tenso_elastic_modulus(cb.tenso_elastic_modulus);
	set_tenso_outer_diameter(cb.tenso_outer_diameter);
	set_tenso_inner_diameter(cb.tenso_inner_diameter);
}

void channel::restore_anydevice(long modul, long mask, const clipboard& cb) /////  для Отмена на разных устройствах
{
	setEnabledPh(cb.enabled);
	setName(cb.name);
	setComment(cb.comment);
	setUnit(cb.unit);
	setSense(cb.sense);
	setReference(cb.reference);
	setShift(cb.shift);
	setHpf(cb.hpf);
	setAmplify(cb.amplify);

	if (zsp_channel_)
	setAmplifyGain(cb.amplifygain);////////////////////

	setDiff(mask, cb.diff);
	//setICP(cb.icp, mask);
    setICPMoreChannel(modul, mask, cb.icp);  // добавлена по сравнению с выше
	setCharge(cb.charge);
	setTensoIcp(cb.tensoIcp);
	setCoordX(cb.coorX);
	setCoordY(cb.coorY);
	setCoordZ(cb.coorZ);
	setCoordP(cb.coorP);
	setUnitSense(cb.unitSense);
	setTensoResistance(cb.tenso_resistance);
	setTensoCoeff(cb.tenso_coeff);
	setTensoYoungMod(cb.tenso_young_mod);
	setInputResistance(cb.inputResistance);
	setAdcInputMode(cb.adcInputMode);
	set_tenso_scheme(cb.tenso_scheme, false);
	set_tenso_exc(cb.tenso_exc, false);
	set_tenso_sensor(cb.tenso_sensor, false);
	set_tenso_sensor_range(cb.tenso_sensor_range);
	set_tenso_sensor_output(cb.tenso_sensor_output);
	set_tenso_resistor_k(cb.tenso_resistor_k);
	set_tenso_resistor_mode(cb.tenso_resistor_mode, false);
	set_tenso_poisson_ratio(cb.tenso_poisson_ratio);
	set_tenso_elastic_modulus(cb.tenso_elastic_modulus);
	set_tenso_outer_diameter(cb.tenso_outer_diameter);
	set_tenso_inner_diameter(cb.tenso_inner_diameter);
}


channel::clipboard channel::toClipboard() const
{
	clipboard cb;
	backup( cb );
	return cb;
}

void channel::fromClipboard( const clipboard& cb )
{
	restore( cb );
}

memento< channel >::snapshot channel::createMemento() const
{
	clipboard* snapshot_(nullptr);
	snapshot_ = new clipboard;
	backup( *snapshot_ );
	return memento< channel >::snapshot(snapshot_);
}

void channel::restoreMemento( const memento< channel >::snapshot& snapshot_)
{
	restore( *snapshot_.get());
}

void channel::restoreMemento_(const memento< channel >::snapshot& snapshot_, long mask)
{
	restore_(*snapshot_.get(), mask);
}

void channel::restoreMemento_anyDevice(long modul, long mask, const memento< channel >::snapshot& snapshot_)
{
	restore_anydevice(modul , mask , *snapshot_.get());
}

void channel::setServer( CDSRV* const server__ )
{
	server_ = server__;
}

void channel::setDevice( device* const device__ )
{
	parent_ = device__;
}

bool channel::set_zsp_config(::std::shared_ptr<zsp_channel> zsp_channel__, ::std::shared_ptr<zsp_metadata> zsp_metadata__)
{
	bool b_result(false);

	if (zsp_channel_ != zsp_channel__)
	{
		zsp_channel_ = zsp_channel__;
		b_result = true;
	}

	if (zsp_channel_ && zsp_channel__)
	{
		if (*zsp_channel_.get() != *zsp_channel__.get())
		{
			*zsp_channel_.get() = *zsp_channel__.get();
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

void channel::save_zsp_config()
{
	if (zsp_channel_)
		zsp_channel_->save();
}

const zetlab::std::device* zetlab::std::channel::getDevice() const
{
	return parent_;
}

void channel::setDeviceID( const long device_id_ )
{
	if (device_id_ < 0)
		device_id = 0xffff;
	else
		device_id = device_id_;
}

const long channel::getDeviceID() const
{
	return device_id;
}

const long channel::getType() const
{
	if (nullptr == parent_)
		return 0;

	return parent_->getType();
}

const long channel::getDSP() const
{
	if (nullptr == parent_)
		return 0;

	return parent_->getDSP();
}

void channel::setServerID( const long& server_id_ )
{
	server_id = server_id_;
}

const long channel::getServerID() const
{
	return server_id;
}

void channel::setEnabled( const bool& enabled_ )
{
	enabled = enabled_;
}

const bool channel::isEnabled() const
{
	return enabled;
}

void channel::setEnabledPhZspMoreChannel(const bool& enabled_)
{
	if (enabled != enabled_)
	{
		setEnabled(enabled_);

		if (zsp)
		{
			if (!zsp_channel_)
				return;

			zsp_channel_->set_enable(enabled_);
			return;
		}
	}
}

void channel::setEnabledPh( const bool& enabled_ )
{
	if (enabled != enabled_)
	{
		setEnabled( enabled_ );

		if (zsp)
		{
			if (!zsp_channel_)
				return;

			zsp_channel_->set_enable(enabled_);
			zsp_channel_->save();

			return;
		}

		if (nullptr != server_ && nullptr != parent_)
			server_->OnOffChannel( parent_->getModule(), device_id, enabled_ ? 1 : 0 );
	}
}

const bool channel::canEnableOrDisable()
{
	return !isAlone() && !(getDiff() && (device_id % 2));
}

const bool channel::isAlone()
{
	if (nullptr == parent_)
		return 0;

	return (isEnabled() && parent_->numChannels() == 1);
}

void channel::setName( const tstring& name_ )
{
	name = name_;
	if (zsp_channel_)
		zsp_channel_->set_name(name);
}

void channel::setComment(const tstring& comment_)
{
	comment = comment_;
	if (zsp_channel_)
		zsp_channel_->set_comment(comment);
}

void channel::setCoordX(const double& coordX_)
{
	cX = coordX_;
	if (zsp_channel_)
		zsp_channel_->set_x(cX);
}

void channel::setCoordY(const double& coordY_)
{
	cY = coordY_;
	if (zsp_channel_)
		zsp_channel_->set_y(cY);
}

void channel::setCoordZ(const double& coordZ_)
{
	cZ = coordZ_;
	if (zsp_channel_)
		zsp_channel_->set_z(cZ);
}

void channel::setCoordP(const long& coordP_)
{
	cP = coordP_;
	if (zsp_channel_)
		zsp_channel_->set_p(cP);
}

const tstring& channel::getName( bool from_server )
{
	if (name.empty() && from_server && nullptr != server_)
		name = tstring(server_->Commentary( server_id ));

	return name;
}

const tstring& channel::getComment(bool from_server)
{// спец символ. не трогать
	if (from_server)
	{
		if (zsp)
			comment = zsp_channel_ ? zsp_channel_->get_comment() : L"⁣⁣";
		else if ((comment.empty() || comment == L"⁣⁣") && nullptr != server_)
			comment = tstring(server_->GetComment(server_id));
	}

	return comment;
}

const double channel::CoordX(bool from_server)
{
	if (cX == double(0xffff) && from_server && nullptr != server_)
	{
		if ((server_->GetCoordinate(server_id, &cX, 0, 0, 0)) != 0)
			cX = 0;
	}
		
	return cX;
}

const double channel::getConstCoordX() const
{
	double constCx = cX;

	return constCx;
}

const double channel::CoordY(bool from_server) 
{
	if (cY == double(0xffff) && from_server && nullptr != server_)
	{
		if ((server_->GetCoordinate(server_id, 0, &cY, 0, 0)) != 0)
			cY = 0;
	}

	return cY;
}

const double channel::getConstCoordY() const
{
	double constCy = cY;

	return constCy;
}

const double channel::CoordZ(bool from_server)
{
	if (cZ == double(0xffff) && from_server && nullptr != server_)
	{
		if ((server_->GetCoordinate(server_id, 0, 0, &cZ, 0)) != 0)
			cZ = 0;
	}

	return cZ;
}

const double channel::getConstCoordZ() const
{
	double constCz = cZ;

	return constCz;
}

const long channel::CoordP(bool from_server)
{
	if (cP == long(-1) && from_server && nullptr != server_)
	{
		if ((server_->GetCoordinate(server_id, 0, 0, 0, &cP)) != 0)
			cP = -1;
	}

	return cP;
}

void channel::SetFreq(const float& freq_)
{
	m_frequency = freq_;
}

// Функция вернет частоту, только когда была вызвана функция SetFreq, из под листа
const float channel::GetFreq()
{
	return m_frequency;
}

const long channel::getConstCoordP() const
{
	long constCp = cP;

	return constCp;
}

void channel::setUnit( const tstring& unit_ )
{
	unit = unit_;
	if (zsp_channel_)
		zsp_channel_->set_unit(unit);
}

const tstring& channel::getUnit( bool from_server )
{
	if (from_server)
	{
		if (zsp)
			unit = zsp_channel_ ? zsp_channel_->get_unit() : L"мВ";
		else if (unit.empty() && nullptr != server_)
			unit = tstring(server_->Conversion(server_id));
	}

	return unit;
}

void channel::setUnitSense(const tstring& unitsense_)
{
	UnitSense = unitsense_;
	if (zsp_channel_)
		zsp_channel_->set_unit_sense(UnitSense);
}

const tstring& channel::getUnitSense(bool from_server )
{
	if (from_server)
	{
		if (zsp)
			UnitSense = zsp_channel_ ? zsp_channel_->get_unit_sense() : L"В";
		else if (UnitSense.empty() && nullptr != server_)
			UnitSense = tstring(server_->GetUnitSense(server_id));
	}
	
	return UnitSense;
}

const tstring& channel::getUnitFor7xxx()
{
	if (nullptr != server_) 
	{
		unit = tstring(server_->Conversion(server_id));
	}

	return unit;
}

CDSRV* channel::getServer() const
{
	return server_;
}

const bool channel::is_zet037_tenso() const
{
	if (zsp)
	{
		if (zsp_channel_)
			return 
				(zsp_channel_->get_type() == zsp_device_type::zet037) &&
				(zsp_channel_->get_modification() == zsp_device_modification::t);
		
	}

	return false;
}

const uint16_t channel::get_tenso_scheme() const
{
	if (zsp)
		return zsp_channel_ ? static_cast<uint16_t>(zsp_channel_->get_tenso_scheme()) : -1;

	return -1;
}

void channel::set_tenso_scheme(const uint16_t scheme, bool b_notify)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_scheme(static_cast<zsp_tenso_scheme>(scheme), b_notify);
}

const double channel::get_tenso_exc() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_exc() : 0.;

	return 0.;
}

void channel::set_tenso_exc(const double exc, bool b_notify)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_exc(exc, b_notify);
}

const uint16_t channel::get_tenso_sensor() const
{
	if (zsp)
		return zsp_channel_ ? static_cast<uint16_t>(zsp_channel_->get_tenso_sensor()) : -1;

	return -1;
}

void channel::set_tenso_sensor(const uint16_t sensor, bool b_notify)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_sensor(static_cast<zsp_tenso_sensor>(sensor), b_notify);
}

const double channel::get_tenso_sensor_range() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_sensor_range() : 0.;

	return 0.;
}

void channel::set_tenso_sensor_range(const double sensor_range)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_sensor_range(sensor_range);
}

const double channel::get_tenso_sensor_output() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_sensor_output() : 0.;

	return 0.;
}

void channel::set_tenso_sensor_output(const double sensor_output)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_sensor_output(sensor_output);
}

const double channel::get_tenso_resistor_k() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_resistor_k() : 0.;

	return 0.;
}

void channel::set_tenso_resistor_k(const double resistor_k)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_resistor_k(resistor_k);
}

const uint16_t channel::get_tenso_resistor_mode() const
{
	if (zsp)
		return zsp_channel_ ? static_cast<uint16_t>(zsp_channel_->get_tenso_resistor_mode()) : -1;

	return -1;
}

void channel::set_tenso_resistor_mode(const uint16_t resistor_mode, bool b_notify)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_resistor_mode(static_cast<zsp_tenso_resistor_mode>(resistor_mode), b_notify);
}

const double channel::get_tenso_poisson_ratio() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_poisson_ratio() : 0.;

	return 0.;
}

void channel::set_tenso_poisson_ratio(const double poisson_ratio)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_poisson_ratio(poisson_ratio);
}

const double channel::get_tenso_elastic_modulus() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_elastic_modulus() : 0.;

	return 0.;
}

void channel::set_tenso_outer_diameter(const double outer_diameter)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_outer_diameter(outer_diameter);
}

const double channel::get_tenso_outer_diameter() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_outer_diameter() : 0.;

	return 0.;
}

void channel::set_tenso_inner_diameter(const double inner_diameter)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_inner_diameter(inner_diameter);
}

const double channel::get_tenso_inner_diameter() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_tenso_inner_diameter() : 0.;

	return 0.;
}

void channel::set_tenso_elastic_modulus(const double elastic_modulus)
{
	if (zsp_channel_)
		zsp_channel_->set_tenso_elastic_modulus(elastic_modulus);
}

const uint32_t channel::get_slot_serial() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_slot_serial() : 0;

	return 0;
}

void channel::set_location(const tstring& location)
{
	if (zsp_metadata_)
	{
		zsp_metadata_->set_location(device_id, location);

		name =
			zsp_metadata_->get_network_code() + L"_" +
			zsp_metadata_->get_station_code() + L"_" +
			zsp_metadata_->get_location(device_id) + L"_" +
			zsp_metadata_->get_channel(device_id);

		if (zsp_channel_)
			zsp_channel_->set_name(name);
	}
}

const tstring channel::get_location() const
{
	if (zsp)
		return zsp_metadata_ ? zsp_metadata_->get_location(device_id) : L"";

	return {};
}

void channel::set_channel(const tstring& channel)
{
	if (zsp_metadata_)
	{
		zsp_metadata_->set_channel(device_id, channel);

		name =
			zsp_metadata_->get_network_code() + L"_" +
			zsp_metadata_->get_station_code() + L"_" +
			zsp_metadata_->get_location(device_id) + L"_" +
			zsp_metadata_->get_channel(device_id);

		if (zsp_channel_)
			zsp_channel_->set_name(name);
	}
}

const tstring channel::get_channel() const
{
	if (zsp)
		return zsp_metadata_ ? zsp_metadata_->get_channel(device_id) : L"";

	return {};
}

const uint16_t channel::get_tr_nsh() const
{
	if (zsp)
		return zsp_channel_ ? static_cast<uint16_t>(zsp_channel_->get_tr_nsh()) : -1;

	return -1;
}

void channel::set_tr_nsh(const uint16_t tr_nsh)
{
	if (zsp_channel_)
		zsp_channel_->set_tr_nsh(static_cast<zsp_tr_nsh>(tr_nsh));
}

const uint16_t channel::get_tc_type() const
{
	if (zsp)
		return zsp_channel_ ? static_cast<uint16_t>(zsp_channel_->get_tc_type()) : -1;

	return -1;
}

void channel::set_tc_type(const uint16_t tc_type)
{
	if (zsp_channel_)
		zsp_channel_->set_tc_type(static_cast<zsp_tc_type>(tc_type));
}

void channel::setSense( const double& sense_ )
{
	sense = sense_;
	if (zsp_channel_)
		zsp_channel_->set_sense(sense);
}

const double channel::Sense( bool from_server )
{
	if (from_server)
	{
		if (zsp)
			sense = zsp_channel_ ? zsp_channel_->get_sense() : 0.001;
		else if (float(0xffff) == sense && nullptr != server_)
			sense = server_->Sense(server_id);
	}
	
	return sense;
}

void channel::setTensoResistance(const double& tenso_resistance_)
{
	tenso_resistance = tenso_resistance_;
	if (zsp_channel_)
		zsp_channel_->set_tenso_resistance(tenso_resistance);
}

const double channel::getTensoResistance(bool from_server)
{
	if (from_server)
	{
		if (zsp)
			tenso_resistance = zsp_channel_ ? zsp_channel_->get_tenso_resistance() : 0.;
		else if (float(0xffff) == tenso_resistance && nullptr != server_)
			tenso_resistance = server_->TensoParameter(server_id, 1);
	}

	return tenso_resistance;
}

void channel::setTensoCoeff(const double& tenso_coeff_)
{
	tenso_coeff = tenso_coeff_;
}

const double channel::getTensoCoeff(bool from_server)
{
	if (float(0xffff) == tenso_coeff && from_server && nullptr != server_)
		tenso_coeff = server_->TensoParameter(server_id, 2);

	return tenso_coeff;
}

void channel::setTensoYoungMod(const double& tenso_young_mod_)
{
	tenso_young_mod = tenso_young_mod_;
}

const double channel::getTensoYoungMod(bool from_server)
{
	if (float(0xffff) == tenso_young_mod && from_server && nullptr != server_)
		tenso_young_mod = server_->TensoParameter(server_id, 3);

	return tenso_young_mod;
}

void channel::setTensoIcp(const long& tensoIcp_)
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveICP())
		return;

	if (KDUVN == parent_->getType())
		return;

	if (tensoIcp_ != 0)
		server_->SetCharge(parent_->getModule(), device_id, 0);

	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		unsigned long filter(0);
		if (tensoIcp_)
			filter = 0x4;

		ZSetChannelStatusFilter(parent_->getType(), parent_->getDSP(), filter);
		ZClose(parent_->getType(), parent_->getDSP());
	}
	if (tensoIcp_ != 0)
		server_->SetICP(parent_->getModule(), device_id, tensoIcp_);
	tenso_icp = (uint16_t)tensoIcp_;
}

const long channel::getTensoIcp(bool from_server)
{
	if (from_server)
	{
		if (zsp)
			tenso_icp = 0;
		else if (float(0xffff) == tenso_icp && from_server && nullptr != server_)
			tenso_icp = (uint16_t)server_->TensoParameter(server_id, 0);
	}

	return tenso_icp;
}

void channel::setReference( const double& reference_ )
{
	reference = reference_;
	if (zsp_channel_)
		zsp_channel_->set_reference(reference);
}

const double channel::Reference( bool from_server )
{
	if (float(0xffff) == reference && from_server && nullptr != server_)
		reference = server_->Reference( server_id );

	if (reference >= 0)
		return reference;

	return float(0);
}

void channel::setShift( const double& shift_ )
{
	shift = shift_;
	if (zsp_channel_)
		zsp_channel_->set_shift(shift);
}

const double channel::Shift( bool from_server )
{
	if (from_server)
	{
		if (zsp)
			shift = zsp_channel_ ? zsp_channel_->get_shift() : 0.;
		else if (float(0xffff) == shift && nullptr != server_)
			shift = server_->ShifLevel(server_id);
	}

	return shift;
}

void channel::setHpf(const uint16_t& hpf_)
{
	hpf = hpf_;
	
	if (zsp_channel_)
	{
		zsp_channel_->set_ac(hpf);
		return;
	}

	if (nullptr == parent_)
		return;

	long mode(0);
	unsigned long flags(0);
	if (0 == ZGetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, &mode, &flags))
	{
		if (!hpf && adcInputMode == 2)
			adcInputMode = 0;
		ZSetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, hpf ? 2 : adcInputMode, flags);
	}
}

const uint16_t& channel::HPF(bool from_server)
{
	if (from_server)
	{
		if (zsp)
			hpf = zsp_channel_ ? zsp_channel_->get_ac() : false;
		else if (0xffff == hpf && nullptr != server_)
			hpf = server_->Hpf(server_id);
	}

	return hpf;
}

const long channel::Preamplifier()
{
	if (getICP() == 1 && !getTensoIcp())
		return 2;

	return 0;
}

const float channel::getLevel()
{
	float result(0);

	if (nullptr != server_)
		result = server_->CurLevel( server_id );

	if (result >= 0)
		return result;

	return float(0);
}

const double channel::getMaxLevel()
{
	double result(0);

	float sSense(0.f);
	if (zsp)
		sSense = static_cast<float>(zsp_channel_ ? zsp_channel_->get_sense() : 0.001);
	else if (nullptr != server_)
		sSense = server_->Sense(server_id);
	if (sSense < 1.0e-8f)
		sSense = 1.0e-8f;
	auto sense = Sense();
	if (sense < 1.0e-8)
		sense = 1.0e-8;

	float sAmplify(0.);
	if (zsp)
		sAmplify = static_cast<float>(zsp_channel_ ? zsp_channel_->get_ext_amplify() : 1.);
	else
		sAmplify = server_->GetAmplify(server_id);

	if (nullptr != server_)
		result = server_->MaxLevel(server_id) *
			(sSense / sense) *
			(sAmplify / Amplify());
	
	return align_value(result);
}

const double channel::getMaxLevelDAC()
{
	if (nullptr == parent_)
		return 0.;

	return parent_->getMaxLevelDAC();
}

const double channel::getRange()
{
	double result(0);

	if (nullptr != server_)
		result = 20. * log10(server_->MaxLevel(server_id) / server_->Reference(server_id));

	return fabs(align_value(result));
}

void channel::setAmplify( const double& amplify_ )
{
	if (amplify_ != 0.)
		amplify = amplify_;
	else
		amplify = 1.;

	if (zsp_channel_)
		zsp_channel_->set_ext_amplify(amplify);
}

const double channel::Amplify( bool from_server )
{
	if (from_server)
	{
		if (zsp)
			amplify = zsp_channel_ ? zsp_channel_->get_ext_amplify() : 1.;
		else if (float(0xffff) == amplify && nullptr != server_)
			amplify = server_->GetAmplify(server_id);
	}

	return amplify;
}

void zetlab::std::channel::setAmplifyGain( const double &amplify_)  // установка КУ на все каналы
{
	double val = 2.;
	if (zsp_channel_)
	{
		if (zsp_channel_->set_amplify(amplify_))
		{
			zsp_channel_->save();
		}
	}

	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		ZSetAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, amplify_, &val);
		//ZClose(parent_->getType(), parent_->getDSP());
	}
}

double zetlab::std::channel::getAmplifyzsp()
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_amplify() : 1.;

}

const double zetlab::std::channel::getAmplify()
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_amplify() : 1.;
	
	if (nullptr == parent_)
		return 1.;

	double val(1.);
	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		if (0 == ZGetAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, &val))
		{
			if (val >= 8.0 && server_->PrusEna(server_id) == 1)
				ZGetPreAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, &val);				
		}
		
		ZClose(parent_->getType(), parent_->getDSP());
	}

	if (val < 5)
		val = (double)(int)(round(val) + 0.5);
	else if (val < 50)
		val = (double)(int)((round(val / 10.) * 10) + 0.5);
	else if (val < 500)
		val = (double)(int)((round(val / 100.) * 100) + 0.5);
	else
		val = (double)(int)((round(val / 1000.) * 1000) + 0.5);

	return val;
}

void channel::setNextRange(bool save)
{
	if (zsp_channel_)
	{
		auto amplifies = zsp_channel_->get_amplifies();
		auto amplify = zsp_channel_->get_amplify();
		if (auto it = ::std::find(amplifies.begin(), amplifies.end(), amplify); it != amplifies.end())
		{
			if (++it != amplifies.end())
			{
				if (zsp_channel_->set_amplify(*it))
				{
					if (save)
						zsp_channel_->save();
				}
			}
		}

		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!ZOpen( parent_->getType(), parent_->getDSP() ))
	{
		double amplify_t(0.0);

		ZGetAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, &amplify_t );
		if (amplify_t >= 8.0 && server_->PrusEna( server_id ) == 1)
		{
			double preamplify = 0.0;

			ZGetPreAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, &preamplify );
			if (preamplify < 32.0)
				ZSetNextPreAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, 1, &preamplify );
		}
		else
			ZSetNextAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, 1, &amplify_t );
		
		ZClose( parent_->getType(), parent_->getDSP() );
	}
}

void channel::setNextRange(const long& mask)
{
	if (zsp_channel_)
	{
		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		ADC_INFO adc_info = {};
		if (!AdcGetInfo(parent_->getType(), parent_->getDSP(), &adc_info))
		{
			auto kod = adc_info.KodAmplify[device_id] + 1;
			for (int i = 0; i < (int)(adc_info.QuantityChannelADC - adc_info.QuantityChannelVirtual); ++i)
			{
				auto channel = 1 << i;
				if (mask & channel)
					adc_info.KodAmplify[i] = kod;
			}

			AdcPutInfo(parent_->getType(), parent_->getDSP(), &adc_info);
		}

		ZClose(parent_->getType(), parent_->getDSP());
	}
}

void channel::setPrevRange(bool save)
{
	if (zsp_channel_)
	{
		auto amplifies = zsp_channel_->get_amplifies();
		auto amplify = zsp_channel_->get_amplify();
		if (auto it = ::std::find(amplifies.begin(), amplifies.end(), amplify); it != amplifies.end())
		{
			if (it != amplifies.begin())
			{
				if (zsp_channel_->set_amplify(*(--it)))
				{
					if (save)
						zsp_channel_->save();
				}
			}
		}
		return;
	}
	
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!ZOpen( parent_->getType(), parent_->getDSP() ))
	{
		double amplify_s = 0.0;

		ZGetAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, &amplify_s );
		if (amplify_s >= 8.0 && server_->PrusEna( server_id ) == 1)
		{
			double preamplify = 0.0;

			ZGetPreAmplifyADC( parent_->getType(), parent_->getDSP(), device_id, &preamplify );
			if (preamplify > 1.0)
				ZSetNextPreAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, -1, &preamplify);
			else
				ZSetNextAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, -1, &amplify_s);
		}
		else
			ZSetNextAmplifyADC(parent_->getType(), parent_->getDSP(), device_id, -1, &amplify_s);

		ZClose( parent_->getType(), parent_->getDSP() );
	}
}

void channel::setPrevRange(const long& mask)
{
	if (zsp_channel_)
	{
		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		ADC_INFO adc_info = {};
		if (!AdcGetInfo(parent_->getType(), parent_->getDSP(), &adc_info))
		{
			auto kod = adc_info.KodAmplify[device_id];
			if (kod > 0)
				--kod;
			for (int i = 0; i < (int)(adc_info.QuantityChannelADC - adc_info.QuantityChannelVirtual); ++i)
			{
				auto channel = 1 << i;
				if (mask & channel)
					adc_info.KodAmplify[i] = kod;
			}

			AdcPutInfo(parent_->getType(), parent_->getDSP(), &adc_info);
		}

		ZClose(parent_->getType(), parent_->getDSP());
	}
}

void channel::setDiff( bool diff, bool save)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_diff_mode())
			return;

		if (zsp_channel_->set_diff_mode(diff))
		{
			if (save)
				zsp_channel_->save();
		}

		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isSupportDiff())
		return;

	server_->SetSinDiffChannel( parent_->getModule(), device_id, long(diff) );

	//m_group = false;
}

void channel::setDiff(const long& mask, bool diff)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_diff_mode())
			return;

		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!ZOpen(parent_->getType(), parent_->getDSP()))
	{
		ADC_INFO adc_info = {};

		if (!AdcGetInfo(parent_->getType(), parent_->getDSP(), &adc_info))
		{
		
			if (diff)
			{
				adc_info.ChannelDiff = mask;
			}
			else
			{
				adc_info.ChannelDiff = 0;
			}

			AdcPutInfo(parent_->getType(), parent_->getDSP(), &adc_info);
		}

		ZClose(parent_->getType(), parent_->getDSP());
	}
}

const long channel::getDiff() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_diff_mode() : 0;

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return 0;

	if (!parent_->isSupportDiff())
		return 0;

	long result(server_->SetSinDiffChannel( parent_->getModule(), device_id, -1 ));

	if (result == 0 || result == 1)
		return result;

	return 0;
}


/// /////////////////////////////////////////////////

void channel::setICPMoreZsp( bool OnOff)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_icp())
			return;

		zsp_channel_->set_icp(OnOff);
		

		return;
	}
}

////////////////////////

void channel::setICP( const long& icp_, bool save)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;
		
		if (!zsp_channel_->has_icp())
			return;

		if (zsp_channel_->set_icp(icp_))
		{
			if (save)
				zsp_channel_->save();
		}

		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveICP())
		return;

	if (KDUVN == parent_->getType())
		return;

	if (icp_ != 0)
		server_->SetCharge(parent_->getModule(), device_id, 0);
	server_->SetICP( parent_->getModule(), device_id, icp_ );
}

void channel::setICP(const long& icp_, const long& mask_)
{
	
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_icp())
			return;

		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveICP())
		return;

	if (KDUVN == parent_->getType())
		return;

	if (icp_ != 0)
		server_->SetCharge(parent_->getModule(), device_id, 0);
	server_->SetICPMask(parent_->getModule(), mask_, icp_);
}
////////////////////////////////////////////////// 

void channel::setICPZsp(const long& icp_)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_icp())
			return;

		zsp_channel_->set_icp(icp_);

	}
}

void channel::saveICPZsp()
{
	zsp_channel_->save();

}


void channel::setICPMoreChannel(const long& modul, const long& mask, const long& onOff) // для различных уст-тв на каналы ICP
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_icp())
			return;

		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveICP())
		return;

	if (KDUVN == parent_->getType())
		return;

	//if (icp_ != 0)
	//	server_->SetCharge(parent_->getModule(), device_id, 0);
	server_->SetICPMask(modul, mask, onOff);  // parent_->getModule()
}

void channel::setMoreChannel(const long& modul, const long& mask_ , const long& onOffChannel)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_icp())
			return;

		zsp_channel_->save();
		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveICP())
		return;

	if (KDUVN == parent_->getType())
		return;

	//if (onOffChannel != 0)
		//server_->OnOffMask(parent_->getModule(), device_id, 0);
	server_->OnOffMask(modul, mask_, onOffChannel); //modul  вместо parent_->getModule()
}

/// ///////////////////////////////////////////////////////////////////////


const long channel::getICP() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_icp() : 0;

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return 0;

	if (!parent_->isHaveICP())
		return 0;

	long result(server_->GetICP( parent_->getModule(), device_id ));

	if (result >= 0)
		return result;

	return 0;
}

const bool channel::isHaveICP() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->has_icp() : false;

	if (nullptr == parent_)
		return false;

	return parent_->isHaveICP();
}

const bool channel::isHaveTensoICP() const
{
	if (nullptr == parent_)
		return false;

	return parent_->isHaveTensoICP();
}

const bool channel::isTensostation() const
{
	if (nullptr == parent_)
		return false;

	return parent_->isTensostation();
}

const bool channel::isHaveTenso(long& new_bridge_mode, long* __pot1, long* __pot2)
{
	if (parent_ == nullptr)
		return false;
	long pot1;
	long pot2;
	bool ret = parent_->isHaveTenso(new_bridge_mode, pot1, pot2, this->device_id);
	if (__pot1 != NULL)
		*__pot1 = pot1;
	if (__pot2 != NULL)
		*__pot2 = pot2;
	return ret;
}

void channel::setTenso(bool group, const long& tenso_mode_, long Pot1, long Pot2)
{
	if (parent_ == nullptr)
		return;

	long long_tens_mode = (int)tenso_mode_;
	if (group)
	{
		parent_->setTenso(long_tens_mode, this->device_id, Pot1, Pot2, group);

	}
	else
	{
		parent_->setTenso(long_tens_mode, this->device_id, Pot1, Pot2);

	}

	if (long_tens_mode == 0 || long_tens_mode == 1 || long_tens_mode == 2 || long_tens_mode == 3)
		tenso_mode = tenso_mode_;
	if (Pot1 >= 0 && Pot2 >= 0)
	{
		PotsChangeFlag = true;
		Pot1_val = Pot2;
		Pot2_val = Pot1;
	}
}
void channel::setTensoMode(const long& tenso_mode_)
{
	long long_tens_mode = (int)tenso_mode_;
	tenso_mode = tenso_mode_;
}
void channel::saveTenso() {
	parent_->saveTenso();
	return;
}

const long channel::Tenso(bool from_server)
{
	//if (float(0xffff) == sense && from_server && nullptr != server_)
		//sense = server_->Sense(server_id);
	return tenso_mode;
}

long channel::Pot1()
{
	return Pot1_val;
}

long channel::Pot2()
{
	return Pot2_val;
}

void channel::setCharge(const long& charge_)
{
	if (zsp)
	{
		if (!zsp_channel_)
			return;

		if (!zsp_channel_->has_charge())
			return;

		if (zsp_channel_->set_charge(charge_))
			zsp_channel_->save();

		return;
	}

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return;

	if (!parent_->isHaveCharge())
		return;

	if (KDUVN == parent_->getType())
		return;

	if (charge_ != 0)
		server_->SetICP(parent_->getModule(), device_id, 0);
	server_->SetCharge(parent_->getModule(), device_id, charge_);
}

const long channel::getCharge() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->get_charge() : 0;

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return 0;

	if (!parent_->isHaveCharge())
		return 0;

	long result(server_->GetCharge(parent_->getModule(), device_id));

	if (result >= 0)
		return result;

	return 0;
}

void channel::setInputResistance(const uint16_t& inputResistance_)
{
	inputResistance = inputResistance_;

	if (nullptr == parent_)
		return;

	long mode(0);
	unsigned long flags(0);
	if (0 == ZGetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, &mode, &flags))
		ZSetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, mode, inputResistance);
}

const uint16_t channel::getInputResistance() const
{
	if (nullptr == parent_)
		return 0;

	long mode(0);
	unsigned long flags(0);
	if (0 == ZGetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, &mode, &flags))
		return flags & 0x1;

	return 0;
}

const long zetlab::std::channel::InputResistance(bool from_server /*= false*/)
{
	inputResistance = getInputResistance();

	return inputResistance;
}

void channel::setAdcInputMode(const uint16_t& adcInputMode_)
{
	adcInputMode = adcInputMode_;

	if (nullptr == parent_)
		return;

	long mode(0);
	unsigned long flags(0);
	if (0 == ZGetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, &mode, &flags))
		ZSetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, hpf ? 2 : adcInputMode, flags);
}

const uint16_t channel::getAdcInputMode() const
{
	if (zsp)
		return 0;

	if (nullptr == parent_)
		return 0;

	long mode(0);
	unsigned long flags(0);
	if (0 == ZGetInputModeADC(parent_->getType(), parent_->getDSP(), device_id, &mode, &flags))
		return (uint16_t)mode;

	return 0;
}

const long zetlab::std::channel::AdcInputMode(bool from_server /*= false*/)
{
	adcInputMode = getAdcInputMode();

	return adcInputMode;
}

const bool channel::isHaveCharge() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->has_charge() : false;

	if (nullptr == parent_)
		return false;

	return parent_->isHaveCharge();
}

const bool channel::isHaveTEDS() const
{
	if (nullptr == parent_)
		return false;

	return parent_->isHaveTEDS();
}

const bool channel::isSupportDiff() const
{
	if (zsp)
		return zsp_channel_ ? zsp_channel_->has_diff_mode() : false;

	if (nullptr == parent_)
		return false;

	return parent_->isSupportDiff();
}

const bool zetlab::std::channel::isSupportInputModeAdc() const
{
	if (nullptr == parent_)
		return false;

	auto type = parent_->getType();
	auto numDsp = parent_->getDSP();

	bool result(false);
	if (!ZOpen(type, numDsp))
	{
		long mode(0);
		unsigned long flags(0);
		result = (0 == ZGetInputModeADC(type, numDsp, device_id, &mode, &flags));
	
		ZClose(type, numDsp);
	}

	return result;
}

const bool channel::isSupportAmplify() const
{
	if (nullptr == parent_)
		return false;

	return parent_->isSupportAmplify();
}

const bool channel::isSupportSensorStruct() const
{
	if (nullptr == parent_)
		return false;

	return parent_->isSupportSensorStruct();
}

const bool channel::isFeedBackChannel() const
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()))
		return false;
	return (server_->IsFeedBackChan(server_id) == 1 || virt);
}

const bool channel::isHaveBuiltinGenerator() const
{
	if (nullptr == parent_)
		return false;

	bool retVal = false;
	
	if (parent_->isHaveBuiltinGenerator() && isFeedBackChannel())
	{
		long err(0);
		err = ZOpen(parent_->getType(), parent_->getDSP());
		if (err == 0)
		{
			long Enable(0);
			if (ZGetStartDAC(parent_->getType(), parent_->getDSP(), &Enable) == 0)
				retVal = Enable == 0;
			ZClose(parent_->getType(), parent_->getDSP());
		}
	}

	return retVal;	
}

const bool channel::GetBuiltinGeneratorEnabled()
{
	if (parent_ != nullptr)
		return parent_->GetBuiltinGeneratorEnabled();
	else
		return 0;
}

const bool channel::GetExternalGeneratorEnabled()
{
	if (parent_ != nullptr)
		return parent_->GetExternalGeneratorEnabled();
	else
		return 0;
}

const long channel::GetGenIndex()
{
	if (parent_ != nullptr)
		return parent_->GetIndexGenerator();

	return -1;
}

const bool channel::SetBuiltinGeneratorEnabled(long Enable)
{
	if (parent_ != nullptr)
		return parent_->SetBuiltinGeneratorEnabled(Enable);
	else
		return 0;
}

const bool channel::GetBuiltinGeneratorSettings(long &Enable, double &freq, double &level, double &offset)
{
	if (parent_ != nullptr)
		return parent_->GetBuiltinGeneratorSettings(Enable, freq, level, offset);
	else
		return 0;
}

const bool channel::SetBuiltinGeneratorSettings(long Enable, double freq, double level, double offset)
{
	if (parent_ != nullptr)
		return parent_->SetBuiltinGeneratorSettings(Enable, freq, level, offset);
	else
		return 0;
}

const bool channel::Get1PPSBuiltinGeneratorSettings(long &Enable)
{
	if (parent_ != nullptr)
		return parent_->Get1PPSBuiltinGeneratorSettings(Enable);
	else
		return 0;
}

const bool channel::Set1PPSBuiltinGeneratorSettings(long Enable)
{
	if (parent_ != nullptr)
		return parent_->Set1PPSBuiltinGeneratorSettings(Enable);
	else
		return 0;
}

const bool channel::IsBuiltinGenChanFree()
{
	// если канал включен без внутреннего генератора, значит он уже задействован какой-то программой
	return isFeedBackChannel() && isEnabled() && !GetBuiltinGeneratorEnabled();
}

void channel::setVirt(const bool& virt_)
{
	virt = virt_;
}

const bool channel::isVirt() const
{
	return virt;
}

void channel::set_zsp(const bool& zsp_)
{
	zsp = zsp_;
}

const bool channel::is_zsp() const
{
	return zsp;
}

const bool channel::isDemo() const
{
	if (nullptr == parent_)
		return false;

	return parent_->getConnectionType() == zetlab::std::demo;
}

const bool channel::isHaveBuiltinGeneratorWithCheck() const
{
	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_)
		return 0;

	bool retVal = false;
	
	if (parent_->isHaveBuiltinGenerator() && isFeedBackChannel())
	{
		long err(0);
		err = ZOpen(parent_->getType(), parent_->getDSP());
		if (err == 0)
		{
			long Enable(0);
			if (ZGetStartDAC(parent_->getType(), parent_->getDSP(), &Enable) == 0)
				retVal = Enable == 0;
			ZClose(parent_->getType(), parent_->getDSP());
		}
	}


	return retVal;
}

const bool channel::GetVersionDeviceNewVersion() const
{
	if (zsp)
		return true;

	if (nullptr == server_ || FALSE == IsWindow(server_->GetSafeHwnd()) || nullptr == parent_) 
		return false;

	if (parent_->getType() == 20)
		return true;

	if (parent_->getType() != 14)
		return false;

	long retvers(0);
	long version(0);
	long ret = server_->GetDeviceLong(parent_->getModule(), ZADC_LONG_VERSION, &version);
	if (ret != 0)
		return false;
	
	if (version != 0)
		retvers = version >> 16;
	
	if (retvers == 7)
		return true;
	
	return false;
}
}} // namespace zetlab::std
