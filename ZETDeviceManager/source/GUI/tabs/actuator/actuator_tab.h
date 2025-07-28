#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

#include "../dig_port/check_groupbox/check_groupbox.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <vector>

class CActuatorTab : public CTab
{
public:
	CActuatorTab(CWnd* pParent = NULL);
	virtual ~CActuatorTab();
    enum { IDD = IDD_ACTUATOR_TAB };
public:
    void SetAdapter( zetlab::std::device* const device__ );

	void Activate();
	void Disactivate();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    void InitChecks( CCheckGroupBox* group, CButton* checks, int count, int flag, int l_, int t_, int r_, int b_ );
    
    afx_msg void    OnActuatorsChecked();
    afx_msg void	OnChecked( UINT nID );
    afx_msg void    UpdateGroupCheck( CCheckGroupBox* group, CButton* check );
    afx_msg void    OnCancel();
    afx_msg void    OnUserCancel();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    int            m_num_actuators;
    CCheckGroupBox m_groupActuators;
    CButton        m_checkActuator[8];
    
private:
    zetlab::std::device*            device_;
    zetlab::std::device_memento_ptr memento_;
};
