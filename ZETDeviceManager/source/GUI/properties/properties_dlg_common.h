#pragma once

#include "properties_dlg_base.h"

#include <IdTabsDeviceManager.h>

/*#include "../../algo/object/object.hpp"
#include "../../algo/7xxx/7xxx/device/config/config_7xxx.hpp"

#include "../tabs/tabs.h"*/
#include "../tabs/common/common_tab.h"
#include "../tabs/frequency/frequency_tab.h"
#include "../tabs/description/description_tab.h"
#include "../tabs/dig_port/digport_tab.h"
#include "../tabs/ip_address/ipaddress_tab.h"
#include "../tabs/sync/sync_tab.h"
#include "../tabs/channel/channel_tab.h"
#include "../tabs/zet037_tenso_channel/zet037_tenso_channel_tab.h"
#include "../tabs/internal_DAC/InternalDACSin_Tab.h"
#include "../tabs/internal_DAC/InternalDACPPS_Tab.h"
#include "../tabs/internal_DAC/InternalDACDC_Tab.h"
#include "../tabs/actuator/actuator_tab.h"
#include "../tabs/selftest/selftest_tab.h"
#include "../tabs/mseed_metadata/mseed_metadata_tab.h"

#include "../tabs/7xxx/7xxx_tab.h"
#include "../tabs/calibration/calibration_tab.h"
#include "../tabs/registrator_zet048/registrator_zet048_tab.h"
#include "../tabs/registrator_zsp/registrator_zsp_tab.h"

class properties_dlg_common :
	public properties_dlg_base
{
public:
	properties_dlg_common(::std::vector< zetlab::object*> objects__, CCFGWorker* pCfgWorker, boost::function< bool(Interconnection, BYTE*) > f, CWnd* pParent = NULL);

	enum { IDD = IDD_PROPERTIES };

	virtual void DoWait(const BOOL wait_ = TRUE);  //Управление курсором ожидания.

	

	bool getIsSave() { return m_isSave; }
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  // Связывает элементы управления MFC с переменными класса.
	virtual BOOL OnInitDialog();  //Настройка диалога при создании.
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnOK();  //Рассылает сообщения WM_USER_SAVE/WM_USER_CANCEL всем вкладкам
	virtual void OnCancel(); //Рассылает сообщения WM_USER_SAVE/WM_USER_CANCEL всем вкладкам

	virtual BOOL Refresh();  // обновление
	virtual BOOL RefreshDevice();  //// обновление устройства
	virtual BOOL RefreshChannel(); // // обновление канала
	virtual BOOL Refresh7xxx();

	afx_msg void OnDestroy();
	afx_msg void OnCopy(); // Копирование/вставка параметров каналов.
	afx_msg void OnPaste(); // Копирование/вставка параметров каналов.
	afx_msg void OnDisable();  //ереключение состояния "Enable/Disable" 
	afx_msg void OnRefresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSave7xxx();

	DECLARE_MESSAGE_MAP()
	
private:
	void On7xxxReady(zetlab::_7xxx::device::config config_);
	void UpdateDisableState();  //Синхронизирует текст кнопки Disable/Enable с состоянием устройства.
	void requestForUpdateToModbus();  // Асинхронный запрос данных через Modbus.

	afx_msg LRESULT OnUserReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserModify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRegistrator(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewError(WPARAM wParam, LPARAM lParam);

protected:
	CImageList                m_tabImages;

	CCommonTab                m_common;
	CFrequencyTab             m_frequency;
	CDescriptionTab			  m_description;
	CDigPortTab               m_digport;
	CIpAddressTab             m_ipaddress;
	CSyncTab                  m_sync;
	CChannelTab               m_channel;
	zet037_tenso_channel_tab  m_zet037_tenso_channel;
	CRegistratorZet048Tab	  m_regZet048;
	registrator_zsp_tab       m_zsp_registrator;
	CInternalDACSin_Tab		  m_Internal_DAC;
	CInternalDACPPS_Tab       m_Internal_PPS;
	CInternalDACDC_Tab		  m_Internal_DC;
	CActuatorTab              m_actuator;
	selftest_tab			  m_selftest;
	mseed_metadata_tab		  m_mseed_metadata;
	::std::vector< boost::shared_ptr< C7xxxTab > > m_7xxx;
	boost::shared_ptr< CCalibrationTab >  m_calibration;
	CButton                   m_disable;
	CButton                   m_refresh;
	CButton                   m_save;
	CButton                   m_cancel;
	BOOL                      m_wait;
	CButton m_save7xxx;

	int m_addIndex;

	typedef enum { Disable = 0, Enable = 1 } DisableCommand;

	DisableCommand    m_disableCommand;

private:
	//zetlab::object* object_ = nullptr;
	//std::vector<zetlab::object*> objects_;
	CCFGWorker* m_pCfgWorker;
	//int tab_;
	bool m_isSave;
	bool m_isRunThreadUpdate;
	bool m_isRefresh7xxx;
	bool m_isFinishThreadUpdate;
	bool m_group;
	boost::function< bool(Interconnection, BYTE*) > m_pFuncRequest;
	UpdateType m_updateType;
};
