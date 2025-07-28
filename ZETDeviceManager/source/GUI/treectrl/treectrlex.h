#pragma once

#include <Uxtheme.h>
#include <CDPI/cdpi.h>

#include <boost/signals2.hpp>

#pragma comment(lib, "uxtheme.lib")

class CTreeCtrlEx : public CTreeCtrl
{
public:
    CTreeCtrlEx( bool b7Style_ = true ) :
        m_7Style(b7Style_),
        m_contextMenu(nullptr)
    {
    };

    virtual ~CTreeCtrlEx()
    {
    };

public:
    virtual void LoadItemImages()
    {
    };

protected:
    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* GetMessageMap() const
    {
        return GetThisMessageMap();
    };

    static const AFX_MSGMAP* PASCAL GetThisMessageMap()
    {
        typedef CTreeCtrlEx ThisClass;
        typedef CTreeCtrl TheBaseClass;
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            // Message map
            ON_WM_CONTEXTMENU()
            ON_NOTIFY_REFLECT( NM_RCLICK, OnRightClick )

            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
        };

        static const AFX_MSGMAP messageMap = {
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0]
        };

        return &messageMap;
    };
    PTM_WARNING_RESTORE

    void OnContextMenu( CWnd* pWnd, CPoint ptMousePos ) 
    {
        // if Shift-F10
        if (ptMousePos.x == -1 && ptMousePos.y == -1)
            ptMousePos = (CPoint)GetMessagePos();

        ScreenToClient( &ptMousePos );

        UINT uFlags(0);
        HTREEITEM htItem;

        htItem = HitTest( ptMousePos, &uFlags );

        if (NULL == htItem)
            return;

        SelectItem( htItem );

        ClientToScreen( &ptMousePos );

        sig_prepare_cm( &m_contextMenu );
        if (!m_contextMenu || !m_contextMenu->m_hMenu)
            return;

        m_contextMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMousePos.x, ptMousePos.y, GetParent() );
    };

    void OnRightClick( NMHDR* pNMHDR, LRESULT* pResult )
    {
        SendMessage( WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos() );

        *pResult = 1;
    }

    void PreSubclassWindow()
    {
        #ifdef UNICODE
        if (m_7Style)
            SetWindowTheme( m_hWnd, L"explorer", 0 );
        #endif

        LoadItemImages();

        CTreeCtrl::PreSubclassWindow();
    };

protected:
    bool                 m_7Style;
    CImageList           m_imageList;
    CMenu*               m_contextMenu;
    CDPI                 m_dpiAware;

public:
    void OnPrepareContextMenu( const boost::function< void(CMenu**) >& f )
        { sig_prepare_cm.connect( f ); }

private:
    boost::signals2::signal< void(CMenu**) > sig_prepare_cm;
};
