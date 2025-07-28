#include "stdafx.h"
#include "tenso_dlg.h"   

#include <boost/lexical_cast.hpp>

IMPLEMENT_DYNAMIC(СTensoDlg, CDialog)

static const zetlab::tstring g_tensoUnit[] = 
{
	g_sUNIT_MKMM.GetString(),
	g_sUNIT_PA.GetString(),
	g_sUNIT_KPA.GetString(),
	g_sUNIT_MPA.GetString(),
	g_sUNIT_KGSMM2.GetString() 
};
static const float g_tensoRef[] = { 1.f, 0.00002f, 0.00000002f, 0.00000000002f, 0.000000000002f };

#define VISIBLE_SET(m_tenso_mode) \
do \
{ \
	if (m_tenso_mode == 1 || m_tenso_mode == 0) \
	{ \
		m_pot1_slider.EnableWindow(TRUE); \
		m_pot1_edit.EnableWindow(TRUE); \
		m_pot2_slider.EnableWindow(FALSE); \
		m_pot2_edit.EnableWindow(FALSE); \
		m_apply_button.EnableWindow(FALSE); \
		m_tenzoResistanceEdit.EnableWindow(FALSE); \
	} \
		else if (m_tenso_mode == 2) \
	{ \
		m_pot1_slider.EnableWindow(TRUE); \
		m_pot1_edit.EnableWindow(TRUE); \
		m_pot2_slider.EnableWindow(TRUE); \
		m_pot2_edit.EnableWindow(TRUE); \
		m_apply_button.EnableWindow(TRUE); \
		m_tenzoResistanceEdit.EnableWindow(TRUE); \
	} \
} while (0) \

long CalculateMean(void* pParam)
{
	const float mySqrt = (float)sqrt(2);
	CodeQuality GlobalQuality(0L);
	GlobalQuality.AllData = Quality_AllData_True;
	ZetThreadParam2* ppar = (ZetThreadParam2*)pParam;
	std::vector<CHANNEL_STRUCT_READ_CALC_MEAN*>* pList = (std::vector<CHANNEL_STRUCT_READ_CALC_MEAN*>*) ppar->pChannels;
	CHANNEL_STRUCT_READ_CALC_MEAN* pm = *(pList->begin() + ppar->index);

//РАСЧЕТ MEAN ДЛЯ 7000
	if (pm->GetFlag(Flag_Chan_7000))
	{
		if (pm->retGetData == 0)
		{
			if (ippsMean_32f(pm->saArray.pData, (int)pm->saArray.size, &pm->mean, ippAlgHintNone)
				!= ippStsNoErr)
			{
				pm->mean = NAN;
			}
		}
		else if (pm->retGetData == 1)
		{
			int n(0);
			pm->mean = 0.f;
			for (UINT i = 0; i < pm->saArray.size; ++i)
			{
				float x = pm->saArray.pData[i];
				if (_isnan(x) == 0)
				{
					pm->mean += x;
					++n;
				}
			}
			if (n > 0)
			{
				pm->mean /= n;
			}
			else
			{
				pm->mean = std::numeric_limits<float>::quiet_NaN();
			}
		}
		else
		{
			pm->mean = std::numeric_limits<float>::quiet_NaN();
		}
		pm->nPageCur = 0;
	}
//РАСЧЕТ MEAN ДЛЯ 7000
	else
	{
		CodeQuality qual(0L);
		if (pm->retGetData != 0)
			qual.Error = 1;
		if (qual.Error == 0)
		{
			if (ippsMean_32f(pm->saArray.pData, (int)pm->saArray.size, pm->meanCur + pm->nPageCur, ippAlgHintNone) == ippStsNoErr)
				pm->counterNotNaN[pm->nPageCur] = 1;
			else
				pm->counterNotNaN[pm->nPageCur] = 0;

			float counter(0);
			ippsMean_32f(pm->saArray.pData, (int)pm->saArray.size, &counter, ippAlgHintFast);
			ippsSubC_32f_I(counter, pm->saArray.pData, (int)pm->saArray.size);
			ippsSqr_32f_I(pm->saArray.pData, (int)pm->saArray.size);
			ippsSum_32f(pm->saArray.pData, (int)pm->saArray.size, &pm->ACmean, ippAlgHintFast);
			pm->ACmean = sqrt(pm->ACmean / pm->saArray.size) * mySqrt;
		}
		else
			pm->counterNotNaN[pm->nPageCur] = 0;

		++pm->nPageCur;
		// усреднение 1.0 сек
		if (pm->nPageCur >= pm->nPageMax)
		{
			pm->mean = 0.f;
			int m(0);
			for (int i = 0; i < pm->nPageMax; ++i)
			{
				int n = pm->counterNotNaN[i];
				if (n > 0)
				{
					pm->mean += pm->meanCur[i];
					m += n;
				}
			}
			if (m > 0)
				pm->mean /= m;
			else
				pm->mean = std::numeric_limits<float>::quiet_NaN();

			pm->nPageCur = 0;
		}
	}

	return GlobalQuality.AllData;
}

СTensoDlg::СTensoDlg(::std::vector<zetlab::std::channel*> _cur_channels_, CWnd* pParent)
	: CDialog(СTensoDlg::IDD, pParent)
	, m_lindexGen(-1)
	, m_pot1(0)
	, m_pot2(0)
	, m_mode(0)
{
	if (_cur_channels_.size() > 1)
	{
		for (auto& element : _cur_channels_)
		{
			cur_channels.push_back(element);

		}
	}

	cur_channel = _cur_channels_[0];
	
}

СTensoDlg::~СTensoDlg()
{
	m_pThread->EndAndWait();
	cur_channels.clear();
}

void СTensoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_POT1, m_pot1_slider);
	DDX_Control(pDX, IDC_SLIDER_POT2, m_pot2_slider);
	DDX_Control(pDX, IDC_GDIPLUSTEXTDISPCTRL1, m_mean_indicator);
	DDX_Control(pDX, IDC_GDIPLUSTEXTDISPCTRL2, m_unit_indicator);
	DDX_Control(pDX, IDC_POT1_EDIT, m_pot1_edit);
	DDX_Control(pDX, IDC_POT2_EDIT, m_pot2_edit);
	DDX_Control(pDX, IDC_BUTTON_BALANCE, m_button_balance);
	DDX_Control(pDX, IDC_APPLY_BUTTON2, m_apply_button);
	DDX_Control(pDX, IDC_STATIC_TURNON_GENERATOR, m_warningTurnOnGen);
	DDX_Control(pDX, IDC_STATIC_AC_DATA, m_AC);
	DDX_Control(pDX, IDC_STATIC_DC_DATA, m_DC);
	DDX_Control(pDX, IDC_TENZORESISTOR_EDIT, m_tenzoResistanceEdit);

	int promm = 0;
	DDX_CBIndex(pDX, IDC_TENSO, promm);
	if (promm == 0)
		m_tenso_mode = 0;
	if (promm == 1)
		m_tenso_mode = 1;
	if (promm == 2)
		m_tenso_mode = 2;

	DDX_Control(pDX, IDC_TENSO, m_tenso_mode_combo);
	DDX_Text(pDX, IDC_COEFFICIENT, m_coefficient);
	DDX_Text(pDX, IDC_YOUNG_MOD, m_youngMod);
	DDX_Control(pDX, IDC_TENSO_UNIT, m_unitCombo);
}

BEGIN_MESSAGE_MAP(СTensoDlg, CDialog)
	ON_WM_TIMER()
	//ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_TENSO, OnTensoChangeRequest)
	ON_MESSAGE(WM_NEW_DATA_SRV, OnGetDataFromServer)
	ON_BN_CLICKED(IDC_BUTTON_BALANCE, OnBalancaButtonPress)
	ON_BN_CLICKED(IDC_APPLY_BUTTON2, OnApplyButtonPress)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_POT1, &СTensoDlg::OnTRBNThumbPosChangingSliderPot1)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_POT2, &СTensoDlg::OnTRBNThumbPosChangingSliderPot1)
	ON_EN_CHANGE(IDC_POT2_EDIT, OnEnChangeEdit1)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_COEFFICIENT, OnCoefficientChangeRequest)
	ON_EN_CHANGE(IDC_YOUNG_MOD, OnYoungModChangeRequest)
	ON_CBN_SELCHANGE(IDC_TENSO_UNIT, OnUnitSelect)
END_MESSAGE_MAP()

BOOL СTensoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pot1_slider.SetRange(POT_MIN1, POT_MAX1);
	m_pot2_slider.SetRange(POT_MIN1, POT_MAX1);
	m_pot1_slider.SetPos(POT_MIN1);
	m_pot2_slider.SetPos(POT_MIN1);

	m_pot1_slider.SetTic(POT_MIN1);
	m_pot1_slider.SetTic((POT_MAX1 + POT_MIN1) / 2);
	m_pot1_slider.SetTic(POT_MAX1);

	m_pot2_slider.SetTic(POT_MIN1);
	m_pot2_slider.SetTic((POT_MAX1 + POT_MIN1) / 2);
	m_pot2_slider.SetTic(POT_MAX1);
	//auto gg = std::bind(&СTensoDlg::WhenSliderWheel, this, std::placeholders::_2);
	m_pot1_slider.WhenWheel = std::bind(&СTensoDlg::WhenSliderWheel, this, std::placeholders::_1);
	m_pot2_slider.WhenWheel = std::bind(&СTensoDlg::WhenSliderWheel, this, std::placeholders::_1);

	SetWindowText(g_sTensoWindow + L": " + cur_channel->getName().c_str());
	GetDlgItem(IDC_LABEL_TENSO)->SetWindowTextW(g_sTenso);
	GetDlgItem(IDC_POT1_STATIC)->SetWindowTextW(g_Balance);
	GetDlgItem(IDC_POT2_STATIC)->SetWindowTextW(g_sReferenceSensorResistance);
	GetDlgItem(IDC_LABEL_TENZO_RESISTANCE)->SetWindowTextW(g_sTenzoResistance);
	GetDlgItem(IDC_LABEL_COEFFICIENT)->SetWindowText(g_sTensoCoeff + L":");
	GetDlgItem(IDC_LABEL_TENSO_UNIT)->SetWindowText(g_sTensoUnit + L":");
	GetDlgItem(IDC_LABEL_YOUNG_MOD)->SetWindowText(g_sYoungMod + L", " + g_sUNIT_GPA + L":");
	GetDlgItem(IDOK)->SetWindowTextW(g_sApply);
	GetDlgItem(IDCANCEL)->SetWindowTextW(g_sCancel);

	m_button_balance.SetWindowTextW(g_sAutoBalance);
	m_apply_button.SetWindowTextW(g_sApply);

	GetDlgItem(IDC_STATIC_POT21)->SetWindowTextW(Pot2Resistense[0]);
	GetDlgItem(IDC_STATIC_POT22)->SetWindowTextW(Pot2Resistense[255]);

	m_pot1_edit.SetReadOnly();
	m_pot2_slider.EnableWindow(FALSE);

	m_resistance.Format(_T("%g"), cur_channel->getTensoResistance(true));
	m_coefficient.Format(_T("%g"), cur_channel->getTensoCoeff(true));
	m_youngMod.Format(_T("%g"), cur_channel->getTensoYoungMod(true));

	m_tenzoResistanceEdit.SetWindowText(L"");

	auto sUnit = cur_channel->getUnit();
	int i(0), i2(-1);
	for (const auto& s : g_tensoUnit)
	{
		m_unitCombo.AddString(s.c_str());
		if (s == sUnit)
			i2 = i;
		++i;
	}
	if (i2 < 0)
	{
		i2 = 0;
		cur_channel->setUnit(g_tensoUnit[0]);
	}
	m_unitCombo.SetCurSel(i2);

	GetDlgItem(IDC_YOUNG_MOD)->EnableWindow(i2 == 0 ? FALSE : TRUE);

	UpdateData(FALSE);

	m_tenso_mode_combo.ResetContent();
	CString Tenso_modes[3] =
	{
		CString(g_sBridge),
		CString(g_sHalfBridge),
		CString(g_sQuarterBridge),
	};

	for (const auto& ts : Tenso_modes)
		m_tenso_mode_combo.AddString((LPCTSTR)ts);

	m_ampl = cur_channel->Amplify(true);

	long pot1 = -1, pot2 = -1;
	om_str = L" " + g_sUNIT_OM;
	bool isbridgemode = cur_channel->isHaveTenso(m_tenso_mode, &pot1, &pot2);
	if (isbridgemode)
	{
		m_tenso_mode_combo.SetCurSel(m_tenso_mode);
		
		if (pot1 != -1)
		{
			m_pot1_slider.SetPos(pot1);
			m_pot1_edit.SetWindowTextW(std::to_wstring(pot1 - 127).c_str());
		}
		if (pot2 != -1)
		{
			m_pot2_slider.SetPos(pot2);
			m_pot2_edit.SetWindowTextW(Pot2Resistense[pot2] + om_str);
			cur_edit2_str = Pot2Resistense[pot2];

			if (2 == m_tenso_mode)
			{
				if (m_ampl >= 1. || m_ampl <= 0.)
					m_tenzoResistanceEdit.SetWindowText(Pot2Resistense[pot2] + om_str);
				else
				{
					auto r = (double)Pot2Resistensef[pot2];
					if (pot2 < 127)
						r *= (2. - m_ampl + 2. * sqrt(1. - m_ampl)) / m_ampl;
					else
						r *= (2. - m_ampl - 2. * sqrt(1. - m_ampl)) / m_ampl;
					CString s;
					s.Format(L"%.2f " + om_str, r);
					m_tenzoResistanceEdit.SetWindowTextW(s);
				}
			}
			else
				m_tenzoResistanceEdit.SetWindowText(Pot2Resistense[pot2] + om_str);
		}
		OnTensoChangeRequest();
	}

	//Настройка потока данных
	m_psrv = cur_channel->getServer();
	m_pThread = std::make_unique<CThreadSRVmultiSelfS>();
	m_pThread->Create(L"", m_hWnd, m_psrv, (std::vector<CHANNEL_STRUCT_READ*>*)&m_channel_data_pointers, &CalculateMean);
	AddChannelByNumber(cur_channel->getServerID());
	m_unit_indicator.SetTextString(cur_channel->getUnit().c_str());

	m_lindexGen = cur_channel->GetGenIndex();
	if (m_lindexGen != -1)
		AddChannelByNumber(m_lindexGen);
	
	m_pThread->Start();
	m_pThread->SetTime(0);

	if (isTurnOnGen())
		m_warningTurnOnGen.ShowWindow(SW_HIDE);
	else
		m_warningTurnOnGen.SetWindowTextW(g_sTurnOnGeneratorTenso);

	GetDlgItem(IDC_LABEL_PROP_GEN)->SetWindowTextW(g_sPropGenerator);

	GetDlgItem(IDC_STATIC_AC)->SetWindowTextW(g_sACSost);
	GetDlgItem(IDC_STATIC_DC)->SetWindowTextW(g_sDCSost);
	return TRUE;
}

void СTensoDlg::OnTensoChangeRequest()
{
	if (nullptr == cur_channel)
		return;

	UpdateData();

	if (cur_channels.size() > 1)
	{
		for (auto& element : cur_channels)
		{
			element->TensoChangeFlag = true;
			element->setTensoMode(m_tenso_mode);
		}
		VISIBLE_SET(m_tenso_mode);
		cur_channel->setTenso(true, m_tenso_mode);
		
	}
	else
	{
		this->cur_channel->TensoChangeFlag = true;
		VISIBLE_SET(m_tenso_mode);
		this->cur_channel->setTenso(false, m_tenso_mode);
	}
}

void СTensoDlg::OnBalancaButtonPress()
{
	DisableAll();
	if (2 == m_tenso_mode)
	{

		if (cur_channels.size() > 1)
		{
			for (auto& element : cur_channels)
			{
				element->isHaveTenso(m_mode, &m_pot1, &m_pot2);
			}
			cur_channel->setTenso(true, m_mode, 127, m_pot2);
			m_pot1_slider.SetPos(127);
			m_pot1_edit.SetWindowTextW(L"0");

			SetTimer(3, 3000, nullptr);
			
		}
		else
		{
			cur_channel->isHaveTenso(m_mode, &m_pot1, &m_pot2);
			cur_channel->setTenso(false, m_mode, 127, m_pot2);
			m_pot1_slider.SetPos(127);
			m_pot1_edit.SetWindowTextW(L"0");

			SetTimer(3, 3000, nullptr);
		}

	}
	else
	{
		AutoBalanceStep(true);
		SetTimer(2, 3000, nullptr);
	}
}

void СTensoDlg::OnApplyButtonPress()
{
	CString ss;
	CString new_val;
	m_pot2_edit.GetWindowText(ss);
	
	if (ss.Find(om_str) != -1)
		ss = ss.Left(ss.Find(om_str));

	float val = (float)_ttof(ss);

	int pos = GetNearestVal(val, Pot2Resistensef, sizeof(Pot2Resistensef) / sizeof(float));
	cur_edit2_str = Pot2Resistense[pos];
	float valllll = Pot2Resistensef[pos];

	new_val.Format(L"%3.2f", valllll);
	m_pot2_edit.SetWindowTextW(new_val + om_str);

	m_pot2_slider.SetPos(pos);
	SetTimer(1, 100, NULL);
}

void СTensoDlg::GetPotSlidersPos(int* pot1, int* pot2) const
{
	*pot1 = m_pot1_slider.GetPos();
	*pot2 = m_pot2_slider.GetPos();
}

void СTensoDlg::OnOK()
{
	auto val = m_mean_indicator.GetTextString();

	int pot1 = -1, pot2 = -1;
	if (cur_channels.size() > 1)
	{

		GetPotSlidersPos(&pot1, &pot2);
		cur_channel->setTenso(true, -1, pot1, pot2);
		for (auto& element : cur_channels)
		{
			element->saveTenso();
		}
		
	}
	else
	{
		GetPotSlidersPos(&pot1, &pot2);
		cur_channel->setTenso(false, -1, pot1, pot2);
		cur_channel->saveTenso();
	}

	CString s;
	m_tenzoResistanceEdit.GetWindowText(s);
	if (s.Find(om_str) != -1)
		s = s.Left(s.Find(om_str));
	auto r1 = _wtof(s);
	auto r2 = Pot2Resistensef[pot2];

	m_ampl = 4. * r1 * r2 / (r1 + r2) / (r1 + r2);
	if (m_ampl >= 0.99)
		m_ampl = 1.;

	m_shift = 0.;
	if (auto res = MessageBox(g_sSetZeroShift, g_sTensoWarning, MB_YESNO | MB_ICONINFORMATION); IDYES == res)
		m_shift = _wtof(val.GetString());
	if (0 == _finite(m_shift))
		m_shift = 0.;
	
	CDialog::OnOK();
}

void СTensoDlg::OnCancel()
{
	CDialog::OnCancel();
}

void СTensoDlg::DisableAll()
{
	m_pot1_edit.EnableWindow(FALSE);
	m_pot2_edit.EnableWindow(FALSE);
	m_tenzoResistanceEdit.EnableWindow(FALSE);
	m_pot1_slider.EnableWindow(FALSE);
	m_pot2_slider.EnableWindow(FALSE);
	m_tenso_mode_combo.EnableWindow(FALSE);
	m_apply_button.EnableWindow(FALSE);
	m_button_balance.EnableWindow(FALSE);
}

void СTensoDlg::EnableAll()
{
	VISIBLE_SET(m_tenso_mode);
	m_button_balance.EnableWindow(TRUE);
	m_tenso_mode_combo.EnableWindow(TRUE);
}

void СTensoDlg::AddChannelByNumber(long channel)
{
	m_channel_data_vec.push_back(std::make_unique<CHANNEL_STRUCT_READ_CALC_MEAN>());
	auto it = m_channel_data_vec.rbegin();
	m_channel_data_pointers.push_back(it->get());

	CHANNEL_STRUCT_READ_CALC_MEAN* m_pCurParam = it->get();

	m_pCurParam->decade = 0;
	m_pCurParam->numKadrMax = m_psrv->DecadeBufferSize(0) / 2;
	m_pCurParam->dataTime.SetMode10sec(m10s_every_1sec);

	if ((0 <= channel) && (channel < m_psrv->QuanChan()))
	{
		m_pCurParam->channel = channel;
		m_psrv->IdChan(channel, &m_pCurParam->id);
		// заполняю параметры в CHANNEL_STRUCT
		m_psrv->GetChannelPar(channel, (BYTE*)m_pCurParam, SIZE_CHANNEL_STRUCT);

		
		// вначале надо проверить на 7000
		m_pCurParam->SetFlag(Flag_Chan_7000,
			m_pCurParam->frequency <= m_pThread->GetMinFreqADC());

		if (m_pCurParam->bFlags.GetFlag(Flag_Chan_7000))
		{
			m_pCurParam->dataTime.SetCalcNumberMode(true);
			m_pCurParam->dataTime.SetAverageCodeMode(1, m_pCurParam->frequency);
		}
		else
		{
			m_pCurParam->dataTime.SetCalcNumberMode(false);
			m_pCurParam->dataTime.SetAverageCodeMode(0);	// читаю всегда порциями по 0,1 сек
		}
	}
	else
	{
		m_pCurParam->ResetFlag(Flag_Chan_Exist);
		m_pCurParam->channel = -1;
		m_pCurParam->name[0] = 0;
		m_pCurParam->measure[0] = 0;
	}

	m_pCurParam->bFlags.PlaceFlag(Flag_Chan_OnOff);
	m_pCurParam->bFlags.PlaceFlag(Flag_Chan_Exist);
	m_pCurParam->bFlags.PlaceFlag(Flag_Change_Status);
	m_pCurParam->bFlags.PlaceFlag(Flag_Change_Freq);
	m_pCurParam->bFlags.PlaceFlag(Flag_Change_Level);
	m_pCurParam->bFlags.PlaceFlag(Flag_Change_Level_DC);
}

bool СTensoDlg::AutoBalanceStep(bool Init)
{
	static int start_ind;
	static int end_ind;
	static long cur_ind, pot2;
	static float f_start;
	static float f_end;
	static int stage = 0;
	static bool f_start_larger_f_end;

	CString cur_mean;
	CString ErrMsg;
	float cur_meanf(0);

	auto SetSliderandText = [this](int cur_index)
	{
		m_pot1_slider.SetPos(cur_index);
		m_pot1_edit.SetWindowTextW(std::to_wstring(cur_index - 127).c_str());
	};

	if (Init)
		stage = 0;

	if (stage == 1 || stage == 2 || stage == 3)
	{
		cur_mean = m_mean_indicator.GetTextString();
		cur_meanf = static_cast<float>(wcstod(cur_mean.GetString(), NULL));
		if (cur_channels.size() > 1)
		{
			for (auto& element : cur_channels)
			{
				cur_meanf += (float)element->Shift(true);
			}
		}
		else
		{
			cur_meanf += (float)cur_channel->Shift(true);
		}
	}
	switch (stage)
	{
	case 0: //Init, f_start set
		long m_tenso_mode_tmp;
		start_ind = 0;
		end_ind = 255;
		f_start = 0;
		f_end = 0;
		if (cur_channels.size() > 1)
		{
			for (auto& element : cur_channels)
			{
				element->isHaveTenso(m_tenso_mode_tmp, &cur_ind, &pot2);
			}
			cur_channel->setTenso(true,-1, start_ind, pot2);
		}
		else
		{
			cur_channel->isHaveTenso(m_tenso_mode_tmp, &cur_ind, &pot2);
			cur_channel->setTenso(false, -1, start_ind, pot2);
		}

		SetSliderandText(start_ind);

		stage++;
		break;
	case 1: //f_end set, f_start init
		f_start = cur_meanf;
		if (cur_channels.size() > 1)
		{

			cur_channel->setTenso(true, -1, end_ind, pot2);

		}
		else
		{
			cur_channel->setTenso(false, -1, end_ind, pot2);
		}
		SetSliderandText(end_ind);

		stage++;
		break;
	case 2: //f_end init, first iter
		f_end = cur_meanf;

		if (f_start > f_end)
			f_start_larger_f_end = true;
		else
			f_start_larger_f_end = false;

		if ((f_start < 0 && f_end < 0) || (f_start > 0 && f_end > 0))
		{
			ErrMsg = L"";
			if (m_tenso_mode == 2)
			{
				if (!f_start_larger_f_end)
				{
					if (f_start < 0)
					{
						if (pot2 != 0)
							ErrMsg = g_sTensoWarningMsg1;
					}
					else if (f_start > 0)
					{
						if (pot2 != 255)
							ErrMsg = g_sTensoWarningMsg2;
					}
				}
				else
				{
					if (f_start < 0)
					{
						if (pot2 != 255)
							ErrMsg = g_sTensoWarningMsg2;
					}
					else if (f_start > 0)
					{
						if (pot2 != 0)
							ErrMsg = g_sTensoWarningMsg1;
					}
				}

				if (f_start < 0)
				{
					if (cur_channels.size() > 1)
					{
						cur_channel->setTenso(true, -1, end_ind, pot2);
						
					}
					else
					{
						cur_channel->setTenso(false, -1, end_ind, pot2);
					}
					SetSliderandText(end_ind);
				}
				else
				{
					if (cur_channels.size() > 1)
					{

						cur_channel->setTenso(true,-1, start_ind, pot2);
					
					}
					else
					{
						cur_channel->setTenso(false, -1, start_ind, pot2);
					}
					SetSliderandText(start_ind);
				}
			}

			goto ErrExit;
		}

		cur_ind = (long)roundf(((0 - f_start) * (end_ind - start_ind)) / (f_end - f_start) + start_ind);

		if (cur_ind >= start_ind && cur_ind <= end_ind)
		{
			if (cur_channels.size() > 1)
			{

				cur_channel->setTenso(true, -1, cur_ind, pot2);
				
			}
			else
			{
				cur_channel->setTenso(false,-1, cur_ind, pot2);
			}
		}
			
		else
		{
			ErrMsg = L"";//L"Неправильно вычислился индекс.";
			goto ErrExit;
		}
		SetSliderandText(cur_ind);
		stage++;
		break;
	case 3:
		if (f_start_larger_f_end)
		{
			if (cur_meanf < 0)
			{
				f_end = cur_meanf;
				end_ind = cur_ind;
			}
			else
			{
				f_start = cur_meanf;
				start_ind = cur_ind;
			}
		}
		else
		{
			if (cur_meanf > 0)
			{
				f_end = cur_meanf;
				end_ind = cur_ind;
			}
			else
			{
				f_start = cur_meanf;
				start_ind = cur_ind;
			}
		}

		long old_ind = cur_ind;
		cur_ind = (long)roundf(((0 - f_start) * (end_ind - start_ind)) / (f_end - f_start) + start_ind);

		if (cur_ind >= start_ind && cur_ind <= end_ind)
		{
			if (cur_channels.size() > 1)
			{

				cur_channel->setTenso(true, -1, cur_ind, pot2);

			}
			else
			{
				cur_channel->setTenso(false, -1, cur_ind, pot2);
			}
		}
	
		else
		{
			ErrMsg = L"";//L"Неправильно вычислился индекс.";
			goto ErrExit;
		}
		SetSliderandText(cur_ind);

		if (f_start_larger_f_end)
		{
			if (((end_ind - start_ind) >= 1) && f_start > 0 && f_end < 0 && old_ind != cur_ind)
				return false;
		}
		else
			if (((end_ind - start_ind) >= 1) && f_start < 0 && f_end > 0 && old_ind != cur_ind)
				return false;

		stage = 0;
		return true;
	}
	return false;
		
	
	ErrExit:
		stage = 0;
		KillTimer(2);
		if (!ErrMsg.IsEmpty())
			MessageBox(ErrMsg, g_sTensoWarning, MB_ICONWARNING);
		return true;
	
}

LRESULT СTensoDlg::OnGetDataFromServer(WPARAM wParam, LPARAM lParam)
{
	if (m_pThread == NULL)
		return -1;

	DWORD index = (DWORD)(int)wParam;	// индекс канала, по которому получены данные
	long quality = (long)lParam;

	CHANNEL_STRUCT_READ_CALC_MEAN *pm = *(m_channel_data_pointers.begin() + index);
	float mean = pm->mean;
	CString mean_str;
	mean_str.Format(L"%f", mean);
	m_pThread->SetSynchro();

	if (m_lindexGen != pm->channel)
		m_mean_indicator.SetTextString(mean_str);
	else
	{
		CString mean_strAC(L"");
		mean_strAC.Format(L"%.2f", pm->ACmean);
		mean_strAC = mean_strAC + L" " + pm->measure;
		m_AC.SetWindowTextW(mean_strAC);

		CString mean_strDC(L"");
		mean_strDC.Format(L"%.2f ", mean);
		mean_strDC += pm->measure;
		m_DC.SetWindowTextW(mean_strDC);
	}

	return 0;
}

void СTensoDlg::WhenSliderWheel(CSliderCtrl* cur_slider)
{
	int pot1 = -1, pot2 = -1;
	GetPotSlidersPos(&pot1, &pot2);
	
	if ((pot1 >= POT_MIN1 && pot1 <= POT_MAX1)
	 && (pot2 >= POT_MIN1 && pot2 <= POT_MAX1))
	{
		if (m_pot1_slider == *cur_slider)
			m_pot1_edit.SetWindowTextW(std::to_wstring(pot1 - 127).c_str());

		else if (m_pot2_slider == *cur_slider)
			m_pot2_edit.SetWindowTextW(Pot2Resistense[pot2] + om_str);

		SetTimer(1, 100, NULL);
	}
}

void СTensoDlg::OnTimer(UINT_PTR nIDEvent)
{


	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);
	
		int pot1, pot2;
		GetPotSlidersPos(&pot1, &pot2);
		cur_edit2_str = Pot2Resistense[pot2];
		if (cur_channels.size() > 1)
		{

			cur_channel->setTenso(true, -1, pot1, pot2);
		}
		else
		{
			cur_channel->setTenso(false, -1, pot1, pot2);
		}

		break;
	case 2:
		if (AutoBalanceStep())
		{
			KillTimer(2);
			EnableAll();
		}
		break;
	case 3:
		KillTimer(3);
		auto result = measureStrainGauge();
		if (cur_channels.size() > 1)
		{

			cur_channel->setTenso(true, m_mode, m_pot1, m_pot2);

		}
		else
		{
			cur_channel->setTenso(false, m_mode, m_pot1, m_pot2);
		}

		if (0 == result)
		{
			AutoBalanceStep(true);
			SetTimer(2, 3000, nullptr);
		}
		else
		{
			EnableAll();
			CString sText;
			if (-1 == result)
				sText = g_sTensoWarningMsg3;
			else if (-2 == result)
				sText = g_sTensoWarningMsg4;
			else if (-3 == result)
				sText = g_sTensoWarningMsg5;
			else if (-4 == result)
				sText = g_sTensoWarningMsg6;
			else if (-5 == result)
				sText = g_sTensoWarningMsg7;
			else if (-6 == result)
				sText = g_sTensoWarningMsg8;
			if (!sText.IsEmpty())
				MessageBox(sText, g_sTensoWarning, MB_ICONWARNING);
		}
	
		break;
	}

}

void СTensoDlg::OnEnChangeEdit1()
{
	CString edit2_text;
	int point_c = 0;
	bool IncorrectFieldVal = false;
	
	m_pot2_edit.GetWindowText(edit2_text);

	if (edit2_text.Find(om_str) != -1)
		edit2_text = edit2_text.Left(edit2_text.Find(om_str));

	for (const wchar_t& s : std::wstring(edit2_text.GetString()))
	{
		if ((s < '0' || s > '9') && s != '.')
		{
			IncorrectFieldVal = true;
			break;
		}
		if (s == '.')
		{
			point_c++;
			if (point_c > 1)
			{
				IncorrectFieldVal = true;
				break;
			}
		}
	}
	if (IncorrectFieldVal)
	{
		m_pot2_edit.SetWindowTextW(cur_edit2_str);
	}
	else
	{
		cur_edit2_str = edit2_text;
	}
}

void СTensoDlg::OnTRBNThumbPosChangingSliderPot1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	int cur_pot = pNMTPC->dwPos;
	if ((cur_pot >= POT_MIN1 && cur_pot <= POT_MAX1)) //|| (cur_pot >= POT_MIN1 && cur_pot <= POT_MAX1))
	{
		if (m_pot1_slider.m_hWnd == pNMTPC->hdr.hwndFrom)
			m_pot1_edit.SetWindowTextW(std::to_wstring(cur_pot - 127).c_str());
		if (m_pot2_slider.m_hWnd == pNMTPC->hdr.hwndFrom)
			m_pot2_edit.SetWindowTextW(Pot2Resistense[cur_pot] + om_str);
			//m_pot2_edit.SetWindowTextW(Pot2Resistense[cur_pot]);
		SetTimer(1, 100, NULL);
	}
}

const CString СTensoDlg::Pot2Resistense[256] =
{
	L"449.3", L"449.2", L"449.2", L"449.1", L"449", L"448.9", L"448.9", L"448.8", L"448.7", L"448.6", L"448.5",
	L"448.4", L"448.4", L"448.3", L"448.2", L"448.1", L"448", L"447.9", L"447.9", L"447.8", L"447.7",
	L"447.6", L"447.5", L"447.4", L"447.3", L"447.2", L"447.1", L"447", L"446.9", L"446.9", L"446.8",
	L"446.7", L"446.6", L"446.5", L"446.4", L"446.3", L"446.2", L"446.1", L"446", L"445.9", L"445.8",
	L"445.7", L"445.5", L"445.4", L"445.3", L"445.2", L"445.1", L"445", L"444.9", L"444.8", L"444.7",
	L"444.6", L"444.4", L"444.3", L"444.2", L"444.1", L"444", L"443.8", L"443.7", L"443.6", L"443.5",
	L"443.3", L"443.2", L"443.1", L"443", L"442.8", L"442.7", L"442.6", L"442.4", L"442.3", L"442.2",
	L"442", L"441.9", L"441.7", L"441.6", L"441.5", L"441.3", L"441.2", L"441", L"440.9", L"440.7",
	L"440.6", L"440.4", L"440.2", L"440.1", L"439.9", L"439.8", L"439.6", L"439.4", L"439.3", L"439.1",
	L"438.9", L"438.8", L"438.6", L"438.4", L"438.2", L"438.1", L"437.9", L"437.7", L"437.5", L"437.3",
	L"437.1", L"436.9", L"436.7", L"436.5", L"436.3", L"436.1", L"435.9", L"435.7", L"435.5", L"435.3",
	L"435.1", L"434.9", L"434.6", L"434.4", L"434.2", L"434", L"433.7", L"433.5", L"433.3", L"433",
	L"432.8", L"432.5", L"432.3", L"432", L"431.8", L"431.5", L"431.2", L"431", L"430.7", L"430.4",
	L"430.1", L"429.9", L"429.6", L"429.3", L"429", L"428.7", L"428.4", L"428.1", L"427.8", L"427.4",
	L"427.1", L"426.8", L"426.4", L"426.1", L"425.8", L"425.4", L"425.1", L"424.7", L"424.3", L"424",
	L"423.6", L"423.2", L"422.8", L"422.4", L"422", L"421.6", L"421.2", L"420.7", L"420.3", L"419.9",
	L"419.4", L"418.9", L"418.5", L"418", L"417.5", L"417", L"416.5", L"416", L"415.5", L"415",
	L"414.4", L"413.9", L"413.3", L"412.7", L"412.1", L"411.5", L"410.9", L"410.3", L"409.7", L"409",
	L"408.3", L"407.6", L"406.9", L"406.2", L"405.5", L"404.8", L"404", L"403.2", L"402.4", L"401.6",
	L"400.7", L"399.9", L"399", L"398.1", L"397.2", L"396.2", L"395.2", L"394.2", L"393.2", L"392.1",
	L"391", L"389.9", L"388.8", L"387.6", L"386.4", L"385.1", L"383.8", L"382.5", L"381.1", L"379.6",
	L"378.2", L"376.6", L"375.1", L"373.4", L"371.8", L"370", L"368.2", L"366.3", L"364.4", L"362.4",
	L"360.3", L"358.1", L"355.8", L"353.5", L"351", L"348.4", L"345.8", L"343", L"340", L"337",
	L"333.7", L"330.4", L"326.8", L"323.1", L"319.1", L"315", L"310.6", L"306", L"301", L"295.8",
	L"290.3", L"284.3", L"278", L"271.2", L"264", L"256.1", L"247.7", L"238.6", L"228.7", L"217.9",
	L"206", L"193.1", L"178.7", L"162.9", L"145.1"
};

const float СTensoDlg::Pot2Resistensef[256] = { 449.3f, 449.2f, 449.2f, 449.1f, 449.f, 448.9f, 448.9f, 448.8f, 448.7f, 448.6f, 448.5f, 
448.4f, 448.4f, 448.3f, 448.2f, 448.1f, 448.f, 447.9f, 447.9f, 447.8f, 447.7f, 447.6f, 447.5f, 447.4f, 447.3f, 447.2f, 447.1f, 447.f, 
446.9f, 446.9f, 446.8f, 446.7f, 446.6f, 446.5f, 446.4f, 446.3f, 446.2f, 446.1f, 446.f, 445.9f, 445.8f, 445.7f, 445.5f, 445.4f, 445.3f, 
445.2f, 445.1f, 445.f, 444.9f, 444.8f, 444.7f, 444.6f, 444.4f, 444.3f, 444.2f, 444.1f, 444.f, 443.8f, 443.7f, 443.6f, 443.5f, 443.3f, 
443.2f, 443.1f, 443.f, 442.8f, 442.7f, 442.6f, 442.4f, 442.3f, 442.2f, 442.f, 441.9f, 441.7f, 441.6f, 441.5f, 441.3f, 441.2f, 441.f, 
440.9f, 440.7f, 440.6f, 440.4f, 440.2f, 440.1f, 439.9f, 439.8f, 439.6f, 439.4f, 439.3f, 439.1f, 438.9f, 438.8f, 438.6f, 438.4f, 438.2f, 
438.1f, 437.9f, 437.7f, 437.5f, 437.3f, 437.1f, 436.9f, 436.7f, 436.5f, 436.3f, 436.1f, 435.9f, 435.7f, 435.5f, 435.3f, 435.1f, 434.9f, 
434.6f, 434.4f, 434.2f, 434.f, 433.7f, 433.5f, 433.3f, 433.f, 432.8f, 432.5f, 432.3f, 432.f, 431.8f, 431.5f, 431.2f, 431.f, 430.7f, 430.4f, 
430.1f, 429.9f, 429.6f, 429.3f, 429.f, 428.7f, 428.4f, 428.1f, 427.8f, 427.4f, 427.1f, 426.8f, 426.4f, 426.1f, 425.8f, 425.4f, 425.1f, 424.7f, 
424.3f, 424.f, 423.6f, 423.2f, 422.8f, 422.4f, 422.f, 421.6f, 421.2f, 420.7f, 420.3f, 419.9f, 419.4f, 418.9f, 418.5f, 418.f, 417.5f, 417.f, 416.5f, 
416.f, 415.5f, 415.f, 414.4f, 413.9f, 413.3f, 412.7f, 412.1f, 411.5f, 410.9f, 410.3f, 409.7f, 409.f, 408.3f, 407.6f, 406.9f, 406.2f, 405.5f, 404.8f, 
404.f, 403.2f, 402.4f, 401.6f, 400.7f, 399.9f, 399.f, 398.1f, 397.2f, 396.2f, 395.2f, 394.2f, 393.2f, 392.1f, 391.f, 389.9f, 388.8f, 387.6f, 386.4f, 
385.1f, 383.8f, 382.5f, 381.1f, 379.6f, 378.2f, 376.6f, 375.1f, 373.4f, 371.8f, 370.f, 368.2f, 366.3f, 364.4f, 362.4f, 360.3f, 358.1f, 355.8f, 353.5f, 
351.f, 348.4f, 345.8f, 343.f, 340.f, 337.f, 333.7f, 330.4f, 326.8f, 323.1f, 319.1f, 315.f, 310.6f, 306.f, 301.f, 295.8f, 290.3f, 284.3f, 278.f, 
271.2f, 264.f, 256.1f, 247.7f, 238.6f, 228.7f, 217.9f, 206.f, 193.1f, 178.7f, 162.9f, 145.1f };

bool СTensoDlg::isTurnOnGen() const
{
	if (cur_channel == nullptr)
		return true;

	// Нет генераторов
	if (!cur_channel->GetBuiltinGeneratorEnabled() && 
		!cur_channel->GetExternalGeneratorEnabled())
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CSliderWheelHook, CSliderCtrl)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

CSliderWheelHook::CSliderWheelHook() : CSliderCtrl()
{

}

BOOL CSliderWheelHook::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int cur_pos = this->GetPos();

	if (zDelta < 0)
		this->SetPos(cur_pos - 1);
	else
		this->SetPos(cur_pos + 1);
	WhenWheel(this);
	return TRUE;
}

template<typename T>
int СTensoDlg::GetNearestVal(const T& val, const T mass[], int mass_size) const
{
	std::map<T, int> mm;
	int cur_s = 0;
	int cur_e = mass_size;
	int cur_pos;
	
	if (val > mass[0])
		return 0;
	else if (val < mass[mass_size - 1])
		return mass_size - 1;

	while ((cur_e - cur_s) > 1)
	{
		cur_pos = static_cast<int>((cur_s + cur_e) / 2);
		T cur_val = mass[cur_pos];

		if (cur_val < val)
			cur_e = cur_pos;
		else
			cur_s = cur_pos;
	}

	mm.insert(std::make_pair(abs((T)(mass[cur_pos] - val)), cur_pos));
	
	if (cur_pos < (mass_size - 1))
		mm.insert(std::make_pair(abs((T)(mass[cur_pos + 1] - val)), cur_pos + 1));
	if (cur_pos > 0)
		mm.insert(std::make_pair(abs((T)(mass[cur_pos - 1] - val)), cur_pos - 1));
		
	return mm.begin()->second;
}

int СTensoDlg::measureStrainGauge()
{
	if (cur_channels.size() > 1)
	{
		for (auto& element : cur_channels)
		{
			auto sMean = m_mean_indicator.GetTextString();
			auto mean = _wtof(sMean.GetString());
			auto val = (mean + element->Shift(true));
			val *= element->Sense(true) * element->Amplify(true);

			// ошибка измерений
			if (0 == isfinite(val))
				return -1;

			auto level = element->getLevel();
			auto amplifier = element->getAmplify();
			int pot1 = -1, pot2 = -1;
			GetPotSlidersPos(&pot1, &pot2);
			if (level >= 0.99)
			{
				// перегрузка вверх
				if (val > 0)
				{
					// минимальное опорное сопротивление
					if (pot2 == 255)
					{
						// нужно проверить тензорезистор
						if (amplifier <= 1.)
							return -2;

						// можно уменьшить КУ
						return -3;
					}

					// можно уменьшить опорное сопротивление
					return -4;
				}
				// перегрузка вниз

				// максимальное опорное сопротивление
				if (pot2 == 0)
				{
					// нужно проверить тензорезистор
					if (amplifier <= 1.)
						return -2;

					// можно уменьшить КУ
					return -3;
				}

				// можно увеличить опорное сопротивление
				return -5;
			}

			long state(0);
			double freq(0.0), ampl(0.0), offset(0.0);
			element->GetBuiltinGeneratorSettings(state, freq, ampl, offset);

			// ошибка измерений сопротивления
			if (abs(val) / offset >= 0.5)
				return -6;

			auto r = (1. - 2. * val / offset) / (1. + 2. * val / offset) * (double)Pot2Resistensef[pot2];

			if (r > 10000)
				return -6;

			CString s;
			s.Format(L"%.2f " + om_str, r);
			m_tenzoResistanceEdit.SetWindowTextW(s);

			// проверка на максимальный ток 5 мА
			auto maxU = 0.005f * (r + Pot2Resistensef[pot2]);
			if (maxU < offset)
			{
				CString sText;
				sText.Format(g_sTensoWarningMsg9, maxU);
				MessageBox(sText, g_sTensoWarning, MB_ICONWARNING);
				return -7;
			}
		}
	}
	else
	{
		auto sMean = m_mean_indicator.GetTextString();
		auto mean = _wtof(sMean.GetString());
		auto val = (mean + cur_channel->Shift(true));
		val *= cur_channel->Sense(true) * cur_channel->Amplify(true);

		// ошибка измерений
		if (0 == isfinite(val))
			return -1;

		auto level = cur_channel->getLevel();
		auto amplifier = cur_channel->getAmplify();
		int pot1 = -1, pot2 = -1;
		GetPotSlidersPos(&pot1, &pot2);
		if (level >= 0.99)
		{
			// перегрузка вверх
			if (val > 0)
			{
				// минимальное опорное сопротивление
				if (pot2 == 255)
				{
					// нужно проверить тензорезистор
					if (amplifier <= 1.)
						return -2;

					// можно уменьшить КУ
					return -3;
				}

				// можно уменьшить опорное сопротивление
				return -4;
			}
			// перегрузка вниз

			// максимальное опорное сопротивление
			if (pot2 == 0)
			{
				// нужно проверить тензорезистор
				if (amplifier <= 1.)
					return -2;

				// можно уменьшить КУ
				return -3;
			}

			// можно увеличить опорное сопротивление
			return -5;
		}

		long state(0);
		double freq(0.0), ampl(0.0), offset(0.0);
		cur_channel->GetBuiltinGeneratorSettings(state, freq, ampl, offset);

		// ошибка измерений сопротивления
		if (abs(val) / offset >= 0.5)
			return -6;

		auto r = (1. - 2. * val / offset) / (1. + 2. * val / offset) * (double)Pot2Resistensef[pot2];

		if (r > 10000)
			return -6;

		CString s;
		s.Format(L"%.2f " + om_str, r);
		m_tenzoResistanceEdit.SetWindowTextW(s);

		// проверка на максимальный ток 5 мА
		auto maxU = 0.005f * (r + Pot2Resistensef[pot2]);
		if (maxU < offset)
		{
			CString sText;
			sText.Format(g_sTensoWarningMsg9, maxU);
			MessageBox(sText, g_sTensoWarning, MB_ICONWARNING);
			return -7;
		}
	}
	return 0;
}

HBRUSH СTensoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_TURNON_GENERATOR)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void СTensoDlg::OnCoefficientChangeRequest()
{
	if (nullptr == cur_channel)
		return;


	CEdit* e = (CEdit*)GetDlgItem(IDC_COEFFICIENT);

	CString coefficient = m_coefficient;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_coefficient.Find(L",") != -1)
			m_coefficient.Replace(L",", L".");
		if (m_coefficient.Find(L"б") != -1)
			m_coefficient.Replace(L"б", L".");
		if (m_coefficient.Find(L"ю") != -1)
			m_coefficient.Replace(L"ю", L".");
		if (m_coefficient.Find(L"\\") != -1)
			m_coefficient.Replace(L"\\", L".");
		if (m_coefficient.Find(L"\\") != -1)
			m_coefficient.Replace(L"\\", L".");
		if (m_coefficient.Find(L"/") != -1)
			m_coefficient.Replace(L"/", L".");

		double _coefficient(boost::lexical_cast<float>((LPCTSTR)m_coefficient));
		if (_coefficient >= 1 && _coefficient <= 200)
		{
			if (cur_channels.size() > 1)
			{
				for (auto& element : cur_channels)
				{

					element->setTensoCoeff(_coefficient);
				}
			}
			else
			{
				cur_channel->setTensoCoeff(_coefficient);
			}
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_coefficient.IsEmpty() && !!m_coefficient.Compare(_T("-")))
			m_coefficient = coefficient;
	}

	UpdateData(FALSE);

	e->SetSel(ret);

}

void СTensoDlg::OnYoungModChangeRequest()
{
	if (nullptr == cur_channel)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_YOUNG_MOD);

	CString youngMod = m_youngMod;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_youngMod.Find(L",") != -1)
			m_youngMod.Replace(L",", L".");
		if (m_youngMod.Find(L"б") != -1)
			m_youngMod.Replace(L"б", L".");
		if (m_youngMod.Find(L"ю") != -1)
			m_youngMod.Replace(L"ю", L".");
		if (m_youngMod.Find(L"\\") != -1)
			m_youngMod.Replace(L"\\", L".");
		if (m_youngMod.Find(L"\\") != -1)
			m_youngMod.Replace(L"\\", L".");
		if (m_youngMod.Find(L"/") != -1)
			m_youngMod.Replace(L"/", L".");

		double _youngMod(boost::lexical_cast<float>((LPCTSTR)m_youngMod));
		if (_youngMod >= 1 && _youngMod <= 1000)
		{
			if (cur_channels.size() > 1)
			{
				for (auto& element : cur_channels)
				{
					element->setTensoYoungMod(_youngMod);
				}
			}
			else
			{
				cur_channel->setTensoYoungMod(_youngMod);
			}
		}
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_youngMod.IsEmpty() && !!m_youngMod.Compare(_T("-")))
			m_youngMod = youngMod;
	}

	UpdateData(FALSE);

	e->SetSel(ret);
}

void СTensoDlg::OnUnitSelect()
{
	CString sUnit;
	auto curSel = m_unitCombo.GetCurSel();
	if (curSel >= 0)
		m_unitCombo.GetLBText(curSel, sUnit);
	else
		m_unitCombo.GetWindowText(sUnit);
	if (cur_channels.size() > 1)
	{
		for (auto& element : cur_channels)
		{
			element->setUnit(sUnit.GetString());
			m_unit_indicator.SetTextString(element->getUnit().c_str());

			GetDlgItem(IDC_YOUNG_MOD)->EnableWindow(curSel == 0 ? FALSE : TRUE);

			element->setReference(g_tensoRef[curSel]);
		}
	}
	else
	{
		cur_channel->setUnit(sUnit.GetString());
		m_unit_indicator.SetTextString(cur_channel->getUnit().c_str());

		GetDlgItem(IDC_YOUNG_MOD)->EnableWindow(curSel == 0 ? FALSE : TRUE);

		cur_channel->setReference(g_tensoRef[curSel]);
	}
}
