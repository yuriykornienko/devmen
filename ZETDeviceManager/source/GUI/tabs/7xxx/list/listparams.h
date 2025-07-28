#pragma once

#include "../../../listctrl/listctrlex.h"

#include "../../../../algo/7xxx/7xxx/device/config/config_7xxx.hpp"

#include <boost/shared_ptr.hpp>

#include <vector>

class CParamsList : public CListCtrlEx
{
public:
    typedef zetlab::_7xxx::device::config::page::param param;
    CParamsList() {};
    virtual ~CParamsList() {};

public:
    int InsertParam( param* param_, const bool& editable );
    bool UpdateAllParams();
    void RestoreAllParams();

protected:
    void PreSubclassWindow();

protected:
    afx_msg void OnEditChange( UINT nID );
    afx_msg void OnComboSelChange( UINT nID );
    afx_msg void OnItemSelect( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnScroll( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );

	void      CreateEditBox(BOOL bEditable, BOOL bPassword = FALSE);
	CComboBox*	  CreateComboBox(BOOL bEditable);
	void	  CreateIpAddrCtrl(BOOL bEditable);
	void       MoveControl(int nItem, int iSubItem, CWnd* pControl);
	bool       UpdateParam(int nItem, param* param_, BOOL post_message = TRUE);

    DECLARE_MESSAGE_MAP()

protected:
    CBrush m_brush;

private:
    typedef ::std::vector< boost::shared_ptr< CWnd > > controls_type;
    controls_type controls;
	bool CheckParameterValue(param* param_, UINT nID);
	void CorrectParameterValue(param* param_, CString& value_);
};
