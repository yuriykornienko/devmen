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

#include "�TreeForCompare.h"
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
	CArray<int, int> m_selectedItems;  // ������� ���������� ����� � ������ 5,06
	enum { IDD = IDD_MAIN };
	COnHelp m_OnHelp;
	bool AreSelectedChannelsEnabled();  /// ��������� - ���� �������� ������ ��� ���, ��� ���� ���
	bool AreSelectedChannelsHPF(); //�������� ������ �� AC
	bool AreSelectedChannelsICP(); // �������� ������ �� ICP
	bool ChannelsSameType(); // �������� ���� �������
	int FindFirstSelectableItem(CListCtrl* pList);
	bool IsSelectableItem(CListCtrl* pList, int nItem, zetlab::object* obj);

	//int  AreSelectedChannelsEnableDisable();  // ���������� / ����������� ������

protected:
	void OnInvalidate(); // ��������� ����������� ������ � ������ ���������.
	void PrepareContextMenu(CMenu** context_menu); //��������� ����������� ���� ��� ���������/������� � ������ �� ���� � ���������
	void funcCheckModbus(CXMLWorker* ptr);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg); //  ������������ ������� ������� (����� ����������� m_hAccel).
	virtual void OnOK() {};
	virtual void OnCancel() {};

	afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void    OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void    OnDestroy();
	afx_msg void    OnClose();
	afx_msg void	OnSizing(UINT wParam, LPRECT pRect);
	afx_msg void    OnServerModify(long type); //
	afx_msg void    OnTimer(UINT_PTR nIDEvent);  //��������� ����������� ��������: ���������� ���������  ��������� ��������� ������
	afx_msg BOOL	OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void    OnTreeSelChange(NMHDR* pNMHDR, LRESULT* pResult); //������������ ��������� ������ �������� � ������.1 ���� ���������
	afx_msg void	OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void    OnListSelChange(NMHDR* pNMHDR, LRESULT* pResult); //�������������� ��������� ����� ������� ��������� (m_tree) � ������� (m_list). 1 ���� ���������
	afx_msg void    OnTreeDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);  //��������� �������� �����:
//	afx_msg void    OnLButtonDown(UINT nFlags, CPoint point); // ��������� ctrl + ���
	//afx_msg void    OnContextMenu(CWnd* pWnd, CPoint point);/////////// 5,06
	//afx_msg void    OnDeleteSelectedItems(); /////////////
	//afx_msg void    OnEditCopy();////////////
	//afx_msg void    OnEditDelete();/////////////5,06
	afx_msg void    OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);  //��������� �������� �����:
	afx_msg void    OnPropertiesRequest();  //   ��������� ������ ������� ��� ���������� ������� (����������/������)
	afx_msg void    OnPropertiesExportRequest();  //������� �������� ���������� � ����
	afx_msg void    OnPropertiesImportRequest();  //������ �������� �� �����
	afx_msg void    OnPropertiesDefaultRequest(); //�������������� �������� �� ���������
	afx_msg void    OnServiceModeRequest(); //������������ ������ ������������ ��� ZSP-���������
	afx_msg void    OnPropertiesSave();  //  ��������� ���������
	afx_msg void    OnRefreshContext();
	afx_msg void    OnStreamData();
	afx_msg void    OnMeasureData();
	afx_msg void    OnDisableRequest(); // ��������/��������� �����
	afx_msg void    OnAcceptRequest();
	afx_msg void    OnAcDcRequest();  // ����������� ����� AC/DC
	afx_msg void    OnPropertiesGroupRequest();  // ��������� ��������� ���� ������� ���������� ���������� properties_dlg_common ��� ����������� �������
	afx_msg void    OnZSPPropertiesGroupRequest(); // �� zsp ������ ��� �����
	afx_msg void    OnPropertiesGroupChannelRequest(); // ��������� ������� ��� ��������� ������� (������������� �����)
	afx_msg void    OnDisableRequestMoreChannel();  //��������� ���������/���������� ��������� �������
	afx_msg void    OnIcpRequest();  //�������� ICP-��������
	afx_msg void    OnIcpRequestMoreChannel(); // ICP �� ���������� ������
	afx_msg void    OnAcDcRequestMoreChannel(); // AC ��������� ���������
	afx_msg void    OnChargeRequest();
	afx_msg void    OnBlinkRequest();  //���������/���������� ������� ��� ���������
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
	afx_msg void    OnNMClickListNot(NMHDR* pNMHDR, LRESULT* pResult); //�� ��������� ���������� �������� � ��� 14
	afx_msg void    OnRefresh();  //��������� ������ ��������� ������� � ��������� ������������
	afx_msg void    OnFormatting();  // �������������� SD-����
	afx_msg void    OnOpen();  //������ � SD-�������:
	afx_msg void    OnConverting(); //����������� ������ ������ � SD-�������:
	afx_msg void    OnNeedWizard(zetlab::object* object_);
	afx_msg void    CheckSensorsFile();
	afx_msg void    OnDetectZet76();

	afx_msg LRESULT OnEnableDevice(WPARAM wParam, LPARAM lParam); //��������/��������� ���������� ��� ������ (����� ���������   WM_ENABLE_DEVICE � WM_ENABLE_DATA).
	afx_msg LRESULT OnEnableData(WPARAM wParam, LPARAM lParam);
	
	void OnHelp(void);
	void OnError(zetlab::object* object_, bool arise_);

	void RestoreWindow();
	void SaveWindow();

	void InitJumpList();
	void CheckCommandLine();  //��������� ���������� ��������� ������
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
	bool			m_notifiedDetectZet76; // ����, ��� ���������� �� ����������� ZETSENSOR ��� ��������
	bool            propertiesgroup = false;

private:
	zetlab::factory_ptr factory_ptr_;
	zetlab::zsp_factory_ptr zsp_factory_ptr_;
	zetlab::crawler_ptr crawler_ptr_;
	CCFGWorker m_cfgWorker;
	std::unique_ptr< zetlab::�TreeForCompare > m_pTreeForCompare;
	std::shared_ptr< CZetPipeClient > m_pSharePipeClient;			// ��� ������� � modbuszetlab
	std::set< uint64_t > m_setSerialRunData;						// ��������� ��������, �� ������� ��������������� ������
	boost::recursive_mutex m_mutex;
	std::unique_ptr<CDSRV> server;
	std::unique_ptr<properties_dlg_base> m_prop_dlg;

	std::mutex zObj_mutex;
	void onUpdateData(zetlab::object* object_);  //����������� ���������� ���������� ��� ����������
	void onRemoveData(zetlab::object* object_); //����������� ���������� ���������� ��� ����������
	void onAddData(zetlab::object* object_, bool isSelect = false);  //����������� ���������� ���������� ��� ����������
	void onAddDataEnded();
	std::unique_ptr<CWaitFormDlg> waitDialog;
	bool addDataFlag{ false };
	std::vector<zetlab::object*> objectList;
public:
	afx_msg void OnAccel();
};
