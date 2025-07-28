#include "stdafx.h"
#include "tenso_icp_dlg.h"
#include "resource/Phrases.h"

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

IMPLEMENT_DYNAMIC(CTensoIcpDlg, CDialog)

static const zetlab::tstring g_tensoUnit[] =
{
	g_sUNIT_MKMM.GetString(),
	g_sUNIT_PA.GetString(),
	g_sUNIT_KPA.GetString(),
	g_sUNIT_MPA.GetString(),
	g_sUNIT_KGSMM2.GetString()
};
static const float g_tensoRef[] = { 1.f, 0.00002f, 0.00000002f, 0.00000000002f, 0.000000000002f };

CTensoIcpDlg::CTensoIcpDlg(zetlab::std::channel* channel, CWnd* pParent /*=nullptr*/)
	: CDialog(CTensoIcpDlg::IDD, pParent)
	, m_channel(channel)
{

}

CTensoIcpDlg::~CTensoIcpDlg()
{
}

void CTensoIcpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_RESISTANCE, m_resistance);
	DDX_Text(pDX, IDC_COEFFICIENT, m_coefficient);
	DDX_Text(pDX, IDC_YOUNG_MOD, m_youngMod);
	DDX_Control(pDX, IDC_TENSO_UNIT, m_unitCombo);
}

BEGIN_MESSAGE_MAP(CTensoIcpDlg, CDialog)
	ON_EN_CHANGE(IDC_RESISTANCE, OnResistanceChangeRequest)
	ON_EN_CHANGE(IDC_COEFFICIENT, OnCoefficientChangeRequest)
	ON_EN_CHANGE(IDC_YOUNG_MOD, OnYoungModChangeRequest)
	ON_CBN_SELCHANGE(IDC_TENSO_UNIT, OnUnitSelect)
END_MESSAGE_MAP()

BOOL CTensoIcpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!memento_) memento_ = boost::make_shared< zetlab::std::channel_memento >(m_channel);

	SetWindowText(g_sTensoBridgeSettings + L": " + m_channel->getName().c_str());
	GetDlgItem(IDC_LABEL_RESISTANCE)->SetWindowText(g_sResistor + L", " + g_sUNIT_OM + L":");
	GetDlgItem(IDC_LABEL_COEFFICIENT)->SetWindowText(g_sTensoCoeff + L":");
	GetDlgItem(IDC_LABEL_TENSO_UNIT)->SetWindowText(g_sTensoUnit + L":");
	GetDlgItem(IDC_LABEL_YOUNG_MOD)->SetWindowText(g_sYoungMod + L", " + g_sUNIT_GPA + L":");
	GetDlgItem(IDOK)->SetWindowText(g_sApply);
	GetDlgItem(IDCANCEL)->SetWindowText(g_sCancel);

	m_resistance.Format(_T("%g"), m_channel->getTensoResistance(true));
	m_coefficient.Format(_T("%g"), m_channel->getTensoCoeff(true));
	m_youngMod.Format(_T("%g"), m_channel->getTensoYoungMod(true));

	auto sUnit = m_channel->getUnit();
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
		m_channel->setUnit(g_tensoUnit[0]);
	}
	m_unitCombo.SetCurSel(i2);

	GetDlgItem(IDC_YOUNG_MOD)->EnableWindow(i2 == 0 ? FALSE : TRUE);

	UpdateData(FALSE);

	return TRUE;
}

void CTensoIcpDlg::OnResistanceChangeRequest()
{
	if (nullptr == m_channel)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_RESISTANCE);

	CString resistance = m_resistance;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_resistance.Find(L",") != -1)
			m_resistance.Replace(L",", L".");
		if (m_resistance.Find(L"á") != -1)
			m_resistance.Replace(L"á", L".");
		if (m_resistance.Find(L"þ") != -1)
			m_resistance.Replace(L"þ", L".");
		if (m_resistance.Find(L"\\") != -1)
			m_resistance.Replace(L"\\", L".");
		if (m_resistance.Find(L"\\") != -1)
			m_resistance.Replace(L"\\", L".");
		if (m_resistance.Find(L"/") != -1)
			m_resistance.Replace(L"/", L".");

		double _resistance(boost::lexical_cast<float>((LPCTSTR)m_resistance));
		if (_resistance >= 50 && _resistance <= 1000)
			m_channel->setTensoResistance(_resistance);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_resistance.IsEmpty() && !!m_resistance.Compare(_T("-")))
			m_resistance = resistance;
	}

	UpdateData(FALSE);

	e->SetSel(ret);
}

void CTensoIcpDlg::OnCoefficientChangeRequest()
{
	if (nullptr == m_channel)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_COEFFICIENT);

	CString coefficient = m_coefficient;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_coefficient.Find(L",") != -1)
			m_coefficient.Replace(L",", L".");
		if (m_coefficient.Find(L"á") != -1)
			m_coefficient.Replace(L"á", L".");
		if (m_coefficient.Find(L"þ") != -1)
			m_coefficient.Replace(L"þ", L".");
		if (m_coefficient.Find(L"\\") != -1)
			m_coefficient.Replace(L"\\", L".");
		if (m_coefficient.Find(L"\\") != -1)
			m_coefficient.Replace(L"\\", L".");
		if (m_coefficient.Find(L"/") != -1)
			m_coefficient.Replace(L"/", L".");

		double _coefficient(boost::lexical_cast<float>((LPCTSTR)m_coefficient));
		if (_coefficient >= 1 && _coefficient <= 200)
			m_channel->setTensoCoeff(_coefficient);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_coefficient.IsEmpty() && !!m_coefficient.Compare(_T("-")))
			m_coefficient = coefficient;
	}

	UpdateData(FALSE);

	e->SetSel(ret);
}

void CTensoIcpDlg::OnYoungModChangeRequest()
{
	if (nullptr == m_channel)
		return;

	CEdit* e = (CEdit*)GetDlgItem(IDC_YOUNG_MOD);

	CString youngMod = m_youngMod;

	UpdateData();

	DWORD ret = e->GetSel();

	try
	{
		if (m_youngMod.Find(L",") != -1)
			m_youngMod.Replace(L",", L".");
		if (m_youngMod.Find(L"á") != -1)
			m_youngMod.Replace(L"á", L".");
		if (m_youngMod.Find(L"þ") != -1)
			m_youngMod.Replace(L"þ", L".");
		if (m_youngMod.Find(L"\\") != -1)
			m_youngMod.Replace(L"\\", L".");
		if (m_youngMod.Find(L"\\") != -1)
			m_youngMod.Replace(L"\\", L".");
		if (m_youngMod.Find(L"/") != -1)
			m_youngMod.Replace(L"/", L".");

		double _youngMod(boost::lexical_cast<float>((LPCTSTR)m_youngMod));
		if (_youngMod >= 1 && _youngMod <= 1000)
			m_channel->setTensoYoungMod(_youngMod);
	}
	catch (boost::bad_lexical_cast&)
	{
		if (!m_youngMod.IsEmpty() && !!m_youngMod.Compare(_T("-")))
			m_youngMod = youngMod;
	}

	UpdateData(FALSE);

	e->SetSel(ret);
}

void CTensoIcpDlg::OnUnitSelect()
{
	CString sUnit;
	auto curSel = m_unitCombo.GetCurSel();
	if (curSel >= 0)
		m_unitCombo.GetLBText(curSel, sUnit);
	else
		m_unitCombo.GetWindowText(sUnit);
	m_channel->setUnit(sUnit.GetString());

	GetDlgItem(IDC_YOUNG_MOD)->EnableWindow(curSel == 0 ? FALSE : TRUE);

	m_channel->setReference(g_tensoRef[curSel]);
}

void CTensoIcpDlg::OnCancel()
{
	if (nullptr == m_channel)
		return;

	memento_->restore();

	CDialog::OnCancel();
}
