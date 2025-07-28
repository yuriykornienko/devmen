#include "stdafx.h"
#include "../../../application/ZETDeviceManager.h"
#include "mini_tree.h"

BEGIN_MESSAGE_MAP( CMiniTree, CTreeCtrlEx )
    ON_NOTIFY_REFLECT( NM_CUSTOMDRAW,  OnCustomDraw )
END_MESSAGE_MAP()

void CMiniTree::LoadItemImages()
{
    SetItemHeight ( m_dpiAware.ScaleY( 22 ) );

    m_imageList.Detach();
    m_imageList.Create( m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ), ILC_COLOR32, 3, 1 );

    // Load icons
    m_imageList.Add( LOAD_ICON( IDI_CHANNEL_ENABLED, m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ) ) );
    m_imageList.Add( LOAD_ICON( IDI_CHANNEL_ICP, m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ) ) );
    m_imageList.Add( LOAD_ICON( IDI_CHANNEL_DISABLED, m_dpiAware.ScaleX( 16 ), m_dpiAware.ScaleY( 16 ) ) );

    SetImageList( &m_imageList, TVSIL_NORMAL );
}

void CMiniTree::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast< LPNMTVCUSTOMDRAW >(pNMHDR);

    *pResult = CDRF_DODEFAULT;

    switch (pNMCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT: 
        *pResult = CDRF_NOTIFYITEMDRAW;     
        break;

    case CDDS_ITEMPREPAINT : 
        {
//             HTREEITEM hItem(*reinterpret_cast< HTREEITEM* >(&pNMCD->nmcd.dwItemSpec));
//             zetlab::object* object_(reinterpret_cast< zetlab::object* >(pNMCD->nmcd.lItemlParam));
// 
//             if (object_)
//             {
//                 zetlab::std::channel* channel_ = object_->getAsChannel();
// 
//                 if (channel_)
//                 {
//                     UINT state = pNMCD->nmcd.uItemState;
//                     int  iIndex(0);
// 
//                     if ((state & 0x11) == 0x11 || (state & 0x10) == 0x10) // selected, drophilited
//                         iIndex = COLOR_HIGHLIGHTTEXT;
//                     else
//                         iIndex = COLOR_WINDOWTEXT;
// 
//                     if (channel_->isEnabled())
//                         pNMCD->clrText = GetSysColor( iIndex );
//                     else
//                         pNMCD->clrText = GetSysColor( COLOR_GRAYTEXT );
//                 }
//             }
        }
        break;
    }
}
