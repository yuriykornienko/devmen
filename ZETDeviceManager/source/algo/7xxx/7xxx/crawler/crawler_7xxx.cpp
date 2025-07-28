#include "stdafx.h"

#include "crawler_7xxx.hpp"
#include "../../../object/object_type.hpp"
#include "../../../utils/hex/hex.hpp"
#include "../../../utils/string_conv/string_conv.hpp"
#include "../../../../GUI/user_messages.hpp" // for WM_DETECT_ZET76

#include <ModbusDefinitions.h>
#include <boost/algorithm/string.hpp>
#include "resource\Phrases.h"
#include "StructForCfgFile.h"

namespace zetlab { namespace _7xxx {

crawler::crawler( const factory_ptr& factory_ptr_, CXMLWorker* in_xml_worker_ptr, CWnd* pParent ) :
	_factory(factory_ptr_),
	_shared (false),
	_ftdi_id(-1),
	_xml_worker_ptr(in_xml_worker_ptr),
	_isFistUpdate(true),
	_parent_wnd(pParent),
	_notified_detect(false)
{
}

bool crawler::CheckFileAccess(CString fileName)
{	//создаем объект проверяемого файла
	bool ret(false);
	FILE *pFile(nullptr);
	long retL = zfFileExists(fileName);

	if (retL == 1)
	{// файл есть
		ret = zfOpenFile(fileName, L"r+b", &pFile) == 0;
		zfCloseFile(pFile);
		pFile = nullptr;
	}
	else if (retL == 0)
	{// файла нет
		ret = zfOpenFile(fileName, L"w+b", &pFile) == 0;
		zfCloseFile(pFile);
		pFile = nullptr;
		DeleteFile(fileName);
	}
	//else
	//{// ошибка
	//}
	return ret;
}

const long crawler::update()
{
// 	auto shared_struct = static_cast< STRUCT7000EX* >(open_shared( "MyData7000" ));
// 
// 	if (!shared_struct)
// 		return 0;
// 
// 	for (long i = 0, j = 0, c = read_controllers_count( shared_struct ); i < NUMLINES; ++i)
// 	{
// 		if (shared_struct->linfo[i].SerialNumber64)
// 			check_n_add( make_path( shared_struct, i ), i ), ++j;
// 	}
	long countLines(0);

	if (!_xml_worker_ptr->getFlagModbus())
		return countLines;

	CString sPath(_xml_worker_ptr->getPathXml());
	
	if (zfFileExists(sPath) && GetFileAttributesEx(sPath,
		GetFileExInfoStandard, &_attrib) != FALSE)
	{
		if (CompareFileTime(&_attrib.ftLastWriteTime, &_lastWriteTime) == 1 || _isFistUpdate)
		{// Файл был перезаписан
			_isFistUpdate = false;
			_lastWriteTime = _attrib.ftLastWriteTime;
			_xml_worker_ptr->getMapsForBuildTree(sPath);
		}
	}

	_notified_detect = false; // сбрасываем флаг, чтобы повторить сообщение не более раза за вызов update

	if (!_xml_worker_ptr->m_mapDeviceForTree.empty())
	{
		tstring sGroupName(_T(""));

		countLines = _xml_worker_ptr->m_mapDeviceForTree.size();

		for (auto & it : _xml_worker_ptr->m_mapDeviceForTree)
		{
			if (it.first._exist)
			{
				sGroupName = _xml_worker_ptr->getGroupName(it.first._serialNumber);
				check_n_add(sGroupName, it.first._line, it.first._serialNumber);
			}
		}
	}

	return /*shared_struct->numftdindex*/countLines;
}

const long crawler::read_controllers_count( void* void_shared_struct )
{
	auto shared_struct = static_cast< STRUCT7000EX* >(void_shared_struct);

	if (!!shared_struct->stepcomm)
		return shared_struct->numftdindex;

	shared_struct->virtchan = -1;
	shared_struct->ftdindex = -1;
	shared_struct->node     = -1;
	shared_struct->IsReady  = 0;
	shared_struct->stepcomm = 1;

	return shared_struct->numftdindex; // doesn't matter
}

inline const tstring crawler::make_path( void* void_shared_struct,
										 const long& index ) const
{
	auto s = static_cast< const STRUCT7000EX* >(void_shared_struct);
	ZET7XXX_DEVICE_TYPE type_ = static_cast<ZET7XXX_DEVICE_TYPE>(s->linfo_ext[index].device_type);
	
	tstring name = utils::cp1251_to_string(s->linfo[index].name);

	tstring full_path = name + _T(" ") +
		tstring(g_sNumber) +  utils::hex( type_ == ZET_7174 ? s->devinfo->serial_num : s->linfo[index].SerialNumber64 );
//	boost::algorithm::to_upper( full_path );

	return full_path;
}

void crawler::check_n_add(const tstring& full_path, const long& ftdi_id, const unsigned long long serialNum)
{
	if (boost::algorithm::istarts_with( full_path, _T("zetkey") ))
		return;
	if (boost::algorithm::istarts_with( full_path, _T("nozetlab") ))
		return;
	if (ftdi_id < 0)
		return;

	object* object_(nullptr);

	_ftdi_id = ftdi_id;
	if (!check_info())
		return;

	if (!_factory->exists( full_path ))
	{
		object_ = _factory->add(full_path, object_type::_7xxx_controller,
			boost::bind(&crawler::update_info, this, boost::placeholders::_1));
	}
	else
	{
		object_ = _factory->get( full_path );
		update_info( object_ );
	}

	device* device_(nullptr);
	device_ = object_->getAs7xxx();

	if (nullptr != device_)
		device_->setDeviceSerial(serialNum);

	boost::this_thread::interruption_point();
}

const bool crawler::check_info()
{
	// Проверяем по shared memory, что устройство следует отобразить
	// Считываем информацию от MODBUSZETLAB, но при этом управляем modes
	auto shared_struct = static_cast<const STRUCT7000EX*>(open_shared("MyData7000"));
	if (!shared_struct || _7xxxData.get_size() < sizeof(STRUCT7000EX))
		return false;

	const auto& linfo_ext = shared_struct->linfo_ext[_ftdi_id];

	if (linfo_ext.flags & LINE_INFO_EXT_FLAG_ZET76)
	{
		// По Ethernet отображаем только уже подключенные
		if (linfo_ext.state == LINE_INFO_EXT_STATE_CONNECTED || linfo_ext.state == LINE_INFO_EXT_STATE_ACCEPTED)
			return true;

		// Если надо отображать еще потоковые подключения, добавить условие:
//		if (linfo_ext.flags & (LINE_INFO_EXT_FLAG_ACCEPTABLE | LINE_INFO_EXT_FLAG_STREAMABLE))
//			return true;

		if (_parent_wnd && linfo_ext.state == LINE_INFO_EXT_STATE_IDLE && !_notified_detect)
		{
			_parent_wnd->PostMessageW(WM_DETECT_ZET76);
			_notified_detect = true;
		}
		return false;
	}

	return true;
}

const bool crawler::update_info( object* object_ )
{
	if (nullptr == object_)
		return true;

	object_->setAlive();

	device* device_(nullptr);
	device_ = object_->getAs7xxx();
	
	if (nullptr != device_)
	{
		if (object_type::_7xxx_controller == object_->getType())
			device_->setFtdiId( _ftdi_id );

		if (device_->update_info())
		{
			tstring descript(L"");
			if (device_->getLabel(descript))
				object_->SetLabel(descript.c_str());

			tstring ip_str(L"");
			device_->getIpStr(ip_str);
			object_->SetIP(ip_str.c_str());

			tstring serNum_str = zetlab::utils::hex(device_->getDeviceSerial());
			object_->SetSerialNum(serNum_str.c_str());

			const auto& full_path = object_->getFullPath();
			_factory->update( full_path );
			if (device_->isConnecting())
				_factory->start_wait( full_path );
			else
				_factory->stop_wait( full_path );
		}
	}

	return true;
}

void* crawler::open_shared( const ::std::string& shared_mem )
{
	using namespace boost::interprocess;

	try
	{
		if (!_shared)
		{
			_7xxxShared = windows_shared_memory(open_only, shared_mem.c_str(), read_write);
			_7xxxData   = mapped_region(_7xxxShared, read_write);
			_shared     = true;
		}

		return _7xxxData.get_address();
	}
	catch (...)
	{
	}

	return nullptr;
}

}} // namespace zetlab::_7xxx
