#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "digport_tab.h"

#include <CDPI/cdpi.h>

#include <boost/make_shared.hpp>

#include <mathZet.h>
#include "resource\Phrases.h"

CDigPortTab::CDigPortTab( CWnd* pParent ) :
    CTab( CDigPortTab::IDD, pParent ),
    device_(nullptr),
    m_num_ports(1)
{
}

CDigPortTab::~CDigPortTab()
{
}

void CDigPortTab::DoDataExchange( CDataExchange* pDX )
{
    CTab::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CDigPortTab, CTab )
    ON_WM_TIMER     ()
    ON_BN_CLICKED   ( IDC_GROUP_INPUTS,        OnInputsChecked )
    ON_BN_CLICKED   ( IDC_GROUP_OUTPUTS,       OnOutputsChecked )
    ON_BN_CLICKED   ( IDC_GROUP_OUTPUTSENABLE, OnOutputsEnableChecked )
    ON_CONTROL_RANGE( BN_CLICKED, 1, 42,       OnChecked )
    ON_MESSAGE_VOID ( WM_USER_CANCEL,          OnUserCancel )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CDigPortTab )
    ANCHOR_MAP_ENTRY( NULL, ANF_TOP | ANF_LEFTRIGHT )
END_ANCHOR_MAP()

BOOL CDigPortTab::OnInitDialog()
{
    CTab::OnInitDialog();

	//SetWindowText(g_sDigitalPort);

	GetDlgItem(IDC_GROUP_INPUTS)->SetWindowTextW(g_sLevelIndicationLine);
	GetDlgItem(IDC_GROUP_OUTPUTS)->SetWindowTextW(g_sControlStatusOutput);
	GetDlgItem(IDC_GROUP_OUTPUTSENABLE)->SetWindowTextW(g_sExitEnableDisable);

    m_groupInputs.       SubclassDlgItem( IDC_GROUP_INPUTS,        this );
    m_groupOutputs.      SubclassDlgItem( IDC_GROUP_OUTPUTS,       this );
    m_groupOutputsEnable.SubclassDlgItem( IDC_GROUP_OUTPUTSENABLE, this );

    return TRUE;
}

void CDigPortTab::SetAdapter( zetlab::std::device* const device__ )
{
    device_  = device__;
    memento_ = boost::make_shared< zetlab::std::device_memento >( device__ );

    int count = device_->numDigitalPorts();
    m_num_ports = 1;

    InitChecks( &m_groupInputs, m_checkInput, count, WS_DISABLED, 17, 40, 47, 60 );
    InitChecks( &m_groupOutputs, m_checkOutput, count, 0, 17, 145, 47, 165 );
    InitChecks( &m_groupOutputsEnable, m_checkOutputEnable, count, 0, 17, 255, 47, 275 );

    ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalInputs, device_, _1 ), &m_groupInputs, m_checkInput );
    ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalOutputs, device_, _1 ), &m_groupOutputs, m_checkOutput );
    ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalOutEnable, device_, _1 ), &m_groupOutputsEnable, m_checkOutputEnable );

    //KillTimer( 1 );
    //SetTimer( 1, 500, NULL );
}

void CDigPortTab::OnTimer( UINT_PTR nIDEvent )
{
    CTab::OnTimer( nIDEvent );

    switch (nIDEvent)
    {
    case 1:
        ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalInputs, device_, _1 ), &m_groupInputs, m_checkInput );
        ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalOutputs, device_, _1 ), &m_groupOutputs, m_checkOutput );
        ReadPortsStatus( boost::bind( &zetlab::std::device::getDigitalOutEnable, device_, _1 ), &m_groupOutputsEnable, m_checkOutputEnable );
        break;
    }
}

void CDigPortTab::InitChecks( CCheckGroupBox* group, CButton* checks, int count, int flag, int l_, int t_, int r_, int b_ )
{
    CDPI dpiAware;

    CString num;
    int l, t, r, b;

    for (int i = 0, j = 0; i < count; ++i, ++j)
    {
        if (i == 10)
            t_ += 30, b_ += 30, j = 0;

        num.Format( _T("%i"), i + 1 );
        l = l_ + (40 * j);
        t = t_;
        r = r_ + (40 * j);
        b = b_;

        CRect rc( l, t, r, b );
        dpiAware.ScaleRect( &rc );

        checks[i].Create( num, WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | flag, rc, this, m_num_ports++ );
        checks[i].SetFont( this->GetFont() );
    }

    group->EnableWindow( (WS_DISABLED != flag) );
}

void CDigPortTab::ReadPortsStatus(
    const boost::function< long(std::vector< bool >&) >& f,
    CCheckGroupBox* group,
    CButton* checks )
{
    long nPorts;
    ::std::vector< bool > v_status;

    // Get
    if ((nPorts = f( v_status )) <= 0)
        nPorts = 0;

    // Show & check
    int nChecked(0);
    for (long i = 0; i < nPorts; ++i)
    {
        nChecked += int(v_status[i]);
        checks[i].SetCheck( v_status[i] );
    }

    UpdateGroupCheck( group, checks );
}

void CDigPortTab::OnInputsChecked()
{
    int nCheck(!m_groupInputs.GetCheck());

    for (int i = 0, c = m_num_ports / 3; i < c; ++i)
    {
        m_checkInput[i].SetCheck( nCheck );
        OnChecked( 1 + i );
    }
}

void CDigPortTab::OnOutputsChecked()
{
    int nCheck(!m_groupOutputs.GetCheck());

    for (int i = 0, c = m_num_ports / 3; i < c; ++i)
    {
        m_checkOutput[i].SetCheck( nCheck );
        OnChecked( 1 + c + i );
    }
}

void CDigPortTab::OnOutputsEnableChecked()
{
    int nCheck(!m_groupOutputsEnable.GetCheck());

    for (int i = 0, c = m_num_ports / 3; i < c; ++i)
    {
        m_checkOutputEnable[i].SetCheck( nCheck );
        OnChecked( 1 + 2 * c + i );
    }
}

void CDigPortTab::OnChecked( UINT nID )
{
    if (nullptr == device_)
        return;

    int port;
    int input_state, output_state, output_e_state;

    UINT num_port_1_3(m_num_ports / 3);
    UINT num_port_2_3(num_port_1_3 * 2);

    if (nID > num_port_1_3 && nID <= num_port_2_3) // Digital outputs
        port = nID - num_port_1_3 - 1;

    else // Digital output enable
        port = nID - num_port_2_3 - 1;

    output_state	= m_checkOutput[port].GetCheck();
    output_e_state	= m_checkOutputEnable[port].GetCheck();
    input_state		= output_e_state ? output_state ? 1 : 0 : 1;

    device_->setDigitalOutput( port, output_state == 1 );
    device_->setDigitalOutEnable( port, output_e_state == 1 );

    m_checkInput[port].SetCheck( input_state );
    m_checkOutput[port].SetCheck( output_state );
    m_checkOutputEnable[port].SetCheck( output_e_state );

    UpdateGroupCheck( &m_groupInputs, m_checkInput );
    UpdateGroupCheck( &m_groupOutputs, m_checkOutput );
    UpdateGroupCheck( &m_groupOutputsEnable, m_checkOutputEnable );
}

void CDigPortTab::UpdateGroupCheck( CCheckGroupBox* group, CButton* check )
{
    int nCheckedItems(0);

    for (int i = 0, c = m_num_ports / 3; i < c; ++i)
        nCheckedItems += check[i].GetCheck();

    if (nCheckedItems == (m_num_ports / 3))
        group->SetCheck( 1 );
    else if (!nCheckedItems)
        group->SetCheck( 0 );
    else
        group->SetCheck( 2 );
}

void CDigPortTab::OnCancel()
{
    if (nullptr == device_)
        return;

    memento_->restore();

    CTab::OnCancel();
}

void CDigPortTab::OnUserCancel()
{
    if (nullptr == device_)
        return;

    memento_->restore();
}

void CDigPortTab::Activate()
{
	KillTimer(1);
	SetTimer(1, 500, NULL);
}

void CDigPortTab::Disactivate()
{
	KillTimer(1);
}
