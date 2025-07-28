#include "stdafx.h"
#include "../../application/application.h"
#include "../user_messages.hpp"
#include "main_dlg.h"

#include "../../algo/path/path.h"
#include "../../algo/saver/saver.hpp"

#include "../sensors/SensorsDlg.h"
#include "../settings/SettingsDlg.h"
#include "../wizard/WizardSheet.h"
#include "../config/ConfigDlg.h"
#include "../properties/properties_dlg_common.h"
#include "../properties/properties_dlg_zet037.h"

#include <atomic>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <filesystem>
#include <optional>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>

#include <mathZet.h>
#include <Dialog_ZET\ConnectToZetTools.h>
#include <Dialog_ZET\ConnectToZetFile.h>
#include <FunctionApplication/FunctionApplication.h>

#include "../../algo/std/crawler/crawler_std.hpp"

#include "../../algo/7xxx/7xxx/device/stream_data_work/stream_data_worker.h"

#include "../../algo/utils/recorder/recorder.hpp"

#include "../../algo/utils/zsp_dev_detect/zsp_dev_detect.hpp"

#include "resource/resource.h"
#include <afxcmn.h>  
//#include "../../ZETTools/ZETView_basic_class/CDSRV.h"     //"D:/newzetlab16.06/ZETTools/ZETView_basic_class/CDSRV.h"
//#include "../../../../../../ZETTools/ZETView_basic_class/CDSRV.h"
////#include "afxdialogex.h"
//#include "\GUI\tabs\channel_tab.h"
//extern "C" long WINAPI ZGetQuantityChannelADC(long typeDevice, long numberDSP, long* quantityChannel);  //// для WINAPI
//using namespace CDSRV; 

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

extern bool isAnyBlinked;
CDZetPath m_path;
std::atomic<bool> g_flagEndThreadCheckModbus = false;
CString g_sNameOrientation[7]{
	L"o",
	L"➙  y+",
	L"←  y-",
	L"➚  x-",
	L"➘  x+",
	L"↑  z+",
	L"↓  z-"
};



CZETDeviceManagerDlg::CZETDeviceManagerDlg(CWnd* pParent) :
	CAnchoredDialog(CZETDeviceManagerDlg::IDD, pParent),
	m_saveOnModify(false),
	m_propertiesOpened(0),
	m_detailed(true),
	m_sNameSelectedItem(_T("")),
	m_isClickItem(false),
	m_bflagWasExport(true),
	m_notifiedDetectZet76(false)
{
	ztInitCreateDumpWhenException();
	InitZetDictionary();
	Phrases::getInstance();

	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
	m_rectOld.left = m_rectOld.right = m_rectOld.top = m_rectOld.bottom = 0;
}

CZETDeviceManagerDlg::~CZETDeviceManagerDlg()
{
	if (server!= nullptr)
		server->Disconnect();
}

void CZETDeviceManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CAnchoredDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE, m_tree);  //m_tree — экземпляр класса CDevicesTree (наследник CTreeCtrlEx), отображает иерархию устройств.
	DDX_Control(pDX, IDC_LIST, m_list); //m_list — экземпляр CDevicesList (наследник CListCtrl), показывает таблицу с атрибутами.
	DDX_Control(pDX, IDC_ZETPATHCTRL1, m_path); 
}

BEGIN_MESSAGE_MAP(CZETDeviceManagerDlg, CAnchoredDialog)
	ON_COMMAND(ID_ShiftDown, OnShiftDown)   // обработка shift + вниз
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnNMClickListNot)   /// для не выделения последнего элемента тип 14
	ON_OTHER_INSTANCE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZING()
	ON_WM_TIMER()
	ON_WM_COPYDATA()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTreeSelChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnListSelChange) // 5.06
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CZETDeviceManagerDlg::OnNMClickList) 
		
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnTreeDoubleClick)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnListDoubleClick)
	ON_COMMAND(ID_PROPERTIES, OnPropertiesRequest)  // свойство 1 канала
	ON_COMMAND(ID_PROPERTIES_EXPORT, OnPropertiesExportRequest)
	ON_COMMAND(ID_PROPERTIES_IMPORT, OnPropertiesImportRequest)
	ON_COMMAND(ID_PROPERTIES_DEFAULT, OnPropertiesDefaultRequest)
	ON_COMMAND(ID_SERVICE_MODE, OnServiceModeRequest)
	ON_COMMAND(ID_SAVE_PROPERTIES, OnPropertiesSave)
	ON_COMMAND(ID_REFRESH_CONTEXT, OnRefreshContext)
	ON_COMMAND(ID_STREAM_DATA, OnStreamData)
	ON_COMMAND(ID_MEASURE_DATA, OnMeasureData)
//	ON_COMMAND(ID_SAVE_ALL_PROPERTIES, OnPropertiesSaveAll)
	ON_COMMAND(ID_IDM_FORMAT, OnFormatting)
	ON_COMMAND(ID_IDM_OPEN_SD, OnOpen)
	ON_COMMAND(ID_IDM_CONVERTER, OnConverting)
	ON_COMMAND(ID_DISABLE, OnDisableRequest)
	ON_COMMAND(ID_ACCEPT, OnAcceptRequest)
	ON_COMMAND(ID_PROPERTIES_GROUP, OnPropertiesGroupRequest)    // групповая настройка канала
	ON_COMMAND(ID_IDM_ZSP_GROUP_PROPERTIES, OnZSPPropertiesGroupRequest)   // zsp настройка всех каналов сразу
	ON_COMMAND(ID_PROPERTIES_CHANNEL_GROUP, OnPropertiesGroupChannelRequest) // 
	ON_COMMAND(ID_DISABLE_MORE_CHANNEL, OnDisableRequestMoreChannel) //выбор каналов на отключение/включение
	ON_COMMAND(ID_AC_DC, OnAcDcRequest)
	ON_COMMAND(ID_AC_DC_MORE_CHANNEL, OnAcDcRequestMoreChannel) // AC групповое выделение
	ON_COMMAND(ID_ICP, OnIcpRequest)
	ON_COMMAND(ID_ICP_MORE_CHANNEL, OnIcpRequestMoreChannel) // // ICP на выделенные каналы
	ON_COMMAND(ID_CHARGE, OnChargeRequest)
	ON_COMMAND(ID_BLINK, OnBlinkRequest)
	ON_COMMAND(ID_SETTINGS, OnSettingsRequest)
	ON_COMMAND(ID_SENSORS_DB, OnSensorsDbRequest)
	ON_COMMAND(ID_FOREIGN_SENSORS_DB, OnForeignSensorsDbRequest)
	ON_COMMAND(ID_CHOOSE_FOLDER_CONVERT, OnStartConverter)
	ON_COMMAND(ID_OPEN_XML, OnOpenXML)
	ON_COMMAND(ID_SAVE, OnUserSave)
	ON_COMMAND(ID_APP_EXIT, OnExit)
	ON_COMMAND(ID_DETAIL, OnDetailedOrShort)
	ON_COMMAND(ID_DETAILED, OnDetailed)
	ON_COMMAND(ID_SHORT, OnShort)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	//     ON_COMMAND( ID_BATCH,           OnBatch )
	ON_COMMAND(ID_HELP, OnHelp)
	ON_MESSAGE_VOID(WM_USER_SAVE, OnUserSave)
	ON_NOTIFY(NM_CLICK, IDC_TREE, &CZETDeviceManagerDlg::OnNMClickTree)
	ON_MESSAGE(WM_ENABLE_DEVICE, OnEnableDevice)
	ON_MESSAGE(WM_ENABLE_DATA, OnEnableData)
	ON_MESSAGE_VOID(WM_DETECT_ZET76, OnDetectZet76)
	//ON_COMMAND(ID_ACCEL, &CZETDeviceManagerDlg::OnAccel)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(CZETDeviceManagerDlg)
	ANCHOR_MAP_ENTRY(NULL, ANF_AUTOMATIC)
END_ANCHOR_MAP()

BEGIN_EVENTSINK_MAP(CZETDeviceManagerDlg, CAnchoredDialog)
	ON_EVENT(CZETDeviceManagerDlg, 1000, 1, OnServerModify, VTS_I4)
END_EVENTSINK_MAP()

BOOL CZETDeviceManagerDlg::OnInitDialog()
{
	m_toolBar.Create(this, IDR_MAIN_TOOLBAR);
	//m_toolBar.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_MULTISELECT);
	CAnchoredDialog::OnInitDialog();

	CString sDebug;
#ifdef _DEBUG
	sDebug = L"!!!DEBUG!!! ";
#endif
	SetWindowTextW(sDebug + g_sZETDeviceManager);

	waitDialog = std::make_unique<CWaitFormDlg>(this);
	waitDialog->Create(IDD_WAITFORM, this);

	server = std::make_unique<CDSRV>();
	if (server != nullptr)
	{
		server->Create(_T(""), WS_CHILD, CRect(0, 0, 0, 0), this, 1000);
		server->Connect();
		server->SetUseConvertUnitByXML(FALSE);
	}
	//m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_MULTISELECT);
	MAIN_MENU->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, L"&" + g_sMenuFile);
	MAIN_MENU->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND | MF_STRING, ID_APP_EXIT, L"&" + g_sMenuExit + L"\tAlt + F4");
	MAIN_MENU->ModifyMenu(1, MF_BYPOSITION | MF_STRING, 1, L"&" + g_sMenuAcions);
	MAIN_MENU->ModifyMenu(ID_SENSORS_DB, MF_BYCOMMAND | MF_STRING, ID_SENSORS_DB, L"&" + g_sEditSensorDatabase + L"\tCtrl+D");
	MAIN_MENU->ModifyMenu(ID_FOREIGN_SENSORS_DB, MF_BYCOMMAND | MF_STRING, ID_FOREIGN_SENSORS_DB, L"&" + g_sEditDatabaseThirdPartyDevices + L"\tCtrl+L");
	MAIN_MENU->ModifyMenu(ID_CHOOSE_FOLDER_CONVERT, MF_BYCOMMAND | MF_STRING, ID_CHOOSE_FOLDER_CONVERT, L"&" + g_sSelectingFolderSilentConverter + L"\tCtrl+H");
	MAIN_MENU->ModifyMenu(ID_OPEN_XML, MF_BYCOMMAND | MF_STRING, ID_OPEN_XML, L"&" + g_sOpenFile + L"\tCtrl+O");
	MAIN_MENU->ModifyMenu(2, MF_BYPOSITION | MF_STRING, 2, L"&" + g_sMenuView);
	MAIN_MENU->ModifyMenu(ID_SHORT, MF_BYCOMMAND | MF_STRING, ID_SHORT, L"&" + g_sMenuShort + L"\tCtrl+1");
	MAIN_MENU->ModifyMenu(ID_DETAILED, MF_BYCOMMAND | MF_STRING, ID_DETAILED, L"&" + g_sMenuDetails + L"\tCtrl+2");
	MAIN_MENU->ModifyMenu(ID_REFRESH, MF_BYCOMMAND | MF_STRING, ID_REFRESH, L"&" + g_sUpdate + L"\tF5");
	MAIN_MENU->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, L"&" + g_sHelp);
	MAIN_MENU->ModifyMenu(ID_HELP, MF_BYCOMMAND | MF_STRING, ID_HELP, L"&" + g_sSeeInformation + L"\tF1");

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CheckSensorsFile();

	CString s(_T("DirConfig"));
	BSTR dir_config = s.AllocSysString();
	auto path_config = std::make_shared<std::wstring>(m_path.ZetPath(&dir_config).GetString());
	SysFreeString(dir_config);

	factory_ptr_ = boost::make_shared< zetlab::factory >(); // 
	zsp_factory_ptr_ = std::make_shared<zetlab::zsp_factory>(std::move(path_config));
	crawler_ptr_ = boost::make_shared< zetlab::crawler >(factory_ptr_, zsp_factory_ptr_, server.get(), &m_cfgWorker.m_xmlWorker, this);

	/*factory_ptr_->onAddListener(boost::bind(&CDevicesTree::Add, &m_tree, _1, _2));
	factory_ptr_->onAddListener(boost::bind(&CDevicesList::Add, &m_list, _1, _2));
	factory_ptr_->onRemoveListener(boost::bind(&CDevicesTree::Remove, &m_tree, _1));
	factory_ptr_->onRemoveListener(boost::bind(&CDevicesList::Remove, &m_list, _1));
	factory_ptr_->onUpdateListener(boost::bind(&CDevicesTree::Update, &m_tree, _1));
	factory_ptr_->onUpdateListener(boost::bind(&CDevicesList::Update, &m_list, _1));
	*/
	factory_ptr_->onAddListener(boost::bind(&CZETDeviceManagerDlg::onAddData, this, _1, _2)); // заполнение дерева
	factory_ptr_->onUpdateListener(boost::bind(&CZETDeviceManagerDlg::onUpdateData, this, _1));
	factory_ptr_->onRemoveListener(boost::bind(&CZETDeviceManagerDlg::onRemoveData, this, _1));

	factory_ptr_->onGetNameSelectedItemListener(boost::bind(&CZETDeviceManagerDlg::GetNameSelectedItem, this));
	factory_ptr_->onWaitListener(boost::bind(&CDevicesTree::SetConnecting, &m_tree, _1));
	factory_ptr_->onWaitListener(boost::bind(&CDevicesList::SetConnecting, &m_list, _1));

	factory_ptr_->onErrorListener(boost::bind(&CZETDeviceManagerDlg::OnError, this, _1, _2));
	factory_ptr_->onFirstConnect(boost::bind(&CZETDeviceManagerDlg::OnNeedWizard, this, _1));

	crawler_ptr_->onNeedInvalidate(boost::bind(&CZETDeviceManagerDlg::OnInvalidate, this));
	m_tree.OnPrepareContextMenu(boost::bind(&CZETDeviceManagerDlg::PrepareContextMenu, this, _1));
	m_list.OnPrepareContextMenu(boost::bind(&CZETDeviceManagerDlg::PrepareContextMenu, this, _1));
	//m_list.OnPrepareContextMenu(boost::bind(&CZETDeviceManagerDlg::OnContextMenu, this, _1));
	
	RestoreWindow();

	InitJumpList();

	CheckCommandLine();
	m_OnHelp.Create(&m_path);

	m_pSharePipeClient = std::make_shared< CZetPipeClient >((LPTSTR)L"\\\\.\\pipe\\ZETInterconnection");
	m_pSharePipeClient->Connect();

	boost::thread thr(&CZETDeviceManagerDlg::funcCheckModbus, this, &m_cfgWorker.m_xmlWorker);
	thr.detach();

	return TRUE;
}




BOOL CZETDeviceManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}

	return CAnchoredDialog::PreTranslateMessage(pMsg);
}

void CZETDeviceManagerDlg::OnSysCommand(UINT nID, LPARAM lParam) ////////////////////////////////////////////////
{
	if (SC_CLOSE == nID)
		PostQuitMessage(WM_QUIT);

	CDialog::OnSysCommand(nID, lParam);
}

////////////////////////////////////////
// Вспомогательная функция для проверки, можно ли выделить строку
bool CZETDeviceManagerDlg::IsSelectableItem(CListCtrl* pList, int nItem, zetlab::object* obj)
{
	if (!obj) return false; // Не выделяем строки без данных

	// Проверяем, не заголовок ли это
	LVITEM item = { 0 };
	item.mask = LVIF_IMAGE;
	item.iItem = nItem;
	if (pList->GetItem(&item) && item.iImage == -1)
		return false; // Это заголовок

	// Для каналов проверяем тип устройства
	if (obj->getAsChannel())
	{
		long typeDevice = obj->getAsChannel()->getType();
		long maxChannel = this->server->QuanPhChan(obj->getAsChannel()->getDevice()->getModule());
		long currentChannel = obj->getAsChannel()->getDeviceID();

		// Не выделяем последнюю строку для типа 14
		if (typeDevice == 14 && (currentChannel == maxChannel - 1 || nItem == pList->GetItemCount() - 1))
			return false;
	}

	return true;
}

// Находит первую строку, доступную для выделения
int CZETDeviceManagerDlg::FindFirstSelectableItem(CListCtrl* pList)
{
	for (int i = 0; i < pList->GetItemCount(); i++)
	{
		DWORD_PTR data = pList->GetItemData(i);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (IsSelectableItem(pList, i, obj))
			return i;
	}
	return -1;
}

void CZETDeviceManagerDlg::OnShiftDown()  /// обработка shift + вниз
{
	CWnd* pFocusWnd = GetFocus();

	if (pFocusWnd && pFocusWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl* pList = (CListCtrl*)pFocusWnd;
		int nAnchor = pList->GetSelectionMark();

		// Если ничего не выделено - начинаем с первой строки (пропуская заголовки)
		if (nAnchor == -1)
		{
			int nFirstItem = FindFirstSelectableItem(pList);
			if (nFirstItem != -1)
			{
				pList->SetItemState(nFirstItem, LVIS_SELECTED, LVIS_SELECTED);
				pList->SetSelectionMark(nFirstItem);
			}
			return;
		}

		// Проверяем текущий элемент
		DWORD_PTR anchorData = pList->GetItemData(nAnchor);
		zetlab::object* anchorObj = reinterpret_cast<zetlab::object*>(anchorData);

		// Находим последнюю выделенную строку
		int nLastSelected = pList->GetNextItem(-1, LVNI_SELECTED);
		while (pList->GetNextItem(nLastSelected, LVNI_SELECTED) != -1)
			nLastSelected++;

		// Ищем следующую допустимую строку для выделения
		int nNextItem = nLastSelected + 1;
		while (nNextItem < pList->GetItemCount())
		{
			DWORD_PTR itemData = pList->GetItemData(nNextItem);
			zetlab::object* itemObj = reinterpret_cast<zetlab::object*>(itemData);

			if (IsSelectableItem(pList, nNextItem, itemObj))
			{
				// Выделяем все от якоря до найденного элемента
				for (int i = min(nAnchor, nLastSelected); i <= nNextItem; i++)
				{
					if (IsSelectableItem(pList, i, reinterpret_cast<zetlab::object*>(pList->GetItemData(i))))
						pList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				}

				pList->EnsureVisible(nNextItem, FALSE);
				return;
			}
			nNextItem++;
		}

		// Если достигли конца списка - ничего не делаем
	}
	// Обработка для CTreeCtrl 
	else if (pFocusWnd && pFocusWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
	{
		CTreeCtrl* pTree = (CTreeCtrl*)pFocusWnd;
		HTREEITEM hAnchor = pTree->GetDropHilightItem() ? pTree->GetDropHilightItem() : pTree->GetSelectedItem();

		if (!hAnchor)
		{
			pTree->SelectItem(pTree->GetRootItem());
			return;
		}

		// Выделяем следующий элемент
		HTREEITEM hNext = pTree->GetNextItem(hAnchor, TVGN_NEXT);
		if (hNext)
		{
			pTree->Select(hNext, TVGN_CARET);
			if (GetKeyState(VK_SHIFT) < 0) // Shift зажат
				pTree->SelectItem(hNext); // , TVGN_DROPHILITE);
		}
	}
}

void CZETDeviceManagerDlg::OnNMClickListNot(NMHDR* pNMHDR, LRESULT* pResult) /// не выделение последней строки генератора и загловков на тип 14
{
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

		auto selected = m_list.GetFirstSelectedItemPosition();
		int nItem = m_list.GetNextSelectedItem(selected);
		if(nItem >= 0)
		{
		    DWORD_PTR data = m_list.GetItemData(nItem);
		    zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);
		
			if (obj->getAsChannel())
			{
				long module = obj->getAsChannel()->getDevice()->getModule(); // получение модуля ус-ва  
				long typeDevice = obj->getAsChannel()->getType();
				long maxСhannel = this->server->QuanPhChan(module);
				long current = obj->getAsChannel()->getDeviceID();
				auto listcurrent = m_list.GetItemCount() - 1;
				auto currow = pNMIA->iItem;

				if (typeDevice == 14 && (GetKeyState(VK_CONTROL) & 0x8000))   ///  на 14 тип
				{
					// Проверяем, что кликнули на последний элемент в списке ИЛИ последний канал в модуле
					if (maxСhannel == currow || current == maxСhannel - 1 || listcurrent == currow)  //(current == maxСhannel - 1 || currow == listcurrent)
					{
						m_list.SetItemState(pNMIA->iItem, 0, LVIS_SELECTED); // Снимаем выделение
						*pResult = 1; // Блокируем стандартную обработку
						return;
					}
				}
				* pResult = 0;
			}
			else
			{
					m_list.SetItemState(pNMIA->iItem, 0, LVIS_SELECTED);
					*pResult = 1; // Блокируем обработку Ctrl+ЛКМ в заголовке
					return;
			}
		}
	}
}

void CZETDeviceManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon(GetSystemMetrics(SM_CXICON));
		int cyIcon(GetSystemMetrics(SM_CYICON));
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CAnchoredDialog::OnPaint();
	}
}

HCURSOR CZETDeviceManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CZETDeviceManagerDlg::OnDestroy()
{
	zetlab::std::crawler cv(factory_ptr_, zsp_factory_ptr_, server.get(), this);

	cv.startServer();

	SaveWindow();

	crawler_ptr_.reset();

	CAnchoredDialog::OnDestroy();
	//ExitProgramm(0);
}

void CZETDeviceManagerDlg::OnClose()
{
	crawler_ptr_.get()->pause();

	// Отключаем генерацию прерываний, так как закрываемся
	// https://www.zachburlingame.com/2011/04/silently-terminate-on-abortunhandled-exception-in-windows/
	SetUnhandledExceptionFilter(static_cast<LPTOP_LEVEL_EXCEPTION_FILTER>([](EXCEPTION_POINTERS*) -> LONG { return EXCEPTION_EXECUTE_HANDLER; }));
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

	g_flagEndThreadCheckModbus = true;
	while (g_flagEndThreadCheckModbus)
	{
		for (int i = 0; i < 50; ++i)
		{
			if (!g_flagEndThreadCheckModbus)
				break;
			Sleep(100);
		}
		break;
	}

	while (m_cfgWorker.getStatusThread() != statusEnd)
	{
		if (m_cfgWorker.getStatusThread() == statusCompare)
			m_cfgWorker.setFlagEndThreadCompare(true);

		Sleep(100);
	}

	// проверяем нужно ли  запустить чтение данных в Modbuszetlab
	if (!m_setSerialRunData.empty())
		for (auto& it : m_setSerialRunData)
			createQueryLaunch(it, true);

	PostQuitMessage(WM_QUIT);
}

void CZETDeviceManagerDlg::OnServerModify(long type)
{
	if (m_saveOnModify)
	{
		m_list.EnableWindow(FALSE);    // FALSE
		m_tree.EnableWindow(FALSE);  // FALSE

		m_toolBar.CreateProgress();

		crawler_ptr_->SetWaitThread(true);

		KillTimer(1);
		SetTimer(1, 1000, NULL);
	}
}

void CZETDeviceManagerDlg::OnTimer(UINT_PTR nIDEvent)    //  это центральный обработчик таймеров в диалоге управления устройствами.
{                                                        //Сохраняет данные и обновляет интерфейс.Ищет устройство по параметрам командной строки.Завершает работу устройств.Обрабатывает завершение добавления данных.
	CAnchoredDialog::OnTimer(nIDEvent);

	if (nIDEvent == 1)
	{
		KillTimer(1);
		m_saveOnModify = false;
		OnUserSave();

		m_toolBar.DestoryProgress();

		m_list.EnableWindow();
		m_tree.EnableWindow();

		m_list.Invalidate();
		m_tree.Invalidate();
		crawler_ptr_->SetWaitThread(false);
	}
	else if (nIDEvent == 2)
	{
		if (crawler_ptr_->isInited())
		{
			KillTimer(2);

			uint32_t type(0);
			auto it = m_cmdParams.find(L"-type");
			if (m_cmdParams.end() != it)
				swscanf_s(it->second.GetString(), L"%u", &type);

			uint64_t serial(0);
			it = m_cmdParams.find(L"-serial");
			if (m_cmdParams.end() != it)
				swscanf_s(it->second.GetString(), L"%I64u", &serial);

			if (type != 0 && serial != 0)
			{
				auto pObject = m_list.GetItemObject(type, serial);
				if (nullptr != pObject)
				{
					m_list.SelectItem(pObject);
					m_tree.SelectItem(pObject);
					OnPropertiesRequest();
				}
			}
		}
	}
	else if (nIDEvent == 3)
	{
		KillTimer(3);
		factory_ptr_->killAll();
		factory_ptr_->removeDead();
	}
	else if (nIDEvent == WAIT_DATA_TIMER)
	{
		onAddDataEnded();
	}
}

void CZETDeviceManagerDlg::OnTreeSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (pNMTV->itemNew.hItem)
	{
		if (m_isClickItem)
		{
			m_sNameSelectedItem = m_tree.GetSelectedItemObject()->getFullPath().c_str();
			m_isClickItem = false;
		}
		m_list.SelectItem(m_tree.GetSelectedItemObject());
	}
}

void CZETDeviceManagerDlg::OnListSelChange(NMHDR* pNMHDR, LRESULT* pResult)  //  Обрабатывает изменение выбора элемента в списке При реальном выборе элемента(не при снятии выделения) :
{                                                                         //Сохраняет путь выбранного элемента(если был клик). Синхронизирует выделение между списком и деревом

	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (pNMLV->uNewState & LVIS_SELECTED)
	{
		if (m_isClickItem)
		{
			m_sNameSelectedItem = m_list.GetSelectedItemObject()->getFullPath().c_str();
			m_isClickItem = false;
		}
		m_list.SetSelectionMark(pNMLV->iItem);
		m_tree.SelectItem(m_list.GetSelectedItemObject());
	}
}

void CZETDeviceManagerDlg::OnTreeDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;

	zetlab::object* object_(m_tree.GetSelectedItemObject());

	if (nullptr == object_)
		return;

	//if (!object_->isHaveChild())
	OnPropertiesRequest();

	if (object_->isHaveChild())
		m_tree.Expand(m_tree.GetSelectedHtreeItem(), TVE_TOGGLE);
}

void CZETDeviceManagerDlg::OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	
	*pResult = CDRF_DODEFAULT;

	zetlab::object* object_(nullptr);
	object_ = m_list.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	//if (!object_->isHaveChild())
	OnPropertiesRequest();
}

void CZETDeviceManagerDlg::OnInvalidate()
{
	m_tree.SortAll();
	m_tree.RedrawWindow(NULL, NULL, RDW_INVALIDATE);
	m_list.SortAll();
	m_list.SmartWidthForList();
}




bool CZETDeviceManagerDlg::AreSelectedChannelsEnabled()  // Проверка активности выбранных каналов
{

	auto selected = m_list.GetFirstSelectedItemPosition();

	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				if (!channel->isEnabled())
				{
					return false; // Найден неактивный канал
				}
			}
		}
	}

	return true; // Все выбранные каналы активны
}

bool CZETDeviceManagerDlg::AreSelectedChannelsHPF()  /// проверка АС на выделенных каналах, если все АС, то горит галочка
{
	auto selected = m_list.GetFirstSelectedItemPosition();

	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				if (!channel-> HPF())
				{
					return false; 
				}
			}
		}
	}

	return true; 
}

bool CZETDeviceManagerDlg::AreSelectedChannelsICP()  // проверяет, есть ли среди выделенных ICP, для галочки
{
	auto selected = m_list.GetFirstSelectedItemPosition();

	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				if (!channel->getICP())
				{
					return false; 
				}
			}
		}
	}

	return true; 
}

bool CZETDeviceManagerDlg::ChannelsSameType()  ///  Проверка каналов на один тип
{
	
	auto selected = m_list.GetFirstSelectedItemPosition();
	long type_device = -1;
	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);
		
		if (obj)
		{
			if (auto device = obj->getAsChannel())// getAsDevice())
			{
				
				if (type_device == -1)
				{
					type_device = device->getType();
				}
				else if (type_device != device->getType())
				{
					return false;
				}

			}
		}
	}

	return true;
}

void CZETDeviceManagerDlg::PrepareContextMenu(CMenu** context_menu)  /////////// //Формирует контекстное меню для устройств/каналов с учётом их типа и состояния
{

	int selected_count = m_list.GetSelectedCount(); /// выбрано несколько элементов

	if (selected_count > 1)           /////////////////////////////////////////////// МНОЖЕСТВЕННЫЙ ВЫБОР
	{
		
		m_contextMenu.DestroyMenu();

		zetlab::object* object_(nullptr);
		object_ = m_tree.GetSelectedItemObject();

		if (nullptr == object_)
			return;

		UINT default_command = ID_PROPERTIES_GROUP; // ID_PROPERTIES;
		CMenu* menu = nullptr;

		// Удаление пункта меню и разделителя после него (при наличии)
		auto RemoveMenuByCommandWithSeparator = [&](UINT id)
			{
				auto count = menu->GetMenuItemCount();
				for (int i = 0; i < count; ++i)
				{
					if (menu->GetMenuItemID(i) == id)
					{
						// Проверяем, есть ли разделитель после найденного пункта меню
						auto state_after = menu->GetMenuState(i + 1, MF_BYPOSITION);
						if (state_after != 0xFFFFFFFF && (state_after & MF_SEPARATOR))
							menu->RemoveMenu(i + 1, MF_BYPOSITION);
						menu->RemoveMenu(i, MF_BYPOSITION);
						return;
					}
				}
			};
		
		/*std::vector<zetlab::object*> selectedObjects;
		selectedObjects.clear();
		auto selected = m_list.GetFirstSelectedItemPosition();
	    while (selected)
		{
			int info = m_list.GetNextSelectedItem(selected);
			if (auto infoObjects = reinterpret_cast<zetlab::object*>(info))
			selectedObjects.emplace_back(infoObjects);
		}*/
	
		if (auto asChannel = object_->getAsChannel())  
		{
			
			if (asChannel->getType() == 30 || asChannel ->getType() == 14  || asChannel->getType() == 37)  // по типу
			{
						
     			if (!ChannelsSameType()) return;  // проверка на тип ус-ва, если разные, меню не открывается
				
				m_contextMenu.LoadMenu(IDM_ZSP_MORE_CHANNEL_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				bool activ_cannel = AreSelectedChannelsEnabled();

				csLocalization = activ_cannel ? g_sDisable : g_sEnable; // Устанавливается текст для пункта "Disable/Enable" в зависимости от состояния каналов:
				menu->ModifyMenu(ID_DISABLE_MORE_CHANNEL, MF_BYCOMMAND | MF_STRING, ID_DISABLE_MORE_CHANNEL, csLocalization);

				csLocalization = g_sGroupProperties;  //g_sProperties;// g_sGroupProperties;             ////////// выбор групповой настройки в меню свойства
				menu->ModifyMenu(ID_PROPERTIES_CHANNEL_GROUP, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_CHANNEL_GROUP, csLocalization);  
				//menu->EnableMenuItem(ID_PROPERTIES_GROUP, MF_DISABLED | MF_GRAYED);
				if (!activ_cannel)//(!asChannel->isEnabled())
				{
					menu->EnableMenuItem(ID_PROPERTIES_CHANNEL_GROUP, MF_DISABLED);// | MF_GRAYED);
					menu->RemoveMenu(ID_ICP_MORE_CHANNEL, MF_BYCOMMAND);
					menu->RemoveMenu(ID_AC_DC_MORE_CHANNEL, MF_BYCOMMAND);
				}
				else
				{
					menu->EnableMenuItem(ID_PROPERTIES_CHANNEL_GROUP, MF_ENABLED);  //ID_PROPERTIES_GROUP
				}

				csLocalization = g_sAc;
				menu->ModifyMenu(ID_AC_DC_MORE_CHANNEL, MF_BYCOMMAND | MF_STRING, ID_AC_DC_MORE_CHANNEL, csLocalization);  
				if (!AreSelectedChannelsHPF())   
						menu->CheckMenuItem(ID_AC_DC_MORE_CHANNEL, MF_BYCOMMAND | MF_UNCHECKED);
				else
						menu->CheckMenuItem(ID_AC_DC_MORE_CHANNEL, MF_BYCOMMAND | MF_CHECKED);

				if (asChannel->isHaveICP())
					{
						csLocalization = g_sICP;
						menu->ModifyMenu(ID_ICP_MORE_CHANNEL, MF_BYCOMMAND | MF_STRING, ID_ICP_MORE_CHANNEL, csLocalization); 
						if (!AreSelectedChannelsICP())          
							menu->CheckMenuItem(ID_ICP_MORE_CHANNEL, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
						else
							menu->CheckMenuItem(ID_ICP_MORE_CHANNEL, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
					}
					else
						menu->RemoveMenu(ID_ICP_MORE_CHANNEL, MF_BYCOMMAND);
				
			}
			/*
			m_contextMenu.LoadMenu(IDM_CHANNEL_CONTEXT);
			menu = m_contextMenu.GetSubMenu(0);

			csLocalization = asChannel->isEnabled() ? g_sDisable : g_sEnable;
			menu->ModifyMenu(ID_DISABLE, MF_BYCOMMAND | MF_STRING, ID_DISABLE, csLocalization);

			// если это канал с обратной связью проверяем используется ли он другими программами
			if (!asChannel->canEnableOrDisable() || asChannel->IsBuiltinGenChanFree())
				menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			else
				menu->CheckMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);

			csLocalization = g_sProperties;
			menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);

			// Отключение вкладки задействовать у анализоторов без встроенного генератора
			if (!asChannel->isHaveBuiltinGenerator() && asChannel->isFeedBackChannel())
				menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED);

			if (!asChannel->isEnabled())
				menu->EnableMenuItem(ID_PROPERTIES, MF_DISABLED);
			else
				menu->EnableMenuItem(ID_PROPERTIES, MF_ENABLED);

			if (asChannel->isFeedBackChannel() || !asChannel->isEnabled())
			{
				menu->RemoveMenu(3, MF_BYPOSITION);
				menu->RemoveMenu(ID_AC_DC, MF_BYCOMMAND);
				menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);
				menu->RemoveMenu(ID_CHARGE, MF_BYCOMMAND);
			}
			else
			{
				csLocalization = g_sAc;
				menu->ModifyMenu(ID_AC_DC, MF_BYCOMMAND | MF_STRING, ID_AC_DC, csLocalization);
				if (asChannel->HPF() == 0)
				{
					menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_UNCHECKED);
					auto mode = asChannel->AdcInputMode();
					if (mode == 1 || mode == 3)
						menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_DISABLED);
					else
						menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_ENABLED);
				}
				else
				{
					menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_CHECKED);
					menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_ENABLED);
				}

				if (asChannel->isHaveICP())
				{
					csLocalization = g_sICP;
					menu->ModifyMenu(ID_ICP, MF_BYCOMMAND | MF_STRING, ID_ICP, csLocalization);
					if (asChannel->getICP() == 0)
						menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
					else
						menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
				}
				else
					menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);

				if (asChannel->isHaveCharge())
				{
					csLocalization = g_sCharge;
					menu->ModifyMenu(ID_CHARGE, MF_BYCOMMAND | MF_STRING, ID_CHARGE, csLocalization);
					if (object_->getAsChannel()->getCharge() == 0)
						menu->CheckMenuItem(ID_CHARGE, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
					else
						menu->CheckMenuItem(ID_CHARGE, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
				}
				else
					menu->RemoveMenu(ID_CHARGE, MF_BYCOMMAND);
			}
		

		*/
		}

		if (menu)
		{
		   // default_command = ID_PROPERTIES_GROUP;
			menu->SetDefaultItem(default_command);
			*context_menu = menu;
		}
		


	}

	///////////////////////////////////////////////////////////////////



	else  // остальной код функции без изменений
	{
		m_contextMenu.DestroyMenu();
		zetlab::object* object_(nullptr);
		object_ = m_tree.GetSelectedItemObject();

		if (nullptr == object_)
			return;

		UINT default_command = ID_PROPERTIES;
		CMenu* menu = nullptr;

		// Удаление пункта меню и разделителя после него (при наличии)
		auto RemoveMenuByCommandWithSeparator = [&](UINT id)
			{
				auto count = menu->GetMenuItemCount();
				for (int i = 0; i < count; ++i)
				{
					if (menu->GetMenuItemID(i) == id)
					{
						// Проверяем, есть ли разделитель после найденного пункта меню
						auto state_after = menu->GetMenuState(i + 1, MF_BYPOSITION);
						if (state_after != 0xFFFFFFFF && (state_after & MF_SEPARATOR))
							menu->RemoveMenu(i + 1, MF_BYPOSITION);
						menu->RemoveMenu(i, MF_BYPOSITION);
						return;
					}
				}
			};

		switch (object_->getType())  //В зависимости от типа объекта (object_->getType()) создается соответствующее контекстное меню
		{
		case zetlab::object_type::channel:
			if (auto asChannel = object_->getAsChannel())
			{
				
				if (asChannel->is_zsp())  // меню на 1 канал уст-ва 30, пкм
				{
					
					m_contextMenu.LoadMenu(IDM_ZSP_CHANNEL_CONTEXT); //загружает меню с идентификатором IDM_ZSP_CHANNEL_CONTEXT.
					menu = m_contextMenu.GetSubMenu(0); //получает первое подменю.

					csLocalization = asChannel->isEnabled() ? g_sDisable : g_sEnable; // Устанавливается текст для пункта "Disable/Enable" в зависимости от состояния канала:
					menu->ModifyMenu(ID_DISABLE, MF_BYCOMMAND | MF_STRING, ID_DISABLE, csLocalization);  // Если канал включен (isEnabled()), текст будет "Disable" Если выключен - "Enable"

					csLocalization = g_sProperties; // Устанавливается текст "Properties" для соответствующего пункта меню.
					menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
					if (!asChannel->isEnabled())                // сли канал выключен:Делает пункт "Properties" недоступным Удаляет пункты "ICP" и "AC/DC" из меню Если канал включен - делает пункт "Properties" доступным.
					{
						menu->EnableMenuItem(ID_PROPERTIES, MF_DISABLED);    
						menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);
						menu->RemoveMenu(ID_AC_DC, MF_BYCOMMAND);
					}
					else
						menu->EnableMenuItem(ID_PROPERTIES, MF_ENABLED);

					csLocalization = g_sAc;  //  Настройка пункта "AC/DC": Устанавливает текст "AC" Если HPF равен 0, пункт помечается как невыбранный Иначе - как выбранный
					menu->ModifyMenu(ID_AC_DC, MF_BYCOMMAND | MF_STRING, ID_AC_DC, csLocalization);
					if (asChannel->HPF() == 0)
						menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_UNCHECKED);
					else
						menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_CHECKED);

					if (asChannel->isHaveICP())   // Если канал поддерживает ICP: Устанавливает текст "ICP" В зависимости от значения getICP() помечает пункт как выбранный или нет 	Если ICP не поддерживается - удаляет этот пункт из меню
					{
						csLocalization = g_sICP;
						menu->ModifyMenu(ID_ICP, MF_BYCOMMAND | MF_STRING, ID_ICP, csLocalization);
						if (asChannel->getICP() == 0)
							menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
						else
							menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
					}
					else
						menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);

					break;
				}
				
				m_contextMenu.LoadMenu(IDM_CHANNEL_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				csLocalization = asChannel->isEnabled() ? g_sDisable : g_sEnable;
				menu->ModifyMenu(ID_DISABLE, MF_BYCOMMAND | MF_STRING, ID_DISABLE, csLocalization);

				// если это канал с обратной связью проверяем используется ли он другими программами
				if (!asChannel->canEnableOrDisable() || asChannel->IsBuiltinGenChanFree())
					menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				else
					menu->CheckMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);

				csLocalization = g_sProperties;
				menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);

				// Отключение вкладки задействовать у анализоторов без встроенного генератора
				if (!asChannel->isHaveBuiltinGenerator() && asChannel->isFeedBackChannel())
					menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED);

				if (!asChannel->isEnabled())
					menu->EnableMenuItem(ID_PROPERTIES, MF_DISABLED);
				else
					menu->EnableMenuItem(ID_PROPERTIES, MF_ENABLED);

				if (asChannel->isFeedBackChannel() || !asChannel->isEnabled())
				{
					menu->RemoveMenu(3, MF_BYPOSITION);
					menu->RemoveMenu(ID_AC_DC, MF_BYCOMMAND);
					menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);
					menu->RemoveMenu(ID_CHARGE, MF_BYCOMMAND);
				}
				else
				{
					csLocalization = g_sAc;
					menu->ModifyMenu(ID_AC_DC, MF_BYCOMMAND | MF_STRING, ID_AC_DC, csLocalization);
					if (asChannel->HPF() == 0)
					{
						menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_UNCHECKED);
						auto mode = asChannel->AdcInputMode();
						if (mode == 1 || mode == 3)
							menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_DISABLED);
						else
							menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_ENABLED);
					}
					else
					{
						menu->CheckMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_CHECKED);
						menu->EnableMenuItem(ID_AC_DC, MF_BYCOMMAND | MF_ENABLED);
					}

					if (asChannel->isHaveICP())
					{
						csLocalization = g_sICP;
						menu->ModifyMenu(ID_ICP, MF_BYCOMMAND | MF_STRING, ID_ICP, csLocalization);
						if (asChannel->getICP() == 0)
							menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
						else
							menu->CheckMenuItem(ID_ICP, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
					}
					else
						menu->RemoveMenu(ID_ICP, MF_BYCOMMAND);

					if (asChannel->isHaveCharge())
					{
						csLocalization = g_sCharge;
						menu->ModifyMenu(ID_CHARGE, MF_BYCOMMAND | MF_STRING, ID_CHARGE, csLocalization);
						if (object_->getAsChannel()->getCharge() == 0)
							menu->CheckMenuItem(ID_CHARGE, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);
						else
							menu->CheckMenuItem(ID_CHARGE, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);
					}
					else
						menu->RemoveMenu(ID_CHARGE, MF_BYCOMMAND);
				}
			}
			// Для демки
			else
			{
				m_contextMenu.LoadMenu(IDM_7XXX_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				RemoveMenuByCommandWithSeparator(ID_REFRESH_CONTEXT);
				RemoveMenuByCommandWithSeparator(ID_STREAM_DATA);
				RemoveMenuByCommandWithSeparator(ID_MEASURE_DATA);
				// 			RemoveMenuByCommandWithSeparator(ID_SAVE_PROPERTIES);
				// 			RemoveMenuByCommandWithSeparator(ID_SAVE_ALL_PROPERTIES);

				csLocalization = g_sProperties;
				menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
			}
			break;

		case zetlab::object_type::device:
			if (zetlab::std::demo != object_->getAsDevice()->getConnectionType())
			{
				if (auto device = object_->getAsDevice())
				{
					if (is_zsp(device->getType())) //код динамически формирует контекстное меню для ZSP-устройств пкм на корне списка для всех каналов
					{
						m_contextMenu.LoadMenu(IDM_ZSP_DEVICE_CONTEXT);
						menu = m_contextMenu.GetSubMenu(0);
						csLocalization = g_sProperties;
						menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);

						// вставка грууповое меню  // ID_IDM_ZSP_GROUP_PROPERTIES

						//csLocalization = g_sGroupProperties;  //g_sProperties;// g_sGroupProperties;             ////////// выбор групповой настройки в меню свойства
						//menu->ModifyMenu(ID_PROPERTIES_CHANNEL_GROUP, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_CHANNEL_GROUP, csLocalization);


						csLocalization = g_sGroupProperties;
						menu->ModifyMenu(ID_IDM_ZSP_GROUP_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_IDM_ZSP_GROUP_PROPERTIES, csLocalization);
						////////////////////////////
						
						if (!device->has_service_mode())
							RemoveMenuByCommandWithSeparator(ID_SERVICE_MODE);
						else
						{
							csLocalization = g_sServiceMode;
							menu->ModifyMenu(ID_SERVICE_MODE, MF_BYCOMMAND | MF_STRING, ID_SERVICE_MODE, csLocalization);
							if (device->get_service_mode())
								menu->CheckMenuItem(ID_SERVICE_MODE, MF_BYCOMMAND | MF_CHECKED);
							else
								menu->CheckMenuItem(ID_SERVICE_MODE, MF_BYCOMMAND | MF_UNCHECKED);
						}

						csLocalization = g_sPropertiesDefault;
						menu->ModifyMenu(ID_PROPERTIES_DEFAULT, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_DEFAULT, csLocalization);

						break;
					}

					m_contextMenu.LoadMenu(IDM_DEVICE_CONTEXT);  // пкм на общем корне для 024/028
					menu = m_contextMenu.GetSubMenu(0);
					
					csLocalization = g_sBlink;
					menu->ModifyMenu(ID_BLINK, MF_BYCOMMAND | MF_STRING, ID_BLINK, csLocalization);

					if (device->isBlinking())
						menu->CheckMenuItem(ID_BLINK, MF_BYCOMMAND | MF_CHECKED | MF_ENABLED);

					else if (isAnyBlinked)
						menu->EnableMenuItem(ID_BLINK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

					else
						menu->CheckMenuItem(ID_BLINK, MF_BYCOMMAND | MF_UNCHECKED | MF_ENABLED);

					csLocalization = g_sProperties;
					menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
					csLocalization = g_sPropertiesExport;
					menu->ModifyMenu(ID_PROPERTIES_EXPORT, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_EXPORT, csLocalization);

					csLocalization = g_sGroupProperties;
					menu->ModifyMenu(ID_PROPERTIES_GROUP, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_GROUP, csLocalization);

					if (!(device->isVCS() || device->getType() == 10 || device->getType() == 15 || device->getType() == 16 || device->getType() == 17 || device->getType() == 19 || device->getType() == 14))
					{
						menu->EnableMenuItem(ID_PROPERTIES_GROUP, MF_BYCOMMAND | MF_DISABLED);
					}


					path          path_;
					zetlab::saver saver_(factory_ptr_);

					path_.Create(this);

					BSTR	bstrDirConfig = _T("DirConfig");
					CString	cstrTabConfig = path_.ZetPath(&bstrDirConfig) + _T("devices.cfg.bak");

					::std::vector<::std::pair<tstring, CZetTime>> cfg;
					saver_.get((LPCTSTR)cstrTabConfig, object_, cfg);
					csLocalization = g_sPropertiesImport;
					if (cfg.empty())
						menu->ModifyMenu(ID_PROPERTIES_IMPORT, MF_BYCOMMAND | MF_STRING | MF_DISABLED, ID_PROPERTIES_IMPORT, csLocalization);
					else
						menu->ModifyMenu(ID_PROPERTIES_IMPORT, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PROPERTIES_IMPORT, csLocalization);
					csLocalization = g_sPropertiesDefault;
					menu->ModifyMenu(ID_PROPERTIES_DEFAULT, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES_DEFAULT, csLocalization);


				}
			}
			break;

		case zetlab::object_type::_7xxx_controller:
			if (object_->getAs7xxx())
			{
				auto as7xxx = object_->getAs7xxx();

				if (as7xxx->isEthernet())
				{
					m_contextMenu.LoadMenu(IDM_7XXX_ETHERNET_CONTEXT);
					menu = m_contextMenu.GetSubMenu(0);

					if (as7xxx->canEnableOrDisable())
					{
						csLocalization = as7xxx->isEnabled() ? g_sDisable : g_sEnable;
						menu->ModifyMenu(ID_DISABLE, MF_BYCOMMAND | MF_STRING, ID_DISABLE, csLocalization);

						csLocalization = g_sUpdate;
						menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, csLocalization);

						// 					csLocalization = g_sWriteConfiguration;
						// 					menu->ModifyMenu(ID_SAVE_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_PROPERTIES, csLocalization);

						// 					csLocalization = g_sWriteAllConfiguration;
						// 					menu->ModifyMenu(ID_SAVE_ALL_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_ALL_PROPERTIES, csLocalization);

						if (!as7xxx->isEnabled())
						{
							RemoveMenuByCommandWithSeparator(ID_REFRESH_CONTEXT);
							// 						RemoveMenuByCommandWithSeparator(ID_SAVE_PROPERTIES);
							// 						RemoveMenuByCommandWithSeparator(ID_SAVE_ALL_PROPERTIES);
						}

						if (as7xxx->isAcceptEnabled())
						{
							menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_DISABLED);
						}
						else
						{
							menu->EnableMenuItem(ID_DISABLE, MF_BYCOMMAND | MF_ENABLED);
						}
					}
					else
					{
						RemoveMenuByCommandWithSeparator(ID_DISABLE);
					}

					if (as7xxx->isAcceptable())
					{
						csLocalization = g_sAccept;
						menu->ModifyMenu(ID_ACCEPT, MF_BYCOMMAND | MF_STRING, ID_ACCEPT, csLocalization);

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
					{
						RemoveMenuByCommandWithSeparator(ID_ACCEPT);
					}


					csLocalization = g_sProperties;
					menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
					//				menu->EnableMenuItem( ID_PROPERTIES, MF_BYCOMMAND | (bEnabled ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)) );
				}
				else if (as7xxx->getDeviceType() != ZET7XXX_DEVICE_TYPE::UNKNOWN)
				{
					m_contextMenu.LoadMenu(IDM_7XXX_CONTEXT);
					menu = m_contextMenu.GetSubMenu(0);

					RemoveMenuByCommandWithSeparator(ID_STREAM_DATA);
					RemoveMenuByCommandWithSeparator(ID_MEASURE_DATA);

					csLocalization = g_sUpdate;
					menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, csLocalization);

					// 				csLocalization = g_sWriteConfiguration;
					// 				menu->ModifyMenu(ID_SAVE_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_PROPERTIES, csLocalization);

					// 				csLocalization = g_sWriteAllConfiguration;
					// 				menu->ModifyMenu(ID_SAVE_ALL_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_SAVE_ALL_PROPERTIES, csLocalization);

					csLocalization = g_sProperties;
					menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
				}
			}
			break;

		case zetlab::object_type::_7xxx_is:
			if (object_->getAs7xxx())
			{
				m_contextMenu.LoadMenu(IDM_7XXX_CONTEXT);
				menu = m_contextMenu.GetSubMenu(0);

				BSTR dir_config = _T("DirConfig");
				stream_data_worker worker(m_path.ZetPath(&dir_config).GetString());

				if (worker.is_stream_data_enable(object_))
				{
					csLocalization = g_sStreamData;
					UINT flags = MF_BYCOMMAND | MF_STRING;
					if (worker.is_stream_data(object_))
						flags |= MF_CHECKED;
					menu->ModifyMenu(ID_STREAM_DATA, flags, ID_STREAM_DATA, csLocalization);

					csLocalization = g_sMeasureData;
					flags = MF_BYCOMMAND | MF_STRING;
					if (worker.is_measure_data(object_))
						flags |= MF_CHECKED;
					menu->ModifyMenu(ID_MEASURE_DATA, flags, ID_MEASURE_DATA, csLocalization);
				}
				else
				{
					RemoveMenuByCommandWithSeparator(ID_STREAM_DATA);
					RemoveMenuByCommandWithSeparator(ID_MEASURE_DATA);
				}

				csLocalization = g_sUpdate;
				menu->ModifyMenu(ID_REFRESH_CONTEXT, MF_BYCOMMAND | MF_STRING, ID_REFRESH_CONTEXT, csLocalization);

				csLocalization = g_sProperties;
				menu->ModifyMenu(ID_PROPERTIES, MF_BYCOMMAND | MF_STRING, ID_PROPERTIES, csLocalization);
			}
			break;

		case zetlab::object_type::sd:
			m_contextMenu.LoadMenu(IDM_SD_CARD_CONTEXT);
			menu = m_contextMenu.GetSubMenu(0);

			csLocalization = g_sConvert;
			menu->ModifyMenu(ID_IDM_CONVERTER, MF_BYCOMMAND | MF_STRING, ID_IDM_CONVERTER, csLocalization);

			csLocalization = g_sOpen;
			menu->ModifyMenu(ID_IDM_OPEN_SD, MF_BYCOMMAND | MF_STRING, ID_IDM_OPEN_SD, csLocalization);

			csLocalization = g_sFormat;
			menu->ModifyMenu(ID_IDM_FORMAT, MF_BYCOMMAND | MF_STRING, ID_IDM_FORMAT, csLocalization);

			default_command = ID_IDM_CONVERTER;
			break;
		}

		if (menu)
		{
			menu->SetDefaultItem(default_command);
			*context_menu = menu;
		}
	}
	
}

void CZETDeviceManagerDlg::OnPropertiesRequest()  // свойство 1 канала  через пкм или doubleclick
{
	
	if (InterlockedCompareExchange(&m_propertiesOpened, 1, 0) != 0)  /// если функция выпролняется -выход, проверка на выполнение
		return;
	
	std::vector<zetlab::object*> objects = { m_tree.GetSelectedItemObject() }; //Получение выбранного объекта в дереве и выбор первого
	zetlab::object* object_ = objects.front();
	
	if (object_)
	{
		bool bCanShow(true), // окно показа своист
			bTenso(false);
		zetlab::std::device* device_(nullptr);
		zetlab::std::channel* channel_(nullptr);
		zetlab::std::sd* sd_(nullptr);
		zetlab::_7xxx::device* object7xxx_(nullptr);

		m_sNameSelectedItem = object_->getFullPath().c_str();  // сохранение пути обьекта

		device_ = object_->getAsDevice();   // проверка что это
		channel_ = object_->getAsChannel();
		sd_ = object_->getAsSd();
		object7xxx_ = object_->getAs7xxx();

		if (device_)
		{
			if (zetlab::std::demo == device_->getConnectionType())
				bCanShow = false;
		}
		if (sd_)
		{
			sd_->launchSilentConverter(object_->getFullPath());
			bCanShow = false;
		}
		if (channel_)
		{
			if (!channel_->isEnabled())  // отключен канал - нет свойств
				bCanShow = false;
			if (channel_->is_zet037_tenso())  // тензо канал показ св-во
				bTenso = true;
		}
		if (object7xxx_)
		{
			if (object7xxx_->getDeviceType() == ZET7XXX_DEVICE_TYPE::UNKNOWN)
				bCanShow = false;
		}

		if (bCanShow)
		{
			crawler_ptr_->pause();  //остановка фонового процесса

			int32_t tab(-1);
			auto it = m_cmdParams.find(L"-tab");
			if (m_cmdParams.end() != it)
				swscanf_s(it->second.GetString(), L"%d", &tab);
			m_cmdParams.clear();
			//CPropertiesDlg dlg(object_, this);

			if (object7xxx_ != nullptr && object_->getParent())
			{
				zetlab::_7xxx::device* device7xxx(object_->getParent()->getAs7xxx());

				object7xxx_->setOffChannel(zetlab::_7xxx::flagViewLable::flagDefault);	// на случай warning-а (настройки не сохранились)

				uint64_t serial(0);

				if (device7xxx)
				{
					serial = device7xxx->getDeviceSerial();
					m_cfgWorker.setSerialParent(serial);
					m_cfgWorker.setFlagMaster(false);
					m_cfgWorker.setNodeDevice(getNodeFromShotName(object_->getName().c_str()));
				}
				else
				{
					m_cfgWorker.setFlagMaster(true);
					serial = object_->getAs7xxx()->getDeviceSerial();
					m_cfgWorker.setNodeDevice(0);
				}

				m_setSerialRunData.insert(serial);
				createQueryLaunch(serial, false);		// отправляем  запрос на остановку данных Modbus-у

				m_cfgWorker.m_hMainHwnd = m_hWnd;
				m_cfgWorker.m_object = object_;
			}

			if (bTenso)
				m_prop_dlg = std::make_unique<properties_dlg_zet037>(objects, this); // окно свойств для тензо
			else
				m_prop_dlg = std::make_unique<properties_dlg_common>(objects, &m_cfgWorker, boost::bind(&CZETDeviceManagerDlg::requestToModbus, this, _1, _2), this);  // окно свойств для обычного канала
			if (m_prop_dlg)
			{
				m_prop_dlg->set_tab(tab);
				switch (m_prop_dlg->DoModal())
				{
				case IDOK:
					if (!object_->getAs7xxx())
					{
						if (device_)
							device_->save_zsp_config();
						else if (channel_)
							channel_->save_zsp_config();
						m_saveOnModify = true;
						OnServerModify(0); //обновление данных
						m_saveOnModify = false;
						break;
					}
				default:
					break;
				}
			}

			m_tree.UpdateDeviceInfo(object_);  //обновление интерфейса
			m_list.UpdateDeviceInfo(object_);
			m_list.UpdateChannelInfo(object_);
			m_tree.UpdateChannelInfo(object_);

			crawler_ptr_->resume();  // запуск фон процесса
		}
	}
	InterlockedExchange(&m_propertiesOpened, 0);  // сброс флага для повторного вызова функции
}

void CZETDeviceManagerDlg::OnPropertiesExportRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (!object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();
	if (!device_)
		return;

	if (zetlab::std::demo == device_->getConnectionType())
		return;

	path          path_;
	zetlab::saver saver_(factory_ptr_);

	path_.Create(this);
	BSTR	bstrDirConfig = _T("DirConfig");
	CString	cstrTabConfig = path_.ZetPath(&bstrDirConfig) + _T("devices.cfg.bak");

	::std::vector<::std::pair<tstring, CZetTime>> cfg;
	saver_.get((LPCTSTR)cstrTabConfig, object_, cfg);

	CConfigDlg configDlg(cfg, true, factory_ptr_);
	if (IDOK == configDlg.DoModal())
	{
		if (configDlg.getCfgSize() != cfg.size())
		{
			// Для безопасности, если пользователь удалил конфиг
			cfg.clear();
			saver_.get((LPCTSTR)cstrTabConfig, object_, cfg);
		}

		bool bFindConfigName(false);
		for (auto& it : cfg)
		{
			if (it.first == configDlg.m_cfgName)
			{
				bFindConfigName = true;
				break;
			}
		}
		if (bFindConfigName)
		{
			if (::AfxMessageBox(g_sNameBusyAlready + L"\n" + g_sReWrite, MB_SYSTEMMODAL | MB_OKCANCEL) == IDCANCEL)
				return;
		}
		saver_.backup(object_, (LPCTSTR)cstrTabConfig, configDlg.m_cfgName);
	}
	else
		m_bflagWasExport = false;
}

void CZETDeviceManagerDlg::OnPropertiesImportRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (!object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();
	if (!device_)
		return;

	if (zetlab::std::demo == device_->getConnectionType())
		return;

	path          path_;
	zetlab::saver saver_(factory_ptr_);

	path_.Create(this);

	BSTR	bstrDirConfig = _T("DirConfig");
	CString	cstrTabConfig = path_.ZetPath(&bstrDirConfig) + _T("devices.cfg");
	CString cstrTabConfigBak = cstrTabConfig;
	cstrTabConfigBak.Append(_T(".bak"));

	::std::vector<::std::pair<tstring, CZetTime>> cfg;
	saver_.get((LPCTSTR)cstrTabConfigBak, object_, cfg);

	CConfigDlg configDlg(cfg, false, factory_ptr_);
	if (IDOK == configDlg.DoModal())
	{
		crawler_ptr_->pause();

		saver_.restore(object_, (LPCTSTR)cstrTabConfig, (LPCTSTR)cstrTabConfigBak, configDlg.m_cfgName);

		m_tree.UpdateDeviceInfo(object_);
		m_list.UpdateDeviceInfo(object_);

		::std::vector< zetlab::object_ptr > v_objects;
		object_->browse(v_objects, zetlab::object_type::channel);
		for (auto& channel_ : v_objects)
		{
			m_tree.UpdateChannelInfo(channel_.get());
			m_list.UpdateChannelInfo(channel_.get());
		}

		crawler_ptr_->resume();
		OnRefresh();
	}
}

void CZETDeviceManagerDlg::OnPropertiesDefaultRequest()   /// НАСТРОЙКИ ПО УМОЛЧАНИЮ
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (!object_)
		return;
	
	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();
	if (!device_)
		return;

	if (zetlab::std::demo == device_->getConnectionType())
		return;

	if (!is_zsp(device_->getType()))
	{
		CString sTemp = g_sDoSaveCurrentConfig;
		int retval = ::AfxMessageBox(sTemp, MB_SYSTEMMODAL | MB_YESNOCANCEL);
		if (retval == IDYES)
			OnPropertiesExportRequest();
		if (retval == IDCANCEL)
			return;
	}

	if (m_bflagWasExport) // из функции OnPropertiesExportRequest, и отмена
	{
		crawler_ptr_->pause();

		device_->setDefaultCfg();      /// запись настроек по умолчанию
		device_->save_zsp_state();

		m_tree.UpdateDeviceInfo(object_);
		m_list.UpdateDeviceInfo(object_);

		::std::vector< zetlab::object_ptr > v_objects;
		object_->browse(v_objects, zetlab::object_type::channel);
		for (auto& channel_ : v_objects)
		{
			m_tree.UpdateChannelInfo(channel_.get());
			m_list.UpdateChannelInfo(channel_.get());
		}

		crawler_ptr_->resume();

		OnRefresh();
	}
	m_bflagWasExport = true;
}

void CZETDeviceManagerDlg::OnServiceModeRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (!object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();
	if (!device_)
		return;

	if (is_zsp(device_->getType()))
		device_->set_service_mode(!device_->get_service_mode());
}

void CZETDeviceManagerDlg::OnFormatting()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr != object_)
	{
		/* 2 параметр это индекс устройства A=0 B=1 и тд */
		SHFormatDrive(this->GetSafeHwnd(), toupper(object_->getFullPath()[0]) - 'A', SHFMT_ID_DEFAULT, 0);
	}
}

void CZETDeviceManagerDlg::OnConverting()
{
	if (zetlab::object* object_ = m_tree.GetSelectedItemObject(); nullptr != object_)
	{
		if (zetlab::std::sd* sd = object_->getAsSd(); nullptr != sd)
			sd->launchSilentConverter(object_->getFullPath());
	}
}

void CZETDeviceManagerDlg::OnOpen()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr != object_)
	{
		bstr_t bstr1(object_->getFullPath().c_str());
		ShellExecute(this->m_hWnd, _T("open"), _T("explorer.exe"), bstr1, _T(""), SW_SHOWNORMAL);
	}
}

void CZETDeviceManagerDlg::OnDisableRequestMoreChannel()  //Отключение - включение на выделенные каналы без устройства 7ххх ////////////////////////////////////////////
{
	
	std::vector<zetlab::std::channel*> channel_;
	channel_.clear();

	// Получаем выбранные элементы из списка
	auto selected = m_list.GetFirstSelectedItemPosition();
	std::vector<zetlab::object*> objects; // Вектор для хранения связанных объектов
	objects.clear();
	bool on_off = false;
	map<long, long> OnOffChannels; //хранение обьектов <модуль, маска>
	map<long, pair<long, long>> moduleMaxСhannelType;  // модуль / количество каналов / тип ус-ва

	while (selected)
	{
		long lMask = 0;
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if ( auto channel = obj->getAsChannel()  ) //&& obj->getAsChannel()->getType() == 14
			{
				channel_.push_back(channel);
				objects.push_back(obj); // Сохраняем соответствующий объект
				if (!channel->isEnabled()) {
					on_off = true;
				}
				long typeDevice = channel->getType();
				long module = channel->getDevice()->getModule(); // получение модуля ус-ва
				long maxСhannel = this->server->QuanPhChan(module); // количество каналов на модуле (общее)
				long channelIndex = channel->getDeviceID();

				moduleMaxСhannelType[module] = { maxСhannel, typeDevice };
		 
					if (OnOffChannels.count(module))
					{
						long inMapRezult = OnOffChannels[module];
						inMapRezult |= (1 << channelIndex);
						OnOffChannels[module] = inMapRezult;
					}
					else
					{
						OnOffChannels[module] = (lMask |= (1 << channelIndex));
					}
					//// проверка 14 тип на отключение всех каналов всавить сюда
			}
		}
	}
	/////////////////////////////////////////////////// проверка 14 тип на отключение всех каналов
	auto takeType = moduleMaxСhannelType.begin();
	if(takeType->second.second == 14)
	{
		for (auto& [modul, mask] : OnOffChannels)
		{
			long maxChannel = moduleMaxСhannelType[modul].first;
			int rez = (1 << (maxChannel - 1)) - 1;
			if (mask == rez)
			{
				long inMapRezult = OnOffChannels[modul]; // оставляем вкл 1ый канал
				inMapRezult = inMapRezult & ~ (1 << 0);
				OnOffChannels[modul] = inMapRezult;

				for (size_t i = 0; i < channel_.size(); i++)  // удаление обьекта из channel_  и obj что бы не обновлялся
				{
					if (channel_[i]->getDevice()->getModule() == modul)
					{
						channel_.erase(channel_.begin() + i);
						objects.erase(objects.begin() + i);
						break;
					}
				}
			}
		}
	}
	//////////////////////////////////////////
	if (!channel_.empty())
	{
		
		for (auto& [modul, mask] : OnOffChannels) {
			channel_[0]->setMoreChannel(modul ,mask, on_off);
		}

		for (size_t i = 0; i < channel_.size(); i++)
		{
			auto* change_channel = channel_[i];
			auto* current_object = objects[i];

			if (!change_channel || !current_object)
				continue;
			if (!channel_[i]->is_zsp())
			{
				change_channel->setEnabledPh(on_off);
			}
			else change_channel->setEnabledPhZspMoreChannel(on_off);

			m_tree.UpdateChannelInfo(current_object);
			m_list.UpdateChannelInfo(current_object);
			m_tree.UpdateDeviceInfo(current_object);/// добавлено
			m_list.UpdateDeviceInfo(current_object); // добавлено
			
		}
		if (channel_[0]->is_zsp())   // если zsp, на каждый серийник кидаем save от канала
		{
			long moduleZsp = channel_[0]->getDevice()->getSerial();
			for (size_t t = 0; t < channel_.size(); t++)
			{
				if (moduleZsp == channel_[t]->getDevice()->getSerial() && t != 0)
					continue;
				else
				{
					channel_[t]->save_zsp_config();
					moduleZsp = channel_[t]->getDevice()->getSerial();
				}
			}
		}
		
		m_tree.Invalidate();
		m_list.Invalidate();
		m_saveOnModify = true;
	}

}

void CZETDeviceManagerDlg::OnDisableRequest()
{
	
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	zetlab::std::channel* channel_(nullptr);
	channel_ = object_->getAsChannel();
	zetlab::_7xxx::device* device_(nullptr);
	device_ = object_->getAs7xxx();

	if (nullptr != channel_)
	{
		long Enable = channel_->isEnabled() ? false : true;
		if (channel_->isFeedBackChannel())
			channel_->SetBuiltinGeneratorEnabled(Enable);
		channel_->setEnabledPh(!channel_->isEnabled());

		m_tree.UpdateChannelInfo(object_);
		m_list.UpdateChannelInfo(object_);

		m_saveOnModify = true;
	}
	else if (nullptr != device_)
	{
		// Для 7xxx проверим шилд
		if (!device_->isEnabled() && !CheckShield())
			return;

		device_->setEnabled(!device_->isEnabled());
		m_tree.UpdateDeviceInfo(object_);
		m_list.UpdateDeviceInfo(object_);

		crawler_ptr_->request_update();
	}

	m_tree.Invalidate();
	m_list.Invalidate();
}

void CZETDeviceManagerDlg::OnAcceptRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	auto device_ = object_->getAs7xxx();
	if (nullptr != device_)
	{
		device_->enableAccept(!device_->isAcceptEnabled());
		crawler_ptr_->request_update();
	}

	m_tree.Invalidate();
	m_list.Invalidate();
}

void CZETDeviceManagerDlg::OnAcDcRequestMoreChannel()     ////////////////////// АС групповое выделение каналов
{
	
	std::vector<zetlab::std::channel*> channel_;
	channel_.clear();

	// Получаем выбранные элементы из списка
	auto selected = m_list.GetFirstSelectedItemPosition();
	std::vector<zetlab::object*> objects; // Вектор для хранения связанных объектов
	objects.clear();
	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				channel_.push_back(channel);
				objects.push_back(obj); // Сохраняем соответствующий объект
			}
		}
	}
	bool enable = false;
	for (size_t i = 0; i < channel_.size(); i++) 
	{
		if (uint16_t hpf = (uint16_t)(!channel_[i]->HPF()))
		{
			enable = true;
			break;
		}
	}

	if (enable)
	{
		for (size_t i = 0; i < channel_.size(); i++) // для изменения части обьектов до АС
		{
			auto* current_object = objects[i];
			if (uint16_t hpf = (uint16_t)(!channel_[i]->HPF()))
			{
				channel_[i]->setHpf(hpf);
			
			}
			m_saveOnModify = true;

			m_tree.UpdateDeviceInfo(current_object);
			m_tree.UpdateChannelInfo(current_object);
			m_list.UpdateDeviceInfo(current_object);
			m_list.UpdateChannelInfo(current_object);

			//SendMessage(WM_USER_SAVE);
		}
		if (channel_[0]->is_zsp())   // если zsp, на каждый серийник кидаем save от канала
		{
			long moduleZsp = channel_[0]->getDevice()->getSerial();
			for (size_t t = 0; t < channel_.size(); t++)
			{
				if (moduleZsp == channel_[t]->getDevice()->getSerial() && t != 0)
					continue;
				else
				{
					channel_[t]->save_zsp_config();
					moduleZsp = channel_[t]->getDevice()->getSerial();
				}
			}
		}
		
	}
	else
	{
		for (size_t i = 0; i < channel_.size(); i++) // для изменения всех обьектов (инверсия) с АС до DC
		{
			auto* current_object = objects[i];
			uint16_t hpf = (uint16_t)(!channel_[i]->HPF());
			channel_[i]->setHpf(hpf);

			m_saveOnModify = true;

			m_tree.UpdateDeviceInfo(current_object);
			m_tree.UpdateChannelInfo(current_object);
			m_list.UpdateDeviceInfo(current_object);
			m_list.UpdateChannelInfo(current_object);

			//SendMessage(WM_USER_SAVE);
		}
		if (channel_[0]->is_zsp())   // если zsp, на каждый серийник кидаем save от канала
		{
			long moduleZsp = channel_[0]->getDevice()->getSerial();
			for (size_t t = 0; t < channel_.size(); t++)
			{
				if (moduleZsp == channel_[t]->getDevice()->getSerial() && t != 0)
					continue;
				else
				{
					channel_[t]->save_zsp_config();
					moduleZsp = channel_[t]->getDevice()->getSerial();
				}
			}
		}
	}
	SendMessage(WM_USER_SAVE);

	m_tree.Invalidate();
	m_list.Invalidate();
	
	//OnRefresh(); //F5
}

void CZETDeviceManagerDlg::OnAcDcRequest()  /////////////// выбор 1 канала в АС
{
	
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	zetlab::std::channel* channel_(nullptr);
	channel_ = object_->getAsChannel();

	if (nullptr != channel_)
	{
		uint16_t hpf = (uint16_t)(!channel_->HPF());
		channel_->setHpf(hpf);

		m_saveOnModify = true;

		m_tree.UpdateDeviceInfo(object_);
		m_tree.UpdateChannelInfo(object_);
		m_list.UpdateDeviceInfo(object_);
		m_list.UpdateChannelInfo(object_);

		SendMessage(WM_USER_SAVE);
	}

	m_tree.Invalidate();
	m_list.Invalidate();
}

void CZETDeviceManagerDlg::OnPropertiesGroupChannelRequest() /// Свойства  Выделенных каналов на 14 и 30 ус-ва  ///////////////////////////////////////////////
{
	
	std::vector<zetlab::std::channel*> channels;
	channels.clear();

	// Получаем выбранные элементы из списка
	auto selected = m_list.GetFirstSelectedItemPosition();
	std::vector<zetlab::object*> objects; // Вектор для хранения связанных объектов
	objects.clear();
	while (selected)
	{
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				channels.push_back(channel);
				objects.push_back(obj); // Сохраняем соответствующий объект
			}
		}
	}
	
	if (InterlockedCompareExchange(&m_propertiesOpened, 1, 0) != 0)
		return;

	zetlab::object* object_(nullptr);

	object_ = m_tree.GetSelectedItemObject(); //Извлекает выбранный объект из дерева (m_tree).
	
	crawler_ptr_->pause();


	m_prop_dlg = std::make_unique<properties_dlg_common>(objects, &m_cfgWorker, boost::bind(&CZETDeviceManagerDlg::requestToModbus, this, _1, _2), this);
	if (m_prop_dlg)  ///Создание и отображение диалога свойств
	{
		switch (m_prop_dlg->DoModal())
		{
		case IDOK: //сохранение изменений Применить
			if ((!object_->getAs7xxx()))  
			{

				for (auto& element : channels)  // сохранение изменений zsp канала
				{
					if(element->is_zsp())
					element->save_zsp_config();
				}

				m_saveOnModify = true;
				OnServerModify(0);
				m_saveOnModify = false;
							
				break;
			}
		default:
			InterlockedExchange(&m_propertiesOpened, 0);
			return;
		}
	}

	m_tree.UpdateDeviceInfo(object_); // Обновление интерфейса
	m_list.UpdateDeviceInfo(object_);

	for (auto& element : objects)
	{
		m_tree.UpdateDeviceInfo(element);
		m_tree.UpdateChannelInfo(element);
		m_list.UpdateDeviceInfo(element);
		m_list.UpdateChannelInfo(element);
	}


	m_tree.Invalidate();
	m_list.Invalidate();

	//v_objects.clear();
	objects.clear();

	crawler_ptr_->resume();

	InterlockedExchange(&m_propertiesOpened, 0);
	m_saveOnModify = true;  /// добавлена для Отмена
}


////////////////////////////////////////////////////
void CZETDeviceManagerDlg::OnZSPPropertiesGroupRequest()    /// ZSP делает групповую обработку устройства всех каналов сразу
{
	
	if (InterlockedCompareExchange(&m_propertiesOpened, 1, 0) != 0)
		return;

	::std::vector< zetlab::object_ptr > v_objects;
	::std::vector< zetlab::object*> objects;
	zetlab::object* object_(nullptr);

	object_ = m_tree.GetSelectedItemObject(); //Извлекает выбранный объект из дерева (m_tree).
	if (!object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();  //Если объект действительно является устройством или может быть приведён к типу устройства, метод возвращает валидный указатель на устройство.


	if (!device_) //// вылетаем с частью каналов здесь
		return;

	if (zetlab::std::demo == device_->getConnectionType()) //// Проверяет, что это устройство (не демо-режим).
		return;


	crawler_ptr_->pause();


	object_->browse(v_objects, zetlab::object_type::channel);  //Получает все каналы устройства через browse().


	for (auto& element : v_objects)
	{
		objects.push_back(element.get());
	}

	int left = 0;
	int right = objects.size() - 1;   /// Сортирует каналы по возрастанию DeviceID.
	while (left <= right) {
		for (int i = right; i > left; --i) {
			if ((objects[i - 1]->getAsChannel()->getDeviceID()) > (objects[i]->getAsChannel()->getDeviceID())) {
				swap(objects[i - 1], objects[i]);
			}
		}
		++left;
		for (int i = left; i < right; ++i) {
			if ((objects[i]->getAsChannel()->getDeviceID()) > (objects[i + 1]->getAsChannel()->getDeviceID())) {
				swap(objects[i], objects[i + 1]);
			}
		}
		--right;

	}

	if (device_->isTensostation())  // Для тензостанций исключаются каналы с isHaveBuiltinGeneratorWithCheck().
	{
		while (true)
		{
			auto it_object = std::find_if(objects.begin(), objects.end(), [](const auto& o)
				{
					return o->getAsChannel()->isHaveBuiltinGeneratorWithCheck();
				});
			if (it_object == objects.end())
				break;
			objects.erase(it_object);
		}
	}

	m_prop_dlg = std::make_unique<properties_dlg_common>(objects, &m_cfgWorker, boost::bind(&CZETDeviceManagerDlg::requestToModbus, this, _1, _2), this);
	if (m_prop_dlg)  ///Создание и отображение диалога свойств
	{
		switch (m_prop_dlg->DoModal())
		{
		case IDOK:
			if (!object_->getAs7xxx())
			{
				if (objects[0]->getAsChannel()->is_zsp())  // сохранение изменений zsp канала
				{
					for (size_t i = 0; i < objects.size(); i++)
					{
						objects[i]->getAsChannel()->save_zsp_config();
					}
					//objects[0]->getAsChannel()->save_zsp_config();
				}

				m_saveOnModify = true;
				OnServerModify(0);
				m_saveOnModify = false;
				break;
			}
		default:
			InterlockedExchange(&m_propertiesOpened, 0);
			return;
		}
	}


	m_tree.UpdateDeviceInfo(object_); // Обновление интерфейса
	m_list.UpdateDeviceInfo(object_);

	for (auto& element : objects)
	{
		m_list.UpdateChannelInfo(element);
		m_tree.UpdateChannelInfo(element);
	}

	v_objects.clear();
	objects.clear();

	crawler_ptr_->resume();

	InterlockedExchange(&m_propertiesOpened, 0);

}



/////////////////////////////////////////////////


void CZETDeviceManagerDlg::OnPropertiesGroupRequest()    /// делает групповую обработку устройства всех каналов сразу
{
	
	if (InterlockedCompareExchange(&m_propertiesOpened, 1, 0) != 0)
		return;

	::std::vector< zetlab::object_ptr > v_objects;
	::std::vector< zetlab::object*> objects;
	zetlab::object* object_(nullptr);

	object_ = m_tree.GetSelectedItemObject(); //Извлекает выбранный объект из дерева (m_tree).
	if (!object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();  //Если объект действительно является устройством или может быть приведён к типу устройства, метод возвращает валидный указатель на устройство.

	
	if (!device_) //// вылетаем с частью каналов здесь
		return;

	if (zetlab::std::demo == device_->getConnectionType()) //// Проверяет, что это устройство (не демо-режим).
		return;
	

	crawler_ptr_->pause();


	object_->browse(v_objects, zetlab::object_type::channel);  //Получает все каналы устройства через browse().


	for (auto& element : v_objects)
	{
		objects.push_back(element.get());
	}

	int left = 0;
	int right = objects.size() - 1;   /// Сортирует каналы по возрастанию DeviceID.
	while (left <= right) {
		for (int i = right; i > left; --i) {
			if ((objects[i - 1]->getAsChannel()->getDeviceID()) > (objects[i]->getAsChannel()->getDeviceID())) {
				swap(objects[i - 1], objects[i]);
			}
		}
		++left;
		for (int i = left; i < right; ++i) {
			if ((objects[i]->getAsChannel()->getDeviceID()) > (objects[i + 1]->getAsChannel()->getDeviceID())) {
				swap(objects[i], objects[i + 1]);
			}
		}
		--right;

	}

	if (device_->isTensostation())  // Для тензостанций исключаются каналы с isHaveBuiltinGeneratorWithCheck().
	{
		while (true)
		{
			auto it_object = std::find_if(objects.begin(), objects.end(), [](const auto& o)
				{
					return o->getAsChannel()->isHaveBuiltinGeneratorWithCheck();
				});
			if (it_object == objects.end())
				break;
			objects.erase(it_object);
		}
	}

	m_prop_dlg = std::make_unique<properties_dlg_common>(objects, &m_cfgWorker, boost::bind(&CZETDeviceManagerDlg::requestToModbus, this, _1, _2), this);  
	if (m_prop_dlg)  ///Создание и отображение диалога свойств
	{
		switch (m_prop_dlg->DoModal())
		{
		case IDOK:
			if (!object_->getAs7xxx())
			{
				m_saveOnModify = true;
				OnServerModify(0);
				m_saveOnModify = false;
				break;
			}
		default:
			InterlockedExchange(&m_propertiesOpened, 0);
			return;
		}
	}

	
	m_tree.UpdateDeviceInfo(object_); // Обновление интерфейса
	m_list.UpdateDeviceInfo(object_);

	for (auto& element : objects)
	{
		m_list.UpdateChannelInfo(element);
		m_tree.UpdateChannelInfo(element);
	}

	v_objects.clear();
	objects.clear();

	crawler_ptr_->resume();

	InterlockedExchange(&m_propertiesOpened, 0);

}

void CZETDeviceManagerDlg::OnIcpRequestMoreChannel()        ////////////////////// // ICP на выделенные каналы  ///////////////
{
	
	UpdateData();
	std::vector<zetlab::std::channel*> channels; // храним включенные каналы
	channels.clear();
	std::vector<zetlab::object*> objects;
	objects.clear();
	
	map<long, long> OnOffChannels;
	bool onOff = !AreSelectedChannelsICP();  // 1 или 0 включение/ выключение 
		// Получаем выбранные элементы из списка
	auto object = m_tree.GetSelectedItemObject();
	auto selected = m_list.GetFirstSelectedItemPosition();
	
	while (selected)
	{
		long lMask = 0;
		int nItem = m_list.GetNextSelectedItem(selected);
		DWORD_PTR data = m_list.GetItemData(nItem);
		zetlab::object* obj = reinterpret_cast<zetlab::object*>(data);

		if (obj)
		{
			if (auto channel = obj->getAsChannel())
			{
				if (channel->is_zsp())  // на  выделенные каналы zsp
				{
					channels.push_back(channel);
					objects.push_back(obj);

					if (auto mode = channel->getAdcInputMode(); mode != 1 && mode != 3)
							channel->setHpf((uint16_t)onOff);		

					//channel->setICPMoreZsp(onOff);
					///if(count == 23)
					channel->setICP(onOff); //zetlab::zsp_channel::set_icp(onOff);//
					//if (channel_[0]->is_zsp())  // сохранение изменений zsp канала
					//{
					//	channel_[0]->save_zsp_config();
					//}
				}

				else  // на каналы 14ого уст-ва
				{
					channels.push_back(channel);
					objects.push_back(obj);

					long module = channel->getDevice()->getModule(); // получение модуля ус-ва
					long channelIndex = channel->getDeviceID();
					if (OnOffChannels.empty())
					{
						OnOffChannels[module] = (lMask |= (1 << channelIndex));
					}
					else
					{
						if (OnOffChannels.count(module))
						{
							long inMapRezult = OnOffChannels[module];
							inMapRezult |= (1 << channelIndex);
							OnOffChannels[module] = inMapRezult;
						}
						else
						{
							OnOffChannels[module] = (lMask |= (1 << channelIndex));
						}
					}

				}

				
			}

		}
	}
	if (channels[0]->is_zsp())  // сохранение изменений zsp канала
	{
		channels[0]->save_zsp_config();
	}
	

	if (!channels.empty() && object->getAsChannel()->getType() == 14)  // бит маска для устройства 14
	{
		for (auto& [modul, mask] : OnOffChannels) {
		
			channels[0]->setICPMoreChannel(modul, mask, onOff);
		}
	}

	if (!channels.empty())    /// включение АС при переключении канала на ICP
	{
		for (auto channel : channels)
		{
			if (auto mode = channel->getAdcInputMode(); mode != 1 && mode != 3)
				channel->setHpf((uint16_t)1);
		}
	}

	m_saveOnModify = true;

	for (auto& obj : objects)
	{
		m_tree.UpdateDeviceInfo(obj);
		m_tree.UpdateChannelInfo(obj);
		m_list.UpdateDeviceInfo(obj);
		m_list.UpdateChannelInfo(obj);
	}
	//SendMessage(WM_USER_SAVE);
	m_tree.Invalidate();
	m_list.Invalidate();
	//OnRefresh();

}

void CZETDeviceManagerDlg::OnIcpRequest()  /// ICP на 1 канал
{
	
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	
	if (nullptr == object_)
		return;

	zetlab::std::channel* channel_(nullptr);
	channel_ = object_->getAsChannel();
	
	if (nullptr != channel_)
	{
		long icp = (long)(!channel_->getICP());
		channel_->setICP(icp);
		if (auto mode = channel_->getAdcInputMode(); mode != 1 && mode != 3)
			channel_->setHpf((uint16_t)icp);

		m_saveOnModify = true;

		m_tree.UpdateDeviceInfo(object_);
		m_tree.UpdateChannelInfo(object_);
		m_list.UpdateDeviceInfo(object_);
		m_list.UpdateChannelInfo(object_);
	}

	m_tree.Invalidate();
	m_list.Invalidate();
}

void CZETDeviceManagerDlg::OnChargeRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	zetlab::std::channel* channel_(nullptr);
	channel_ = object_->getAsChannel();

	if (nullptr != channel_)
	{
		long charge = (long)(!channel_->getCharge());
		channel_->setCharge(charge);

		m_saveOnModify = true;

		m_tree.UpdateDeviceInfo(object_);
		m_tree.UpdateChannelInfo(object_);
		m_list.UpdateDeviceInfo(object_);
		m_list.UpdateChannelInfo(object_);
	}

	m_tree.Invalidate();
	m_list.Invalidate();
}

void CZETDeviceManagerDlg::OnBlinkRequest()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();

	if (nullptr == object_)
		return;

	zetlab::std::device* device_(nullptr);
	device_ = object_->getAsDevice();

	if (nullptr == device_)
		return;

	if (device_->isBlinking())
	{
		device_->stopBlinking();
		m_tree.stopBlinking(object_);
		m_list.stopBlinking(object_);
	}
	else
	{
		device_->startBlinking();
		m_tree.startBlinking(object_);
		m_list.startBlinking(object_);
	}
}

void CZETDeviceManagerDlg::OnSettingsRequest()
{
	CSettingsDlg* m_settings = new CSettingsDlg;
	m_settings->DoModal();

	if (m_settings != nullptr)
	{
		delete m_settings;
		m_settings = nullptr;
	}
}

void CZETDeviceManagerDlg::OnSensorsDbRequest()
{
	CSensorsDlg* m_sensors = new CSensorsDlg;
	m_sensors->DoModal();

	if (m_sensors != nullptr)
	{
		delete m_sensors;
		m_sensors = nullptr;
	}
}

void CZETDeviceManagerDlg::OnForeignSensorsDbRequest()
{
	ShellExecute(NULL, _T("open"), L"ExtModbusDeviceConfigurator.exe", L"", _T(""), SW_SHOW);
}
void CZETDeviceManagerDlg::OnStartConverter()
{
	CFolderDialog* cd = new CFolderDialog(g_sSelectingFolderSilentConverter + L"... ", L"", L"", this->GetSafeHwnd());

	CString _currPCDir(L"");
	if (cd->DoModal() == IDOK)
	{
		_currPCDir = cd->GetSelectedFolder();

		if (_currPCDir.Right(1) != L"\\")
			_currPCDir += L'\\';

		bool FileForConvertationFound(false);

		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(_currPCDir + L"*", &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && zetlab::utils::check_convertible(findFileData.cFileName))
				{
					FileForConvertationFound = true;
					break;
				}
			} while (FindNextFile(hFind, &findFileData) != NULL);
			FindClose(hFind);
		}

		if (FileForConvertationFound)
		{
			if (_currPCDir.Right(1) == L"\\")
				_currPCDir.TrimRight(L"\\");
			CString params;
			params.Format(L"-path \"%s\"", _currPCDir.GetString());
			ShellExecute(NULL, _T("open"), L"SilentConverter.exe", params, _T(""), SW_SHOW);
		}
		else
			MessageBox(g_sEmptyFilesForConv, g_sError, MB_OK);
	}

	if (cd)
	{
		delete cd;
		cd = nullptr;
	}
}

void CZETDeviceManagerDlg::OnUserSave()
{
	path          path_;
	zetlab::saver saver_(factory_ptr_);

	path_.Create(this);
	BSTR	bstrDirConfig = _T("DirConfig");
	CString	cstrTabConfig = path_.ZetPath(&bstrDirConfig) + _T("devices.cfg");

	saver_.save((LPCTSTR)cstrTabConfig);
}

void CZETDeviceManagerDlg::OnExit()
{
	PostQuitMessage(WM_QUIT);
}

void CZETDeviceManagerDlg::OnDetailedOrShort()
{
	m_detailed = !m_detailed;

	if (m_detailed)
		OnDetailed();
	else
		OnShort();
}

void CZETDeviceManagerDlg::OnDetailed()
{
	GetMenu()->CheckMenuRadioItem(ID_DETAILED, ID_SHORT, ID_DETAILED, MF_BYCOMMAND);

	m_detailed = true;
	m_toolBar.SetButtonStyle(m_toolBar.CommandToIndex(ID_DETAIL), TBBS_PRESSED);
	m_tree.ShowWindow(SW_HIDE);
	m_list.ShowWindow(SW_SHOW);
}

void CZETDeviceManagerDlg::OnShort()
{
	GetMenu()->CheckMenuRadioItem(ID_DETAILED, ID_SHORT, ID_SHORT, MF_BYCOMMAND);

	m_detailed = false;
	m_toolBar.SetButtonStyle(m_toolBar.CommandToIndex(ID_DETAIL), TBBS_BUTTON);
	m_tree.ShowWindow(SW_SHOW);
	m_list.ShowWindow(SW_HIDE);
}

void CZETDeviceManagerDlg::OnRefresh()   // обновление F5
{
	m_tree.DeleteAllItems();
	m_list.DeleteAllItems();

	SetTimer(3, 250, NULL);
}

void CZETDeviceManagerDlg::OnHelp(void)
{
	m_OnHelp.OnHelp();
}

void CZETDeviceManagerDlg::OnNeedWizard(zetlab::object* object_)
{
	CWizardSheet wizard(object_, this);
	wizard.DoModal();
}

void CZETDeviceManagerDlg::OnError(zetlab::object* object_, bool arise_)
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
	ei.Error_Number = 0x0013;
	ei.Error_Arose = arise_;

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = sizeof(ei);
	cds.lpData = &ei;
	::SendMessage(zetlab_hwnd, WM_COPYDATA, (WPARAM)zetlab_hwnd, (LPARAM)&cds);
}

void CZETDeviceManagerDlg::CheckSensorsFile()
{
	path path_;

	path_.Create(this);
	BSTR	bstrParam = _T("DirConfig");
	CString	sensorsConfig = path_.ZetPath(&bstrParam) + _T("sensors.cfg");

	// Проверяем существование файла
	if (!PathFileExists(sensorsConfig))
	{
		// файл не существует, копируем его %ZETLab%\template
		CString csDatchFileNew;

		BSTR bstrParam = _T("InstallLocation");
		csDatchFileNew = path_.ZetPath(&bstrParam) + _T("template\\sensors.cfg");

		CopyFile(csDatchFileNew, sensorsConfig, TRUE);
	}
}

void CZETDeviceManagerDlg::OnDetectZet76()
{
	if (m_notifiedDetectZet76)
		return;

	// Проверяем наличие конфигурационного файла
	// Если он есть, то выдавать оповещение уже не нужно
	std::wstring config_dir = ::zpGetDirConfig();
	if (!config_dir.empty() && ::zfFileExistsW((config_dir + L"NetServerNew.cfg").c_str()))
		return;

	// Выдаем предупреждение и предлагаем запустить NetWizard
	m_notifiedDetectZet76 = true;
	if (::MessageBoxW(GetSafeHwnd(), g_sDetectedZet76, g_sNetWizard, MB_ICONINFORMATION | MB_OK) != IDOK)
		return;

	std::wstring zetlab_dir = ::zpGetInstallLocation();
	::ShellExecuteW(NULL, L"open", L"NetWizardNew.exe", L"", zetlab_dir.c_str(), SW_SHOW);
}

void CZETDeviceManagerDlg::RestoreWindow()
{
	SetWindowPos(&wndTop, 0, 0,
		theApp.GetProfileInt(_T(""), _T("CX"), 600),
		theApp.GetProfileInt(_T(""), _T("CY"), 400), SWP_NOMOVE);

	if (theApp.GetProfileInt(_T(""), _T("Maximized"), 0))
		ShowWindow(SW_MAXIMIZE);

	if (theApp.GetProfileInt(_T(""), _T("Detailed"), 1))
		OnDetailed();
	else
		OnShort();
}

void CZETDeviceManagerDlg::SaveWindow()
{
	WINDOWPLACEMENT wps;

	GetWindowPlacement(&wps);

	theApp.WriteProfileInt(_T(""), _T("CX"), wps.rcNormalPosition.right - wps.rcNormalPosition.left);
	theApp.WriteProfileInt(_T(""), _T("CY"), wps.rcNormalPosition.bottom - wps.rcNormalPosition.top);
	theApp.WriteProfileInt(_T(""), _T("Maximized"), wps.showCmd == SW_SHOWMAXIMIZED);
	theApp.WriteProfileInt(_T(""), _T("Detailed"), int(m_detailed));
	theApp.WriteProfileInt(_T(""), _T("Autosave"), 1);
}

void CZETDeviceManagerDlg::InitJumpList()
{
	CString csPath;

	GetModuleFileName(NULL, csPath.GetBuffer(MAX_PATH), MAX_PATH);
	csPath.ReleaseBuffer();

	csLocalization = g_sEditSensorDatabase;
	m_jumpList.AddTask(csPath, _T("sdb"), csLocalization, csPath, 2);
}
//------------------------------------------------------------------------------
void CZETDeviceManagerDlg::OnSizing(UINT wParam, LPRECT pRect) {// ограничение минимальных размеров окна
	if (pRect->right - pRect->left < Width_Min) {
		pRect->left = m_rectOld.left;
		pRect->right = m_rectOld.right;
	}
	if (pRect->bottom - pRect->top < Height_Min) {
		pRect->top = m_rectOld.top;
		pRect->bottom = m_rectOld.bottom;
	}
	m_rectOld = *pRect; // запоминаю последнее положение
	CRect rect;
	m_toolBar.GetWindowRect(rect);
	m_list.CheckSize(pRect->bottom - pRect->top - (rect.Height() + GetSystemMetrics(SM_CYMENUSIZE)) * 2);
}

void CZETDeviceManagerDlg::CheckCommandLine()
{
	CString sCommandLine = GetCommandLine();
	int nArgs;
	LPWSTR* szArglist = CommandLineToArgvW(sCommandLine, &nArgs);

	if (nArgs <= 1)
		return;

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = (sCommandLine.GetLength() + 1) * sizeof(wchar_t);
	cds.lpData = (PVOID)sCommandLine.GetString();
	if (cds.lpData != 0)
		memcpy_s(cds.lpData, cds.cbData, sCommandLine.GetString(), cds.cbData);
	::SendMessage(m_hWnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}

BOOL CZETDeviceManagerDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CString sCommandLine = (LPWSTR)(pCopyDataStruct->lpData);
	int nArgs(0);
	LPWSTR* szArglist = CommandLineToArgvW(sCommandLine, &nArgs);
	m_cmdParams.clear();

	if (nullptr != szArglist)
	{
		for (int i = 1; i < nArgs; ++i)
		{
			CString sParam = szArglist[i];
			if (++i < nArgs)
				m_cmdParams[sParam] = szArglist[i];
		}
	}

	SetTimer(2, 100, NULL);

	return CAnchoredDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CZETDeviceManagerDlg::OnPropertiesSave()
{
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (object_ && object_->getAs7xxx())
	{
		bool isModify(false);

		if (!(!isModify && !object_->isModify()))
		{
			m_tree.ChangeName(object_, isModify);
			m_list.ChangeName(object_, isModify);

			object_->setModify(isModify);

			m_tree.UpdateDeviceInfo(object_);
			m_tree.UpdateChannelInfo(object_);
			m_list.UpdateDeviceInfo(object_);
			m_list.UpdateChannelInfo(object_);
		}

		object_->getAs7xxx()->SetCfgWorkerPtr(&m_cfgWorker);
		object_->getAs7xxx()->saveSettings();
	}
}

void CZETDeviceManagerDlg::OnRefreshContext()
{
	
	zetlab::object* object_(nullptr);
	object_ = m_tree.GetSelectedItemObject();
	if (object_ && object_->getAs7xxx())
	{
		object_->getAs7xxx()->SetCfgWorkerPtr(&m_cfgWorker);
		m_cfgWorker.setNodeDevice(getNodeFromShotName(object_->getName().c_str()));
		object_->getAs7xxx()->removeFileConfiguration();
	}
}

void CZETDeviceManagerDlg::OnStreamData()
{
	CString s = _T("DirConfig");
	BSTR dir_config = s.AllocSysString();
	stream_data_worker worker(m_path.ZetPath(&dir_config).GetString());
	SysFreeString(dir_config);
	if (auto object = m_tree.GetSelectedItemObject(); worker.is_stream_data_enable(object))
		worker.change_stream_data_state(object);
}

void CZETDeviceManagerDlg::OnMeasureData()
{
	CString s = _T("DirConfig");
	BSTR dir_config = s.AllocSysString();
	stream_data_worker worker(m_path.ZetPath(&dir_config).GetString());
	SysFreeString(dir_config);
	if (auto object = m_tree.GetSelectedItemObject(); worker.is_measure_data_enable(object))
		worker.change_measure_data_state(object);
}

// void CZETDeviceManagerDlg::OnPropertiesSaveAll()
// {
// 	zetlab::object* object_(nullptr);
// 	object_ = m_tree.GetSelectedItemObject();
// 	if (object_ && object_->getAs7xxx())
// 	{
// 		m_tree.ChangeNameAll(object_, false);
// 		m_list.ChangeNameAll(object_, false);
// 		object_->setModify(false, true);
// 
// 		m_tree.UpdateDeviceInfo(object_);
// 		m_tree.UpdateChannelInfo(object_);
// 		m_list.UpdateDeviceInfo(object_);
// 		m_list.UpdateChannelInfo(object_);
// 				
// 		object_->getAs7xxx()->SetCfgWorkerPtr(&m_cfgWorker);
// 		object_->getAs7xxx()->saveSettings(true);	
// 	}
// }

CString& CZETDeviceManagerDlg::GetNameSelectedItem()
{
	return m_sNameSelectedItem;
}

void CZETDeviceManagerDlg::OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_isClickItem = true;
}

void CZETDeviceManagerDlg::OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult) //Обрабатывает клик на элементе списка Получает объект, связанный с выбранным элементом 
{                                                                         // Если объект существует, сохраняет его полный путь в переменную класса
	zetlab::object* object_(nullptr);
	object_ = m_list.GetSelectedItemObject();

	if (nullptr == object_)
		return;
	else
		m_sNameSelectedItem = object_->getFullPath().c_str();
}

void CZETDeviceManagerDlg::OnOpenXML()
{
	CString sConfigPath(zpGetDirConfig().c_str());
	sConfigPath.Append(_T("ZET7xxx\\"));

	// получаем путь к файлу
	CFileDialog dlg(TRUE,
		NULL,
		sConfigPath,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы устройств") + CString(_T(" (*.xml)|*.xml|")) + _T("Все файлы") + _T(" (*.*)|*.*|"));//Файлы данных (*.dtu)|*.dtu|Все файлы (*.*)|*.*|

	if (dlg.DoModal() == IDOK)
	{
		// готовим окно для отображения дерева
		if (m_pTreeForCompare.get() == nullptr)
		{
			m_pTreeForCompare = std::make_unique< zetlab::СTreeForCompare >();
			m_pTreeForCompare->setPathXml(dlg.GetPathName());
			m_pTreeForCompare->Create(IDD_TREE_FOR_COMPARE);
		}
		else
		{
			m_pTreeForCompare->setPathXml(dlg.GetPathName());
			m_pTreeForCompare->updateTree();
		}

		m_pTreeForCompare->ShowWindow(SW_SHOW);
		m_pTreeForCompare->SetFocus();
		m_pTreeForCompare->SetActiveWindow();
	}

}

long CZETDeviceManagerDlg::getNodeFromShotName(CString sName)
{
	CString sNode(_T(""));
	sNode = sName.Mid(sName.ReverseFind(_T('(')) + 1, sName.ReverseFind(_T(')')) - sName.ReverseFind(_T('(')) - 1);
	return _wtol(sNode);
}

LRESULT CZETDeviceManagerDlg::OnEnableDevice(WPARAM wParam, LPARAM lParam)
{
	zetlab::object* device(nullptr);
	device = reinterpret_cast<zetlab::object*>(reinterpret_cast<void*>(lParam));

	auto device7xxx = device->getAs7xxx();

	if (device7xxx != nullptr)
		device7xxx->setOffChannel(static_cast<zetlab::_7xxx::flagViewLable>(wParam));

	OnInvalidate();
	return 0;
}

LRESULT CZETDeviceManagerDlg::OnEnableData(WPARAM wParam, LPARAM lParam)
{
	long rserial = static_cast<long>(lParam);
	unsigned long long serial(static_cast<long>(wParam));
	serial = serial << 32 | rserial;
	createQueryLaunch(serial, true);

	// удаляем серийник 
	auto it = m_setSerialRunData.find(serial);
	if (it != m_setSerialRunData.end())
		m_setSerialRunData.erase(it);

	return 0;
}

void CZETDeviceManagerDlg::funcCheckModbus(CXMLWorker* ptr)
{
	bool flagNew(false);
	bool flagOld(false);

	while (true)
	{
		if (g_flagEndThreadCheckModbus)
			break;

		flagNew = ShellFindProcesses(_T("MODBUSZETLAB.exe"));

		if (flagNew != flagOld)
		{
			flagOld = flagNew;
			ptr->setFlagModbus(flagOld);
		}

		Sleep(1000);
	}

	g_flagEndThreadCheckModbus = false;
}

#include <tlhelp32.h>
bool CZETDeviceManagerDlg::ShellFindProcesses(LPCTSTR progName)
{
	bool flagReturn(false);

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	for (BOOL bFound = ::Process32First(hSnap, &pe); bFound; bFound = ::Process32Next(hSnap, &pe))
	{
		if (_tcsicmp(pe.szExeFile, progName) == 0)
		{
			flagReturn = true;
			break;
		}
	}
	::CloseHandle(hSnap);

	return flagReturn;
}

bool CZETDeviceManagerDlg::requestToModbus(_In_ Interconnection reqStruct, _Out_ BYTE* repl)
{
	boost::recursive_mutex::scoped_lock(m_mutex);

	bool isOk(true);
	int countAttemptRecconect(3);

	for (int i = 0; i < countAttemptRecconect; ++i)
	{
		if (m_pSharePipeClient->RequestData(reinterpret_cast<BYTE*>(&reqStruct), sizeof(reqStruct), repl, 1) == FALSE)
		{
			m_pSharePipeClient->Reconnect();
			isOk = false;
		}
		else
			break;

		Sleep(1000);		// подождем одну секунду
	}

	return isOk;
}

bool CZETDeviceManagerDlg::createQueryLaunch(_In_ const unsigned long long& serial, _In_ bool flagLaunch)
{
	bool isOk(true);
	BYTE repl(0);
	Interconnection reqStruct;
	reqStruct._serial = serial;
	reqStruct._flagLaunch = flagLaunch;
	reqStruct._requestType = RequestType::launch_type;
	reqStruct._launchType = LaunchType::launch_all;

	if (!requestToModbus(reqStruct, &repl))
	{
		OutputDebugString(L"#test FAIL Send QueryLaunch To Modbuszetlab");
		isOk = false;
	}

	return isOk;
}

long CZETDeviceManagerDlg::CheckShield()
{
	char lerr = 0;
	if (Shield(24, &lerr) >= 0 && !(lerr & 1))
		return 1;
	else
		AfxMessageBox(g_sForProgrammableComponents);
	return 0;
}

void CZETDeviceManagerDlg::onUpdateData(zetlab::object* object_)
{
	m_tree.Update(object_);
	m_list.Update(object_);
}

void CZETDeviceManagerDlg::onAddData(zetlab::object* object_, bool isSelect)  //objectList формирование objectList
{
	std::lock_guard m_lock(zObj_mutex);
	 static int x = 0;
	if (!addDataFlag)
	{
		addDataFlag = true;
		this->EnableWindow(FALSE);
		if (::GetForegroundWindow() == m_hWnd)
			waitDialog->ShowWindow(SW_SHOW);
	}
	if (x> 0 && x<= 2) //(x != 0 && x % 4 == 0)   ////   лишнее, для формирования обьектов
	{
		objectList.push_back(objectList[0]);
		//objectList.push_back(object_);
	}
	else
	objectList.push_back(object_);

	++x;
	SetTimer(WAIT_DATA_TIMER, 1000, NULL);
}

void CZETDeviceManagerDlg::onAddDataEnded() // здесь добавляется list / tree из objectList
{
	std::lock_guard m_lock(zObj_mutex);

	KillTimer(WAIT_DATA_TIMER);

	if (addDataFlag)  // флаг, указывающий, что шёл процесс добавления данных.
	{
		addDataFlag = false;
		this->EnableWindow(TRUE);
		waitDialog->ShowWindow(SW_HIDE);
	}
	int z = 0;
	
	for (auto object_ = 0; object_< objectList.size();  object_++)
	{
		
		if(1)// (object_->getAsDevice()->getType() == 37)  для тип 37 код здесь
		{
			m_list.AddRow037(objectList[object_], false); //m_list.AddEmptyRow(objectList[0], false); //AddRow037
			m_tree.AddRow037(objectList[object_], false);  //AddRow037
	
		}
		else
		{
			m_list.Add(objectList[object_], false); //данные отображаются в виде таблицы с колонками (атрибуты устройств/каналов). 
			m_tree.Add(objectList[object_], false);  // Формируется иерархия на основе родительских связей (object_->getParent()).
		}
		++z;
	}
	m_list.SortAll();
	m_tree.SortAll();

	OnInvalidate();
	objectList.clear();

}

void CZETDeviceManagerDlg::onRemoveData(zetlab::object* object_)
{
	if (InterlockedCompareExchange(&m_propertiesOpened, 1, 1) != 0)
	{
		if (auto o = m_prop_dlg->get_object())
		{
			if (object_->getFullPath() == o->getFullPath())
			::SendMessage(m_prop_dlg->m_hWnd, WM_CLOSE, 0, 0);
	}
	}
	m_list.Remove(object_);
	m_tree.Remove(object_);
}

void CZETDeviceManagerDlg::OnAccel()
{
	//throw;
	//throw;
}
