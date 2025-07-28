#include "stdafx.h"
#include <Localized/Localized.h>
#include "ÑTreeForCompare.h"
#include "afxdialogex.h"
#include "source/gui/properties/properties_dlg_common.h"

namespace zetlab {
//-----------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(ÑTreeForCompare, CDialogEx)
//-----------------------------------------------------------------------------------------------------------------------------
ÑTreeForCompare::ÑTreeForCompare(CWnd* pParent /*=NULL*/)
	: CDialogEx(ÑTreeForCompare::IDD, pParent)
	, m_pathToXml(_T(""))
	, _root(boost::make_shared< object >(object_type::undefined, _T("")))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TREE);
}
//-----------------------------------------------------------------------------------------------------------------------------
ÑTreeForCompare::~ÑTreeForCompare()
{
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}
//-----------------------------------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(ÑTreeForCompare, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SIZING()

	ON_BN_CLICKED(IDOK, &ÑTreeForCompare::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ÑTreeForCompare::OnBnClickedCancel)

	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &ÑTreeForCompare::OnNMDblclkTree1)

	//	ON_COMMAND(ID_SAVE_ALL_PROPERTIES,	&ÑTreeForCompare::OnPropertiesSaveAll)
	ON_COMMAND(ID_SAVE_PROPERTIES, &ÑTreeForCompare::OnPropertiesSave)
	ON_COMMAND(ID_PROPERTIES, &ÑTreeForCompare::OnPropertiesRequest)
END_MESSAGE_MAP()
//-----------------------------------------------------------------------------------------------------------------------------
BOOL ÑTreeForCompare::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_btnOk.ShowWindow(SW_HIDE);
	m_btnCancel.ShowWindow(SW_HIDE);

	CString sDebug(_T(""));
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif
	SetWindowTextW(sDebug + g_sTreeDevices);

	bool isOk(true);
	initResizer();
	updateTree();

	m_tree.OnPrepareContextMenu(boost::bind(&ÑTreeForCompare::PrepareContextMenu, this, _1));

	return TRUE;
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnBnClickedOk()
{
	CDialogEx::OnOK();
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void ÑTreeForCompare::OnSizing(UINT wParam, LPRECT pRect)
{
	if (pRect->right - pRect->left < widthMainWindow)
		if (wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_TOPRIGHT)
			pRect->right = pRect->left + widthMainWindow;
		else
			pRect->left = pRect->right - widthMainWindow;

	if (pRect->bottom - pRect->top < sizeWindow::heightMainWindow)
		if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_BOTTOMLEFT)
			pRect->bottom = pRect->top + heightMainWindow;
		else
			pRect->top = pRect->bottom - heightMainWindow;
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}
//-----------------------------------------------------------------------------------------------------------------------------
CString ÑTreeForCompare::getPathXml()
{
	return m_pathToXml;
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::setPathXml(const wchar_t* sPath)
{
	m_pathToXml = sPath;
	// ñðàçó îáîçíà÷èì ïóòü äëÿ ðàáîòû ñ êîíôèãóðàöèîííûìè ôàéëàìè óñòðîéñòâ
	CString sPathCfg(m_pathToXml.Left(m_pathToXml.ReverseFind(_T('\\'))));
	sPathCfg.Append(L"\\Configs");
	m_cfgWorker.setPathCfgDevice(sPathCfg);
}
//-----------------------------------------------------------------------------------------------------------------------------
bool ÑTreeForCompare::updateTree()
{
	bool isOK(true);

	tstring sName(_T(""));
	CString sNode(_T(""));

	clearTreeForUpdate();

	m_xmlWorker.getMapsForBuildTree(m_pathToXml);

	if (!m_xmlWorker.m_mapDeviceForTree.empty())
	{
		for (auto& it : m_xmlWorker.m_mapDeviceForTree)
		{
			sName = m_xmlWorker.getGroupName(it.first._serialNumber);
			object* parent = add(sName, _root.get(), object_type::_7xxx_controller);
			parent->getAs7xxx()->setDeviceSerial(it.first._serialNumber);

			// äîáàâëÿåì äåòåé 
			if (!it.second.empty())
			{
				for (auto& itChild : it.second)
				{
					// áåðåì èíôó ïî êàíàëàì
					auto itMapWithChannels = m_xmlWorker.m_mapDeviceWithChannel.find(itChild);
					if (!itMapWithChannels->second.empty())
					{
						// äîáàâëÿåì êàíàëû â äåðåâî
						for (auto& itChannel : itMapWithChannels->second)
						{
							sNode.Format(_T(" (%d)"), itChannel._node);
							sName = itChannel._name + sNode;
							add(sName, parent, object_type::_7xxx_is)->getAs7xxx()->setDeviceSerial(itChild._serialNumber);
						}
					}
				}
			}
		}
	}

	return isOK;
}
//-----------------------------------------------------------------------------------------------------------------------------
bool ÑTreeForCompare::initResizer()
{
	BOOL isOk(TRUE);

	isOk = m_resizer.Hook(this);
	ASSERT(isOk && _T("m_resizer.Hook"));

	CUIntArray arrayID;
	arrayID.Add(IDOK);
	arrayID.Add(IDCANCEL);

	isOk &= m_resizer.CreatePanel(_T("Panel_ok_cancel"), &arrayID, TRUE);
	isOk &= m_resizer.SetAnchor(_T("Panel_ok_cancel"), ANCHOR_RIGHT | ANCHOR_BOTTOM);
	ASSERT(isOk && _T("initStatisticsTable: Panel_ok_cancel"));
	arrayID.RemoveAll();

	isOk &= m_resizer.CreatePanel(IDC_TREE1);
	isOk &= m_resizer.SetAnchor(IDC_TREE1, ANCHOR_ALL);
	ASSERT(isOk && _T("initStatisticsTable: IDC_TREE1"));

	isOk &= m_resizer.InvokeOnResized();
	ASSERT(isOk && _T("initStatisticsTable: InvokeOnResized"));

	return isOk == TRUE;
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;

	zetlab::object* object_(m_tree.GetSelectedItemObject());

	if (nullptr == object_)
		return;

	OnPropertiesRequest();

	if (object_->isHaveChild())
		m_tree.Expand(m_tree.GetSelectedHtreeItem(), TVE_TOGGLE);
}
//-----------------------------------------------------------------------------------------------------------------------------
object* const ÑTreeForCompare::add(const tstring& full_path, object* parent, object_type::values type)
{
	if (exists(full_path))
		return _objects.find(full_path)->second.get();

	object_ptr new_object;

	new_object = boost::make_shared< object >(type, full_path, parent);
	parent->addChild(new_object);
	_objects.insert(objects_pair(full_path, new_object));

	m_tree.Add(new_object.get());

	return new_object.get();
}
//-----------------------------------------------------------------------------------------------------------------------------
const bool ÑTreeForCompare::exists(const tstring& full_path) const
{
	return (_objects.end() != _objects.find(full_path));
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::clearTreeForUpdate()
{
	m_tree.DeleteAllItems();
	_objects.clear();
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::PrepareContextMenu(CMenu** context_menu)
{
	m_contextMenu.DestroyMenu();

	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	CString s(_T(""));

	UINT default_command = ID_PROPERTIES;
	CMenu* menu = nullptr;

	// Óäàëåíèå ïóíêòà ìåíþ è ðàçäåëèòåëÿ ïîñëå íåãî (ïðè íàëè÷èè)
	auto RemoveMenuByCommandWithSeparator = [&](UINT id)
	{
		auto count = menu->GetMenuItemCount();
		for (int i = 0; i < count; ++i)
		{
			if (menu->GetMenuItemID(i) == id)
			{
				// Ïðîâåðÿåì, åñòü ëè ðàçäåëèòåëü ïîñëå íàéäåííîãî ïóíêòà ìåíþ
				auto state_after = menu->GetMenuState(i + 1, MF_BYPOSITION);
				if (state_after != 0xFFFFFFFF && (state_after & MF_SEPARATOR))
					menu->RemoveMenu(i + 1, MF_BYPOSITION);
				menu->RemoveMenu(i, MF_BYPOSITION);
				return;
			}
		}
	};

	switch (object_->getType())
	{
	case object_type::_7xxx_controller:
		if (object_->getAs7xxx())
		{
			auto as7xxx = object_->getAs7xxx();

			if (as7xxx->isEthernet())
			{
				m_contextMenu.LoadMenu(IDM_7XXX_ETHERNET_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				if (as7xxx->canEnableOrDisable())
				{
					menu->ModifyMenu(ID_DISABLE, MF_BYCOMMAND | MF_STRING, ID_DISABLE, as7xxx->isEnabled() ? g_sDisable : g_sEnable);

					menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, g_sUpdate);

					/*					menu->ModifyMenu(ID_SAVE_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_PROPERTIES, g_sWriteConfiguration);*/

					/*					menu->ModifyMenu(ID_SAVE_ALL_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_ALL_PROPERTIES, g_sWriteAllConfiguration);*/

					if (!as7xxx->isEnabled())
					{
						RemoveMenuByCommandWithSeparator(ID_REFRESH_CONTEXT);
						// 						RemoveMenuByCommandWithSeparator(ID_SAVE_PROPERTIES);
						// 						RemoveMenuByCommandWithSeparator(ID_SAVE_ALL_PROPERTIES);
					}

					if (as7xxx->isAcceptEnabled())
						menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED);
					else
						menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_ENABLED);
				}
				else
					RemoveMenuByCommandWithSeparator(ID_DISABLE);

				if (as7xxx->isAcceptable())
				{
					menu->ModifyMenu(ID_ACCEPT, MF_BYCOMMAND | MF_STRING, ID_ACCEPT, g_sAccept);

					if (as7xxx->isEnabled())
					{
						menu->EnableMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_DISABLED);
						menu->CheckMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_UNCHECKED);
					}
					else if (as7xxx->isAcceptEnabled())
					{
						menu->EnableMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_ENABLED);
						menu->CheckMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_CHECKED);
					}
					else
					{
						menu->EnableMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_ENABLED);
						menu->CheckMenuItem(ID_ACCEPT, MF_BYCOMMAND | MF_UNCHECKED);
					}
				}
				else
					RemoveMenuByCommandWithSeparator(ID_ACCEPT);

				menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, g_sProperties);
			}
			else
			{
				m_contextMenu.LoadMenu(IDM_7XXX_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				RemoveMenuByCommandWithSeparator(ID_STREAM_DATA);
				RemoveMenuByCommandWithSeparator(ID_MEASURE_DATA);

				menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, g_sUpdate);

				/*				menu->ModifyMenu(ID_SAVE_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_PROPERTIES, g_sWriteConfiguration);*/

				/*				menu->ModifyMenu(ID_SAVE_ALL_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_ALL_PROPERTIES, g_sWriteAllConfiguration);*/

				menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, g_sProperties);
			}
		}
		break;

	case object_type::_7xxx_is:
		if (object_->getAs7xxx())
		{
			m_contextMenu.LoadMenu(IDM_7XXX_CONTEXT);
			menu = m_contextMenu.GetSubMenu(0);

			RemoveMenuByCommandWithSeparator(ID_STREAM_DATA);
			RemoveMenuByCommandWithSeparator(ID_MEASURE_DATA);

			menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, g_sUpdate);
			//			menu->ModifyMenu(ID_SAVE_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_PROPERTIES, g_sSave);

			// 			if (object_->getAs7xxx())
			// 				menu->RemoveMenu(ID_SAVE_ALL_PROPERTIES, MF_BYCOMMAND);

			menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, g_sProperties);
		}
		break;
	}

	if (menu)
	{
		menu->SetDefaultItem(default_command);
		*context_menu = menu;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------
// void ÑTreeForCompare::OnPropertiesSaveAll()
// {
// 	object* object_(nullptr);
// 	object_ = m_tree.GetSelectedItemObject();
// 	if (object_ && object_->getAs7xxx())
// 	{
// 		m_tree.ChangeNameAll(object_, false);	
// 		object_->setModify(false, true);
// 
// 		m_tree.UpdateDeviceInfo(object_);
// 		m_tree.UpdateChannelInfo(object_);
// 	
// 		object_->getAs7xxx()->SetCfgWorkerPtr(&m_cfgWorker);
// 		object_->getAs7xxx()->saveSettings(true);
// 
// //		startTimer(timer::timerUpdateTree, 2000);
// 	}
// }
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnPropertiesRequest()
{
	object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (object_ && object_->getParent())
	{
		int32_t tab(-1);
		zetlab::_7xxx::device* device7xxx(object_->getParent()->getAs7xxx());
		if (device7xxx)
		{
			m_cfgWorker.setSerialParent(device7xxx->getDeviceSerial());
			m_cfgWorker.setNodeDevice(getNodeFromShotName(object_->getName().c_str()));
		}
		else
			m_cfgWorker.setNodeDevice(0);

		//		CPropertiesDlg* dlg(nullptr);
		boost::function< bool(Interconnection, BYTE*) > funcRequest;
		properties_dlg_common* dlg = new properties_dlg_common({ object_ }, &m_cfgWorker, funcRequest, this);
		if (dlg != nullptr)
		{
			dlg->set_tab(tab);
			bool isModify(false);
			switch (dlg->DoModal())
			{
			case IDOK:
				if (!object_->getAs7xxx())
					break;
			default:
				if (object_->getAs7xxx())
				{
					if (dlg->getIsSave())
					{
						if (!(!isModify && !object_->isModify()))
							m_tree.ChangeName(object_, isModify);

						object_->setModify(isModify);
					}
					else
					{
						isModify = m_cfgWorker.checkModifyDevice() ? true : false;

						if (!((isModify && object_->isModify()) || (!isModify && !object_->isModify())))
							m_tree.ChangeName(object_, isModify);

						object_->setModify(isModify);
					}
				}
				break;
			}

			delete dlg;
			dlg = nullptr;
		}

		m_tree.UpdateDeviceInfo(object_);
		m_tree.UpdateChannelInfo(object_);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnPropertiesSave()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (object_ && object_->getAs7xxx())
	{
		bool isModify(false);

		if (!(!isModify && !object_->isModify()))
		{
			m_tree.ChangeName(object_, isModify);

			object_->setModify(isModify);

			m_tree.UpdateDeviceInfo(object_);
			m_tree.UpdateChannelInfo(object_);
		}

		object_->getAs7xxx()->SetCfgWorkerPtr(&m_cfgWorker);
		object_->getAs7xxx()->saveSettings();

		//		startTimer(timer::timerUpdateTree, 2000);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case static_cast<UINT_PTR>(timer::timerUpdateTree):
		updateTree();
		KillTimer(nIDEvent);
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------
void ÑTreeForCompare::startTimer(timer typeTimer, long time)
{
	SetTimer(static_cast<UINT_PTR>(typeTimer), 1000, NULL);
}
//-----------------------------------------------------------------------------------------------------------------------------
long ÑTreeForCompare::getNodeFromShotName(CString sName)
{
	CString sNode(_T(""));
	sNode = sName.Mid(sName.ReverseFind(_T('(')) + 1, sName.ReverseFind(_T(')')) - sName.ReverseFind(_T('(')) - 1);
	return _wtol(sNode);
}
//-----------------------------------------------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------------
