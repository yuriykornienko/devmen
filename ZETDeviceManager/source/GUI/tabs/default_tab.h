#pragma once

#include <Localized/Localized.h>
#include <AnchoredDialog/AnchoredDialog.h>

class CTab :
    public CAnchoredDialog,
    public CLocalized
{
public:
    CTab( UINT nIDTemplate, CWnd* pParent = NULL );
    virtual ~CTab();

	virtual void Activate() = 0;
	virtual void Disactivate() = 0;

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    virtual void InitToolTips();
    virtual void OnOK() { GetParent()->GetParent()->SendMessage( WM_COMMAND, IDOK, 0 ); };
    virtual void OnCancel() { GetParent()->GetParent()->SendMessage( WM_COMMAND, IDCANCEL, 0 ); };
            void AddToolTip( CWnd* const pWnd, UINT nResID );
            void AddToolTip( CWnd* const pWnd, LPCTSTR lpszText );
            
    afx_msg void	OnSize( UINT nType, int cx, int cy );
    afx_msg HBRUSH  OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg LRESULT OnDoTab(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    CToolTipCtrl m_toolTips;
    unsigned int n_tooltips;
    CBrush       m_brush;
};
