#pragma once

#include <AnchoredDialog/AnchoredDialog.h>
#include <Localized/Localized.h>

#include "../../algo/sensors/sensors.hpp"

#include "../listctrl/listsensors.h"

class CSensorsDlg :
    public CAnchoredDialog,
    public CLocalized
{
public:
    CSensorsDlg( CWnd* pParent = NULL );

    enum { IDD = IDD_SENSORS };

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    void InitList();
    virtual void OnOK();

    afx_msg void    OnAdd();
    afx_msg void    OnRemove();
    afx_msg void    OnSensorSelect( NMHDR* pNMHDR, LRESULT* pResult );

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    HICON        m_hIcon;
    CListSensors m_list;
    CButton      m_add;
    CButton      m_remove;

private:
    zetlab::sensors sensors_;
};
