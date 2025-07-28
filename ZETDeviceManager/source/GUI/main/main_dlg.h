#pragma once

#include <Localized/Localized.h>
#include <AnchoredDialog/AnchoredDialog.h>
#include "../../algo/factory/factory.hpp"
#include "../../algo/crawler/crawler.hpp"

#include "toolbar/main_toolbar.h"
#include "tree/tree.h"
#include "list/list.h"

#include <Dialog_ZET\Translate.h>
#include "resource\Phrases.h"
#include <Dialog_ZET\OnHelp.h>

#include "СTreeForCompare.h"
#include <NamedPipes\Client\zetPipeClient.h>
//#include "../properties/PropertiesDlg.h"
#include "../properties/properties_dlg_base.h"
#include "../waitForm/WaitFormDlg.h"

//
#define Width_Min 320
#define Height_Min 256

#define WAIT_DATA_TIMER 5


class CZETDeviceManagerDlg :
	public CAnchoredDialog,
	public CLocalized {
public:
	CZETDeviceManagerDlg(CWnd* pParent = NULL);
	~CZETDeviceManagerDlg();
	RECT m_rectOld;
	CArray<int, int> m_selectedItems;  // Индексы выделенных строк в списке 5,06
	enum { IDD = IDD_MAIN };
	COnHelp m_OnHelp;
	bool AreSelectedChannelsEnabled();  /// сравнение - есть активные каналы или нет, для меню пкм
	bool AreSelectedChannelsHPF(); //проверка канала на AC
	bool AreSelectedChannelsICP(); // проверка канала на ICP
	bool ChannelsSameType(); // проверка типа каналов
	int FindFirstSelectableItem(CListCtrl* pList);
	bool IsSelectableItem(CListCtrl* pList, int nItem, zetlab::object* obj);

	//int  AreSelectedChannelsEnableDisable();  // включенные / выключенные каналы

protected:
	void OnInvalidate(); // Обновляет отображение дерева и списка устройств.
	void PrepareContextMenu(CMenu** context_menu); //Формирует контекстное меню для устройств/каналов с учётом их типа и состояния
	void funcCheckModbus(CXMLWorker* ptr);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg); //  Обрабатывает горячие клавиши (через акселератор m_hAccel).
	virtual void OnOK() {};
	virtual void OnCancel() {};

	afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void    OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void    OnDestroy();
	afx_msg void    OnClose();
	afx_msg void	OnSizing(UINT wParam, LPRECT pRect);
	afx_msg void    OnServerModify(long type); //
	afx_msg void    OnTimer(UINT_PTR nIDEvent);  //Обработка асинхронных операций: Сохранение изменений  Обработка командной строки
	afx_msg BOOL	OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void    OnTreeSelChange(NMHDR* pNMHDR, LRESULT* pResult); //Обрабатывает изменение выбора элемента в дереве.1 клик изменения
	afx_msg void	OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void    OnListSelChange(NMHDR* pNMHDR, LRESULT* pResult); //Синхронизируют выделение между деревом устройств (m_tree) и списком (m_list). 1 клик изменения
	afx_msg void    OnTreeDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);  //Обработка двойного клика:
//	afx_msg void    OnLButtonDown(UINT nFlags, CPoint point); // обработка ctrl + лкм
	//afx_msg void    OnContextMenu(CWnd* pWnd, CPoint point);/////////// 5,06
	//afx_msg void    OnDeleteSelectedItems(); /////////////
	//afx_msg void    OnEditCopy();////////////
	//afx_msg void    OnEditDelete();/////////////5,06
	afx_msg void    OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);  //Обработка двойного клика:
	afx_msg void    OnPropertiesRequest();  //   Открывает диалог свойств для выбранного объекта (устройства/канала)
	afx_msg void    OnPropertiesExportRequest();  //Экспорт настроек устройства в файл
	afx_msg void    OnPropertiesImportRequest();  //Импорт настроек из файла
	afx_msg void    OnPropertiesDefaultRequest(); //Восстановление настроек по умолчанию
	afx_msg void    OnServiceModeRequest(); //Переключение режима обслуживания для ZSP-устройств
	afx_msg void    OnPropertiesSave();  //  сохраняет изменения
	afx_msg void    OnRefreshContext();
	afx_msg void    OnStreamData();
	afx_msg void    OnMeasureData();
	afx_msg void    OnDisableRequest(); // включает/выключает канал
	afx_msg void    OnAcceptRequest();
	afx_msg void    OnAcDcRequest();  // переключает режим AC/DC
	afx_msg void    OnPropertiesGroupRequest();  // Групповая обработка всех каналов устройства Использует properties_dlg_common для отображения свойств
	afx_msg void    OnZSPPropertiesGroupRequest(); // на zsp каналы все сразу
	afx_msg void    OnPropertiesGroupChannelRequest(); // Обработка свойств для выбранных каналов (множественный выбор)
	afx_msg void    OnDisableRequestMoreChannel();  //Групповое включение/отключение выбранных каналов
	afx_msg void    OnIcpRequest();  //правляет ICP-питанием
	afx_msg void    OnIcpRequestMoreChannel(); // ICP на выделенные каналы
	afx_msg void    OnAcDcRequestMoreChannel(); // AC групповое выделение
	afx_msg void    OnChargeRequest();
	afx_msg void    OnBlinkRequest();  //Включение/выключение мигания для устройств
	afx_msg void    OnSettingsRequest();
	afx_msg void    OnSensorsDbRequest();
	afx_msg void    OnForeignSensorsDbRequest();
	afx_msg void    OnStartConverter();
	afx_msg void    OnOpenXML();
	afx_msg void    OnUserSave();
	afx_msg void    OnExit();
	afx_msg void    OnDetailedOrShort();
	afx_msg void    OnDetailed();
	afx_msg void    OnShort();
	afx_msg void    OnShiftDown();  // shift+ down
	afx_msg void    OnNMClickListNot(NMHDR* pNMHDR, LRESULT* pResult); //не выделение последнего элемента в тип 14
	afx_msg void    OnRefresh();  //обновляет список устройств Очистка и повторное сканирование
	afx_msg void    OnFormatting();  // форматирование SD-карт
	afx_msg void    OnOpen();  //Работа с SD-картами:
	afx_msg void    OnConverting(); //конвертация данных Работа с SD-картами:
	afx_msg void    OnNeedWizard(zetlab::object* object_);
	afx_msg void    CheckSensorsFile();
	afx_msg void    OnDetectZet76();

	afx_msg LRESULT OnEnableDevice(WPARAM wParam, LPARAM lParam); //включают/отключают устройства или каналы (через сообщения   WM_ENABLE_DEVICE и WM_ENABLE_DATA).
	afx_msg LRESULT OnEnableData(WPARAM wParam, LPARAM lParam);
	
	void OnHelp(void);
	void OnError(zetlab::object* object_, bool arise_);

	void RestoreWindow();
	void SaveWindow();

	void InitJumpList();
	void CheckCommandLine();  //Обработка параметров командной строки
	CString& GetNameSelectedItem();
	long getNodeFromShotName(CString sName);
	bool ShellFindProcesses(LPCTSTR progName);
	long CheckShield();
	bool requestToModbus(_In_ Interconnection reqStruct, _Out_ BYTE* repl);
	bool createQueryLaunch(_In_ const unsigned long long& serial, _In_ bool flagLaunch);

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	DECLARE_ANCHOR_MAP()
	DECLARE_INSTANCE()
	bool m_bflagWasExport;
protected:
	HICON           m_hIcon;
	HACCEL          m_hAccel;
	CJumpList       m_jumpList;
	CMainToolBar    m_toolBar;
	bool            m_detailed;
	CDevicesTree    m_tree;
	CDevicesList    m_list;
	CMenu           m_contextMenu;
	bool            m_saveOnModify;
	volatile LONG   m_propertiesOpened;
	std::map<CString, CString> m_cmdParams;
	CString			m_sNameSelectedItem;
	bool			m_isClickItem;
	bool			m_notifiedDetectZet76; // флаг, что оповещение об обнаружении ZETSENSOR уже выдавали
	bool            propertiesgroup = false;

private:
	zetlab::factory_ptr factory_ptr_;
	zetlab::zsp_factory_ptr zsp_factory_ptr_;
	zetlab::crawler_ptr crawler_ptr_;
	CCFGWorker m_cfgWorker;
	std::unique_ptr< zetlab::СTreeForCompare > m_pTreeForCompare;
	std::shared_ptr< CZetPipeClient > m_pSharePipeClient;			// для общения с modbuszetlab
	std::set< uint64_t > m_setSerialRunData;						// серийники мастеров, по которым останавливались данные
	boost::recursive_mutex m_mutex;
	std::unique_ptr<CDSRV> server;
	std::unique_ptr<properties_dlg_base> m_prop_dlg;

	std::mutex zObj_mutex;
	void onUpdateData(zetlab::object* object_);  //Асинхронное обновление интерфейса при изменениях
	void onRemoveData(zetlab::object* object_); //Асинхронное обновление интерфейса при изменениях
	void onAddData(zetlab::object* object_, bool isSelect = false);  //Асинхронное обновление интерфейса при изменениях
	void onAddDataEnded();
	std::unique_ptr<CWaitFormDlg> waitDialog;
	bool addDataFlag{ false };
	std::vector<zetlab::object*> objectList;
public:
	afx_msg void OnAccel();
};
