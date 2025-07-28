#pragma once

#include "afxwin.h"
#include <ZetThread\ThreadSRVmultiSelfS.h>
#include <Grafic1\gdiplustextdispctrl1.h>
#include <memory>
#include <map>
#include "source/algo/std/channel/channel.hpp"
#include "resource/resource.h"
#include "resource/Phrases.h"

struct CHANNEL_STRUCT_READ_CALC_MEAN : public CHANNEL_STRUCT_READ
{
	uint16_t nPageMax;		// кол-во порций (от 1, 10 или 100)
	uint16_t nPageCur;		// номер текущей порции (от 0 до nPageMax-1)

	float mean;				// результат среднее

	float ACmean;

	float meanCur[100];
	int counterNotNaN[100];

	CHANNEL_STRUCT_READ_CALC_MEAN() : CHANNEL_STRUCT_READ()
		, nPageMax(10)
		, nPageCur(0)
		, mean(0.f)
		, ACmean(0.f)
	{
		ZeroMemory(meanCur, sizeof(meanCur));
		ZeroMemory(counterNotNaN, sizeof(counterNotNaN));
	}
};


class CSliderWheelHook : public CSliderCtrl
{
public:
	CSliderWheelHook();
	std::function<void(CSliderCtrl*)> WhenWheel;
protected:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
protected:
	DECLARE_MESSAGE_MAP()
};

class СTensoDlg : public CDialog
{
public:
	СTensoDlg(::std::vector<zetlab::std::channel*> _cur_channels_, CWnd* pParent = NULL);
	virtual ~СTensoDlg();

	DECLARE_DYNAMIC(СTensoDlg)
	enum { IDD = IDD_TENSO };
	
	void GetPotSlidersPos(int* pot1, int* pot2) const;
//	void setchans(std::vector<zetlab::std::channel*> cur_channels_);

	double m_ampl = 1.;
	double m_shift = 0.;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;

	LRESULT OnGetDataFromServer(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTensoChangeRequest();
	afx_msg void OnBalancaButtonPress();
	afx_msg void OnApplyButtonPress();
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void WhenSliderWheel(CSliderCtrl* cur_slider);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnCoefficientChangeRequest();
	afx_msg void OnYoungModChangeRequest();
	afx_msg void OnUnitSelect();

	DECLARE_MESSAGE_MAP()
protected:
	static const CString Pot2Resistense[256];
	static const float Pot2Resistensef[256];
	static const int POT_MAX1 = 255;
	static const int POT_MIN1 = 0;

	CDSRV* m_psrv;
	zetlab::std::channel* cur_channel;
	std::vector<zetlab::std::channel*> cur_channels;

	std::unique_ptr<CThreadSRVmultiSelfS> m_pThread;
	std::vector<std::unique_ptr<CHANNEL_STRUCT_READ_CALC_MEAN>> m_channel_data_vec;
	std::vector<CHANNEL_STRUCT_READ_CALC_MEAN*> m_channel_data_pointers;

	long m_tenso_mode;
	CString m_resistance;
	CString m_coefficient;
	CString m_youngMod;

	CComboBox m_unitCombo;
	
	CSliderWheelHook m_pot1_slider;
	CSliderWheelHook m_pot2_slider;

	CEdit m_tenzoResistanceEdit;
	CEdit m_pot1_edit;
	CEdit m_pot2_edit;
	CGdiplustextdispctrl1 m_mean_indicator;
	CGdiplustextdispctrl1 m_unit_indicator;

	CStatic m_pots_text;
	CComboBox  m_tenso_mode_combo;
	CButton m_button_balance;
	CButton m_apply_button;
	CStatic m_warningTurnOnGen;
	long m_lindexGen;
	CStatic m_AC;
	CStatic m_DC;

	long m_mode, m_pot1, m_pot2;
				
private:
	CString cur_edit2_str;
	CString om_str;
	void DisableAll();
	void EnableAll();
	void AddChannelByNumber(long channel);
	bool AutoBalanceStep(bool Init = false);
	bool isTurnOnGen() const;

	template<typename T>
	int GetNearestVal(const T& val, const T mass[], int mass_size) const;

	int measureStrainGauge();

public:
	afx_msg void OnTRBNThumbPosChangingSliderPot1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};