#pragma once
#include <Teds/teds_struct.h>

#include "../default_tab.h"

#include "../../../algo/std/channel/channel.hpp"
#include "../../../algo/std/device/device_std.hpp"
#include "../../../algo/sensors/sensors.hpp"
#include "../../../algo/utils/teds/teds.h"

#include "../../horscale/horscale.h"
#include "afxwin.h"
#include <source/algo/object/object.hpp>

class CZETDeviceManagerDlgl;  ///////////////для icp друж класс

class CChannelTab : public CTab
{
public:
	CChannelTab( CWnd* pParent = nullptr );
	virtual ~CChannelTab() = default;
	enum { IDD = IDD_CHANNEL_TAB };
	enum { IDS = IDS_IDD_CHANNEL_TAB };

public:
	void SetAdapter( zetlab::std::channel* channel__,  /// Настраивает интерфейс для работы с конкретным каналом
					 zetlab::std::device* device__ );
	void SetAdapter_s(std::vector<zetlab::object*> objects__,  // Настраивает интерфейс для работы с группой каналов
					  zetlab::std::device* device__,
					  bool m_group);
	void Activate() override{}
	void Disactivate() override{}
	static UINT readInfoTEDS(LPVOID pParam);

	void CopyToClipboard();  // Копирует/вставляет параметры канала через буфер обмена
	void PasteFromClipboard();
	BOOL IsClipboardAvailable();

	void SetVisible(_In_ int Show); //Управляет видимостью элементов интерфейса
	void SetVisibledemo(); //Настраивает интерфейс для демо-режима
	void HideControls(); //скрывает ненужные элементы управления
	void WriteNewDataOnOK();  // обработка ОК на групповом свойстве, если не меняли значения, обновляем в любом случае
	void OnRangeChangeRequestMoreChannel();  // для установки КУ на все каналы, если не меняли значение
	
protected:
	void DoDataExchange( CDataExchange* pDX ) override;
	BOOL OnInitDialog() override;

	afx_msg HBRUSH  OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg void    OnTimer( UINT_PTR nIDEvent );
	afx_msg void    OnEnableDisable();  //включение/отключение канала
	afx_msg void    OnInphDiff();  //Переключает дифференциальный режим входа 
	afx_msg void    OnNameChangeRequest();  //Обновляют имя и комментарий канала.
	afx_msg void    OnNameLocationChangeRequest();
	afx_msg void    OnNameChannelChangeRequest();
	afx_msg void    OnCommentChangeRequest(); //Обновляют имя и комментарий канала.
	afx_msg void    OnCoordXChangeRequest();
	afx_msg void    OnCoordYChangeRequest();
	afx_msg void    OnCoordZChangeRequest();
	afx_msg void    OnCoordPChangeRequest();  //Устанавливает ориентацию датчика
	afx_msg void	OnCoordPEdit();
	afx_msg void    OnUnitSenseChangeRequest();  //Меняет единицы измерения чувствительности 
	afx_msg void	OnUnitSenseEdit();
	afx_msg void    OnSensorSelect();  //Применяет настройки выбранного датчика из списка (m_nameCombo)
	afx_msg void    OnUnitChangeRequest(); // Обрабатывает изменение единиц измерения канала (
	afx_msg void    OnUnitSelect(); //Обрабатывает выбор единиц измерения из выпадающего списка с пересчетом значений
	afx_msg void    OnSenseChangeRequest();  // Обрабатывает изменение чувствительности канала
	afx_msg void    OnReferenceChangeRequest(); //Обрабатывает изменение опорного уровня
	afx_msg void    OnShiftChangeRequest();  //Обрабатывает изменение смещения нуля
	afx_msg void    OnAmplifyChangeRequest(); //брабатывает изменение коэффициента усиления
	afx_msg void    OnRangeChangeRequest( NMHDR* pNMHDR, LRESULT* pResult ); //Обрабатывает изменение диапазона измерений
	afx_msg void    OnIcpOnOff(); //Управляют режимами ICP
	afx_msg void    OnTensoIcpOnOff(); //- Управляет тензометрическим мостом
	afx_msg void	OnTensoIcpSettingsClick();
	afx_msg void    OnChargeOnOff(); // Управляет зарядным усилителем
	afx_msg void    OnTedsClick();
	afx_msg void	OnTensoClick();  //Открывает диалог настройки тензометрического датчика.
	afx_msg void    OnCancel() override;  //Отменяет изменения (возвращает предыдущие значения)
	afx_msg void    OnUserCancel();   //при Отмена, сохраняет предыдущее состояние
	afx_msg LRESULT	OnStateTeds(WPARAM wParam, LPARAM lParam);
	afx_msg void    OnHpfOnOff();  // Включает/выключает 
	afx_msg void    OnInputResistanceSelect();
	afx_msg void	OnSelftestOnOff();
	afx_msg void	OnNoiseOnOff();
	DECLARE_MESSAGE_MAP()

private:
	void InitSensors();  //Загружает конфигурацию датчиков из файла sensors.cfg.
	void InitUnits();  //Заполняет ComboBox-ы (m_unitCombo, m_UnitSenseCombo, m_coordPCombo) списком единиц

protected:
	CString   m_nameDevice;
	CString   m_postfix;
	CString   m_name;
	CString   m_name_location;
	CString   m_name_channel;
	CString   m_comment;
	CString   m_CoordinateX;
	CString   m_CoordinateY;
	CString   m_CoordinateZ;
	CString   m_CoordinateP;
	CComboBox m_nameCombo;
	CString   m_unit;
	CString   m_UnitSense;
	CComboBox m_unitCombo;
	CComboBox m_coordPCombo;
	CComboBox m_UnitSenseCombo;
	int       m_state;
	CButton   m_stateCheck;
	int       m_diff;
	CButton m_tenso_button;
	CButton   m_diffCheck;
	CString   m_sense;
	CString   m_reference;
	CString   m_shift;
	CString   m_amplify;
	CHorScale m_level;
	CString   m_range;
	CStatic   m_rangeStatic;
	CSpinButtonCtrl m_spin;
	int       m_icp;
	CButton   m_icpCheck;
	int       m_tensoIcp;
	CButton   m_tensoIcpCheck;
	CButton   m_tensoIcpSettingsButton;
	int       m_charge;
	CButton   m_chargeCheck;
	int       m_hpf;
	CButton   m_hpfCheck;
	CComboBox m_inputResistanceCombo;
	int       m_selftest;
	CButton   m_selftestCheck;
	int       m_noise;
	CButton   m_noiseCheck;
	CProgressCtrl m_progress;
	CWinThread * m_pThread = nullptr;
	bool m_groups;

private:
	void update_range();  //Обновляет текст, отображающий диапазон измерений канала 

	zetlab::std::channel*            channel_;
	zetlab::std::channel_memento_ptr memento_;
	zetlab::sensors                  sensors_;
	TEDS			                 teds_;
	std::vector<zetlab::object*> objects_;
	std::vector<zetlab::std::channel*> channels_;
	std::vector<zetlab::std::channel_memento_ptr> mementos;

	bool m_b_metadata = false;

	
};
