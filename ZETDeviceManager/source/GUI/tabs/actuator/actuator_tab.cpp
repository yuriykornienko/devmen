#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "actuator_tab.h"

#include <CDPI/cdpi.h>

#include <boost/make_shared.hpp>

#include <mathZet.h>
#include "resource\Phrases.h"

CActuatorTab::CActuatorTab(CWnd* pParent) :
    CTab( CActuatorTab::IDD, pParent ),
    device_(nullptr),
    m_num_actuators(0)
{
}

CActuatorTab::~CActuatorTab()
{
}

void CActuatorTab::DoDataExchange(CDataExchange* pDX)
{
    CTab::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP(CActuatorTab, CTab)
    ON_BN_CLICKED   ( IDC_GROUP_ACTUATORS,     OnActuatorsChecked )
    ON_CONTROL_RANGE( BN_CLICKED, 1, 8,        OnChecked )
    ON_MESSAGE_VOID ( WM_USER_CANCEL,          OnUserCancel )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(CActuatorTab)
    ANCHOR_MAP_ENTRY( NULL, ANF_TOP | ANF_LEFTRIGHT )
END_ANCHOR_MAP()

BOOL CActuatorTab::OnInitDialog()
{
    CTab::OnInitDialog();

	GetDlgItem(IDC_GROUP_ACTUATORS)->SetWindowTextW(g_sActuatorState);
	
    m_groupActuators.SubclassDlgItem( IDC_GROUP_ACTUATORS, this );

    return TRUE;
}

void CActuatorTab::SetAdapter(zetlab::std::device* const device__)
{
    device_  = device__;
    memento_ = boost::make_shared< zetlab::std::device_memento >( device__ );

	int count = device_->numChannels() / 4;
    m_num_actuators = 0;

    InitChecks( &m_groupActuators, m_checkActuator, count, 0, 17, 40, 47, 60 );

	if (!ZOpen(device_->getType(), device_->getDSP()))
	{
		long mask(0);
		if (!ZGetActuator(device_->getType(), device_->getDSP(), &mask))
		{
			auto nActuators = count;
			for (long i = 0; i < nActuators; ++i)
				m_checkActuator[i].SetCheck((mask >> i) & 0x1);
		}

		ZClose(device_->getType(), device_->getDSP());
	}

	UpdateGroupCheck(&m_groupActuators, m_checkActuator);
}

void CActuatorTab::InitChecks(CCheckGroupBox* group, CButton* checks, int count, int flag, int l_, int t_, int r_, int b_)
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

        checks[i].Create( num, WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | flag, rc, this, ++m_num_actuators );
        checks[i].SetFont( this->GetFont() );
    }

    group->EnableWindow( (WS_DISABLED != flag) );
}

void CActuatorTab::OnActuatorsChecked()
{
    int nCheck(!m_groupActuators.GetCheck());

	for (int i = 0, c = m_num_actuators; i < c; ++i)
    {
        m_checkActuator[i].SetCheck( nCheck );
        OnChecked( 1 + i );
    }
}

void CActuatorTab::OnChecked(UINT nID)
{
    if (nullptr == device_)
        return;

	long mask(0);
	for (int i = 0, c = m_num_actuators; i < c; ++i)
		if (m_checkActuator[i].GetCheck())
			mask += (1 << i);

	int port = nID - 1;
    int actuator_state = m_checkActuator[port].GetCheck();
   
	if (!ZOpen(device_->getType(), device_->getDSP()))
	{
		ZSetActuator(device_->getType(), device_->getDSP(), mask);
		ZClose(device_->getType(), device_->getDSP());
	}

    m_checkActuator[port].SetCheck( actuator_state );

    UpdateGroupCheck( &m_groupActuators, m_checkActuator );
}

void CActuatorTab::UpdateGroupCheck(CCheckGroupBox* group, CButton* check)
{
    int nCheckedItems(0);

    for (int i = 0, c = m_num_actuators; i < c; ++i)
        nCheckedItems += check[i].GetCheck();

	if (nCheckedItems == m_num_actuators)
        group->SetCheck( 1 );
    else if (!nCheckedItems)
        group->SetCheck( 0 );
    else
        group->SetCheck( 2 );
}

void CActuatorTab::OnCancel()
{
    if (nullptr == device_)
        return;

    memento_->restore();

    CTab::OnCancel();
}

void CActuatorTab::OnUserCancel()
{
    if (nullptr == device_)
        return;

    memento_->restore();
}

void CActuatorTab::Activate()
{
	KillTimer(1);
	SetTimer(1, 500, NULL);
}

void CActuatorTab::Disactivate()
{
	KillTimer(1);
}
