//----------------------------------------------------------------------------------------------------------------------
#include "stream_data_worker.h"
#include <pugixml/auto_link.h>
//----------------------------------------------------------------------------------------------------------------------
using namespace pugi;
//----------------------------------------------------------------------------------------------------------------------
stream_data_worker::stream_data_worker(const std::wstring& cfg_path)
	: m_cfg_path(cfg_path)
{
}
//----------------------------------------------------------------------------------------------------------------------
stream_data_worker::~stream_data_worker()
{

}
//----------------------------------------------------------------------------------------------------------------------
bool stream_data_worker::is_stream_data_enable(const zetlab::object* object) const
{
	if (!object)
		return false;
		
	if (auto as_7xxx = object->getAs7xxx(); as_7xxx && as_7xxx->getChanNum() < 3)
	{
		if (auto device_type = as_7xxx->getDeviceType();
			device_type == ZET_139 || device_type == ZET_139A || device_type == ZET_139F || 
			device_type == ZET_139D || device_type == ZET_139B || device_type == ZET_139C)
		{
			auto parent = object->getParent();
			if (!parent)
				return false;
			if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
				return false;

			return true;
		}
	}

	return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool stream_data_worker::is_stream_data(const zetlab::object* object) const
{
	if (!object)
		return false;
	
	auto as_7xxx = object->getAs7xxx();
	if (!as_7xxx)
		return false;

	auto device_type = as_7xxx->getDeviceType();
	if (device_type != ZET_139 && device_type != ZET_139A && device_type != ZET_139F &&
		device_type != ZET_139D && device_type != ZET_139B && device_type != ZET_139C)
		return false;

	auto parent = object->getParent();
	if (!parent)
		return false;
	if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
		return false;

	xml_document stream_data_cfg;
	std::wstring path = m_cfg_path + L"ZET7xxx\\Configs\\stream_data.xml";
	if (!stream_data_cfg.load_file(path.c_str()))
		return false;

	xml_node devices_node = stream_data_cfg.child(L"devices");
	if (!devices_node)
		return false;

	auto serial = std::to_wstring(as_7xxx->getDeviceSerial());
	xml_node sensor_node = devices_node.find_child_by_attribute(L"sensor", L"serial", serial.c_str());
	if (!sensor_node)
		return false;

	auto channel = sensor_node.attribute(L"channel").as_int(-1);
	if (channel != as_7xxx->getChanNum())
		return false;

	return true;
}
bool stream_data_worker::is_measure_data_enable(const zetlab::object* object) const
{
	if (!object)
		return false;

	if (auto as_7xxx = object->getAs7xxx(); as_7xxx && as_7xxx->getChanNum() < 3)
	{
		if (auto device_type = as_7xxx->getDeviceType();
			device_type == ZET_139 || device_type == ZET_139A || device_type == ZET_139F ||
			device_type == ZET_139D || device_type == ZET_139B || device_type == ZET_139C)
		{
			auto parent = object->getParent();
			if (!parent)
				return false;
			if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
				return false;

			return true;
		}
	}

	return false;
}
bool stream_data_worker::is_measure_data(const zetlab::object* object) const
{
	if (!object)
		return false;

	auto as_7xxx = object->getAs7xxx();
	if (!as_7xxx)
		return false;

	auto device_type = as_7xxx->getDeviceType();
	if (device_type != ZET_139 && device_type != ZET_139A && device_type != ZET_139F &&
		device_type != ZET_139D && device_type != ZET_139B && device_type != ZET_139C)
		return false;

	auto parent = object->getParent();
	if (!parent)
		return false;
	if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
		return false;

	xml_document stream_data_cfg;
	std::wstring path = m_cfg_path + L"ZET7xxx\\Configs\\stream_data.xml";
	if (!stream_data_cfg.load_file(path.c_str()))
		return false;

	xml_node devices_node = stream_data_cfg.child(L"devices");
	if (!devices_node)
		return false;

	auto serial = std::to_wstring(as_7xxx->getDeviceSerial());
	xml_node sensor_node = devices_node.find_child_by_attribute(L"sensor", L"serial", serial.c_str());
	if (!sensor_node)
		return false;

	return sensor_node.attribute(L"measure_data").as_bool(false);
}
//----------------------------------------------------------------------------------------------------------------------
void stream_data_worker::change_stream_data_state(const zetlab::object* object)
{
	if (!object)
		return;

	auto as_7xxx = object->getAs7xxx();
	if (!as_7xxx)
		return;

	auto device_type = as_7xxx->getDeviceType();
	if (device_type != ZET_139 && device_type != ZET_139A && device_type != ZET_139F &&
		device_type != ZET_139D && device_type != ZET_139B && device_type != ZET_139C)
		return;

	auto parent = object->getParent();
	if (!parent)
		return;
	if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
		return;

	xml_document stream_data_cfg;
	std::wstring path = m_cfg_path + L"ZET7xxx\\Configs\\stream_data.xml";
	stream_data_cfg.load_file(path.c_str());

	xml_node devices_node = stream_data_cfg.child(L"devices");
	if (!devices_node)
		devices_node = stream_data_cfg.append_child(L"devices");

	auto serial = std::to_wstring(as_7xxx->getDeviceSerial());
	xml_node sensor_node = devices_node.find_child_by_attribute(L"sensor", L"serial", serial.c_str());
	if (!sensor_node)
	{
		sensor_node = devices_node.append_child(L"sensor");
		sensor_node.append_attribute(L"type").set_value(as_7xxx->getDeviceType());
		sensor_node.append_attribute(L"serial").set_value(serial.c_str());
		sensor_node.append_attribute(L"channel").set_value(as_7xxx->getChanNum());
		sensor_node.append_attribute(L"measure_data").set_value(false);
	}
	else
	{
		auto channel = sensor_node.attribute(L"channel").as_int(-1);
		if (channel == as_7xxx->getChanNum())
			sensor_node.attribute(L"channel").set_value(-1);
		else
			sensor_node.attribute(L"channel").set_value(as_7xxx->getChanNum());
	}

	stream_data_cfg.save_file(path.c_str());
}
void stream_data_worker::change_measure_data_state(const zetlab::object* object)
{
	if (!object)
		return;

	auto as_7xxx = object->getAs7xxx();
	if (!as_7xxx)
		return;

	auto device_type = as_7xxx->getDeviceType();
	if (device_type != ZET_139 && device_type != ZET_139A && device_type != ZET_139F &&
		device_type != ZET_139D && device_type != ZET_139B && device_type != ZET_139C)
		return;

	auto parent = object->getParent();
	if (!parent)
		return;
	if (auto name = parent->getName(); name.find(L"ZET 139") == 0)
		return;

	xml_document stream_data_cfg;
	std::wstring path = m_cfg_path + L"ZET7xxx\\Configs\\stream_data.xml";
	stream_data_cfg.load_file(path.c_str());

	xml_node devices_node = stream_data_cfg.child(L"devices");
	if (!devices_node)
		devices_node = stream_data_cfg.append_child(L"devices");

	auto serial = std::to_wstring(as_7xxx->getDeviceSerial());
	xml_node sensor_node = devices_node.find_child_by_attribute(L"sensor", L"serial", serial.c_str());
	if (!sensor_node)
	{
		sensor_node = devices_node.append_child(L"sensor");
		sensor_node.append_attribute(L"type").set_value(as_7xxx->getDeviceType());
		sensor_node.append_attribute(L"serial").set_value(serial.c_str());
		sensor_node.append_attribute(L"channel").set_value(-1);
		sensor_node.append_attribute(L"measure_data").set_value(true);
	}
	else
	{
		auto measure_data = sensor_node.attribute(L"measure_data").as_bool(false);
		sensor_node.attribute(L"measure_data").set_value(!measure_data);
	}

	stream_data_cfg.save_file(path.c_str());
}
//----------------------------------------------------------------------------------------------------------------------