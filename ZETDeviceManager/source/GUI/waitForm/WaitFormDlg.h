#pragma once

#include <AnchoredDialog/AnchoredDialog.h>
#include <Localized/Localized.h>

class CWaitFormDlg :
    public CAnchoredDialog,
    public CLocalized
{
public:
    CWaitFormDlg( CWnd* pParent = NULL );

    enum { IDD = IDD_WAITFORM };

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual void OnDestroy();

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

private:
    CWnd* parent;

};


