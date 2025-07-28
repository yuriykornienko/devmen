#include "stdafx.h"
#include "../../../../application/application.h"
#include "../../../user_messages.hpp"
#include "table.h"

#include "../../../../types.hpp"
#include "../calibration_tab.h"

#include <boost/lexical_cast.hpp>
#include "resource\Phrases.h"
#include <vector>

BEGIN_MESSAGE_MAP( CTable, CListCtrlEx )
	ON_CONTROL_RANGE ( EN_CHANGE,        1, 20, OnEditChange )
	ON_NOTIFY        ( HDN_BEGINTRACK, 0,OnBeginTrack )
	ON_NOTIFY_REFLECT( LVN_ITEMCHANGING, OnItemSelect )
	ON_NOTIFY_REFLECT( LVN_ENDSCROLL,    OnScroll )
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CTable::PreSubclassWindow()
{
	SetExtendedStyle( GetExtendedStyle() | LVS_EX_GRIDLINES );

	InsertColumn(g_sPoint, LVCFMT_CENTER);
	InsertColumn(g_sMeasuredValue, LVCFMT_CENTER);
	InsertColumn(g_sReferenceValue, LVCFMT_CENTER);

	SetItemHeight( 20 );

	m_brush.Detach();
	m_brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
}

int CTable::InsertParam( zetlab::_7xxx::device::config::page::param* param_, const bool& editable )
{
	if (t_calibration != param_->getType())
		return -1;

	::std::vector< zetlab::tstring > extra;
	param_->getValue( extra );

	int iItem = InsertItem( -1, -1, param_->getName().c_str(), (LPCTSTR)0 );

	if (-1 != iItem)
	{
		SetItemData( iItem, reinterpret_cast< DWORD_PTR >(param_) );

		CEdit* edit = CreateEditBox(editable ? param_->isEditable() : editable);
		edit->SetWindowText( extra[0].c_str() );

		edit = CreateEditBox( editable ? param_->isEditable() : editable );
		edit->SetWindowText( extra[1].c_str() );
	}

	return iItem;
}

bool CTable::UpdateParam( int nItem, param* param_, BOOL post_message )
{
	bool modified = param_->isModified();

	if (post_message)
	{
		bool total_modified = false;
		for (int nItem = 0; nItem < GetItemCount(); nItem++)
		{
			param* param_ = reinterpret_cast< param* >(GetItemData(nItem));
			if (param_->isModified())
			{
				total_modified = true;
				break;
			}
		}
		GetParent()->PostMessage( WM_USER_MODIFY, 0, total_modified );
	}

	return modified;
}

bool CTable::UpdateAllParams()
{
	typedef zetlab::_7xxx::device::config::page::param param;

	bool modified = false;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		param* param_ = reinterpret_cast< param* >(GetItemData(nItem));

		if (UpdateParam( nItem, param_, FALSE ))
			modified = true;

		::std::vector< zetlab::tstring > extra;
		param_->getValue( extra );

		controls[nItem * 2 + 0]->SetWindowText( extra[0].c_str() );
		controls[nItem * 2 + 1]->SetWindowText( extra[1].c_str() );
	}

	reinterpret_cast< CCalibrationTab* >(GetParent())->UpdateGrid();
	GetParent()->PostMessage( WM_USER_MODIFY, 0, modified );

	return modified;
}

void CTable::RestoreAllParams()
{
	typedef zetlab::_7xxx::device::config::page::param param;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		param* param_ = reinterpret_cast< param* >(GetItemData(nItem));

		if (nullptr == param_)
			continue;

		param_->restoreDefaultValue();
	}

	UpdateAllParams();
}


CEdit* CTable::CreateEditBox( BOOL bEditable )
{	
	CEdit* edit = new CEdit;

	edit->Create( ES_CENTER | WS_VISIBLE | WS_CHILD,
				  CRect( 0, 0, 0, 0 ), this, controls.size() + 1 );
	edit->SetFont( GetFont() );
	edit->SetReadOnly( !bEditable );

	controls.push_back( boost::shared_ptr< CWnd >(edit) );

	return edit;
}

void CTable::MoveControl( int nItem, int iSubItem, CWnd* pControl )
{
	if (!pControl) return;

	CRect rc;

	GetSubItemRect( nItem, iSubItem, LVIR_BOUNDS, rc );

	if (0 == iSubItem)
	{
		CRect rc2;

		GetSubItemRect( nItem, iSubItem + 1, LVIR_BOUNDS, rc2 );
		rc.right = rc2.left;
	}

	rc.DeflateRect( m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 3 ),
					m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 2 ) );

	pControl->SetWindowPos( &wndTop, rc.left, rc.top,
							rc.Width() - m_dpiAware.ScaleX( 5 ), rc.Height(),
							(nItem < GetTopIndex()) ?
							SWP_HIDEWINDOW : SWP_SHOWWINDOW |
							SWP_NOACTIVATE | SWP_NOZORDER );
	pControl->RedrawWindow();
}

void CTable::OnEditChange( UINT nID )
{
	if (!IsWindowVisible())
		return;

	int nItem = ((nID % 2) ? (nID + 1) : nID) / 2 - 1;
	if (nItem < 0)
		nItem = 0;
	
	param* param_  = reinterpret_cast< param* >(GetItemData(nItem));

	if (nullptr == param_)
		return;

	if (!param_->isEditable())
		return;

	CString valueX, valueY;

	if (nID % 2)
	{
		controls[nID - 1]->GetWindowText( valueX );
		controls[nID - 0]->GetWindowText( valueY );
	}
	else
	{
		controls[nID - 2]->GetWindowText( valueX );
		controls[nID - 1]->GetWindowText( valueY );
	}

	try
	{
		union
		{
			double coded;
			struct
			{
				float valueX;
				float valueY;
			};
		} ch;

		ch.valueX = boost::lexical_cast< float >( (LPCTSTR)valueX );
		ch.valueY = boost::lexical_cast< float >( (LPCTSTR)valueY );

		param_->setValue( boost::lexical_cast< zetlab::tstring >( ch.coded ) );

		reinterpret_cast< CCalibrationTab* >(GetParent())->UpdateGrid();

		UpdateParam( nItem, param_ );
	}
	catch (boost::bad_lexical_cast&)
	{
	}
}

void CTable::OnBeginTrack( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 1;
}

void CTable::OnItemSelect( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMLISTVIEW pNMLV(reinterpret_cast< LPNMLISTVIEW >(pNMHDR));

	*pResult = CDRF_DODEFAULT;

	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
		*pResult = 1;
}

void CTable::OnScroll( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = CDRF_DODEFAULT;

	for (int i = 0, j = 0, c = GetItemCount(); i < c && j < c * 2; ++i, ++j)
	{
		MoveControl( i, 1, controls[j].get() );
		MoveControl( i, 2, controls[++j].get() );
	}
}

void CTable::OnSize( UINT nType, int cx, int cy )
{
	CListCtrlEx::OnSize( nType, cx, cy );

	if (!IsWindowVisible())
		return;

	for (int i = 0, j = 0, c = GetItemCount(); i < c && j < c * 2; ++i, ++j)
	{
		MoveControl( i, 1, controls[j].get() );
		MoveControl( i, 2, controls[++j].get() );
	}
}

HBRUSH CTable::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return m_brush;
}
