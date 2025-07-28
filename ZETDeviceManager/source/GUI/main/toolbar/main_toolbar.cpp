#include "stdafx.h"
#include "../../../application/application.h"
#include "main_toolbar.h"
#include <mathZet.h>
#include "resource\Phrases.h"

CMainToolBar::CMainToolBar()
{
}

CMainToolBar::~CMainToolBar()
{
}

BEGIN_MESSAGE_MAP( CMainToolBar, CToolBar )
	ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText )
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CMainToolBar::Create( CWnd* pParentWnd, UINT nIDResource )
{
	if (CToolBar::Create( pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS ) && LoadToolBar( nIDResource ))
	{
		m_imageList.Detach();
		m_imageList.Create( m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ), ILC_COLOR32, 6, 1 );

		// Load icons
		m_imageList.Add( AfxGetApp()->LoadIcon( IDI_SENSORS ) );
		m_imageList.Add( AfxGetApp()->LoadIcon( IDI_DETAIL ) );
		m_imageList.Add( AfxGetApp()->LoadIcon( IDI_PROPERTIES ) );
		m_imageList.Add( AfxGetApp()->LoadIcon( IDI_REFRESH ) );
		m_imageList.Add( AfxGetApp()->LoadIcon( IDI_HELP ) );

		GetToolBarCtrl().SetImageList( &m_imageList );
 
		pParentWnd->RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );

		CRect rc;
		GetWindowRect( rc );
		ScreenToClient( &rc );
		MoveWindow( 0, -m_dpiAware.ScaleY( 2 ), rc.Width(), m_dpiAware.ScaleY( rc.Height() + 2 ), FALSE );

		return rc.Height();
	}

	return 0;
}

BOOL CMainToolBar::OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult )
{
	LPTOOLTIPTEXT pTTT = reinterpret_cast< LPTOOLTIPTEXT >(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	CString cstrText;

	if (pTTT->hdr.idFrom)
	{
		switch (pTTT->hdr.idFrom)
		{
		case ID_SENSORS_DB:
			cstrText = g_sEditSensorDatabase;
			break;

		case ID_DETAIL:
			cstrText = g_sDetailedShort;
			break;

		case ID_PROPERTIES:
			cstrText = g_sProperties;
			break;

 		case ID_REFRESH:
 			cstrText = g_sUpdate;
 			break;

		case ID_HELP:
			cstrText = g_sHelp;
			break;
		}

		LPTSTR buffer = lstrcpyn( pTTT->szText, cstrText, sizeof(pTTT->szText) / sizeof(TCHAR) );

		return TRUE;
	}

	return FALSE;
}

void CMainToolBar::OnSize( UINT nType, int cx, int cy )
{
	CToolBar::OnSize( nType, cx, cy );

	if (m_progress.m_hWnd && IsWindow( m_progress.m_hWnd ))
		m_progress.SetWindowPos( &wndTop, cx - m_dpiAware.ScaleX( 150 ), m_dpiAware.ScaleY( 2 ), 0, 0, SWP_NOSIZE );
}

void CMainToolBar::CreateProgress()
{
	if (m_progress.m_hWnd && IsWindow( m_progress.m_hWnd ))
		return;

	CRect rc;

	GetClientRect( rc );
	rc.top -= m_dpiAware.ScaleY( 1 );
	rc.left = rc.right - m_dpiAware.ScaleX( 150 );
	rc.DeflateRect( 0, m_dpiAware.ScaleY( 3 ) );

	m_progress.Create( WS_CHILD | WS_VISIBLE | PBS_MARQUEE| PBS_SMOOTH, rc, this, 1001 );
	m_progress.SetMarquee( TRUE, 0 );
}

void CMainToolBar::DestoryProgress()
{
	m_progress.DestroyWindow();
}
