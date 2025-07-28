#include "stdafx.h"
#include "../../../application/application.h"
#include "../../../algo/utils/hex/hex.hpp"
#include "../../user_messages.hpp"
#include "tree.h"
#include "resource\Phrases.h"
#include "ZetDeviceSort.h"

extern CString  g_sNameOrientation[7];

enum TreeTimer : UINT
{
	kBlinking = 500,
	kConnecting = 100,
	updateIcon = 101
};

BEGIN_MESSAGE_MAP(CDevicesTree, CTreeCtrlEx)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(TVN_GETINFOTIP, OnNeedToolTip)
END_MESSAGE_MAP()

void CDevicesTree::LoadItemImages()
{
	SetItemHeight(m_dpiAware.ScaleY(22));

	m_imageList.Detach();
	m_imageList.Create(m_dpiAware.ScaleX(16), m_dpiAware.ScaleY(16), ILC_COLOR32, 10, 1);

	// Load icons
	auto select_icon = [](int image_index) {
		using image = zetlab::object::image;
		switch (image_index)
		{
		case image::pci: return IDI_PCI;
		case image::usb: return IDI_USB;
		case image::ethernet: return IDI_ETHERNET;
		case image::channel_enabled: return IDI_CHANNEL_ENABLED;
		case image::channel_icp: return IDI_CHANNEL_ICP;
		case image::channel_disabled: return IDI_CHANNEL_DISABLED;
		case image::_7xxx: return IDI_7XXX;
		case image::blinked_on: return IDI_BLINKED_ON;
		case image::blinked_off: return IDI_BLINKED_OFF;
		case image::demo: return IDI_DEMO;
		case image::ethernet_locked: return IDI_ETHERNET_LOCKED;
		case image::ethernet_error: return IDI_ETHERNET_ERROR;
		case image::channel_charge: return IDI_CHANNEL_CHARGE;
		case image::channel_service: return IDI_CHANNEL_SERVICE;
		case image::generator: return IDI_GENERATOR_ON;
		case image::sdcard: return IDI_SD_CARD;
		case image::connecting1: return IDI_CONNECTING1;
		case image::connecting2: return IDI_CONNECTING2;
		case image::connecting3: return IDI_CONNECTING3;
		case image::connecting4: return IDI_CONNECTING4;
		//case image::ethernet_connect: return IDI_ETHERNET_CONNECT;
		//case image::ethernet_accepted: return IDI_ETHERNET_ACCEPT;
		case image::ethernet_warning: return IDI_ETHERNET_WARNING;
		case image::usb_warning: return IDI_USB_WARNING;
		}
		return IDI_DEMO;
	};
	while (m_imageList.GetImageCount() < zetlab::object::image::count)
		m_imageList.Add(LOAD_ICON(select_icon(m_imageList.GetImageCount()), m_dpiAware.ScaleX(16), m_dpiAware.ScaleY(16)));

	SetImageList(&m_imageList, TVSIL_NORMAL);
}

void CDevicesTree::Add(zetlab::object* object_, bool isSelect /* = false*/)
{
	HTREEITEM hParent = TVI_ROOT;  //TVI_ROOT; корень дерева

	auto parent_object = object_->getParent();     // Если у объекта есть родитель и он не является корневым, ищется соответствующий HTREEITEM для родителя
	if (parent_object && !parent_object->isRoot()) //  Иначе используется корень дерева(TVI_ROOT)
		hParent = FindItem(parent_object, GetRootItem());
	
	if (hParent)
	{
		/*Создаётся элемент дерева с :
		Текстом(TVIF_TEXT) - имя объекта
		Иконками(TVIF_IMAGE | TVIF_SELECTEDIMAGE)
		Привязкой данных(TVIF_PARAM) - указатель на объект сохраняется в LPARAM
		Элемент добавляется в конец списка дочерних элементов(TVI_LAST)*/
		auto image = object_->getImage();
		//CString addname;
		//addname.Format(L" %d", object_->getLevel());
		auto name = object_->getNameForShow();//object_->getName().c_str();// +addname; // имя обьекта
		HTREEITEM hItem = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM,
			name,
			image, image,
			0, 0,
			reinterpret_cast<LPARAM>(object_),
			hParent,
			TVI_SORT);	 // Добавляет элемент в конец дочерних узлов 	 
		Expand(hParent, TVE_EXPAND);  //Родительский узел автоматически раскрывается для показа нового элемента
		//SortAll();
		if (isSelect)
			SelectItem(object_);
	}

	SetTimer(TreeTimer::updateIcon, 1000, NULL);
}


void CDevicesTree::AddRow037(zetlab::object* object_, bool isSelect /* = false*/)
{

	HTREEITEM hParent = TVI_ROOT;  //TVI_ROOT; корень дерева

	auto parent_object = object_->getParent();     // Если у объекта есть родитель и он не является корневым, ищется соответствующий HTREEITEM для родителя
	if (parent_object && !parent_object->isRoot()) //  Иначе используется корень дерева(TVI_ROOT)
		hParent = FindItem(parent_object, GetRootItem());
	//hParent = 0;
	if (hParent)
	{
		/*Создаётся элемент дерева с :
		Текстом(TVIF_TEXT) - имя объекта
		Иконками(TVIF_IMAGE | TVIF_SELECTEDIMAGE)
		Привязкой данных(TVIF_PARAM) - указатель на объект сохраняется в LPARAM
		Элемент добавляется в конец списка дочерних элементов(TVI_LAST)*/
		auto image = object_->getImage();
		//CString addname;
		//addname.Format(L" %d", object_->getLevel());
		auto name = object_->getNameForShow();//object_->getName().c_str();// +addname; // имя обьекта
		HTREEITEM hItem = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM,
			name,
			image, image,
			0, 0,
			reinterpret_cast<LPARAM>(object_),
			hParent,
			TVI_SORT);	 // Добавляет элемент в конец дочерних узлов 	 
		Expand(hParent, TVE_EXPAND);  //Родительский узел автоматически раскрывается для показа нового элемента
		//SortAll();
		if (isSelect)
			SelectItem(object_);
	}

	SetTimer(TreeTimer::updateIcon, 1000, NULL);

}

void CDevicesTree::Remove(zetlab::object* object_)
{
	if (GetCount() == 0)
		return;

	auto hItem = FindItem(object_, GetRootItem());

	if (hItem)
		DeleteItem(hItem);
}

void CDevicesTree::Update(zetlab::object* object_)
{
	UpdateDeviceInfo(object_);
	UpdateChannelInfo(object_);
	Invalidate();
}


HTREEITEM CDevicesTree::FindItem(zetlab::object* const object_, HTREEITEM startItem)  // Рекурсивный поиск элемента дерева по связанному объекту.
{
	HTREEITEM hItem(startItem);

	while (hItem)
	{
		if (object_ == nullptr)
			return hItem;
			

		if (object_ == reinterpret_cast<zetlab::object*>(GetItemData(hItem)))
			return hItem;

		if (ItemHasChildren(hItem))
		{
			HTREEITEM hTmp(FindItem(object_, GetChildItem(hItem)));
			if (hTmp)
				return hTmp;
		}

		hItem = GetNextItem(hItem, TVGN_NEXT);
	}

	return nullptr;
}

zetlab::object* CDevicesTree::GetSelectedItemObject()
{
	HTREEITEM hItem(GetSelectedItem());

	if (NULL == hItem)
		return nullptr;

	return reinterpret_cast<zetlab::object*>(GetItemData(hItem));
}

HTREEITEM CDevicesTree::GetSelectedHtreeItem()
{
	HTREEITEM hItem(GetSelectedItem());
	return hItem;
}

void CDevicesTree::SelectItem(zetlab::object* const object_)
{
	HTREEITEM hItem(FindItem(object_, GetRootItem()));

	if (hItem)
		CTreeCtrlEx::SelectItem(hItem);
}

void CDevicesTree::UpdateDeviceInfo(zetlab::object* const object_)
{
	HTREEITEM hItem(FindItem(object_, GetRootItem()));

	if (!hItem)
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
			zetlab::tstring description;
			device_->readDescription(description);
			object_->SetLabel(description.c_str());
		}
		CString name = object_->getNameForShow();
		SetItemText(hItem, name);
	}
	else if (object_->getParent() && object_->getParent()->getAs7xxx())
	{
		if (object_->getParent()->getAs7xxx()->isConnecting())
			update_image = false;
	}
	else if (as7xxx)
	{
		//auto as7xxx = object_->getAs7xxx();

		if (object_->getType() == zetlab::object_type::_7xxx_controller && !object_->isModify())
		{
			CString name = object_->getNameForShow(); //object_->getName().c_str();
// 			zetlab::tstring label;
// 			if (as7xxx->getLabel(label) && !label.empty())
// 				name.AppendFormat(L" (%s)", label.c_str());
			if (GetItemText(hItem) != name)
				SetItemText(hItem, name);
		}
		if (as7xxx->isConnecting())
			update_image = false;
	}

	if (update_image)
		SetNewImage(hItem, object_);
}

void CDevicesTree::UpdateChannelInfo(zetlab::object* const object_)
{
	HTREEITEM hItem(FindItem(object_, GetRootItem()));

	if (!hItem)
		return;

	zetlab::std::channel* channel = object_->getAsChannel();

	if (nullptr == channel)		
		return;

	SetNewImage(hItem, object_);
}

static int CALLBACK Sort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	zetlab::object* obj1 = reinterpret_cast<zetlab::object*>(lParam1);
	zetlab::object* obj2 = reinterpret_cast<zetlab::object*>(lParam2);

	
	//для подключенных клиентов
	zetlab::object* parent1 = obj1;
	zetlab::object* parent2 = obj2;
	while (parent1->getParent() != nullptr && !parent1->getParent()->isRoot())
		parent1 = parent1->getParent();
	while (parent2->getParent() != nullptr && !parent2->getParent()->isRoot())
		parent2 = parent2->getParent();

	int collevel1 = parent1->colLevel();
	int collevel2 = parent2->colLevel();
	if ((collevel1 != collevel2) && (collevel1 + collevel2 > 4))
	{
		if (collevel1 < collevel2)
			return -1;
		if (collevel1 > collevel2)
			return 1;
	}
	
	if (obj1 && obj2)
		return obj1->compare(*obj2);

	return 0;
}

void CDevicesTree::SortItems(HTREEITEM hItem)
{
	TVSORTCB tvs;

	tvs.hParent = hItem;
	tvs.lpfnCompare = Sort;
	tvs.lParam = NULL;

	SortChildrenCB(&tvs);
}

void CDevicesTree::SortAll(HTREEITEM startItem)
{
	//               Windows XP fix
	//HTREEITEM hItem(startItem == TVI_ROOT ? GetRootItem() : startItem);

	//while (hItem)
	//{
	//	if (ItemHasChildren(hItem))
	//	{
	//		SortItems(hItem);
	//		SortAll(GetChildItem(hItem));
	//	}

	//	hItem = GetNextItem(hItem, TVGN_NEXT);
	//}
	if (startItem != NULL)
	{
		if (startItem == TVI_ROOT || ItemHasChildren(startItem))
		{
			HTREEITEM child = GetChildItem(startItem);

			while (child != NULL)
			{
				SortAll(child);
				child = GetNextItem(child, TVGN_NEXT);
			}

			SortItems(startItem);
		}
	}
	
}

void CDevicesTree::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
	{
		HTREEITEM hItem(*reinterpret_cast<HTREEITEM*>(&pNMCD->nmcd.dwItemSpec));
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
			{
				pNMCD->clrText = GetSysColor((pNMCD->nmcd.uItemState & CDIS_FOCUS) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
			}
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
	break;
	}
}

void CDevicesTree::OnNeedToolTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVGETINFOTIP pNMIT = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	if (pNMIT->hItem)
	{
		CString cstrText(L"");
		zetlab::object* object_(nullptr);
		object_ = reinterpret_cast<zetlab::object*>(pNMIT->lParam);
		if (object_)
		{
			if (object_->getAsChannel())
			{
				auto channels = object_->getAsChannel();
				std::wstring  wStrCommentary =  channels->getComment();
				cstrText = wStrCommentary.c_str();

				if (cstrText == L"⁣")// спец символ. не трогать
					cstrText = L"";

				long index_p = channels->CoordP();
				if (channels->CoordX() != 0 || channels->CoordY() != 0 ||
					channels->CoordZ() != 0 || (index_p >= 0 && index_p < 7))
				{
					CString Position = g_sNameOrientation[index_p];

					if (!cstrText.IsEmpty())
						cstrText.Append(L"\n");

					cstrText.Append(g_sCoordinate + L":");
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

								if (device_->isHaveEth())
								{
									unsigned char ip[4];
									unsigned char mock1[6];
									unsigned short mock2;

									if (device_->readEthSettings( ip, mock1, mock1, mock2, mock1, mock2, mock1[0], mock1[1] ))
										cstrText.Format(L"%s %i.%i.%i.%i", g_IDS_TT_STD_DEVICE_IP, ip[0], ip[1], ip[2], ip[3]);
								}
				*/
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
							auto name = device_->getDeviceName();
							auto serial = device_->getDeviceSerial();
							if (!name.empty() && serial != 0)
							{
								if (device_->getState() == zetlab::_7xxx::device_state::busy)
								{
									if (ret)
										cstrText.Format(L"%s №%s \n%s %s\n%s: [%s %s]", name.c_str(), zetlab::utils::hex(serial).c_str(), (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), g_sBusy.GetString(), PeerIpv4.c_str());
									else
										cstrText.Format(L"%s №%s \n%s %s\n%s: [%s]", name.c_str(), zetlab::utils::hex(serial).c_str(), (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), g_BusyAnotherPC.GetString());
								}
								else
									cstrText.Format(L"%s №%s \n%s %s\n%s: [%s]", name.c_str(), zetlab::utils::hex(serial).c_str(), (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), state_str.GetString());
							}
							else
							{
								if (device_->getState() == zetlab::_7xxx::device_state::busy)
								{
									if (ret)
										cstrText.Format(L"%s №%s \n%s %s\n%s: [%s %s]", name.c_str(), zetlab::utils::hex(serial).c_str(), (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), g_sBusy.GetString(), PeerIpv4.c_str());
									else
										cstrText.Format(L"%s №%s \n%s %s\n%s: [%s]", name.c_str(), zetlab::utils::hex(serial).c_str(), (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), g_BusyAnotherPC.GetString());
								}
								else
									cstrText.Format(L"%s %s\n%s: [%s]", (L"IP " + g_sAddress + L":").GetString(), ip_str.c_str(), g_sStatus.GetString(), state_str.GetString());
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
				}
				else
				{
					auto name = device_->getDeviceName();
					auto serial = device_->getDeviceSerial();
					if (!name.empty() && serial != 0)
						cstrText.Format(L"%s %s%s", name.c_str(), g_sNumber.GetString(), zetlab::utils::hex(serial).c_str());
				}
			}

			_tcscpy_s(pNMIT->pszText, pNMIT->cchTextMax, cstrText.GetBuffer());
			cstrText.ReleaseBuffer();
		}
	}
}

void CDevicesTree::SetConnecting(bool started)
{
	if (started)
	{
		_connectingImage = zetlab::object::image::connecting1;
		UpdateConnecting();
		SetTimer(TreeTimer::kConnecting, TreeTimer::kConnecting, NULL);
	}
	else
	{
		KillTimer(TreeTimer::kConnecting);
	}
}

void CDevicesTree::UpdateConnecting()
{
	for (auto hItem = GetFirstVisibleItem(); hItem; hItem = GetNextVisibleItem(hItem))
	{
		auto object_ = reinterpret_cast<zetlab::object*>(GetItemData(hItem));
		if (object_)
		{
			auto as7xxx = object_->getAs7xxx();
			if (as7xxx && as7xxx->isConnecting())
				SetItemImage(hItem, _connectingImage, _connectingImage);
		}
	}

	++_connectingImage;
	if (_connectingImage > zetlab::object::image::connecting4)
		_connectingImage = zetlab::object::image::connecting1;
}

void CDevicesTree::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TreeTimer::kBlinking:
		if (hBlinkedItem)
		{
			blink_on_off = !blink_on_off;
			int image = blink_on_off ? zetlab::object::image::blinked_on : zetlab::object::image::blinked_off;

			SetItemImage(hBlinkedItem, image, image);
		}
		break;

	case TreeTimer::kConnecting:
		UpdateConnecting();
		break;
	case TreeTimer::updateIcon:
		for (auto hItem = GetFirstVisibleItem(); hItem; hItem = GetNextVisibleItem(hItem))
		{
			auto object_ = reinterpret_cast<zetlab::object*>(GetItemData(hItem));
			if (object_)
			{
				int nCurImage, nCurSelImage; GetItemImage(hItem, nCurImage, nCurSelImage);
				if (object_->getImage() != nCurImage)
					SetItemImage(hItem, object_->getImage(), object_->getImage());
			}
		}
		break;
	}

	CTreeCtrl::OnTimer(nIDEvent);
}

void CDevicesTree::startBlinking(zetlab::object* object_)
{
	HTREEITEM hItem(FindItem(object_, GetRootItem()));

	if (hItem)
	{
		hBlinkedItem = hItem;
		SetTimer(TreeTimer::kBlinking, TreeTimer::kBlinking, NULL);
	}
}

void CDevicesTree::stopBlinking(zetlab::object* object_)
{
	KillTimer(TreeTimer::kBlinking);
	int iImage(object_->getImage());
	SetItemImage(hBlinkedItem, iImage, iImage);
	hBlinkedItem = NULL;
}

void CDevicesTree::SetNewImage(HTREEITEM hItem, zetlab::object* const object_)
{
	int nCurImage, nCurSelImage;
	GetItemImage(hItem, nCurImage, nCurSelImage);

	int nNewImage = object_->getImage();

	if (hBlinkedItem && hBlinkedItem == hItem)
		nNewImage = blink_on_off ? zetlab::object::image::blinked_on : zetlab::object::image::blinked_off;

	if ((nNewImage != nCurImage) || (nNewImage != nCurSelImage))
		SetItemImage(hItem, nNewImage, nNewImage);
}

void CDevicesTree::ChangeName(zetlab::object* const object_, bool isModify)
{
	HTREEITEM hItem(FindItem(object_, GetRootItem()));

	if (object_->getAs7xxx())
	{	
// 		auto parent = object_->getParent();	
// 		if (parent)
// 			ChangeName(object_->getParent(), isModify);
// 		else
// 			return;

// 		if (!object_->isNeedChangeNameParent())
// 			return;

		std::wstring nameCur(GetItemText(hItem));		
		
		std::wstring nameNew(object_->GetNameModify(nameCur, isModify));		
				
		if (GetItemText(hItem) != nameNew.c_str())
			SetItemText(hItem, nameNew.c_str());
	}
}

void CDevicesTree::ChangeNameAll(zetlab::object* const objectParent_, bool isModify)
{
	HTREEITEM hItem(FindItem(objectParent_, GetRootItem()));

	std::wstring nameCur(_T(""));
	std::wstring nameNew(_T(""));

	auto changeItemText = [&](HTREEITEM hItem){
		nameCur = GetItemText(hItem);
		nameNew = objectParent_->GetNameModify(nameCur, isModify);

		if (GetItemText(hItem) != nameNew.c_str())
			SetItemText(hItem, nameNew.c_str());
	};

	if (objectParent_->getAs7xxx())
	{
		changeItemText(hItem);

		HTREEITEM hItemChild(GetChildItem(hItem));
		while (hItemChild != nullptr)
		{
			changeItemText(hItemChild);
			hItemChild = GetNextSiblingItem(hItemChild);
		}
	}
}
