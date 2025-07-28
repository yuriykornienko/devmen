#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

#include "check_groupbox/check_groupbox.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <vector>

class CDigPortTab : public CTab
{
public:
    CDigPortTab( CWnd* pParent = NULL );
    virtual ~CDigPortTab();
    enum { IDD = IDD_DIGPORT_TAB };
public:
    void SetAdapter( zetlab::std::device* const device__ );

	void Activate();
	void Disactivate();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    void InitChecks( CCheckGroupBox* group, CButton* checks, int count, int flag, int l_, int t_, int r_, int b_ );
    void ReadPortsStatus( const boost::function< long(::std::vector< bool >&) >& f,
                          CCheckGroupBox* group,
                          CButton* checks );

    afx_msg void    OnTimer( UINT_PTR nIDEvent );
    afx_msg void    OnInputsChecked();
    afx_msg void    OnOutputsChecked();
    afx_msg void    OnOutputsEnableChecked();
    afx_msg void	OnChecked( UINT nID );
    afx_msg void    UpdateGroupCheck( CCheckGroupBox* group, CButton* check );
    afx_msg void    OnCancel();
    afx_msg void    OnUserCancel();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    int            m_num_ports;
    CCheckGroupBox m_groupInputs;
    CButton        m_checkInput[14];
    CCheckGroupBox m_groupOutputs;
    CButton        m_checkOutput[14];
    CCheckGroupBox m_groupOutputsEnable;
    CButton        m_checkOutputEnable[14];

private:
    zetlab::std::device*            device_;
    zetlab::std::device_memento_ptr memento_;
};
