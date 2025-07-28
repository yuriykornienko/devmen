#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "list.h"
#include <mathZet.h>
#include "resource\Phrases.h"
#include "../../../algo/utils/hex/hex.hpp"
#include <FunctionApplication/FunctionApplication.h>
#include <Common_exe_ocx/FunctionForAll.h>

extern CString g_sNameOrientation[7];

enum ListTimer : UINT
{
	kBlinking = 500,
	kConnecting = 100,
	updateIcon = 101
};

BEGIN_MESSAGE_MAP(CDevicesList, CListCtrlEx)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedToolTip)

END_MESSAGE_MAP()

void CDevicesList::LoadItemImages()
{
	InsertColumn(_T(" "));
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	InsertColumn(_T(" "), LVCFMT_CENTER);
	
	m_imageList.Detach();
	m_imageList.Create(m_dpiAware.ScaleX(18), m_dpiAware.ScaleY(21), ILC_COLOR32, 10, 1);


	// Load icons
	auto select_icon = [](int image_index) {
		using image = zetlab::object::image;
		switch (image_index)
		{
		case image::pci: return IDI_PCI_18;
		case image::usb: return IDI_USB_18;
		case image::ethernet: return IDI_ETHERNET_18;
		case image::channel_enabled: return IDI_CHANNEL_ENABLED_18;
		case image::channel_icp: return IDI_CHANNEL_ICP_18;
		case image::channel_disabled: return IDI_CHANNEL_DISABLED_18;
		case image::_7xxx: return IDI_7XXX_18;
		case image::blinked_on: return IDI_BLINKED_ON_18;
		case image::blinked_off: return IDI_BLINKED_OFF_18;
		case image::demo: return IDI_DEMO_18;
		case image::ethernet_locked: return IDI_ETHERNET_LOCKED_18;
		case image::ethernet_error: return IDI_ETHERNET_ERROR_18;
		case image::channel_charge: return IDI_CHANNEL_CHARGE_18;
		case image::channel_service: return IDI_CHANNEL_SERVICE_18;
		case image::generator: return IDI_GENERATOR_ON_18;
		case image::sdcard: return IDI_SD_CARD_18;
		case image::connecting1: return IDI_CONNECTING1_18;
		case image::connecting2: return IDI_CONNECTING2_18;
		case image::connecting3: return IDI_CONNECTING3_18;
		case image::connecting4: return IDI_CONNECTING4_18;
		//case image::ethernet_connect: return IDI_ETHERNET_18;
		//case image::ethernet_accepted: return IDI_ETHERNET_18;
		case image::ethernet_warning: return IDI_ETHERNET_WARNING_18;
		case image::usb_warning: return IDI_USB_WARNING_18;
		}
		return IDI_DEMO;
	};
	while (m_imageList.GetImageCount() < zetlab::object::image::count)
		m_imageList.Add(LOAD_ICON(select_icon(m_imageList.GetImageCount()), m_dpiAware.ScaleX(18), m_dpiAware.ScaleY(21)));

	SetImageList(&m_imageList, LVSIL_SMALL);
}

void CDevicesList::Add(zetlab::object* object_, bool isSelect /*= false*/ )
{	
	int iParent = -1;
	if (object_->getParent() == nullptr)
		return;
	if (!object_->getParent()->isRoot())
		iParent = FindItem(object_->getParent());

	if (iParent < -1)   // Найти родительский элемент в списке.
		return;
	 
	CString freq, sense, icp, charge, inputType, amplify, extAmplify, range, reference, shift, X(L"0"), Y(L"0"), Z(L"0"), P(L"o"); // Подготовка данных для отображения


	zetlab::std::channel* channel_ = object_->getAsChannel();  //Пытается привести объект к каналу (channel_).
	bool is7xxx = false;
	if (!channel_) 
	{
		channel_ = object_->get7xxxAsChannel();
		if (channel_)
			is7xxx = true;
	}

	if (channel_)
	{
		CString unit(channel_->getUnit().c_str());
		CString unitSense(channel_->getUnitSense().c_str());
		double channelsense(channel_->Sense());

		if (unitSense == g_sUNIT_MKV)  // перевод единиц чувствительности
			channelsense *= 1000000;
		else if (unitSense == g_sUNIT_MV)
			channelsense *= 1000;
		else if (unitSense == g_sUNIT_KV)
			channelsense /= 1000;
		
		if (!is7xxx && 0 != channelsense)
		{
			sense.Format(_T("%g "), channelsense);
			if (!unitSense.IsEmpty())
			{
				auto b_div_unit_sense = (-1 != unitSense.Find(L"/"));
				if (b_div_unit_sense)
					sense += L"(";
				sense += unitSense;
				if (b_div_unit_sense)
					sense += L")";

				if (!unit.IsEmpty())
				{
					sense += L"/";
					auto b_div_unit = (-1 != unit.Find(L"/") || -1 != unit.Find(L"·") || -1 != unit.Find(L"*"));
					if (b_div_unit)
						sense += L"(";
					sense += unit;
					if (b_div_unit)
						sense += L")";
				}
			}
			else if (!unit.IsEmpty())
				sense += unit;
		}
		else
			sense = unit;

		float m_fFreq(0);
		bool is7xxx = false;
		bool b_zsp(false);
		if (channel_->is_zsp())
		{
			if (channel_->is_zet037_tenso() &&
				(channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tr) ||
				channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tc)))
				sense = unit;
			m_fFreq = channel_->getDevice()->getAdcFrequency(); // получение частоты канала
			b_zsp = true;
		}
		else if (channel_->isDemo())
			m_fFreq = 100;
		else if (channel_->getDevice() != nullptr)
			m_fFreq = channel_->getDevice()->getAdcFrequency();
		else if (object_->get7xxxAsChannel()) {
			m_fFreq = channel_->getServer()->Frequency(channel_->getServerID());
			is7xxx = true;
		}

		freq = ConvertToFreq(m_fFreq);
		channel_->SetFreq(m_fFreq);

		icp = channel_->getICP() && !channel_->getTensoIcp() ? g_sYes : g_sNo; // прочие столбцы
		charge = channel_->getCharge() ? g_sYes : g_sNo;
		inputType = channel_->HPF() ? g_sAc : g_sDc;
		amplify.Format(_T("%g"), channel_->getAmplify());
		extAmplify.Format(_T("%g"), channel_->Amplify());
		range.Format(_T("%g"), channel_->getMaxLevel());
		reference.Format(_T("%g"), channel_->Reference());
		shift.Format(_T("%g"), channel_->Shift());

         if (channel_->getType() == 17 /*ZET_DEVICE_230 */ || channel_->getType() == 10 /*ZET_DEVICE_210 */ ||
			channel_->getType() == 16  /*ZET_DEVICE_220 */ || channel_->getType() == 12  /*ZET_DEVICE_110 */ || 
			channel_->getType() == 211 /*ZET_DEVICE_211*/ || channel_->getType() == 221 /*ZET_DEVICE_221*/)
		{
			X = g_sNo;
			Y = g_sNo;
			Z = g_sNo;
			P = g_sNo;
		}
		else
			ConvertToCoordinates(channel_, &X, &Y, &Z, &P, is7xxx || b_zsp);

		if (!channel_->isEnabled() && !is7xxx && !b_zsp) // координаты
		{
			sense = freq = icp = extAmplify = reference = shift = inputType = charge = amplify = range = L"-";
			X = Y = Z = P = L"-";
		}
	}
	else if (object_->getAsDevice() ||
			(object_->getAs7xxx() && object_->getType() == zetlab::object_type::_7xxx_controller))  //  Обработка устройств (если объект — устройство)
	{
		freq = g_sFrequency;

		sense = object_->getAs7xxx() && object_->getType() == zetlab::object_type::_7xxx_controller
			? g_sMeasureUnit : g_sChannelSensitivity;

		icp = L"ICP";
		charge = g_sCharge;
		inputType = g_sInputType;
		amplify = g_sAmplifier;
		extAmplify = g_sConstantIncrease;
		range = g_sRange;
		reference = g_sReferenceValueLevel;
		shift = g_sOffsetDC;
		X = L"X", Y = L"Y", Z = L"Z", P = g_sOrientation;
	}

	setList(true);
	
	auto name = object_->getNameForShow();//object_->getName().c_str();// +addname;
	int iItem = InsertItem(iParent,  //  Добавляет строку в список с указанными данными:
			object_->getImage(),
			name,
			sense, freq, icp, extAmplify, reference, shift, inputType, charge, amplify, range, X, Y, Z, P, (LPCTSTR)0);

	SetItemData(iItem, reinterpret_cast<DWORD_PTR>(object_));  //Привязывает объект object_ к строке через DWORD_PTR.
	//SortAll();
	
	SetTimer(ListTimer::updateIcon, 1000, NULL);
	
}



void CDevicesList::AddRow037(zetlab::object* object_, bool isSelect /*= false*/)
{

	static int z = 0;
	//++z;
	int iParent = -1;
	if (object_->getParent() == nullptr)
		return;
	if (!object_->getParent()->isRoot())
		iParent = FindItem(object_->getParent());

	if (iParent < -1)   // Найти родительский элемент в списке.
		return;

	CString freq, sense, icp, charge, inputType, amplify, extAmplify, range, reference, shift, X(L"0"), Y(L"0"), Z(L"0"), P(L"o"); // Подготовка данных для отображения


	zetlab::std::channel* channel_ = object_->getAsChannel();  //Пытается привести объект к каналу (channel_).
	bool is7xxx = false;
	if (!channel_)
	{
		channel_ = object_->get7xxxAsChannel();
		if (channel_)
			is7xxx = true;
	}

	if (channel_)
	{
		CString unit(channel_->getUnit().c_str());
		CString unitSense(channel_->getUnitSense().c_str());
		double channelsense(channel_->Sense());

		if (unitSense == g_sUNIT_MKV)  // перевод единиц чувствительности
			channelsense *= 1000000;
		else if (unitSense == g_sUNIT_MV)
			channelsense *= 1000;
		else if (unitSense == g_sUNIT_KV)
			channelsense /= 1000;

		if (!is7xxx && 0 != channelsense)
		{
			sense.Format(_T("%g "), channelsense);
			if (!unitSense.IsEmpty())
			{
				auto b_div_unit_sense = (-1 != unitSense.Find(L"/"));
				if (b_div_unit_sense)
					sense += L"(";
				sense += unitSense;
				if (b_div_unit_sense)
					sense += L")";

				if (!unit.IsEmpty())
				{
					sense += L"/";
					auto b_div_unit = (-1 != unit.Find(L"/") || -1 != unit.Find(L"·") || -1 != unit.Find(L"*"));
					if (b_div_unit)
						sense += L"(";
					sense += unit;
					if (b_div_unit)
						sense += L")";
				}
			}
			else if (!unit.IsEmpty())
				sense += unit;
		}
		else
			sense = unit;

		float m_fFreq(0);
		bool is7xxx = false;
		bool b_zsp(false);
		if (channel_->is_zsp())
		{
			if (channel_->is_zet037_tenso() &&
				(channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tr) ||
					channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tc)))
				sense = unit;
			m_fFreq = channel_->getDevice()->getAdcFrequency(); // получение частоты канала
			b_zsp = true;
		}
		else if (channel_->isDemo())
			m_fFreq = 100;
		else if (channel_->getDevice() != nullptr)
			m_fFreq = channel_->getDevice()->getAdcFrequency();
		else if (object_->get7xxxAsChannel()) {
			m_fFreq = channel_->getServer()->Frequency(channel_->getServerID());
			is7xxx = true;
		}

		freq = ConvertToFreq(m_fFreq);
		channel_->SetFreq(m_fFreq);

		icp = channel_->getICP() && !channel_->getTensoIcp() ? g_sYes : g_sNo; // прочие столбцы
		charge = channel_->getCharge() ? g_sYes : g_sNo;
		inputType = channel_->HPF() ? g_sAc : g_sDc;
		amplify.Format(_T("%g"), channel_->getAmplify());
		extAmplify.Format(_T("%g"), channel_->Amplify());
		range.Format(_T("%g"), channel_->getMaxLevel());
		reference.Format(_T("%g"), channel_->Reference());
		shift.Format(_T("%g"), channel_->Shift());

		if (channel_->getType() == 17 /*ZET_DEVICE_230 */ || channel_->getType() == 10 /*ZET_DEVICE_210 */ ||
			channel_->getType() == 16  /*ZET_DEVICE_220 */ || channel_->getType() == 12  /*ZET_DEVICE_110 */ ||
			channel_->getType() == 211 /*ZET_DEVICE_211*/ || channel_->getType() == 221 /*ZET_DEVICE_221*/)
		{
			X = g_sNo;
			Y = g_sNo;
			Z = g_sNo;
			P = g_sNo;
		}
		else
			ConvertToCoordinates(channel_, &X, &Y, &Z, &P, is7xxx || b_zsp);

		if (!channel_->isEnabled() && !is7xxx && !b_zsp) // координаты
		{
			sense = freq = icp = extAmplify = reference = shift = inputType = charge = amplify = range = L"-";
			X = Y = Z = P = L"-";
		}
	}
	else if (object_->getAsDevice() ||
		(object_->getAs7xxx() && object_->getType() == zetlab::object_type::_7xxx_controller))  //  Обработка устройств (если объект — устройство)
	{
		if (z == 0)
		{
			freq = g_sFrequency;

			sense = object_->getAs7xxx() && object_->getType() == zetlab::object_type::_7xxx_controller
				? g_sMeasureUnit : g_sChannelSensitivity;

			icp = L"ICP";
			charge = g_sCharge;
			inputType = g_sInputType;
			amplify = g_sAmplifier;
			extAmplify = g_sConstantIncrease;
			range = g_sRange;
			reference = g_sReferenceValueLevel;
			shift = g_sOffsetDC;
			X = L"X", Y = L"Y", Z = L"Z", P = g_sOrientation;
		}
	}

	setList(true);
	if (z >= 1 && z <= 2)///////////////////////
	{
		iParent = 0;
		//object_->getAsChannel()->setDeviceID(20);
	}
	if (z >= 3 && z < 12)///////////////////////
	{
		iParent = 1;
		//object_->getAsChannel()->setDeviceID(20);
	}
	if (z >= 12 && z < 25)///////////////////////
	{
		iParent = 11;
		//object_->getAsChannel()->setDeviceID(20);
	}

	auto name = object_->getNameForShow();//object_->getName().c_str();// +addname;
	int iItem = InsertItem(iParent,  //  Добавляет строку в список с указанными данными:
		object_->getImage(),
		name,
		sense, freq, icp, extAmplify, reference, shift, inputType, charge, amplify, range, X, Y, Z, P, (LPCTSTR)0);

	SetItemData(iItem, reinterpret_cast<DWORD_PTR>(object_));  //Привязывает объект object_ к строке через DWORD_PTR.
	//SortAll();
	++z;
	SetTimer(ListTimer::updateIcon, 1000, NULL);



}






void CDevicesList::Remove(zetlab::object* object_)
{
	if (GetItemCount() == 0)
		return;

	int iItem(FindItem(object_));
	if (iItem >= 0)
		DeleteItem(iItem);
}

void CDevicesList::Update(zetlab::object* object_)
{
	UpdateDeviceInfo(object_);
	UpdateChannelInfo(object_);
	Invalidate();
}
/*
void CDevicesList::Update(std::vector<zetlab::object*>&  obj_list)
{
	for (auto& object_ : obj_list)
	{
		UpdateDeviceInfo(object_);
		UpdateChannelInfo(object_);
	}

	Invalidate();
}
*/

int CDevicesList::FindItem(zetlab::object* const object_)
{
	LVFINDINFO find;  //Эта структура используется для задания параметров поиска в CListCtrl.
	find.flags = LVFI_PARAM;  //указывает, что поиск будет выполняться по значению lParam элемента.
	find.lParam = (LPARAM)object_;  //задаёт указатель на объект, который нужно найти.


	int iItem = CListCtrl::FindItem(&find);
	if (iItem < 0)
		iItem = -2;

	return iItem;
}

zetlab::object* CDevicesList::GetSelectedItemObject()
{
	int iItem(GetSelectionMark());

	if (-1 == iItem)
		return nullptr;

	return reinterpret_cast<zetlab::object*>(GetItemData(iItem));
}

zetlab::object* CDevicesList::GetItemObject(const uint32_t type_, const uint64_t serial_)
{
	int count(GetItemCount());
	for (int i = 0; i < count; ++i)
	{
		zetlab::object* pObject = reinterpret_cast<zetlab::object*>(GetItemData(i));
		auto p7xxx = pObject->getAs7xxx();
		if (nullptr != p7xxx)
		{
			auto type = p7xxx->getDeviceType();
			auto serial = p7xxx->getDeviceSerial();
			if (type_ == type && serial_ == serial)
				return pObject;
		}
		else
		{
			auto tempDevice = pObject->getAsDevice();
			if (nullptr != tempDevice)
			{
				auto type = tempDevice->getType();
				auto serial = tempDevice->getSerial();
				if (type_ == type && serial_ == serial)
					return pObject;
			}
		}
	}

	return nullptr;
}

void CDevicesList::SelectItem(zetlab::object* const object_)
{
	int iItem(FindItem(object_));

	if (iItem >= 0)
	{
		SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		//int nSelected = m_list.GetNextItem(-1, LVNI_SELECTED);
		//while (nSelected != -1)
		//{
			//m_list.SetItemState(nSelected, 0, LVIS_SELECTED); 
			//nSelected = m_list.GetNextItem(nSelected, LVNI_SELECTED);
		//}
	}
	//SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
}

void CDevicesList::UpdateDeviceInfo(zetlab::object* const object_)
{
	int iItem(FindItem(object_));

	if (iItem < 0)
		return;

	bool update_image(true);
	// Это не для 7xxx!!!

	auto device_ = object_->getAsDevice();
	auto as7xxx = object_->getAs7xxx();
	if (device_ != nullptr)
		//&& as7xxx == nullptr)	
	{
 		if (device_->isHaveDescription())
 		{
 			zetlab::tstring description(L"");
 			device_->readDescription(description);
 			object_->SetLabel(description.c_str());
 		}
		CString name = object_->getNameForShow();
		SetItemText(iItem, 0, name);
	}
	else if (as7xxx != nullptr)
	{
		//auto as7xxx = object_->getAs7xxx();
		if (object_->getType() == zetlab::object_type::_7xxx_controller && !object_->isModify())
		{
			CString name = object_->getNameForShow(); //object_->getName().c_str();
// 			zetlab::tstring label;
// 			if (as7xxx->getLabel(label) && !label.empty())
// 				name.AppendFormat(L" (%s)", label.c_str());
			if (GetItemText(iItem, NULL) != name)
				SetItemText(iItem, NULL, name);
		}
		if (as7xxx->isConnecting())
			update_image = false;
	}

	if (update_image)
		SetNewImage(iItem, object_);
}

void CDevicesList::UpdateChannelInfo(zetlab::object* const object_)
{
	int iItem(FindItem(object_));

	if (iItem < 0)
		return;

	if (object_->getAsChannel())
	{
		zetlab::std::channel* channel_ = object_->getAsChannel();
		SetNewImage(iItem, object_);

		CString sense, freq, icp, charge, inputType, amplify, extAmplify, range, reference, shift, X(L"0"), Y(L"0"), Z(L"0"), P(L"o");
		CString unit(channel_->getUnit().c_str());
		CString unitSense(channel_->getUnitSense().c_str());
		double channelsense(channel_->Sense());

		if (unitSense == g_sUNIT_MKV)
			channelsense *= 1000000;
		else if (unitSense == g_sUNIT_MV)
			channelsense *= 1000;
		else if (unitSense == g_sUNIT_KV)
			channelsense /= 1000;

		sense.Format(_T("%g "), channelsense);

		if (!unitSense.IsEmpty())
		{
			auto b_div_unit_sense = (-1 != unitSense.Find(L"/"));
			if (b_div_unit_sense)
				sense += L"(";
			sense += unitSense;
			if (b_div_unit_sense)
				sense += L")";

			if (!unit.IsEmpty())
			{
				sense += L"/";
				auto b_div_unit = (-1 != unit.Find(L"/") || -1 != unit.Find(L"·") || -1 != unit.Find(L"*"));
				if (b_div_unit)
					sense += L"(";
				sense += unit;
				if (b_div_unit)
					sense += L")";
			}
		}
		else if (!unit.IsEmpty())
			sense += unit;

		float m_fFreq(0.);
		bool is7xxx = false;
		bool b_zsp(false);
		if (channel_->is_zsp())
		{
			if (channel_->is_zet037_tenso() &&
				(channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tr) ||
				channel_->get_tenso_sensor() == static_cast<uint16_t>(zetlab::zsp_tenso_sensor::tc)))
				sense = unit;
			m_fFreq = channel_->getDevice()->getAdcFrequency();
			b_zsp = true;
		}
		else if (channel_->isDemo())
			m_fFreq = 100;
		else if (channel_->getDevice() != nullptr)
			m_fFreq = channel_->getDevice()->getAdcFrequency();
		else if (object_->get7xxxAsChannel()) {
			m_fFreq = channel_->getServer()->Frequency(channel_->getServerID());
			is7xxx = true;
		}

		freq = ConvertToFreq(m_fFreq);
		channel_->SetFreq(m_fFreq);

		icp = channel_->getICP() && !channel_->getTensoIcp() ? g_sYes : g_sNo;
		charge = channel_->getCharge() ? g_sYes : g_sNo;
		inputType = channel_->HPF() ? g_sAc : g_sDc;
		amplify.Format(_T("%g"), channel_->getAmplify());
		extAmplify.Format(_T("%g"), channel_->Amplify());
		range.Format(_T("%g"), channel_->getMaxLevel());
		reference.Format(_T("%g"), channel_->Reference());
		shift.Format(_T("%g"), channel_->Shift());

		if (channel_->getType() == 17 /*ZET_DEVICE_230 */ || channel_->getType() == 10 /*ZET_DEVICE_210 */ ||
			channel_->getType() == 16  /*ZET_DEVICE_220 */ || channel_->getType() == 12  /*ZET_DEVICE_110 */ ||
			channel_->getType() == 211 /*ZET_DEVICE_211*/ || channel_->getType() == 221 /*ZET_DEVICE_221*/)
		{
			X = g_sNo;
			Y = g_sNo;
			Z = g_sNo;
			P = g_sNo;
		}
		else
			ConvertToCoordinates(channel_, &X, &Y, &Z, &P);

		if(!channel_->isEnabled() && !is7xxx && !b_zsp)
		{
			sense = freq = icp = extAmplify = reference = shift = inputType = charge = amplify = range = L"-";
			X = Y = Z = P = L"-";
		}
		
		CString name = object_->getNameForShow(); //object_->getName().c_str();
		SetItemText(iItem, 0, name);
		SetItemText(iItem, 1, sense);
		SetItemText(iItem, 2, freq);
		SetItemText(iItem, 3, icp);
		SetItemText(iItem, 4, extAmplify);
		SetItemText(iItem, 5, reference);
		SetItemText(iItem, 6, shift);
		SetItemText(iItem, 7, inputType);
		SetItemText(iItem, 8, charge);
		SetItemText(iItem, 9, amplify);
		SetItemText(iItem, 10, range);
		SetItemText(iItem, 11, X);
		SetItemText(iItem, 12, Y);
		SetItemText(iItem, 13, Z);
		SetItemText(iItem, 14, P);
	}
	else if (object_->get7xxxAsChannel())
	{
	// Координаты
		CString X(L"0"), Y(L"0"), Z(L"0"), P(L"o");
		if (ConvertToCoordinates(object_->get7xxxAsChannel(), &X, &Y, &Z, &P, true) == 0)
		{
			SetItemText(iItem, 11, X);
			SetItemText(iItem, 12, Y);
			SetItemText(iItem, 13, Z);
			SetItemText(iItem, 14, P);
		}

		CString unit(L"");
		int a(GetItemCount());
		if (a > 0)
		{
			while (a != 0)
			{
				a--;
				zetlab::object* Object7xxx = EnumerationAllChannelsFromDevice(a);
				if (Object7xxx != nullptr)
				{
					auto Object7xxxUint = Object7xxx->get7xxxAsChannel();
					if (Object7xxxUint != nullptr)
					{
						unit = Object7xxxUint->getUnitFor7xxx().c_str();
						//if (!Object7xxxUint->isEnabled())
						//	unit = L"-";
						
						if (!unit.IsEmpty())
							SetItemText(a, 1, unit);

						//**			
						if (!ConvertToFreq(Object7xxxUint->getServer()->Frequency(Object7xxxUint->getServerID())).IsEmpty())
						{
							CString freq(ConvertToFreq(Object7xxxUint->getServer()->Frequency(Object7xxxUint->getServerID())));
							//if (!Object7xxxUint->isEnabled())
							//	unit = L"-";
							//
							SetItemText(a, 2, freq);
							Object7xxxUint->SetFreq(Object7xxxUint->getServer()->Frequency(Object7xxxUint->getServerID()));
						}
					}
				}
			}
		}	
	}
	//Если у устройства изменили частоту, поменяем в листе для всех каналов
	else if (object_->getAsDevice())
	{
		int i(GetItemCount());
		if (i > 0)
		{
			while (i != 0)
			{
				--i;
				zetlab::object* ObjectDevice = EnumerationAllChannelsFromDevice(i);
				if (ObjectDevice != nullptr)
				{
					auto ObjectDeviceChannel = ObjectDevice->getAsChannel();
					if (ObjectDeviceChannel != nullptr)
					{
						float m_fFreq(0.);
						if (!ObjectDeviceChannel->isDemo())
						{
							if (auto device = ObjectDeviceChannel->getDevice())
								m_fFreq = device->getAdcFrequency();
						}
						else
							m_fFreq = 100;
			
						SetItemText(i, 2, ConvertToFreq(m_fFreq));
						ObjectDeviceChannel->SetFreq(m_fFreq);
					}
				}
			}
		}
	}
}

long CDevicesList::ConvertToCoordinates(zetlab::std::channel* channel_, CString* X, CString* Y, CString* Z, CString* P, bool strict)
{
	if (channel_ == nullptr)
		return -1;
	if (X == nullptr || Y == nullptr || Z == nullptr || P == nullptr)
		return -2;

	X->Format(_T("%f"), channel_->CoordX());
	Y->Format(_T("%f"), channel_->CoordY());
	Z->Format(_T("%f"), channel_->CoordZ());

	long m_lP(channel_->CoordP());
	//Sleep(50);
	if(!channel_->isEnabled() && !strict)
	{
		if (!channel_->is_zsp())
		{
			*X = *Y = *Z = L"-";  /// закоментировано для отображения тпи030 XYZ Ориентация
			m_lP = -2;  // /// закоментировано для отображения тпи030 XYZ Ориентация
		}
	}	

	if (m_lP != -1)
	{
		if (m_lP == -2)
			*P = L"-";
		else if ((m_lP >= 0) && (m_lP < 7))
			*P = g_sNameOrientation[m_lP];
	}
	else
		*P = g_sNo;

	DeleteLastZerosInString(*X);
	DeleteLastZerosInString(*Y);
	DeleteLastZerosInString(*Z);

	return 0;
}

CString CDevicesList::ConvertToFreq(const float& freq_)
{
	CString ret(L"");

	if (freq_ < 1000)
	{
		ret.Format(_T("%g "), freq_);
		ret += g_sUNIT_Hz;
	}
	else
	{
		ret.Format(_T("%g "), freq_ / 1000);
		ret += g_sUNIT_kHz;
	}

	return ret;
}


void CDevicesList::GetItemImage(int iItem, int &iImage)
{
	LVITEM lvi;

	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE;

	GetItem(&lvi);
	iImage = lvi.iImage;
}

void CDevicesList::SetItemImage(int iItem, int iImage)
{
	LVITEM lvi;

	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE;
	lvi.iImage = iImage;

	SetItem(&lvi);
}

static int CALLBACK Sort(zetlab::object* obj1, zetlab::object* obj2)
{
	if (!obj1 || !obj2)
		return 0;

	zetlab::object* _obj1 = obj1;
	zetlab::object* _obj2 = obj2;

	//привести к одному уровню
	if (_obj1->getLevel() != _obj2->getLevel())
	{
		while (_obj1->getLevel() != _obj2->getLevel()) 
		{
			if (_obj1->getLevel() > _obj2->getLevel())	_obj1 = _obj1->getParent();
			else										_obj2 = _obj2->getParent();
		}
	}

	//найти общего родителя
	zetlab::object* _parent1 = _obj1;
	zetlab::object* _parent2 = _obj2;
	
	bool doit = (_parent1 != _parent2);
	while (doit)
	{
		_obj1 = _parent1;
		_obj2 = _parent2;
		_parent1 = _obj1->getParent();
		_parent2 = _obj2->getParent();
		if ((_parent1 == _parent2) ||
			    _parent1->isRoot() ||
			    _parent2->isRoot())
			doit = false;
	}

	//получить родителей
	zetlab::object* _root1 = _obj1;
	zetlab::object* _root2 = _obj2;
	while (!_root1->getParent()->isRoot())
	{
		_root1 = _root1->getParent();
	}
	while (!_root2->getParent()->isRoot())
	{
		_root2 = _root2->getParent();
	}
	//при разном уровне вложенности преимущество имеет элемент с меньшим уровнем вложенность - локальзый сервер(для подключенных клиентов)
	int collevel1 = _root1->colLevel();
	int collevel2 = _root2->colLevel();
	if ((collevel1 != collevel2) && (collevel1 + collevel2 > 4))
	{
		if (collevel1 < collevel2)
			return -1;
		if (collevel1 > collevel2)
			return 1;
	}

	int answ = _obj1->compare(*_obj2);
	return answ;

}

static int CALLBACK Sort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	zetlab::object* obj1 = reinterpret_cast<zetlab::object*>(lParam1);
	zetlab::object* obj2 = reinterpret_cast<zetlab::object*>(lParam2);

	return Sort(obj1, obj2);
}

void CDevicesList::SortAll()
{
	SortItems(Sort, NULL);
}

void CDevicesList::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	CListCtrlEx::OnCustomDraw(pNMHDR, pResult);

	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		int iItem(pNMCD->nmcd.dwItemSpec);
		auto object_ = reinterpret_cast<zetlab::object*>(pNMCD->nmcd.lItemlParam);

		if (object_)
		{
			bool normal(true);

			if (object_->getAsChannel())
			{	
				if (!object_->getAsChannel()->isEnabled())
					normal = false;				
			}
			else if (object_->getAs7xxx())
			{
				auto as7xxx = object_->getAs7xxx();
				if (object_->getParent() && object_->getParent()->getAs7xxx())
					as7xxx = object_->getParent()->getAs7xxx();

				if ((!as7xxx->isEnabled()   && !as7xxx->isAcceptEnabled()) ||
					(!as7xxx->isConnected() && !as7xxx->isAccepted()))
				{
					normal = false;
				}
			}

			if (normal)
				pNMCD->clrText = GetSysColor(COLOR_WINDOWTEXT);
			else
				pNMCD->clrText = GetSysColor(COLOR_GRAYTEXT);

			auto object7xxx = object_->getAs7xxx();
			if (object7xxx)
			{
				if (object7xxx->getOffChannel() == zetlab::_7xxx::flagViewLable::flagWait)
					pNMCD->clrText = GetSysColor(COLOR_GRAYTEXT);
				else if (object7xxx->getOffChannel() == zetlab::_7xxx::flagViewLable::flagWarning)
					pNMCD->clrText = RGB(255, 153, 0);
// 				else
// 					pNMCD->clrText = GetSysColor(COLOR_WINDOWTEXT);
			}
		}
	}
}


void CDevicesList::startBlinking(zetlab::object* object_)
{
	int iItem(FindItem(object_));

	if (iItem >= 0)
	{
		iBlinkedItem = iItem;
		SetTimer(ListTimer::kBlinking, ListTimer::kBlinking, NULL);
	}
}

void CDevicesList::stopBlinking(zetlab::object* object_)
{
	KillTimer(ListTimer::kBlinking);
	int iImage(object_->getAsDevice()->getConnectionType() - 1);
	SetItemImage(iBlinkedItem, iImage);
	iBlinkedItem = -1;
}

void CDevicesList::SetNewImage(int iItem, zetlab::object* const object_)
{
	int nCurImage;
	GetItemImage(iItem, nCurImage);

	int nNewImage = object_->getImage();

	if (-1 != iBlinkedItem && iBlinkedItem == iItem)
		nNewImage = blink_on_off ? zetlab::object::image::blinked_on : zetlab::object::image::blinked_off;

	if (nNewImage != nCurImage)
		SetItemImage(iItem, nNewImage);
}

BOOL CDevicesList::OnNeedToolTip(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	LPTOOLTIPTEXT pTTT = reinterpret_cast<LPTOOLTIPTEXT>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	CPoint ptMousePos = (CPoint)GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags(0);
	
	int iItem(HitTest(ptMousePos, &uFlags));

	if (-1 == iItem)
		return FALSE;
	cstrText = (L"");
	LVHITTESTINFO ht;
	ht.pt = ptMousePos;
	ListView_SubItemHitTest(m_hWnd, &ht);
	if (ht.iSubItem != 0)
	{
		cstrText = GetItemText(iItem, ht.iSubItem);
		if (cstrText == g_sConstantIncrease)
			cstrText = g_sConstantIncrease3;
		else if (cstrText == g_sOffsetDC)
			cstrText = g_sOffsetDC2;
		else if (cstrText == g_sAmplifier)
			cstrText = g_sConstantIncrease2;

		
		pTTT->lpszText = (LPTSTR)(LPCTSTR)cstrText;
	}
	else
	{
		zetlab::object* object_ = reinterpret_cast<zetlab::object*>(GetItemData(iItem));

		if (object_)
		{
			if (object_->getAsChannel())
			{
				auto channels = object_->getAsChannel();
				std::wstring  wStrCommentary = channels->getComment();
				cstrText = wStrCommentary.c_str();

				if (cstrText == L"⁣")// спец символ. не трогать
					cstrText = L"";

				long index_p = channels->CoordP();
				if (channels->CoordX() != 0 || channels->CoordY() != 0 ||
					channels->CoordZ() != 0 || (index_p >= 0 && index_p < 7))
				{
					CString Position = g_sNameOrientation[index_p];

					cstrText.Append(L" " + g_sCoordinate + L":");
					cstrText.AppendFormat(L" [X: %0.3f, Y: %0.3f, Z: %0.3f, P: %s]", channels->CoordX(), channels->CoordY(), channels->CoordZ(), Position.GetString());
				}
			}
			else if (object_->getAsDevice())
			{
				auto device = object_->getAsDevice();
				// Калибровка
				if (!device->GetCalibrationDevice())
				{
					cstrText = g_sCalibrationDevice;
				}
				/*
				auto device_ = object_->getAsDevice();
				//L"%s %s\nЧастота дискретизации %g Гц"
				cstrText.Format(L"%s %s\n" + g_sSamplingFrequency + L" %g " + g_sUNIT_Hz,
					device_->getTypeStr(), object_->getName().c_str(),
					device_->getAdcFrequency() );

				if (device_->isHaveEth())
				{
					unsigned char ip[4];
					unsigned char mock1[6];
					unsigned short mock2;

					if (device_->readEthSettings( ip, mock1, mock1, mock2, mock1, mock2, mock1[0], mock1[1] ))
					{//L"\nIP-адрес: %i.%i.%i.%i"
						cstrText.AppendFormat(L"\n" + g_sIPAddress + L":" + L" %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
					}
				}*/
			}
			else if (object_->getAs7xxx())
			{
				auto device_ = object_->getAs7xxx();

				if (object_->getType() == zetlab::object_type::_7xxx_controller)
				{
					std::wstring ip_str;
					if (device_->getIpStr(ip_str))
					{
						CString check(ip_str.c_str());
						if (!check.IsEmpty())
						{
							CString state_str;
							switch (device_->getState())
							{
							case zetlab::_7xxx::device_state::connecting:
								state_str = g_sConnecting;
								break;
							case zetlab::_7xxx::device_state::connected:
								state_str = g_sConnected;
								break;
							case zetlab::_7xxx::device_state::accepted:
								state_str = g_sAccepted;
								break;
							case zetlab::_7xxx::device_state::busy:
								state_str = g_sBusy;
								break;
							case zetlab::_7xxx::device_state::conflict:
								state_str = g_sIPConflict;
								break;
							default:
								state_str = g_sAvailable;
								break;
							}

							std::wstring PeerIpv4;
							bool ret = device_->getPeerIpv4(PeerIpv4);
							// У листа с всплывающей подсказкой вообще беда, pTTT->lpszText должен закончиться\n, 
							// поэтому мы сами этот символ писать не можем
							// Обратите внимание L" " тут не пробел, а скрытый символ, потому что подсказка будет переносить по пробелу
							int maxWidth(0);
							CString sTypeAndSerial(device_->getDeviceName().c_str()); 
							sTypeAndSerial += L" "; 
							sTypeAndSerial += L"№"; 
							sTypeAndSerial += zetlab::utils::hex(device_->getDeviceSerial()).c_str();
							
							CString sIpAdress(L"IP"); 
							sIpAdress += L" "; 
							sIpAdress += g_sAddress; 
							sIpAdress += L":"; 
							sIpAdress += L" "; 
							sIpAdress += ip_str.c_str();

							CString sStatus(g_sStatus); sStatus += L":"; sStatus += " "; sStatus += L"[";
							if (device_->getState() == zetlab::_7xxx::device_state::busy)
							{
								if (ret)
								{
									sStatus += state_str;
									sStatus += L" ";
									sStatus += PeerIpv4.c_str();
								}
								else
									sStatus += g_BusyAnotherPC;
							}
							else
								sStatus += state_str;

							sStatus += L"]";

							SIZE sz; CPaintDC dc(this);

							//Сначала найдем ширину нашей подсказки
							GetTextExtentPoint32W(dc.GetSafeHdc(), sTypeAndSerial, sTypeAndSerial.GetLength(), &sz);
							if (sz.cx > maxWidth)
								maxWidth = sz.cx;
							GetTextExtentPoint32W(dc.GetSafeHdc(), sIpAdress, sIpAdress.GetLength(), &sz);
							if (sz.cx > maxWidth)
								maxWidth = sz.cx;
							GetTextExtentPoint32W(dc.GetSafeHdc(), sStatus, sStatus.GetLength(), &sz);
							if (sz.cx > maxWidth)
								maxWidth = sz.cx;

							// Кому не хватает добъем пробелами, чтобы след. строка перешла на новую строку
							GetTextExtentPoint32W(dc.GetSafeHdc(), sTypeAndSerial, sTypeAndSerial.GetLength(), &sz);
							if (sz.cx < maxWidth)
							{
								auto diff(maxWidth - sz.cx);
								for (int i = 0; i <= ceil(diff / 4); ++i)
									sTypeAndSerial += L" ";
							}
							GetTextExtentPoint32W(dc.GetSafeHdc(), sIpAdress, sIpAdress.GetLength(), &sz);
							if (sz.cx < maxWidth)
							{
								auto diff(maxWidth - sz.cx);
								for (int i = 0; i <= ceil(diff / 4); ++i)
									sIpAdress += L" ";
							}
							GetTextExtentPoint32W(dc.GetSafeHdc(), sStatus, sStatus.GetLength(), &sz);
							if (sz.cx < maxWidth)
							{
								auto diff(maxWidth - sz.cx);
								for (int i = 0; i <= ceil(diff / 4); ++i)
									sStatus += L" ";
							}

							cstrText.Format(L"%s %s %s ", sTypeAndSerial.GetString(), sIpAdress.GetString(), sStatus.GetString());
							CToolTipCtrl* pToolTip = GetToolTips();
							if (pToolTip)
								pToolTip->SetMaxTipWidth(maxWidth);
						}
						else
						{
							auto name = device_->getDeviceName();
							auto serial = device_->getDeviceSerial();
							if (!name.empty() && serial != 0)
								cstrText.Format(L"%s %s%s", name.c_str(), g_sNumber.GetString(), zetlab::utils::hex(serial).c_str());
						}
					}
					else
					{
						auto name = device_->getDeviceName();
						auto serial = device_->getDeviceSerial();
						if (!name.empty() && serial != 0)
							cstrText.Format(L"%s %s%s", name.c_str(), g_sNumber.GetString(), zetlab::utils::hex(serial).c_str());
					}
				}
				else
				{
					auto name = device_->getDeviceName();
					auto serial = device_->getDeviceSerial();
					if (!name.empty() && serial != 0)
						cstrText.Format(L"%s %s%s", name.c_str(), g_sNumber.GetString(), zetlab::utils::hex(serial).c_str());
				}
			}
			pTTT->lpszText = (LPTSTR)(LPCTSTR)cstrText;
		}
	}

	return TRUE;
}

void CDevicesList::SetConnecting(bool started)
{
	if (started)
	{
		_connectingImage = zetlab::object::image::connecting1;
		UpdateConnecting();
		SetTimer(ListTimer::kConnecting, ListTimer::kConnecting, NULL);
	}
	else
	{
		KillTimer(ListTimer::kConnecting);
	}
}

void CDevicesList::UpdateConnecting()
{
	for (int iItem = 0; iItem < GetItemCount(); ++iItem)
	{
		auto object_ = reinterpret_cast<zetlab::object*>(GetItemData(iItem));
		if (object_)
		{
			auto as7xxx = object_->getAs7xxx();
			if (as7xxx && as7xxx->isConnecting())
				SetItemImage(iItem, _connectingImage);
		}
	}

	++_connectingImage;
	if (_connectingImage > zetlab::object::image::connecting4)
		_connectingImage = zetlab::object::image::connecting1;
}

void CDevicesList::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ListTimer::kBlinking:
		if (-1 != iBlinkedItem)
		{
			blink_on_off = !blink_on_off;

			if (blink_on_off)
				SetItemImage(iBlinkedItem, 7);
			else
				SetItemImage(iBlinkedItem, 8);
		}
		break;

	case ListTimer::kConnecting:
		UpdateConnecting();
		break;
	case ListTimer::updateIcon:// Данный таймер, можно расширить на проверку других параметров
		for (auto i = 0; i < GetItemCount(); ++i)
		{
			auto object_ = reinterpret_cast<zetlab::object*>(GetItemData(i));
			if (object_)
			{
				int nCurImage; GetItemImage(i, nCurImage);
				if (object_->getImage() != nCurImage)
					SetItemImage(i, object_->getImage());
			}
		}
		break;
	}
}

void CDevicesList::SendErrorInfo(long error_, const zetlab::object* object_, bool arose_) const
{
	auto as7xxx = object_->getAs7xxx();
	if (!as7xxx)
		return;

	auto zetlab_hwnd = FindWindowFromProcessID(IsProcess(L"ZETLab.exe", nullptr));
	if (!zetlab_hwnd)
		return;

	ErrorInfoStruct ei;
	ZeroMemory(&ei, sizeof(ei));
	ei.Struct_Size = sizeof(ei);
	ei.Error_Sourse_ID = Other_Error;
	wcscpy_s(ei.Error_Sourse_Name, L"DeviceManager.exe");
	ei.Is7xxxDevice = true;
	wcscpy_s(ei.Device_Name, as7xxx->getDeviceName().c_str());
	ei.Device_Type = as7xxx->getDeviceType();
	ei.Device_Serial = as7xxx->getDeviceSerial();
	ei.Error_Number = error_;
	ei.Error_Arose = arose_;

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = sizeof(ei);
	cds.lpData = &ei;
	::SendMessage(zetlab_hwnd, WM_COPYDATA, (WPARAM)zetlab_hwnd, (LPARAM)&cds);
}

// Для поиска каналов одного Устройства
zetlab::object* CDevicesList::EnumerationAllChannelsFromDevice(int Item)
{
	zetlab::object* object_(nullptr);
	return reinterpret_cast<zetlab::object*>(GetItemData(Item));
}

void CDevicesList::ChangeName(zetlab::object* const object_, bool isModify)
{
	int iItem(FindItem(object_));

	if (iItem < 0)
		return;

	if (object_->getAs7xxx())
	{		
		std::wstring nameCur(GetItemText(iItem, NULL));

		std::wstring nameNew(object_->GetNameModify(nameCur, isModify));		

		if (GetItemText(iItem, NULL) != nameNew.c_str())
			SetItemText(iItem, NULL, nameNew.c_str());
	}
}

void CDevicesList::ChangeNameAll(zetlab::object* const objectParent_, bool isModify)
{
	int iItem(FindItem(objectParent_));

	std::wstring nameCur(_T(""));
	std::wstring nameNew(_T(""));

	auto changeItemText = [&](int iItem){
		nameCur = GetItemText(iItem, NULL);
		nameNew = objectParent_->GetNameModify(nameCur, isModify);

		if (GetItemText(iItem, NULL) != nameNew.c_str())
			SetItemText(iItem, NULL, nameNew.c_str());
	};

	if (objectParent_->getAs7xxx())
	{		
		changeItemText(iItem);

		objectParent_->SetNameChildren(boost::bind(&CDevicesList::FindItem, this, boost::placeholders::_1), changeItemText);
	}
}


