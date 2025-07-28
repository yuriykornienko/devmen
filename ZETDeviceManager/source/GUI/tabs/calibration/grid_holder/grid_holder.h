#pragma once

#include <CDPI/cdpi.h>

class CGridHolder : public CStatic
{
public:
    CGridHolder() : m_ax(NULL)
    {
    };

    virtual ~CGridHolder()
    {
    };

public:
    void SetAx( CWnd* ax )
    {
        m_ax = ax;
    };

protected:
    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* GetMessageMap() const
    {
        return GetThisMessageMap();
    };

    static const AFX_MSGMAP* PASCAL GetThisMessageMap()
    {
        typedef CGridHolder ThisClass;
        typedef CButton TheBaseClass;
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            // Message map
            ON_WM_SIZE()

            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
        };

        static const AFX_MSGMAP messageMap = {
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0]
        };

        return &messageMap;
    };
    PTM_WARNING_RESTORE

    afx_msg void OnSize( UINT nType, int cx, int cy )
    {
        CStatic::OnSize( nType, cx, cy );

        if (!m_ax)
            return;

        m_ax->SetWindowPos( &wndTop,
                -m_dpiAware.ScaleX(5),     -m_dpiAware.ScaleY(18),
            cx + m_dpiAware.ScaleX(5), cy + m_dpiAware.ScaleY(18),
            SWP_NOACTIVATE );
    };

protected:
    CWnd* m_ax;
    CDPI  m_dpiAware;
};
