#include "stdafx.h"
#include "../../../application/application.h"
#include "clipboard_toolbar.h"
#include <mathZet.h>
#include "resource\Phrases.h"

CClipboardToolBar::CClipboardToolBar()
{
}

CClipboardToolBar::~CClipboardToolBar()
{
}

BEGIN_MESSAGE_MAP( CClipboardToolBar, CToolBar )
    ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText )
    ON_WM_SIZE()
END_MESSAGE_MAP()

int CClipboardToolBar::Create( CWnd* pParentWnd, UINT nIDResource )  // создание кнопок Копировать / Вставить
{
    if (CToolBar::Create( pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_BOTTOM | CBRS_TOOLTIPS | BTNS_AUTOSIZE ) && LoadToolBar( nIDResource ))
    {
        m_imageList.Detach();
        m_imageList.Create( m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ), ILC_COLOR32, 2, 1 );

        // Load icons
        m_imageList.Add( AfxGetApp()->LoadIcon( IDI_COPY ) );
        m_imageList.Add( AfxGetApp()->LoadIcon( IDI_PASTE ) );

        GetToolBarCtrl().SetImageList( &m_imageList );
 
        pParentWnd->RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );

        CRect parent_rc;
        pParentWnd->GetWindowRect(parent_rc);
        
        CRect rc;
        GetWindowRect( rc );
        ScreenToClient( &rc );
        MoveWindow( m_dpiAware.ScaleX( 2 ), m_dpiAware.ScaleY(parent_rc.Height() - 74), rc.Width(), m_dpiAware.ScaleY( rc.Height() ), FALSE );

		SetButtonText(0, g_sCopy);
       	SetButtonText(1, g_sPaste);
        ModifyStyle( 0, TBSTYLE_LIST );

        return rc.Height();
    }

    return 0;
}

BOOL CClipboardToolBar::OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult )
{
    LPTOOLTIPTEXT pTTT = reinterpret_cast< LPTOOLTIPTEXT >(pNMHDR);

    *pResult = CDRF_DODEFAULT;

    CString cstrText;

    if (pTTT->hdr.idFrom)
    {
        switch (pTTT->hdr.idFrom)
        {
        case ID_COPY:
			cstrText = g_sCopy;
            break;

        case ID_PASTE:
           	cstrText = g_sPaste;
            break;
        }

        LPTSTR buffer = lstrcpyn( pTTT->szText, cstrText, sizeof(pTTT->szText) / sizeof(TCHAR) );

        return TRUE;
    }

    return FALSE;
}

BOOL CClipboardToolBar::SetButtonText(int nIndex, CString uResID)  //Задаёт текст, отображаемый на кнопке панели инструментов.
{
    CString cstrText;
	cstrText = uResID;
    return CToolBar::SetButtonText( nIndex, cstrText );
}

void CClipboardToolBar::SetPasteAvailable( BOOL bAvailable )   // включает или отключает кнопку "Вставить" в зависимости от наличия данных в буфере обмена
{
    
    SetButtonStyle( 1, bAvailable ? TBBS_BUTTON : TBBS_DISABLED );
    SetSizes( CSize(m_dpiAware.ScaleX( 100 ), m_dpiAware.ScaleY( 24 )),
              CSize(m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 )) );
}
