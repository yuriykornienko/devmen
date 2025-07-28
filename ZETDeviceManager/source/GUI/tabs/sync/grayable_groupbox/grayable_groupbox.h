#pragma once

#include <CDPI/cdpi.h>

//#define WM_GROUP_CHECKED    (WM_USER + 1)

class CGrayableGroupBox : public CStatic
{
public:
    CGrayableGroupBox()
    {
    };

    virtual ~CGrayableGroupBox()
    {
    };

public:
    BOOL EnableWindow( BOOL bEnable = TRUE )
    {
        return m_label.EnableWindow( bEnable );
    };

protected:
    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* GetMessageMap() const
    {
        return GetThisMessageMap();
    };

    static const AFX_MSGMAP* PASCAL GetThisMessageMap()
    {
        typedef CGrayableGroupBox ThisClass;
        typedef CButton TheBaseClass;
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            // Message map
            ON_BN_CLICKED( 1, OnClicked )
            ON_WM_CTLCOLOR()

            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
        };

        static const AFX_MSGMAP messageMap = {
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0]
        };

        return &messageMap;
    };
    PTM_WARNING_RESTORE

    void OnClicked()
    {
        ::SendMessage( GetParent()->m_hWnd, WM_COMMAND,
                       MAKELONG( ::GetDlgCtrlID( m_hWnd ), BN_CLICKED ),
                       reinterpret_cast< LPARAM >(m_hWnd) );
    };

    HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
    {
        return m_brush;
    };

    void PreSubclassWindow()
    {
        CString cstrText;
        GetWindowText( cstrText );
        SetWindowText( _T("") );

        CClientDC dc(this);
        CFont* pOldFont = dc.SelectObject( GetFont() );
        CSize czText    = dc.GetTextExtent( cstrText );
        dc.SelectObject( pOldFont );

        m_label.Create( cstrText,
            WS_CHILD | WS_VISIBLE,
            CRect(m_dpiAware.ScaleX( 9 ), 0, czText.cx + m_dpiAware.ScaleX( 9 ), czText.cy), this, 1 );
        m_label.SetFont( GetFont() );

        m_brush.Detach();
        m_brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );

        CStatic::PreSubclassWindow();
    };

protected:
    CStatic m_label;
    CBrush  m_brush;
    CDPI    m_dpiAware;
};
