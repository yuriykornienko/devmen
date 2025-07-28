#include "stdafx.h"

#include "crawler_std.hpp"

#include "../../../types.hpp"
#include "../../object/object_type.hpp"

#include "../../utils/recorder/recorder.hpp"

#include "../../utils/zsp_dev_detect/zsp_dev_detect.hpp"
#include "../../utils/zet139_dev_detect/zet139_dev_detect.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace zetlab { namespace std {

crawler::crawler(const factory_ptr& factory_ptr_, const zsp_factory_ptr& zsp_factory_ptr_, CDSRV* server,  CWnd* pParent)
	: factory_(factory_ptr_)
	, zsp_factory_(zsp_factory_ptr_)
	, module(0)
	, ph_id(0)
	, srv_id(0)
	, enabled(false)
	, virt(false)
	, zsp(false)
	, isClose(false)
	, server_(server)
{
}

crawler::~crawler()
{}

void crawler::keepAlive()
{
	// Сообщаем серверу, что мы живы, чтобы он нас случайно не отключил
	if (server_->NumFileUsed(0L)  == -1)
	{
		// Если же он сам умер - выходим из программы
		//exit(0);
		if (isClose == false)
		{
			::PostMessage(GetParent(server_->m_hWnd), WM_CLOSE, 0, 0);
			isClose = true;
		}
	}
}

const long crawler::update()
{
	long count(0);
	long old_module(-1);
	long old_ph_id(0);
	long max_channels(0);
	long virt_channels(0);
	long _7xxx_id(0);

	tstring full_path(L"");
	tstring group_name(L""); 
	tstring commentary(L"");

	// Magic algorithm
	long collChan = server_->QuanChan();
	for (long i = 0; i < collChan; ++i)
	{
		server_->NumFileUsed(i);

		long status_ = server_->GetStatus(i);
		long ph_id_ = server_->NumPhChan(i);
		bool enabled_(false);
		bool virt_(false);
		bool remote_device(server_->GetRemoteDev(i) == 1);
		bool zsp(false), zet139(false);

		if (remote_device)
		{
			status_ = status_ + 100;
			bool b_continue(true);
			if (status_ == status::virtualR_ || status_ == status::no_active_virtualR)
			{
				GUID guid = GUID_NULL;
				if (0 == server_->IdChan(i, &guid))
				{
					if (is_zsp(guid))
					{
						zsp = true;
						ph_id_ = *reinterpret_cast<uint16_t*>((reinterpret_cast<char*>(&guid) + sizeof(GUID) - 2 * sizeof(uint16_t)));
						if (ph_id_ < 56)
							b_continue = false;
					}
					if (is_zet139(guid))
					{
						zet139 = true;
						b_continue = false;
					}
				}
			}
			if (b_continue)
				continue;
		}

		switch (status_)
		{
		case status::adc:
		case status::virtualR_:
			count++;
			enabled_ = true;
			break;

		case status::lostchannel:
		case status::no_active_virtualR:
			//ph_id_   = old_ph_id;
			break;

		case status::_7xxx:
		case status::fast_7xxx:
			check_n_add_2(make_path(server_->GroupName(i).GetString(), server_->Commentary(i).GetString(), _7xxx_id), _7xxx_id, i);
			_7xxx_id++;
			continue;

		case status::dac:
		case status::virtual_:
		case status::digital:
		case status::lostdevice:
		case status::no_active_virtual:
			continue;

		case status::lost_7xxx:
			_7xxx_id++;
			continue;

		//для каналов, передающихся через client data, пока функции настройки этих каналов не реализованы 
		case status::adcR:
		case status::dacR:
		//case status::virtualR_:
		case status::digitalR:
		case status::lostchannelR:
		case status::lostdeviceR:
		case status::_7xxxR:
		case status::lost_7xxxR:
		case status::fast_7xxxR:
		//case status::no_active_virtualR:
			continue;
		
		default:
			continue;
		}

		if (zsp)
		{
			check_n_add(make_path(server_->GroupName(i).GetString(), server_->Commentary(i).GetString(), ph_id_), -1, ph_id_, false, i, enabled_, true);
			continue;
		}
		if (zet139)
		{
			check_n_add_2(make_path(server_->GroupName(i).GetString(), server_->Commentary(i).GetString(), _7xxx_id), _7xxx_id, i);
			_7xxx_id++;
			continue;
		}

		long module_(server_->NumModule(i));

		if (module_ < 0)
		{
			if (old_ph_id < max_channels)
				module_ = old_module;
			else
				module_ = old_module + 1, ph_id_ = 0;
		}

		if (module_ != old_module)
		{
			while (old_ph_id < max_channels)
			{
				virt_ = max_channels - virt_channels <= old_ph_id;
				check_n_add(make_path(group_name, _T(""), old_ph_id),
							 old_module, old_ph_id, virt_);
				++old_ph_id;
			}

			old_ph_id    = 0;
			old_module   = module_;
			max_channels = server_->QuanPhChan(module_);
			virt_channels = server_->QuanVirtChan(module_);
			group_name   = server_->GroupName(i);
		}

		if (group_name.empty())
			group_name = server_->GroupName(i);

		while (ph_id_ > old_ph_id)
		{
			virt_ = max_channels - virt_channels <= old_ph_id;
			check_n_add(make_path(group_name, _T(""), old_ph_id),
						module_, old_ph_id, virt_);
			++old_ph_id;
		}

		virt_ = max_channels - virt_channels <= ph_id_;
		commentary = server_->Commentary(i);
		check_n_add(make_path(group_name, commentary, ph_id_),
					 module_, ph_id_, virt_, i, enabled_);
		++old_ph_id;
	}

	while (old_ph_id < max_channels)
	{
		bool virt_ = max_channels - virt_channels <= old_ph_id;
		check_n_add(make_path(group_name, _T(""), old_ph_id),
					 old_module, old_ph_id, virt_);
		++old_ph_id;
	}


	TCHAR buffer[100] = { 0 };
	DWORD conv = GetLogicalDriveStrings(sizeof(buffer) / sizeof(TCHAR), buffer);
	for (TCHAR* s = buffer; *s; s += _tcslen(s) + 1)
	{
		boost::this_thread::interruption_point();
		UINT uDriveType = GetDriveType(s);   // Определяем тип диска
		if (uDriveType == DRIVE_REMOVABLE)
		{
			WIN32_FIND_DATAW wfd;
			CString checkdir(s);
			checkdir.Append(L"*");
			bool findZdtorDat(false);

			HANDLE const hFind = FindFirstFileW(checkdir, &wfd);
			if (INVALID_HANDLE_VALUE != hFind)
			{
				do
				{
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (_tcscmp(wfd.cFileName, L"ZET_DATA") == 0)
						{
							findZdtorDat = true;
							break;
						}
					}
					else
					{
						if (zetlab::utils::check_convertible(wfd.cFileName))
						{
							findZdtorDat = true;
							break;
						}
					}
				} while (FindNextFileW(hFind, &wfd) != NULL);

				if (findZdtorDat)
					check_n_add_SD(s);

				FindClose(hFind);
			}
		}
	}

	return count;
}

inline const tstring crawler::make_path(const tstring& group_name,
										 const tstring& elem_name,
										 const long& ph_id_)
{
	return/*return boost::algorithm::to_upper_copy(*/ group_name /*) */+ _T(";") +
		   elem_name + _T("|") + boost::lexical_cast< tstring >(ph_id_);
}

void crawler::check_n_add(const tstring& full_path,
						   const long& module_,
						   const long& ph_id_,
						   const bool& virt_,
						   const long srv_id_,
						   const bool enabled_,
						   const bool zsp_)
{
	object* object_(nullptr);

	module  = module_;
	ph_id   = ph_id_;
	srv_id  = srv_id_;
	virt	= virt_;
	enabled = enabled_;
	zsp	= zsp_;

	if (!factory_->exists(full_path))
	{
		object_ = factory_->add(full_path, object_type::channel,
							 boost::bind(&crawler::update_info, this, _1));

	}
	else
	{
		object_ = factory_->get(full_path);
		update_info(object_->getParent());
		update_info(object_);
	}

	boost::this_thread::interruption_point();
}

void crawler::check_n_add_2(const tstring& full_path,
							 const long& node_id,
							 const long& srv_id_)
{
	object* object_(nullptr);

	ph_id  = node_id;
	srv_id = srv_id_;

	if (!factory_->exists(full_path))
	{
		object_ = factory_->add(full_path, object_type::_7xxx_is,
								 boost::bind(&crawler::update_info, this, _1));
	}
	else
	{
		object_ = factory_->get(full_path);
		update_info(object_);

		// проверка на единицу измерения
		auto channel = object_->get7xxxAsChannel();
		if (channel)
		{
			tstring oldUnit(channel->getUnit(false));
			tstring newUnit(channel->getServer()->Conversion(channel->getServerID()));
			float oldFreq(channel->GetFreq());
			float newFreq(channel->getServer()->Frequency(channel->getServerID()));

			if (oldUnit != newUnit || ((oldFreq != newFreq) && oldFreq!=0))
				factory_->update(full_path);
		}
	}

	boost::this_thread::interruption_point();
}

void crawler::check_n_add_SD(const tstring& full_path)
{
	object* object_(nullptr);
	if (!factory_->exists(full_path))
	{
		object_ = factory_->add(full_path, object_type::sd,
			boost::bind(&crawler::update_info, this, _1));
	}
	else
	{
		object_ = factory_->get(full_path);
		update_info(object_);
	}
	boost::this_thread::interruption_point();
}

const bool crawler::update_info(object* object_)
{
	if (nullptr == object_)
		return true;

	object_->setAlive();

	device* device_ = object_->getAsDevice();
	if (device_ != nullptr)
	{
		device_->setServer(server_);
		device_->setModule(module);

		//загрузка Label
		tstring name(L"");
		if (device_->readDescription(name))
			object_->SetLabel(name.c_str());

		//загрузка IP  - убрать до решения дальнейшей организации обмена
		//unsigned char ip[4], mask[4], gateway[4], mac[6], duplex, speed;
		//unsigned short port, timeout;
		//CString ip_strC;
		//if (device_->readEthSettings(ip, mask, gateway, port, mac, timeout, duplex, speed))
		//{
		//	ip_strC.Format(L"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		//	object_->SetIP(ip_strC);
		//}
		BYTE ip1, ip2, ip3, ip4;
		if (device_->readEthIP(&ip1, &ip2, &ip3, &ip4))
		{
			CString ip_strC;
			ip_strC.Format(L"%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			object_->SetIP(ip_strC);
		}

		if (device_->getType() == -1)
		{
			long tmp_srv_id(srv_id);

			do
			{
				bool b_zsp(false);
				unsigned long long serial(0);
				server_->GetSerialNum(tmp_srv_id, &serial);
				if (serial == 0)
				{
					GUID guid = GUID_NULL;
					if (0 == server_->IdChan(tmp_srv_id, &guid))
					{
						if (is_zsp(guid))
						{
							auto type = *reinterpret_cast<uint16_t*>((reinterpret_cast<char*>(&guid) + sizeof(GUID) - sizeof(uint16_t)));
							serial = *reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(&guid));
							device_->setType(type);
							
							b_zsp = true;
						}
					}
				}
				if (!b_zsp)
				{
					device_->setType(server_->TypeAdc(tmp_srv_id));
					device_->setDSP(server_->NumDSP(tmp_srv_id));
				}
								
				if (device_->getSerial() <= 0)
				{
					device_->setSerial((long)serial);
					device_->setSerial64(serial);
					object_->SetObjectSerialNum((long)serial);
				}
			
				if (server_->TypeAdc(tmp_srv_id) == -4)
					break;

				if (b_zsp)
					break;

				boost::this_thread::interruption_point();
			}
			while (server_->TypeAdc(tmp_srv_id++) < 0);
		}
		else
		{
			if (server_->TypeAdc(srv_id) >= 0)
			{
				unsigned long long serial(0);
				server_->GetSerialNum(srv_id, &serial);

				device_->setType  (server_->TypeAdc(srv_id));
				device_->setDSP   (server_->NumDSP(srv_id));
				if (device_->getSerial() <= 0)
				{
					device_->setSerial((long)serial);
					device_->setSerial64(serial);
					object_->SetObjectSerialNum((long)serial);
				}
			}
		}

		if (zsp_device_id device_id = { get_device_type(static_cast<uint32_t>(device_->getType())), device_->getSerial64() };
			device_id.first != zsp_device_type::unknown)
		{
			zsp_factory_->check(device_id);
			if (device_->set_zsp_config(zsp_factory_->get_device(device_id), zsp_factory_->get_state(device_id), zsp_factory_->get_metadata(device_id)))
				factory_->update(object_->getFullPath());
		}
	}

	channel* channel_(nullptr);
	channel_ = object_->getAsChannel();

	if (channel_ == nullptr)
		channel_ = object_->get7xxxAsChannel();

	if (channel_ != nullptr)
	{
		bool b_update(false);
		channel_->setServer  (server_);
		auto pParent = object_->getParent();
		if (nullptr != pParent)
		{
			device_ = pParent->getAsDevice();
			channel_->setDevice(device_);
			if (device_)
			{
				if (zsp_device_id device_id = { get_device_type(static_cast<uint32_t>(device_->getType())), device_->getSerial64() };
					device_id.first != zsp_device_type::unknown)
				{
					if (channel_->set_zsp_config(zsp_factory_->get_channel(device_id, channel_->getDeviceID()), zsp_factory_->get_metadata(device_id)))
						b_update = true;
				}
			}
		}
		channel_->setDeviceID(ph_id);
		channel_->setServerID(srv_id);
		channel_->setEnabled (enabled);
		channel_->setVirt	 (virt);
		channel_->set_zsp	 (zsp);
		channel_->getName    (true);
		channel_->getComment (true);
		channel_->getUnit	 (true);
		channel_->CoordX	 (true);
		channel_->CoordY	 (true);
		channel_->CoordZ	 (true);
		channel_->CoordP	 (true);
		channel_->Sense      (true);
		channel_->getUnitSense(true);
		channel_->Reference  (true);
		channel_->Shift      (true);
		channel_->HPF		 (true);
		channel_->Amplify    (true);
		channel_->getTensoIcp(true);
		channel_->getTensoResistance(true);
		channel_->getTensoCoeff(true);
		channel_->getTensoYoungMod(true);
		channel_->InputResistance(true);
		channel_->AdcInputMode(true);

		if (b_update)
			factory_->update(object_->getFullPath());
	}

	_7xxx::device* _7xxx_device(nullptr);
	_7xxx_device = object_->getAs7xxx();

	if (_7xxx_device != nullptr)
	{
		if (object_->getType() == object_type::_7xxx_is)
		{
			_7xxx_device->setNodeId(ph_id);

			GUID guid = GUID_NULL;
			if (0 == server_->IdChan(srv_id, &guid))
			{
				if (is_zet139(guid))
				{
					auto type = *reinterpret_cast<uint16_t*>((reinterpret_cast<char*>(&guid) + sizeof(GUID) - sizeof(uint16_t)));
					auto serial = *reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(&guid));
					_7xxx_device->setDeviceSerial(serial);
					_7xxx_device->setDeviceType((ZET7XXX_DEVICE_TYPE)type);
				}
				else
				{
					long serial(0);
					server_->IdChan(srv_id, &serial);
					_7xxx_device->setDeviceSerialNType(serial);
				}
			}
		}
	}

	return true;
}

const long crawler::startServer()
{
	long ret = server_->Start();
	return ret;
}

}} // namespace zetlab::std
