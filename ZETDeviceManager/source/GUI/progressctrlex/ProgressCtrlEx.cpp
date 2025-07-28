#include "stdafx.h"
#include "Afxglobals.h"
#include "ProgressCtrlEx.h"

IMPLEMENT_DYNAMIC( CProgressCtrlEx, CProgressCtrl )

CProgressCtrlEx::CProgressCtrlEx()
:pTaskbarList(nullptr)
{
}

CProgressCtrlEx::~CProgressCtrlEx()
{
}


BEGIN_MESSAGE_MAP( CProgressCtrlEx, CProgressCtrl )
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CProgressCtrlEx::OnPaint()
{
	CProgressCtrl::OnPaint();

	CClientDC	dc( this );
	CRect		rc;
	CFont*		pOldFont(nullptr) ;

	// Устанавливаем НОРМАЛЬНЫЙ шрифт
	pOldFont = dc.SelectObject( GetParent()->GetFont() );
	GetClientRect( &rc );

	// Рисуем текст
	dc.SetBkMode( TRANSPARENT );
	dc.DrawText( showText, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	// Восстанавливаем предыдущий шрифт
	dc.SelectObject( pOldFont );
}

void CProgressCtrlEx::SetShowText( CString c_str )
{
	showText = c_str;
}

CString CProgressCtrlEx::GetShowText()
{
	return showText;
}

void CProgressCtrlEx::SetZeroRange( int range )
{
	SetRange( 0, range );
	SetPos( 0 );

	CWnd* pParent = GetParent();

	// Таскбар Win7
	pTaskbarList = afxGlobalData.GetITaskbarList3();
	if (NULL == pTaskbarList) return;
	pTaskbarList->SetProgressValue( pParent->GetSafeHwnd(), 0, range );
	pTaskbarList->SetProgressState( pParent->GetSafeHwnd(), TBPF_NOPROGRESS );
}

void CProgressCtrlEx::SetPosition( int pos )
{
	SetPos( pos );

	CWnd* pParent = GetParent();

	// Таскбар Win7
	pTaskbarList = afxGlobalData.GetITaskbarList3();
	if (NULL == pTaskbarList) return;
	int _min, _max; GetRange( _min, _max );
	pTaskbarList->SetProgressValue( pParent->GetSafeHwnd(), pos, _max );
	pTaskbarList->SetProgressState( pParent->GetSafeHwnd(), TBPF_NORMAL );
}

void CProgressCtrlEx::SetIndeterminate( BOOL bInf )
{
	CWnd* pParent = GetParent();

	// Таскбар Win7
	pTaskbarList = afxGlobalData.GetITaskbarList3();
	pTaskbarList->SetProgressState( pParent->GetSafeHwnd(), bInf ? TBPF_INDETERMINATE : TBPF_NOPROGRESS );
}

void CProgressCtrlEx::Pause()
{
	CWnd* pParent = GetParent();

	// Таскбар Win7
	pTaskbarList = afxGlobalData.GetITaskbarList3();
	pTaskbarList->SetProgressState( pParent->GetSafeHwnd(), TBPF_PAUSED );
}

void CProgressCtrlEx::Error()
{
	CWnd* pParent = GetParent();

	// Таскбар Win7
	pTaskbarList = afxGlobalData.GetITaskbarList3();
	pTaskbarList->SetProgressState( pParent->GetSafeHwnd(), TBPF_ERROR );
}
