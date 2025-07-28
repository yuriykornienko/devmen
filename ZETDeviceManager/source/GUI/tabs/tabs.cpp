#include "stdafx.h"

#include "../user_messages.hpp"

#include "tabs.h"

BEGIN_MESSAGE_MAP( CTabs, CTabCtrl )
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT( TCN_SELCHANGING, OnSelChangingTabs )
    ON_NOTIFY_REFLECT( TCN_SELCHANGE,   OnSelChangeTabs )
    ON_MESSAGE( WM_USER_MODIFY,         OnUserModify )
	ON_MESSAGE( WM_USER_REGISTRATOR,    OnUserRegistrator)
	
END_MESSAGE_MAP()

BOOL CTabs::DeleteItem( int nItem )
{
    if (CTabCtrl::DeleteItem( nItem ))
    {
        m_tabInfo.erase( nItem );
        return TRUE;
    }
    return FALSE;
}

void CTabs::OnSize( UINT nType, int cx, int cy )
{
    CTabCtrl::OnSize( nType, cx, cy );

    OnSelChangeTabs( NULL, NULL );
}

void CTabs::OnSelChangingTabs( NMHDR* pNMHDR, LRESULT* pResult )
{
    if (!IsWindow( this->m_hWnd )) return;

    // Hide current tab
    TCITEM      tci;
    CDialog*    tabView(nullptr);

    tci.mask = TCIF_PARAM;
    if (!GetItem( GetCurSel(), &tci )) return;

    tabView = reinterpret_cast< CDialog* >(tci.lParam);
    if (tabView && tabView->m_hWnd && IsWindow( tabView->m_hWnd ) && m_bCanChangeTab)
    {
        tabView->SetWindowPos( &wndTop,
                               0, 0, 0, 0,
                               SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE );
//        tabView->SendMessage( WM_USER_RESTORE );

		if (m_bCanChangeTab)
			tabView->SendMessage(WM_USER_DOTAB, 0, 0);
    }

	if (!m_bCanChangeTab)
		*pResult = 1;
}

void CTabs::OnSelChangeTabs( NMHDR* pNMHDR, LRESULT* pResult )
{
    if (!IsWindow( this->m_hWnd )) return;

    // Show new tab
    TCITEM      tci;
    int         nItem(0);
    CDialog*    tabView(nullptr);
    CRect       rc;

    GetWindowRect( &rc );
    ScreenToClient( &rc );
    AdjustRect( FALSE, &rc );

    nItem = GetCurSel();

    tci.mask = TCIF_PARAM;
    if (!GetItem( nItem, &tci )) return;

    tabView = reinterpret_cast< CDialog* >(tci.lParam);
    if (tabView && tabView->m_hWnd && IsWindow( tabView->m_hWnd ) && m_bCanChangeTab)
    {
        tabView->SetWindowPos( &wndTop,
                               rc.left, rc.top, rc.Width(), rc.Height(),
                               SWP_SHOWWINDOW );

		if (m_bCanChangeTab)
			tabView->SendMessage(WM_USER_DOTAB, 0, 1);
    }

	if (m_bCanChangeTab)
		SetModified(nItem, m_tabInfo[nItem].modified);
}

int CTabs::SetActiveTab( int nItem )
{
    OnSelChangingTabs( NULL, NULL );

	int nOld = SetCurSel(nItem);	

    OnSelChangeTabs( NULL, NULL );

    return nOld;
}

BOOL CTabs::SetTabName( int nItem, LPCTSTR lpszText )
{
    if (nItem >= GetItemCount())
        return FALSE;

    m_tabInfo[nItem].name = lpszText;

    CString modifiedName;
    if (m_tabInfo[nItem].modified)
        modifiedName.Format(_T("%s*"), lpszText);
    else
        modifiedName = lpszText;

    TCITEM tci;
    tci.mask    = TCIF_TEXT;
    tci.pszText = modifiedName.LockBuffer();
    BOOL result = SetItem( nItem, &tci );
    modifiedName.UnlockBuffer();

    return result;
}

BOOL CTabs::SetModified( int nItem, BOOL modified )
{
    if (nItem >= GetItemCount())
        return FALSE;

    TabInfo& info = m_tabInfo[nItem];

    if (nItem == GetCurSel())
		GetParent()->PostMessage(WM_USER_MODIFY, nItem, modified);

    if (info.modified == modified)
        return modified;

    info.modified = modified;
    SetTabName( nItem, m_tabInfo[nItem].name );

    RedrawWindow();

    TCITEM      tci;
    tci.mask = TCIF_PARAM;
    if (GetItem( GetCurSel(), &tci ))
    {
        CRect       rc;
        GetWindowRect( &rc );
        ScreenToClient( &rc );
        AdjustRect( FALSE, &rc );

		CDialog* tabView = reinterpret_cast< CDialog* >(tci.lParam);
        if (tabView && tabView->m_hWnd && IsWindow( tabView->m_hWnd ))
        {
            tabView->SetWindowPos( &wndTop,
                                   0, 0, 0, 0,
                                   SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE );
            tabView->SetWindowPos( &wndTop,
                                   rc.left, rc.top, rc.Width(), rc.Height(),
                                   SWP_SHOWWINDOW );
        }
    }

    return !modified;
}

LRESULT CTabs::OnUserModify( WPARAM wParam, LPARAM lParam )
{
    SetModified( wParam, lParam );
    return 0;
}

LRESULT CTabs::OnUserRegistrator( WPARAM wParam, LPARAM lParam )
{
	GetParent()->PostMessage( WM_USER_REGISTRATOR, wParam, lParam );
	m_bCanChangeTab = !static_cast< BOOL >(wParam);

	return 0;
}

int CTabs::GetIndexItemTab(UINT idTabUser)
{
	int index(-1);
	// перебираем map табов
	for (auto it = m_tabInfo.begin(); it != m_tabInfo.end(); ++it)
	{
		index++;
		if (it->second.idTabUser == idTabUser)
			return index;
	}

	return -1;
}
