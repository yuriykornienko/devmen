#pragma once

#include "../default_tab.h"

#include "../../../algo/7xxx/7xxx/device/config/config_7xxx.hpp"

#include "table/table.h"
#include "grid/grid.h"
#include "grid_holder/grid_holder.h"

class CCalibrationTab : public CTab
{
public:
    CCalibrationTab( CWnd* pParent = NULL );
    virtual ~CCalibrationTab();
    enum { IDD = IDD_CALIBRATION_TAB };

public:
    void SetAdapter( const zetlab::_7xxx::device::config::page_ptr& page_, const bool& editable );
	
	void Activate(){}
	void Disactivate(){}

    void UpdateGrid();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();

    afx_msg LRESULT OnUserModify( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnUserSetItem( WPARAM wParam, LPARAM lParam );
    afx_msg void OnUserRestore();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    CString     m_desc;
    CFont       m_descFont;
    CTable      m_table;
    CGridHolder m_gridHolder;
    CGrid       m_grid;

    int         m_item;

private:
    zetlab::_7xxx::device::config::page_ptr page_holder;
};
