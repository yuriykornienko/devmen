#include "stdafx.h"
/*#include "../../application/application.h"
#include "../user_messages.hpp"*/
#include "../../algo/utils/zsp_dev_detect/zsp_dev_detect.hpp"
#include "properties_dlg_common.h"

/*#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <mathZet.h>
#include <Dialog_ZET\ConnectToZetTools.h>*/
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

properties_dlg_common::properties_dlg_common(std::vector<zetlab::object*> objects__, CCFGWorker* pCfgWorker, boost::function< bool(Interconnection, BYTE*) > f, CWnd* pParent) :


	m_pCfgWorker(pCfgWorker),
	properties_dlg_base(objects__, properties_dlg_common::IDD, pParent),
	m_disableCommand(Disable),
	m_wait(FALSE),
	//tab_(-1),
	m_isSave(false),
	m_isRunThreadUpdate(false),
	m_isRefresh7xxx(false),
	m_isFinishThreadUpdate(true),
	m_pFuncRequest(f),
	m_updateType(UpdateType::update_dat),
	m_addIndex(0)
{
}

void properties_dlg_common::DoDataExchange(CDataExchange* pDX)
{
	properties_dlg_base::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TABS, m_tabs);

	DDX_Control(pDX, ID_DISABLE, m_disable);
	DDX_Control(pDX, ID_REFRESH, m_refresh);
	DDX_Control(pDX, IDOK, m_save);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_SAVE, m_save7xxx);
}

BEGIN_MESSAGE_MAP(properties_dlg_common, properties_dlg_base)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER_READY,       OnUserReady)
	ON_MESSAGE(WM_USER_MODIFY,      OnUserModify)
	ON_MESSAGE(WM_USER_REGISTRATOR, OnUserRegistrator)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_COPY,       OnCopy)
	ON_COMMAND(ID_PASTE,      OnPaste)
	ON_BN_CLICKED(ID_DISABLE, OnDisable)
	ON_BN_CLICKED(ID_REFRESH, OnRefresh)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SAVE, &properties_dlg_common::OnBnClickedSave7xxx)
	ON_MESSAGE(WM_UPDATE_DATA, OnUpdateData)
	ON_MESSAGE(WM_VIEW_ERROR, OnViewError)
END_MESSAGE_MAP()

BOOL properties_dlg_common::OnInitDialog()
{
	if (FALSE == properties_dlg_base::OnInitDialog())
		return FALSE;

	m_tabImages.Create(16, 16, ILC_COLOR32, 1, 1);
	m_tabs.SetImageList(&m_tabImages);

	csLocalization = g_sDisable;
	m_disable.SetWindowText(csLocalization);
	m_disable.ShowWindow(SW_HIDE);

	csLocalization = g_sUpdate;
	m_refresh.SetWindowText(csLocalization);
	m_refresh.ShowWindow(SW_HIDE);

	m_save7xxx.ShowWindow(SW_HIDE);

	switch (m_object->getType())
	{
		case zetlab::object_type::_7xxx_controller:
		case zetlab::object_type::_7xxx_is:
			if (m_object->getAs7xxx()->canEnableOrDisable())
			{
				UpdateDisableState();
				m_disable.ShowWindow(SW_SHOW);
			}

#ifdef UPDATE_FOR_ZETDEVICEMANAGER
			m_refresh.ShowWindow(SW_SHOW);
#endif // UPDATE_FOR_ZETDEVICEMANAGER	
			break;

		default:
			break;
	}

	m_pCfgWorker->SetHwndWindowDistanation(m_hWnd);

	return Refresh();
}

void properties_dlg_common::OnDestroy()
{
	for (auto& element : m_objects) {
		if (m_isRunThreadUpdate)
		{
			m_isRunThreadUpdate = false;
			while (!m_isFinishThreadUpdate)
				Sleep(100);
		}

		if (element && element->getAs7xxx())
			element->getAs7xxx()->stopReadSettings();

		CAnchoredDialog::OnDestroy();
	}
}

BOOL properties_dlg_common::Refresh()
{
#ifdef UPDATE_FOR_ZETDEVICEMANAGER
		m_refresh.EnableWindow(FALSE);
#endif // UPDATE_FOR_ZETDEVICEMANAGER
		m_save.EnableWindow(FALSE);

		switch (m_object->getType())
		{
		case zetlab::object_type::device:
			return RefreshDevice();

		case zetlab::object_type::channel:
			return RefreshChannel();

		case zetlab::object_type::_7xxx_controller:
		case zetlab::object_type::_7xxx_is:
			//		m_save7xxx.EnableWindow(FALSE);
			m_save.EnableWindow(TRUE);
			m_isRefresh7xxx = false;
			return Refresh7xxx();
		}
		return FALSE;
}

BOOL properties_dlg_common::RefreshDevice()
{
		if (m_tabs.GetItemCount() > 0)
			return TRUE;

		m_tabs.InsertItem< CCommonTab >(0, &m_common, -1, ID_TAB_COMMON, TRUE, g_sGENERAL);
		m_common.SetAdapter(m_object->getAsDevice(), m_object->getName());

		m_tabs.InsertItem< CFrequencyTab >(1, &m_frequency, -1, ID_TAB_FREQUENCY, TRUE, g_sSamplingFrequency);
		m_frequency.SetAdapter(m_object->getAsDevice());

		auto it = m_object->getAsDevice();
		if (it->isHaveDescription())
		{
			m_tabs.InsertItem< CDescriptionTab >(2, &m_description, -1, ID_TAB_DESCRIPTION, TRUE, g_sIdentification);
			m_description.SetAdapter(m_object->getAsDevice());
		}

		if (it->isHaveSd())
		{
			if (is_zsp(static_cast<uint16_t>(it->getType())))
			{
				m_tabs.InsertItem<registrator_zsp_tab>(3, &m_zsp_registrator, -1, ID_TAB_ZSP_REGISTRATOR, TRUE, g_sAUTOREGISTRATOR);
				m_zsp_registrator.SetAdapter(it);
			}
			else
			{
				m_tabs.InsertItem<CRegistratorZet048Tab>(3, &m_regZet048, -1, ID_TAB_REGZET048, TRUE, g_sAUTOREGISTRATOR);
				m_regZet048.SetAdapter(it);
			}
		}

		if (it->numDigitalPorts())
		{
			if ((it->getType() != ZET_DEVICE_ZET048) && (it->getType() != ZET_DEVICE_240))
			{
				m_tabs.InsertItem< CDigPortTab >(4, &m_digport, -1, ID_TAB_DIGPORT, TRUE, g_sDigitalPort);
				m_digport.SetAdapter(it);
			}
		}

		if (it->isHaveEth())
		{
			m_tabs.InsertItem< CIpAddressTab >(5, &m_ipaddress, -1, ID_TAB_IPADDRESS, TRUE, g_sEthernet);
			m_ipaddress.SetAdapter(it);
		}

		if (it->isHaveDigSync() ||
			it->isHaveGps() ||
			it->isHaveSync())
		{
			m_tabs.InsertItem< CSyncTab >(6, &m_sync, -1, ID_TAB_SYNC, TRUE, g_sSynchronization);
			m_sync.SetAdapter(it);
		}

		if (it->isHaveBuiltinGenDC())
		{
			m_tabs.InsertItem< CInternalDACDC_Tab >(7, &m_Internal_DC, -1, ID_TAB_INTERNAL_DC, TRUE, g_sGENERATOR);
			m_Internal_DC.SetAdapter(it);
		}

		if (it->isHaveActuator())
		{
			m_tabs.InsertItem< CActuatorTab >(8, &m_actuator, -1, ID_TAB_ACTUATOR, TRUE, g_sActuator);
			m_actuator.SetAdapter(it);
		}

		if (it->has_mseed_metadata())
		{
			m_tabs.InsertItem< mseed_metadata_tab >(9, &m_mseed_metadata, -1, ID_TAB_MSEED_METADATA, TRUE, g_s_mseed_metadata);
			m_mseed_metadata.SetAdapter(it);
		}

		if (it->has_selftest())
		{
			m_tabs.InsertItem< selftest_tab >(10, &m_selftest, -1, ID_TAB_SELFTEST, TRUE, g_s_selftest);
			m_selftest.SetAdapter(it);
		}


		int tempIndex;
		tempIndex = m_tabs.GetIndexItemTab(m_tab);

		if (tempIndex != -1)
			m_tabs.SetActiveTab(tempIndex);
		else
			m_tabs.SetActiveTab(0);

		m_tool_bar.ShowWindow(SW_HIDE);

		return TRUE;
}

BOOL properties_dlg_common::RefreshChannel()
{
	m_group = FALSE;
	
	if (m_tabs.GetItemCount() > 0)
		return TRUE;

	auto it = m_object->getAsChannel();

	if (m_objects.size() > 1)
		m_group = TRUE;

	if (it->is_zet037_tenso())
	{
		m_tabs.InsertItem< zet037_tenso_channel_tab >(0, &m_zet037_tenso_channel, -1, ID_TAB_CHANNEL, TRUE, g_sMeasureName);

		m_zet037_tenso_channel.SetAdapter(it, const_cast<zetlab::std::device*>(it->getDevice()));

		m_tabs.SetActiveTab(0);
		
		m_tool_bar.ShowWindow(TRUE);
		m_tool_bar.SetPasteAvailable(m_zet037_tenso_channel.IsClipboardAvailable());
	}
	else
	{
		m_channel.SetAdapter_s(m_objects, m_object->getParent()->getAsDevice(), m_group);

		if (it->isDemo() || !it->isFeedBackChannel())
			m_tabs.InsertItem< CChannelTab >(0, &m_channel, -1, ID_TAB_CHANNEL, TRUE, g_sMeasureName);
		else
			m_tabs.InsertItem< CChannelTab >(0, &m_channel, -1, ID_TAB_CHANNEL, TRUE, g_sGenerator);

		if (m_objects.size() > 1)
			m_channel.SetAdapter_s(m_objects, m_object->getParent()->getAsDevice(), m_group);
		if (m_object->getParent())
			m_channel.SetAdapter(it, m_object->getParent()->getAsDevice());

		if (it->isHaveBuiltinGenerator())
		{
			m_tabs.InsertItem< CInternalDACSin_Tab >(1, &m_Internal_DAC, -1, ID_TAB_INTERNAL_DAC, TRUE, g_sSINUS);
			m_Internal_DAC.SetAdapter(it);
			m_tabs.InsertItem< CInternalDACPPS_Tab >(2, &m_Internal_PPS, -1, ID_TAB_INTERNAL_PPS, TRUE, g_sPPS);
			m_Internal_PPS.SetAdapter(it);
		}

		m_tabs.SetActiveTab(0);
		m_tool_bar.ShowWindow(m_object->getAsChannel()->isFeedBackChannel() || (it->getServerID() == -1) ? SW_HIDE : SW_SHOW);

		m_tool_bar.SetPasteAvailable(m_channel.IsClipboardAvailable());
	}
	
	return TRUE;
}

BOOL properties_dlg_common::Refresh7xxx()
{
		m_save.EnableWindow(FALSE);
		m_tabs.EnableWindow(FALSE);
		//	m_save7xxx.EnableWindow(FALSE);
		DoWait();

		if (m_tabs.GetItemCount() == 0)
		{
			m_tabs.InsertItem< CCommonTab >(0, &m_common, -1, ID_TAB_COMMON, FALSE, g_sGENERAL);
			m_tabs.SetActiveTab(0);
			m_common.SetAdapter(m_object->getAs7xxx(), m_object->getName());
			m_tabImages.Add(LOAD_ICON(IDI_LOCK, 16, 16));
			m_tool_bar.ShowWindow(SW_HIDE);
		}

		m_object->getAs7xxx()->readSettings(boost::bind(&properties_dlg_common::On7xxxReady, this, _1), m_pCfgWorker, m_isRefresh7xxx);
		return TRUE;
}

void properties_dlg_common::On7xxxReady(zetlab::_7xxx::device::config config_)
{
	SendMessage(WM_USER_READY, reinterpret_cast<WPARAM>(&config_));
}

LRESULT properties_dlg_common::OnUserReady(WPARAM wParam, LPARAM lParam)
{
		typedef zetlab::_7xxx::device::config config;
		typedef zetlab::utils::user_type      user_type;

		config* config_(nullptr);
		config_ = reinterpret_cast<config*>(wParam);

		if (!config_)
			return 0;

		user_type user = config_->getUserType();

		int last_tab = 0;

		if (m_tabs.GetItemCount() > 1)
		{
			last_tab = m_tabs.GetCurSel();
			m_tabs.SetActiveTab(0);

			m_calibration.reset();
			m_7xxx.clear();
			m_addIndex = 0;
			while (m_tabs.GetItemCount() > 1)
				m_tabs.DeleteItem(m_tabs.GetItemCount() - 1);
		}

		bool zet_440 = false;
		if (m_object->getName().find(_T("ZET 440 ")) == 0 ||
			m_object->getName().find(_T("ZET440 ")) == 0)
		{
			zet_440 = true;
		}

		int activeTab(-1);
		for (auto it = config_->begin(), end = config_->end(); it != end; ++it)
		{
			if ((*it)->getAccessType() > user)
				continue;

			bool editable(user > zetlab::utils::user_operator);
			int  nTab(0);
			int  nImage(editable ? -1 : 0);
			int  type(((*it)->getType()) & STRUCT_TYPE_MASK);

			if (type == SENSOR_CALIBRATE_STRUCT)
			{
				m_calibration = boost::make_shared< CCalibrationTab >();

				nTab = m_tabs.InsertItem< CCalibrationTab >(m_7xxx.size() + 1, m_calibration.get(), nImage, ID_TAB_CALIBRATION, m_pCfgWorker->checkItemTabForModify(static_cast<int>(m_7xxx.size()) + 1), g_sCalibration);
				++m_addIndex;

				m_calibration->SetAdapter(*it, editable);
			}
			else if (zet_440 &&
				type != DEV_PAR_STRUCT &&
				type != SETTINGS_RELAY_SWITCH_STRUCT)
			{
				continue;
			}
			else
			{
				m_7xxx.push_back(boost::make_shared< C7xxxTab >(m_pCfgWorker));

				nTab = m_tabs.InsertItem< C7xxxTab >(m_7xxx.size() + m_addIndex, m_7xxx.back().get(), nImage, ID_TAB_7xxx, /*FALSE*/m_pCfgWorker->checkItemTabForModify(static_cast<int>(m_7xxx.size())), g_sXXXX);

				m_7xxx.back().get()->SetAdapter(*it, editable);
			}
			if (m_tab == type)
				activeTab = nTab;

			m_tabs.SetTabName(nTab, (*it)->getName().c_str());
		}

		if (last_tab < m_tabs.GetItemCount())
			m_tabs.SetActiveTab(last_tab);

		if (-1 != activeTab)
			m_tabs.SetActiveTab(activeTab);

		m_tabs.EnableWindow(TRUE);

#ifdef UPDATE_FOR_ZETDEVICEMANAGER
		m_refresh.EnableWindow(TRUE);
#endif // UPDATE_FOR_ZETDEVICEMANAGER	

		DoWait(false);
		return 0;
}

BOOL properties_dlg_common::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_wait)
	{
		AfxGetApp()->DoWaitCursor(0);

		return TRUE;
	}

	return CAnchoredDialog::OnSetCursor(pWnd, nHitTest, message);
}

void properties_dlg_common::DoWait(const BOOL wait_)
{
	m_wait = wait_;

	AfxGetApp()->DoWaitCursor(m_wait ? 1 : -1);
}

void properties_dlg_common::OnOK()  // обработка ОК на сво-во канала или групповая настройка
{
	m_channel.WriteNewDataOnOK();

	if (IsWindow(m_common.m_hWnd))			m_common.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_frequency.m_hWnd))		m_frequency.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_description.m_hWnd))		m_description.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_digport.m_hWnd))			m_digport.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_ipaddress.m_hWnd))		m_ipaddress.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_sync.m_hWnd))			m_sync.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_channel.m_hWnd))			m_channel.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_zet037_tenso_channel.m_hWnd)) m_zet037_tenso_channel.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_regZet048.m_hWnd))		m_regZet048.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_zsp_registrator.m_hWnd))	m_zsp_registrator.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_Internal_DAC.m_hWnd))	m_Internal_DAC.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_Internal_PPS.m_hWnd))	m_Internal_PPS.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_Internal_DC.m_hWnd))		m_Internal_DC.SendMessage(WM_USER_SAVE);
	if (IsWindow(m_actuator.m_hWnd))		m_actuator.SendMessage(WM_USER_SAVE);
	
	if (auto _7xxx_ = m_object->getAs7xxx())
	{
		if (_7xxx_->CheckSettings())
			_7xxx_->saveSettings();
		else
		{
			MessageBox(g_sDescription + L": \n" + g_sIdentificatorBusy, g_sZETDeviceManager, MB_OK | MB_ICONSTOP);
			return;
		}
	}
	/*else if (auto _device_ = objects_[0]->getAsDevice())
	{
		if (is_zsp(static_cast<uint16_t>(_device_->getType())))
			_device_->save_zsp_config();
	}*/

	CAnchoredDialog::OnOK();
}

void properties_dlg_common::OnCancel()
{
	
		if (IsWindow(m_common.m_hWnd))			m_common.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_frequency.m_hWnd))		m_frequency.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_description.m_hWnd))		m_description.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_digport.m_hWnd))			m_digport.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_ipaddress.m_hWnd))		m_ipaddress.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_sync.m_hWnd))			m_sync.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_channel.m_hWnd))			m_channel.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_zet037_tenso_channel.m_hWnd)) m_zet037_tenso_channel.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_regZet048.m_hWnd))		m_regZet048.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_zsp_registrator.m_hWnd))	m_zsp_registrator.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_DAC.m_hWnd))	m_Internal_DAC.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_PPS.m_hWnd))	m_Internal_PPS.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_DC.m_hWnd))		m_Internal_DC.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_actuator.m_hWnd))		m_actuator.SendMessage(WM_USER_CANCEL);

		if (m_object->getAs7xxx())				m_object->getAs7xxx()->noSaveSettings();

		CAnchoredDialog::OnCancel();
}

void properties_dlg_common::OnCopy()
{
	if (IsWindow(m_channel.m_hWnd))   m_channel.CopyToClipboard();
	if (IsWindow(m_zet037_tenso_channel.m_hWnd))   m_zet037_tenso_channel.CopyToClipboard();

	m_tool_bar.SetPasteAvailable(m_channel.IsClipboardAvailable());
}

void properties_dlg_common::OnPaste()
{
	if (IsWindow(m_channel.m_hWnd))
	{
				m_channel.PasteFromClipboard();
	}
	if (IsWindow(m_zet037_tenso_channel.m_hWnd))   m_zet037_tenso_channel.PasteFromClipboard();
}

void properties_dlg_common::OnDisable()
{
		if (!m_object->getAs7xxx())
			return;

		m_disable.EnableWindow(FALSE);
		m_object->getAs7xxx()->setEnabled(m_disableCommand == Enable);
		UpdateDisableState();
		SetTimer(3, 1000, NULL);
}

void properties_dlg_common::OnRefresh()
{
	
		// проверяем, если тип _7xxx нужно отправить запрос modbuszetlab на обновление 
		auto type = m_object->getType();
		if (type == zetlab::object_type::_7xxx_controller || type == zetlab::object_type::_7xxx_is)
		{
			if (m_isRunThreadUpdate)
				return;

			m_isRunThreadUpdate = true;
			boost::thread thr(&properties_dlg_common::requestForUpdateToModbus, this);
			thr.detach();
		}
		else
			Refresh();
}

void properties_dlg_common::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 3:
		// Включение Enable/Disable через некоторое время после нажатия
		UpdateDisableState();
		m_disable.EnableWindow(TRUE);
		KillTimer(3);
		break;
	}

	CAnchoredDialog::OnTimer(nIDEvent);
}

LRESULT properties_dlg_common::OnUserModify(WPARAM wParam, LPARAM lParam)
{
		if (m_object->getAs7xxx())
		{
			if (static_cast<int>(wParam) != 0)
			{
				int _Item(wParam);
				auto itt = std::find_if(m_7xxx.begin(), m_7xxx.end(), [_Item](boost::shared_ptr< C7xxxTab > data) {
					return data.get()->getItem() == _Item;
					});

				if (static_cast<BOOL>(lParam) == TRUE)
				{
					if (itt != m_7xxx.end())
						m_pCfgWorker->addNumberModifiedItemTab(static_cast<int>(wParam), itt->get()->getParam());
					else
						m_pCfgWorker->addNumberModifiedItemTab(static_cast<int>(wParam), std::vector<CompareParametres>());
				}
				else
					m_pCfgWorker->removeNumberModifiedItemTab(static_cast<int>(wParam));
			}

			m_save.EnableWindow(m_pCfgWorker->checkModifyDevice());
		}
		else
			m_save.EnableWindow(static_cast<BOOL>(lParam));
	return 0;

}

LRESULT properties_dlg_common::OnUserRegistrator(WPARAM wParam, LPARAM lParam)
{
	m_save.EnableWindow(!static_cast<BOOL>(wParam));
	m_cancel.EnableWindow(!static_cast<BOOL>(wParam));

	CMenu* pSysMenu(nullptr);
	pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		if (static_cast<BOOL>(wParam))
			pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
		else
			pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	}

	return 0;
}

void properties_dlg_common::UpdateDisableState()
{
		zetlab::_7xxx::device* _7xxx_(nullptr);
		_7xxx_ = m_object->getAs7xxx();

		if (!_7xxx_ || !_7xxx_->canEnableOrDisable())
			return;

		DisableCommand cmd = _7xxx_->isEnabled() ? Disable : Enable;

		if (m_disableCommand != cmd)
		{
			m_disableCommand = cmd;

			CString csLoc;
			csLoc = cmd == Enable ? g_sEnable : g_sDisable;
			m_disable.SetWindowText(csLoc);
		}
}

void properties_dlg_common::OnBnClickedSave7xxx()
{
		if (IsWindow(m_common.m_hWnd))			m_common.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_frequency.m_hWnd))		m_frequency.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_description.m_hWnd))		m_description.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_digport.m_hWnd))			m_digport.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_ipaddress.m_hWnd))		m_ipaddress.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_sync.m_hWnd))			m_sync.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_channel.m_hWnd))			m_channel.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_zet037_tenso_channel.m_hWnd)) m_zet037_tenso_channel.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_regZet048.m_hWnd))		m_regZet048.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_zsp_registrator.m_hWnd))	m_zsp_registrator.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_DAC.m_hWnd))	m_Internal_DAC.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_PPS.m_hWnd))	m_Internal_PPS.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_Internal_DC.m_hWnd))		m_Internal_DC.SendMessage(WM_USER_CANCEL);
		if (IsWindow(m_actuator.m_hWnd))		m_actuator.SendMessage(WM_USER_CANCEL);

		if (m_object->getAs7xxx())				m_object->getAs7xxx()->saveSettings();

		m_isSave = true;

		CAnchoredDialog::OnCancel();
}

void properties_dlg_common::requestForUpdateToModbus()
{
		// отправить запрос на обновление данных в modbuszetlab
		if (m_pFuncRequest.empty())		 // запрос из вспомогательного дерева
			return;

		m_isFinishThreadUpdate = false;

		uint64_t serial(0);
		long numVirtChan(0);
		bool isReconnect(true);
		BYTE repl(0);
		Interconnection interCon;
		int wait_seconds = 10;

		serial = m_object->getAs7xxx()->getDeviceSerial();

		interCon._requestType = RequestType::update_type;
		interCon._updateType = m_updateType;

		m_pCfgWorker->m_xmlWorker.getValueAttributeDevice(serial, ToString(nameAttributeDevice::line), &interCon._ftdindex);

		if (m_object->getAs7xxx()->getNodeId() == -1)
		{
			// мастер
			interCon._node = 1;
			wait_seconds = 5;
		}
		else
		{
			interCon._node = m_pCfgWorker->getNodeDevice();
			if (m_object->getAs7xxx()->getDeviceName().rfind(L"ZET 71", 0) == 0)
				wait_seconds = 10; // CAN - ждем поменьше
			else
				wait_seconds = 20; // не CAN - ждем подольше
		}

		m_refresh.EnableWindow(FALSE);
		m_save.EnableWindow(FALSE);

		for (int i = 0; i < wait_seconds; ++i)
		{
			if (!m_isRunThreadUpdate)
				break;

			if (isReconnect)
			{
				isReconnect = false;
				interCon._serial = serial;
			}
			else
				interCon._serial = 0;

			if (!m_pFuncRequest(interCon, &repl))
				isReconnect = true;

			if (repl)
			{
				m_isRefresh7xxx = true;
				Refresh7xxx();
				break;
			}

			Sleep(1000);
		}

		if (!repl)
		{
			if (::IsWindowVisible(m_common.m_hWnd) == TRUE)
				m_common.ShowMessageError7xxx();
		}

#ifdef UPDATE_FOR_ZETDEVICEMANAGER
		m_refresh.EnableWindow(TRUE);
#endif

		m_isRunThreadUpdate = false;
		m_isFinishThreadUpdate = true;
}

LRESULT properties_dlg_common::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	m_updateType = static_cast<UpdateType>(wParam);
	OnRefresh();
	return 0;
}

LRESULT properties_dlg_common::OnViewError(WPARAM wParam, LPARAM lParam)
{
	if (::IsWindowVisible(m_common.m_hWnd) == TRUE)
		m_common.ShowMessageError7xxx();

	return 0;
}

