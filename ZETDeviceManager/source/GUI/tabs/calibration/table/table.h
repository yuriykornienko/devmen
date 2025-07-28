#pragma once

#include "../../../listctrl/listctrlex.h"

#include "../../../../algo/7xxx/7xxx/device/config/config_7xxx.hpp"

#include <boost/shared_ptr.hpp>

#include <vector>

class CTable : public CListCtrlEx
{
public:
    typedef zetlab::_7xxx::device::config::page::param param;
    CTable() {};
    virtual ~CTable() {};

public:
    int InsertParam( zetlab::_7xxx::device::config::page::param* param_, const bool& editable );
    bool UpdateAllParams();
    void RestoreAllParams();

protected:
    void PreSubclassWindow();

protected:
    afx_msg void OnEditChange( UINT nID );
    void OnBeginTrack( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnItemSelect( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnScroll( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );

    CEdit*     CreateEditBox( BOOL bEditable );
    void       MoveControl( int nItem, int iSubItem, CWnd* pControl );
    bool       UpdateParam( int nItem, param* param_, BOOL post_message = TRUE );

    DECLARE_MESSAGE_MAP()

protected:
    CBrush m_brush;

private:
    typedef ::std::vector< boost::shared_ptr< CWnd > > controls_type;
    controls_type controls;
};
