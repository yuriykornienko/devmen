#include "stdafx.h"

#include "default_tab.h"
#include "../user_messages.hpp"
#include <mathZet.h>

CTab::CTab( UINT nIDTemplate, CWnd* pParent ) :
    CAnchoredDialog( nIDTemplate, pParent ),
    n_tooltips(1)
{
}

CTab::~CTab()
{
}

void CTab::DoDataExchange( CDataExchange* pDX )
{
    CAnchoredDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CTab, CAnchoredDialog )
    ON_WM_SIZE()
    ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_USER_DOTAB, OnDoTab)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CTab )
END_ANCHOR_MAP()

BOOL CTab::OnInitDialog()
{
    CAnchoredDialog::OnInitDialog();

    m_brush.Detach();
    m_brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );

    InitAnchors();

    InitToolTips();

    return TRUE;
}

void CTab::InitToolTips()
{
    if (m_toolTips.m_hWnd)
        m_toolTips.DestroyToolTipCtrl();

    m_toolTips.Create( this );
    m_toolTips.SetMaxTipWidth( 300 );
}

void CTab::OnSize( UINT nType, int cx, int cy )
{
    CAnchoredDialog::OnSize( nType, cx, cy );

    HandleAnchors( NULL );
    Invalidate   ( false );
}

BOOL CTab::PreTranslateMessage( MSG* pMsg )
{
    m_toolTips.RelayEvent( pMsg );

    return CAnchoredDialog::PreTranslateMessage( pMsg );
}

HBRUSH CTab::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
//     if (CTLCOLOR_DLG == nCtlColor)
    return m_brush;
//     return CAnchoredDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}

void CTab::AddToolTip( CWnd* const pWnd, UINT nResID )
{
	if (csLocalization = TranslateResourse(nResID))
        AddToolTip( pWnd, csLocalization );
}

void CTab::AddToolTip( CWnd* const pWnd, LPCTSTR lpszText )
{
    CRect rc;

    pWnd->GetClientRect( rc );
    m_toolTips.AddTool( pWnd, lpszText, rc, n_tooltips++ );
}

LRESULT CTab::OnDoTab(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
		Disactivate();
	else
		Activate();

	return 0;
}