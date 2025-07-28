#pragma once

#include <AnchoredDialog/AnchoredDialog.h>
#include <Localized/Localized.h>

class CSettingsDlg :
    public CAnchoredDialog,
    public CLocalized
{
public:
    CSettingsDlg( CWnd* pParent = NULL );

    enum { IDD = IDD_SETTINGS };

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()
};
