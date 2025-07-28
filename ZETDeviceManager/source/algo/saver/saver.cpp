#include "stdafx.h"
#include "../../application/application.h"

#include "saver.hpp"

#include "../utils/zsp_dev_detect/zsp_dev_detect.hpp"

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/lexical_cast.hpp>

#include <ZetTools/include/ZetTools.h>
#include <Dialog_ZET/Translate.h>
//  Добавляет или обновляет атрибут XML-узла:
#define append_child_attr( _xml_node_, _attr_, _value_ ) \
    if (_xml_node_.attribute( L#_attr_ )) \
        _xml_node_.attribute( L#_attr_ ).set_value( _value_ ); \
    else \
        _xml_node_.append_attribute( L#_attr_ ).set_value( _value_ );
//Сравнивает значение атрибута XML-узла с заданным значением:   true/false через параметр _result_.
#define compare_child_attr( _xml_node_, _attr_, _value_, _type_, _result_ ) \
    if (_xml_node_.attribute( L#_attr_ )) \
	{ \
		try \
		{ \
			if (boost::lexical_cast<_type_>(_xml_node_.attribute( L#_attr_ ).value()) != _value_) \
				_result_ = false; \
			else \
				_result_ = true; \
		} \
		catch (boost::bad_lexical_cast&) \
		{ \
			_result_ = false; \
		} \
	} \
	else \
		_result_ = false;
//Добавляет или обновляет текстовое значение дочернего XML-узла:
#define append_child_value( _xml_node_, _obj_, _accessor_, _child_, _brackets_ ) \
    if (_xml_node_.child( L#_child_ )) \
        _xml_node_.child( L#_child_ ).first_child().set_value( boost::lexical_cast< tstring >( _obj_ _accessor_ _child_ _brackets_ ).c_str() ); \
    else \
        _xml_node_.append_child( L#_child_ ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( _obj_ _accessor_ _child_ _brackets_ ).c_str() );
//Сравнивает значение дочернего XML-узла с заданным значением:  налогично compare_child_attr, но для текстовых значений узлов.
#define compare_child_value( _xml_node_, _obj_, _accessor_, _child_, _brackets_, _type_, _result_ ) \
	if (_xml_node_.child( L#_child_ )) \
	{ \
		try \
		{ \
			if (boost::lexical_cast<_type_>(_xml_node_.child( L#_child_ ).first_child().value()) != _obj_ _accessor_ _child_ _brackets_) \
				_result_ = false; \
			else \
				_result_ = true; \
		} \
		catch (boost::bad_lexical_cast&) \
		{ \
			_result_ = false; \
		} \
	} \
	else \
		_result_ = false;
// Форматирует массив значений в строку (через запятую) и сохраняет в XML-узел.
#define append_child_values( _xml_node_, _obj_, _child_, len ); \
    { tstring value = boost::lexical_cast< tstring >( _obj_._child_[0] ); \
    for (int i = 1; i < len; ++i) value += _T(",") + boost::lexical_cast< tstring >( _obj_._child_[i] ); \
    if (_xml_node_.child( L#_child_ )) \
        _xml_node_.child( L#_child_ ).first_child().set_value( value.c_str() ); \
    else \
        _xml_node_.append_child( L#_child_ ).append_child( pugi::node_pcdata ).set_value( value.c_str() ); }
// Сравнивает массив значений из XML-узла с массивом в объекте.
#define compare_child_values( _xml_node_, _obj_, _child_, len, _type_, _result_ ) \
	{ _type_ value = boost::lexical_cast< _type_ >( _obj_._child_[0] ); \
    for (int i = 1; i < len; ++i) value += _T(",") + boost::lexical_cast< _type_ >( _obj_._child_[i] ); \
	if (_xml_node_.child( L#_child_ )) \
	{ \
		try \
		{ \
			if (boost::lexical_cast<_type_>(_xml_node_.child( L#_child_ ).first_child().value()) != value) \
				_result_ = false; \
			else \
				_result_ = true; \
		} \
		catch (boost::bad_lexical_cast&) \
		{ \
			_result_ = false; \
		} \
	} \
	else \
		_result_ = false; }
// Копирует значение из одного XML-узла в другой.
#define copy_child_value( _xml_node_from_, _xml_node_to_, _child_ ) \
	if (_xml_node_from_.child( L#_child_ )) \
	{ \
	    if (_xml_node_to_.child( L#_child_ )) \
			_xml_node_to_.child( L#_child_ ).first_child().set_value( _xml_node_from_.child( L#_child_ ).first_child().value() ); \
		else \
			_xml_node_to_.append_child( L#_child_ ).append_child( pugi::node_pcdata ).set_value( _xml_node_from_.child( L#_child_ ).first_child().value() ); \
	} 
namespace zetlab {

saver::saver( const factory_ptr& factory_ptr_ ) : 
    factory_(factory_ptr_)
{
}

void saver::save( const tstring& file_name )  ////Сохраняет конфигурацию всех устройств в XML-файл.
{
    using namespace pugi;

    xml_document xml;
    xml_node	 root;

    // Load file
    if (!xml.load_file( file_name.c_str(), parse_default, encoding_utf8 ))
    {
        xml.load( _T("") );
        xml.append_child( _T("Config") );
    }
    root = xml.first_child();

    // Check version
    if (root.attribute( _T("version") ).as_double() < CONFIG_VERSION)
    {
        xml.remove_child( root );
        root = xml.append_child( _T("Config") );
    }

    // Config version
    if (!root.attribute( _T("version") ))
        root.append_attribute( _T("version") );
    root.attribute( _T("version") ).set_value( CONFIG_VERSION );

    ::std::vector< object_ptr > v_objects;
    factory_->getRoot()->browse( v_objects, object_type::device );

	bool bChanged(false);
    boost::range::for_each( v_objects, [&]( object_ptr& device_ ) {
		if (isDeviceChanged(device_.get(), root))  //Проверяет, изменилось ли оно (isDeviceChanged). Если да — сохраняет его (saveDevice).
		{
			saveDevice(device_.get(), root);
			bChanged = true;
		}
    });

	// Save changes
	if (bChanged)
		xml.save_file( file_name.c_str(), _T("    "), format_default, encoding_utf8 );
}

void saver::backup(object* const object_, const tstring& file_name, tstring sConfigName)   // Создает резервную копию конфигурации конкретного устройства.
{  // обавляет метку времени (configTime) и имя конфига (configName).
	using namespace pugi;

	xml_document xml;
	xml_node	 root;

	// Load file
	if (!xml.load_file(file_name.c_str(), parse_default, encoding_utf8))
	{
		xml.load(_T(""));
		xml.append_child(_T("Config"));
	}
	root = xml.first_child();

	// Check version
	if (root.attribute(_T("version")).as_double() < CONFIG_VERSION)
	{
		xml.remove_child(root);
		root = xml.append_child(_T("Config"));
	}

	// Config version
	if (!root.attribute(_T("version")))
		root.append_attribute(_T("version"));
	root.attribute(_T("version")).set_value(CONFIG_VERSION);
	saveDevice(object_, root, sConfigName);

	// Save changes
	xml.save_file(file_name.c_str(), _T("    "), format_default, encoding_utf8);
}

void saver::restore(  // Восстанавливает конфигурацию устройства из резервной копии. Копирует данные в основной конфиг (copyDevice).
	object* const object_, 
	const tstring& file_name,
	const tstring& file_name_bak,
	tstring sConfigName)
{
	std::device* device_ = object_->getAsDevice();
	if (nullptr == device_)
		return;

	using namespace pugi;

	xml_document xmlBak;
	if (!xmlBak.load_file(file_name_bak.c_str(), parse_default, encoding_utf8))
		return;

	xml_node rootBak = xmlBak.first_child();

	pugi::xml_node nodeBak;
	for (nodeBak = rootBak.child(_T("Device")); nodeBak; nodeBak = nodeBak.next_sibling(_T("Device")))
	{
		if (nodeBak.attribute(_T("serial")) &&
			device_->getSerial() == nodeBak.attribute(_T("serial")).as_int() &&
			nodeBak.attribute(_T("type")) &&
			device_->getType() == nodeBak.attribute(_T("type")).as_int() &&
			nodeBak.attribute(_T("configName")) &&
			sConfigName == tstring(nodeBak.attribute(_T("configName")).value()))
			break;
	}

	if (nullptr == nodeBak)
		return;

	xml_document xml;

	if (!xml.load_file(file_name.c_str(), parse_default, encoding_utf8))
	{
		xml.load(_T(""));
		xml.append_child(_T("Config"));
	}
	xml_node root = xml.first_child();

	// Check version
	if (root.attribute(_T("version")).as_double() < CONFIG_VERSION)
	{
		xml.remove_child(root);
		root = xml.append_child(_T("Config"));
	}

	// Config version
	if (!root.attribute(_T("version")))
		root.append_attribute(_T("version"));
	root.attribute(_T("version")).set_value(CONFIG_VERSION);

	pugi::xml_node node;
	for (node = root.child(_T("Device")); node; node = node.next_sibling(_T("Device")))
	{
		if (node.attribute(_T("serial")) &&
			device_->getSerial() == node.attribute(_T("serial")).as_int() &&
			node.attribute(_T("type")) &&
			device_->getType() == node.attribute(_T("type")).as_int())
			break;
	}
	if (!node)
		return;

	copyDevice(nodeBak, node);

	xml.save_file(file_name.c_str(), _T("    "), format_default, encoding_utf8);
}

void saver::get(   // Возвращает список сохраненных конфигураций для устройства.
	const tstring& fileName,
	object* const object_,
	::std::vector<::std::pair<tstring, CZetTime>>& cfg)
{
	std::device*   device_ = object_->getAsDevice();
	if (nullptr == device_)
		return;

	using namespace pugi;

	xml_document xml;
	xml_node	 root;

	if (!xml.load_file(fileName.c_str(), parse_default, encoding_utf8))
		return;

	root = xml.first_child();
	if (!root)
		return;

	for (pugi::xml_node node = root.child(_T("Device")); node; node = node.next_sibling(_T("Device")))
	{
		if (node.attribute(_T("serial")) &&
			device_->getSerial() == node.attribute(_T("serial")).as_int() &&
			node.attribute(_T("type")) &&
			device_->getType() == node.attribute(_T("type")).as_int() &&
			node.attribute(_T("configTime")) &&
			node.attribute(_T("configName")))
		{
			CZetTime t = ZetTimeMin;
			t.ConvertFromString(node.attribute(_T("configTime")).value(), L"dd.MM.yyyy HH:mm:ss", false);
			if (t == ZetTimeMin)
				continue;
			cfg.push_back(::std::make_pair(node.attribute(_T("configName")).value(), t));
		}
	}

	::std::sort(cfg.begin(), cfg.end(), [](::std::pair<tstring, CZetTime>& _1, ::std::pair<tstring, CZetTime>& _2)
	{
		return _1.second > _2.second;
	});
}

bool saver::deleteConfig(const tstring& file_name,   // Удаляет конфигурацию по имени (configName).
						 tstring sConfigName)
{
	using namespace pugi;
	xml_document xml;
	xml_node	 root;
	xml_node	 xmlDevice;
	// Load file
	if (!xml.load_file(file_name.c_str(), parse_default, encoding_utf8))
	{
		xml.load(_T(""));
		xml.append_child(_T("Config"));
	}
	root = xml.first_child();

	// Check version
	if (root.attribute(_T("version")).as_double() < CONFIG_VERSION)
	{
		xml.remove_child(root);
		root = xml.append_child(_T("Config"));
	}

	root = xml.child(L"Config");
	xmlDevice = root.child(L"Device");

	while (!xmlDevice.empty())
	{
		if (xmlDevice.attribute(_T("configName")).value() == sConfigName)
			break;
		else
			xmlDevice = xmlDevice.next_sibling();
	}
	if (!xmlDevice.empty())
	{
		root.remove_child(xmlDevice);
		xml.save_file(file_name.c_str(), _T("    "), format_default, encoding_utf8);
		return true;
	}
	return false;
}

bool saver::isDeviceChanged( object* const object_,   // Изменились ли параметры устройства / Макросы compare_child_attr и compare_child_value.
							 pugi::xml_node& parent_ )
{
	std::device*   device_ = object_->getAsDevice();
	pugi::xml_node node;

	for (node = parent_.child(_T("Device")); node; node = node.next_sibling(_T("Device")))
	{
		if (node.attribute(_T("serial")) &&
			device_->getSerial() == node.attribute(_T("serial")).as_int() &&
			node.attribute(_T("type")) &&
			device_->getType() == node.attribute(_T("type")).as_int())
			break;
	}

	if (!node)
		return true;

	bool result(false);
	
	compare_child_attr(node, name, object_->getName(), tstring, result);
	if (!result)
		return true;
	compare_child_attr(node, type, device_->getType(), long, result);
	if (!result)
		return true;
	compare_child_attr(node, serial, device_->getSerial(), long, result);
	if (!result)
		return true;
	tstring desc(L"");
	device_->readDescription(desc);
	compare_child_attr(node, label, desc, tstring, result);
	if (!result)
		return true;

	ADC_INFO adc_info = { 0 };
	double   ExtFreqDAC(0.);
	long     MasterSync(0);
	long	 BuiltinGenActive(0);
	long	 BuiltinGenSineActive(0);
	double	 BuiltinGenSineFreq(0.);
	double	 BuiltinGenSineAmpl(0.);
	double	 BuiltinGenSineBias(0.);
	unsigned long ChargeChannel;

	device_->rawAdcInfo(&adc_info, &ExtFreqDAC, &MasterSync, 
		&BuiltinGenActive, &BuiltinGenSineActive, &BuiltinGenSineFreq, &BuiltinGenSineAmpl, &BuiltinGenSineBias,
		&ChargeChannel);

	compare_child_value(node, adc_info, ., typeADC, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., Channel, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., ChannelDAC, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., HCPChannel, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., ModaADC, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., Rate, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., RateDAC, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., sizeInterrupt, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., sizeInterruptDAC, , unsigned long, result);
	if (!result)
		return true;
	compare_child_values(node, adc_info, KodAmplify, 16, tstring, result);
	if (!result)
		return true;
	compare_child_values(node, adc_info, PRUS, 8, tstring, result);
	if (!result)
		return true;
	compare_child_values(node, adc_info, Atten, 3, tstring, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., EnaExtFreq, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., EnaExtStart, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., ExtOporFreq, , double, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., ChannelDiff, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., DigitalInput, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., DigitalOutput, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., DigitalOutEnable, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., EnaExtFreqDAC, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., EnaExtStartDAC, , unsigned short, result);
	if (!result)
		return true;
	compare_child_value(node, , , ExtFreqDAC, , double, result);
	if (!result)
		return true;
	compare_child_value(node, , , MasterSync, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , BuiltinGenActive, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , BuiltinGenSineActive, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , BuiltinGenSineFreq, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , BuiltinGenSineAmpl, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , BuiltinGenSineBias, , long, result);
	if (!result)
		return true;
	compare_child_value(node, , , ChargeChannel, , unsigned long, result);
	if (!result)
		return true;
	compare_child_value(node, adc_info, ., Freq, , double, result);
	if (!result)
		return true;

	node = node.child(_T("Channels"));
	if (!node)
		return true;

	::std::vector< object_ptr > v_objects;
	object_->browse(v_objects, object_type::channel);
	
	for (auto& channel_ : v_objects)
	{
		if (isChannelChanged(channel_.get(), node))
			return true;
	}
	
	return false;
}

void saver::saveDevice( object* const object_,         //  Все параметры устройства (включая каналы) в XML.
                        pugi::xml_node& parent_,
						tstring sConfigName)
{
	std::device*   device_ = object_->getAsDevice();
	if (is_zsp(device_->getType()))
		return;

    pugi::xml_node node;

    for (node = parent_.child( _T("Device") ); node; node = node.next_sibling( _T("Device") ))
    {
		if (node.attribute(_T("serial")) &&
			device_->getSerial() == node.attribute(_T("serial")).as_int() &&
			node.attribute(_T("type")) &&
			device_->getType() == node.attribute(_T("type")).as_int())
		{
			if (node.attribute(_T("configName")))
			{
				if (sConfigName == tstring(node.attribute(_T("configName")).value()))
					break;
			}
			else
				break;
		}
    }

	ADC_INFO adc_info = { 0 };
    double   ExtFreqDAC(0.);
    long     MasterSync(0);
	long	 BuiltinGenActive(0);
	long	 BuiltinGenSineActive(0);
	double	 BuiltinGenSineFreq(0.);
	double	 BuiltinGenSineAmpl(0.);
	double	 BuiltinGenSineBias(0.);
	unsigned long ChargeChannel(0);
	device_->rawAdcInfo(&adc_info, &ExtFreqDAC, &MasterSync,
		&BuiltinGenActive, &BuiltinGenSineActive, &BuiltinGenSineFreq, &BuiltinGenSineAmpl, &BuiltinGenSineBias,
		&ChargeChannel);

    if (!node)
        node = parent_.append_child( _T("Device") );

	CZetTime t;
	t.SetCurrentTime();
	wchar_t sTime[20] = {0};
	t.ConvertToString(sTime, 20, 1., L"dd.MM.yyyy HH:mm:ss", false);

    append_child_attr  ( node, name,     object_->getName().c_str() );
    append_child_attr  ( node, type,     device_->getType() );
    append_child_attr  ( node, serial,   device_->getSerial() );
	tstring desc(L"");
	device_->readDescription(desc);
	append_child_attr  ( node, label,    desc.c_str());
	append_child_attr  ( node, configTime, sTime);
	append_child_attr  ( node, configName, sConfigName.c_str());
    append_child_value ( node, adc_info, ., typeADC, );
    append_child_value ( node, adc_info, ., Channel, );
    append_child_value ( node, adc_info, ., ChannelDAC, );
    append_child_value ( node, adc_info, ., HCPChannel, );
	append_child_value ( node, adc_info, ., ModaADC, );
    append_child_value ( node, adc_info, ., Rate, );
    append_child_value ( node, adc_info, ., RateDAC, );
    append_child_value ( node, adc_info, ., sizeInterrupt, );
    append_child_value ( node, adc_info, ., sizeInterruptDAC, );
    append_child_values( node, adc_info, KodAmplify, 16 );
    append_child_values( node, adc_info, PRUS, 8 );
    append_child_values( node, adc_info, Atten, 3 );
    append_child_value ( node, adc_info, ., EnaExtFreq, );
    append_child_value ( node, adc_info, ., EnaExtStart, );
    append_child_value ( node, adc_info, ., ExtOporFreq, );
    append_child_value ( node, adc_info, ., ChannelDiff, );
    append_child_value ( node, adc_info, ., DigitalInput, );
    append_child_value ( node, adc_info, ., DigitalOutput, );
    append_child_value ( node, adc_info, ., DigitalOutEnable, );
    append_child_value ( node, adc_info, ., EnaExtFreqDAC, );
    append_child_value ( node, adc_info, ., EnaExtStartDAC, );
    append_child_value ( node,         ,  , ExtFreqDAC, );
    append_child_value ( node,         ,  , MasterSync, );
	append_child_value(node, , , BuiltinGenActive, );
	append_child_value(node, , , BuiltinGenSineActive, );
	append_child_value(node, , , BuiltinGenSineFreq, );
	append_child_value(node, , , BuiltinGenSineAmpl, );
	append_child_value(node, , , BuiltinGenSineBias, );
	append_child_value(node, , , ChargeChannel, );
	append_child_value(node, adc_info, ., Freq, );
    if (node.child( _T("Channels") ))
        node = node.child( _T("Channels") );
    else
        node = node.append_child( _T("Channels") );

    ::std::vector< object_ptr > v_objects;
    object_->browse( v_objects, object_type::channel );

    boost::range::sort( v_objects, [&]( object_ptr& o1, object_ptr& o2 ) {
        return o1->getAsChannel()->getDeviceID() < o2->getAsChannel()->getDeviceID();
    });
    boost::range::for_each( v_objects, [&]( object_ptr& channel_ ) {
		tstring unit, unitSense;
		if(auto chan = channel_->getAsChannel(); chan && GetLanguageDst() != L"ru-RU")
		{
			unit = chan->getUnit();
			chan->setUnit(tstring(TranslateString(unit.c_str())));
			unitSense = chan->getUnitSense();
			chan->setUnitSense(tstring(TranslateString(unitSense.c_str())));
		}
        saveChannel( channel_.get(), node );
		if (auto chan = channel_->getAsChannel(); chan && GetLanguageDst() != L"ru-RU")
		{
			if (!unit.empty())
				chan->setUnit(unit);
			if (!unitSense.empty())
				chan->setUnitSense(unitSense);
		}
    });
}

void saver::copyDevice(           //  Данные из одного XML-узла (резерв) в другой (основной).
	pugi::xml_node& src_,
	pugi::xml_node& dst_)
{
	CZetTime t;
	t.SetCurrentTime();
	wchar_t sTime[20] = { 0 };
	t.ConvertToString(sTime, 20, 1., L"dd.MM.yyyy HH:mm:ss", false);

	append_child_attr(dst_, configTime, sTime);
	append_child_attr(dst_, configName, _T(""));
	copy_child_value(src_, dst_, typeADC);
	copy_child_value(src_, dst_, Channel);
	copy_child_value(src_, dst_, ChannelDAC);
	copy_child_value(src_, dst_, HCPChannel);
	copy_child_value(src_, dst_, ModaADC);
	copy_child_value(src_, dst_, Rate);
	copy_child_value(src_, dst_, RateDAC);
	copy_child_value(src_, dst_, sizeInterrupt);
	copy_child_value(src_, dst_, sizeInterruptDAC);
	copy_child_value(src_, dst_, KodAmplify);
	copy_child_value(src_, dst_, PRUS);
	copy_child_value(src_, dst_, Atten);
	copy_child_value(src_, dst_, EnaExtFreq);
	copy_child_value(src_, dst_, EnaExtStart);
	copy_child_value(src_, dst_, ExtOporFreq);
	copy_child_value(src_, dst_, ChannelDiff);
	copy_child_value(src_, dst_, DigitalInput);
	copy_child_value(src_, dst_, DigitalOutput);
	copy_child_value(src_, dst_, DigitalOutEnable);
	copy_child_value(src_, dst_, EnaExtFreqDAC);
	copy_child_value(src_, dst_, EnaExtStartDAC);
	copy_child_value(src_, dst_, ExtFreqDAC);
	copy_child_value(src_, dst_, MasterSync);
	copy_child_value(src_, dst_, BuiltinGenActive);
	copy_child_value(src_, dst_, BuiltinGenSineActive);
	copy_child_value(src_, dst_, BuiltinGenSineFreq);
	copy_child_value(src_, dst_, BuiltinGenSineAmpl);
	copy_child_value(src_, dst_, BuiltinGenSineBias);
	copy_child_value(src_, dst_, ChargeChannel);
	copy_child_value(src_, dst_, Freq);

	auto nodeDst = dst_.child(_T("Channels"));
	if (!nodeDst)
		return;

	auto nodeSrc = src_.child(_T("Channels"));
	if (!nodeSrc)
		return;

	nodeSrc = nodeSrc.child(_T("Channel"));
	while (nodeSrc)
	{
		auto node = nodeDst.find_child_by_attribute(_T("id"), nodeSrc.attribute(_T("id")).value());
		if (node)
			copyChannel(nodeSrc, node);
		nodeSrc = nodeSrc.next_sibling(_T("Channel"));
	}
}

bool saver::isChannelChanged( object* const object_,        // Изменились ли параметры канала (имя, координаты, чувствительность и т. д.).
							  pugi::xml_node& parent_ )
{
	std::channel*  channel_ = object_->getAsChannel();
	pugi::xml_node node(parent_.find_child_by_attribute(_T("id"), boost::lexical_cast<tstring>(channel_->getDeviceID()).c_str()));

	if (!node)
		return true;
	
	bool result(false);
	tstring enabled_ = channel_->isEnabled() ? _T("true") : _T("false");
	compare_child_attr(node, enabled, enabled_, tstring, result);
	if (!result)
		return true;

	if (!channel_->isEnabled())
		return false;

	compare_child_attr(node, id, channel_->getDeviceID(), long, result);
	if (!result)
		return true;
	compare_child_attr(node, name, channel_->getName(), tstring, result);
	if (!result)
		return true;

	compare_child_attr(node, comment, channel_->getComment(), tstring, result);
	if (!result)
		return true;

	compare_child_attr(node, m_coodX, channel_->CoordX(), double, result);
	if (!result)
		return true;

	compare_child_attr(node, m_coodY, channel_->CoordY(), double, result);
	if (!result)
		return true;

	compare_child_attr(node, m_coodZ, channel_->CoordZ(), double, result);
	if (!result)
		return true;

	compare_child_attr(node, m_coodP, channel_->CoordP(), long, result);
	if (!result)
		return true;

	compare_child_attr(node, units, channel_->getUnit(), tstring, result);
	if (!result)
		return true;

	compare_child_attr(node, unitsense, channel_->getUnitSense(), tstring, result);
	if (!result)
		return true;

	compare_child_value(node, channel_, ->, Sense, (), double, result);
	if (!result)
		return true;

	compare_child_value(node, channel_, ->, Amplify, (), double, result);
	if (!result)
		return true;
	compare_child_value(node, channel_, ->, Preamplifier, (), long, result);
	if (!result)
		return true;
	compare_child_value(node, channel_, ->, Reference, (), double, result);
	if (!result)
		return true;
	compare_child_value(node, channel_, ->, Shift, (), double, result);
	if (!result)
		return true;
	compare_child_value(node, channel_, ->, AFCH, (), tstring, result);
	if (!result)
		return true;
	compare_child_value(node, channel_, ->, HPF, (), uint16_t, result);
	if (!result)
		return true;

	pugi::xml_node node2(node.child(L"TensoIcp"));
	if (!node2)
		return true;

	compare_child_attr(node2, state, channel_->getTensoIcp(), long, result);
	if (!result)
		return true;

	compare_child_attr(node2, resistance, channel_->getTensoResistance(), double, result);
	if (!result)
		return true;

	compare_child_attr(node2, coeff, channel_->getTensoCoeff(), double, result);
	if (!result)
		return true;

	compare_child_attr(node2, young_mod, channel_->getTensoYoungMod(), double, result);
	if (!result)
		return true;
	
	compare_child_value(node, channel_, ->, Tenso, (), int, result);
	if (!result)
		return true;
	
	compare_child_value(node, channel_, ->, Pot1, (), int, result);
	if (!result)
		return true;
	
	compare_child_value(node, channel_, ->, Pot2, (), int, result);
	if (!result)
		return true;

	compare_child_value(node, channel_, ->, InputResistance, (), uint16_t, result);
	if (!result)
		return true;

	compare_child_value(node, channel_, ->, AdcInputMode, (), uint16_t, result);
	if (!result)
		return true;

	return false;
}

void saver::saveChannel( object* const object_,           // Параметры канала
                         pugi::xml_node& parent_ )
{
	std::channel*  channel_ = object_->getAsChannel();
    pugi::xml_node node(parent_.find_child_by_attribute( _T("id"), boost::lexical_cast< tstring >( channel_->getDeviceID() ).c_str() ));

    if (!node)
    {
        node = parent_.append_child( _T("Channel") );
    }
    else if (!channel_->isEnabled())
    {
        append_child_attr ( node, enabled,  channel_->isEnabled() );
        return;
    }

    append_child_attr ( node, id,       channel_->getDeviceID() );
    append_child_attr ( node, name,     channel_->getName().c_str() );
	append_child_attr ( node, comment,  channel_->getComment().c_str());

	append_child_value(node, channel_, ->, CoordX, ());
	append_child_value(node, channel_, ->, CoordY, ());
	append_child_value(node, channel_, ->, CoordZ, ());
	append_child_value(node, channel_, ->, CoordP, ());

    append_child_attr ( node, units,    channel_->getUnit().c_str() );
    append_child_attr ( node, enabled,  channel_->isEnabled() );
	append_child_attr(  node, unitsense, channel_->getUnitSense().c_str());
    append_child_value( node, channel_, ->, Sense, () );
    append_child_value( node, channel_, ->, Amplify, () );
    append_child_value( node, channel_, ->, Preamplifier, () );
    append_child_value( node, channel_, ->, Reference, () );
    append_child_value( node, channel_, ->, Shift, () );
    append_child_value( node, channel_, ->, AFCH, () );
	append_child_value( node, channel_, ->, HPF, ());

	pugi::xml_node node2(node.child(L"TensoIcp"));
	if (!node2)
		node2 = node.append_child(_T("TensoIcp"));
	
	append_child_attr(node2, state, channel_->getTensoIcp());
	append_child_attr(node2, resistance, channel_->getTensoResistance());
	append_child_attr(node2, coeff, channel_->getTensoCoeff());
	append_child_attr(node2, young_mod, channel_->getTensoYoungMod());

	if (channel_->TensoChangeFlag)
		append_child_value(node, channel_, ->, Tenso, ());

	if (channel_->PotsChangeFlag)
	{
		append_child_value(node, channel_, ->, Pot1, ());
		append_child_value(node, channel_, ->, Pot2, ());
	}

	append_child_value(node, channel_, ->, InputResistance, ());
	append_child_value(node, channel_, ->, AdcInputMode, ());
}

void saver::copyChannel(pugi::xml_node& src_, pugi::xml_node& dst_)    // Данные канала из резерва в основной конфиг.
{
	append_child_attr(dst_, name, src_.attribute(_T("name")).value());
	append_child_attr(dst_, comment, src_.attribute(_T("comment")).value());
	append_child_attr(dst_, units, src_.attribute(_T("units")).value());
	append_child_attr(dst_, enabled, src_.attribute(_T("enabled")).value());
	append_child_attr(dst_, unitsense, src_.attribute(_T("unitsense")).value());
	copy_child_value(src_, dst_, cX);
	copy_child_value(src_, dst_, cY);
	copy_child_value(src_, dst_, cZ);
	copy_child_value(src_, dst_, cP);
	copy_child_value(src_, dst_, Sense);
	copy_child_value(src_, dst_, Amplify);
	copy_child_value(src_, dst_, Preamplifier);
	copy_child_value(src_, dst_, Reference);
	copy_child_value(src_, dst_, Shift);
	copy_child_value(src_, dst_, AFCH);
	copy_child_value(src_, dst_, HPF);
	auto src2_ = src_.child(L"TensoIcp");
	if (src2_)
	{
		auto dst2_(dst_.child(L"TensoIcp"));
		if (!dst2_)
			dst2_ = dst_.append_child(_T("TensoIcp"));
		append_child_attr(dst2_, state, src2_.attribute(_T("state")).value());
		append_child_attr(dst2_, resistance, src2_.attribute(_T("resistance")).value());
		append_child_attr(dst2_, coeff, src2_.attribute(_T("coeff")).value());
		append_child_attr(dst2_, young_mod, src2_.attribute(_T("young_mod")).value());
	}
	copy_child_value(src_, dst_, InputResistance);
	copy_child_value(src_, dst_, AdcInputMode);
}

} // namespace zetlab
