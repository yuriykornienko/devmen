#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "frequency_tab.h"

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include "resource\Phrases.h"
#include <mathZet.h>

CFrequencyTab::CFrequencyTab( CWnd* pParent ) 
	:    CTab( CFrequencyTab::IDD, pParent )
	,    device_(nullptr)
	,    m_adc_value(0.f)
	,	 m_dac_value(0.f)
{
}

CFrequencyTab::~CFrequencyTab()
{
}

void CFrequencyTab::DoDataExchange( CDataExchange* pDX )
{
	CTab::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_COMBO_ADC, m_comboAdc );
	DDX_Text   ( pDX, IDC_COMBO_ADC, m_adc_value );
	DDX_Control( pDX, IDC_COMBO_DAC, m_comboDac );
	DDX_Text   ( pDX, IDC_COMBO_DAC, m_dac_value );
}

BEGIN_MESSAGE_MAP( CFrequencyTab, CTab )
	ON_CBN_SELCHANGE( IDC_COMBO_ADC,  OnAdcChangeRequest )
	ON_CBN_SELCHANGE( IDC_COMBO_DAC,  OnDacChangeRequest )
	ON_MESSAGE_VOID ( WM_USER_CANCEL, OnUserCancel )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CFrequencyTab )
	ANCHOR_MAP_ENTRY( NULL, ANF_TOP | ANF_LEFTRIGHT )
END_ANCHOR_MAP()

BOOL CFrequencyTab::OnInitDialog()
{
	CTab::OnInitDialog();

	GetDlgItem(IDC_GROUP_ADC)->SetWindowTextW(g_sADC);
	GetDlgItem(IDC_LABEL_ADC)->SetWindowTextW(g_sSamplingFrequency + L", " + g_sUNIT_Hz + L":");//L"„астота дискретизации, √ц:"
	GetDlgItem(IDC_GROUP_DAC)->SetWindowTextW(g_sDAC);
	GetDlgItem(IDC_LABEL_DAC)->SetWindowTextW(g_sSamplingFrequency + L", " + g_sUNIT_Hz + L":");//L"„астота дискретизации, √ц:"

	return TRUE;
}

void CFrequencyTab::SetAdapter( zetlab::std::device* const device__ )
{
	device_  = device__;
	memento_ = boost::make_shared< zetlab::std::device_memento >( device__ );

	InsertFrequencies( boost::bind( &zetlab::std::device::getAdcFrequency, device_, _1 ), &m_comboAdc );
	InsertFrequencies( boost::bind( &zetlab::std::device::getDacFrequency, device_, _1 ), &m_comboDac );

	if (device_ != nullptr && device_->getType() == KDU8500 && !device_->isTensostation())
		GetDlgItem(IDC_COMBO_DAC)->EnableWindow(FALSE);
}

void CFrequencyTab::InsertFrequencies( const boost::function< float(::std::vector< float >&) >& f,
									   CComboBox* combo )
{
	combo->ResetContent();

	::std::vector< float > v_freq;

	float freq(f( v_freq ));

	if (v_freq.empty())
	{
		combo->EnableWindow( FALSE );
		combo->SetCurSel( combo->AddString( _T("0")) );
	}
	else
	{
		combo->EnableWindow( TRUE );

		int nSel(CB_ERR);

		boost::range::for_each( v_freq, [&]( float& f ) {
			int n = combo->AddString( boost::lexical_cast< zetlab::tstring >( f ).c_str() );

			if (f == freq)
				nSel = n;
		});

		combo->SetCurSel( nSel );
	}
}

void CFrequencyTab::OnAdcChangeRequest()
{
	if (nullptr == device_)
		return;

	UpdateData();

	device_->setAdcFrequency( m_adc_value );

	if (device_->getType() == KDU8500 && !device_->isTensostation())
	{
		if (m_adc_value >= 50e3)
			m_dac_value = 100e3;
		else if (m_adc_value >= 25e3)
			m_dac_value = 50e3;
		else
			m_dac_value = 10e3;

		// SelectString не подойдет, она не отличает 100000 и 10000
		int ret = m_comboDac.FindStringExact(-1, boost::lexical_cast<zetlab::tstring>(m_dac_value).c_str());
		if (ret != CB_ERR)
		{
			m_comboDac.SetCurSel(ret);
			device_->setDacFrequency(m_dac_value);
		}
	}
}

void CFrequencyTab::OnDacChangeRequest()
{
	if (nullptr == device_)
		return;

	UpdateData();

	device_->setDacFrequency(m_dac_value);
}

void CFrequencyTab::OnCancel()
{
	if (nullptr == device_)
		return;

	memento_->restore();

	CTab::OnCancel();
}

void CFrequencyTab::OnUserCancel()
{
	if (nullptr == device_)
		return;

	memento_->restore();
}
