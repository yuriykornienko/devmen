#include "stdafx.h"
#include "../../../../application/application.h"
#include "../../../../types.hpp"
#include "../../../user_messages.hpp"
#include "listparams.h"

#include <boost/lexical_cast.hpp>

#include <vector>
#include "resource\Phrases.h"

BEGIN_MESSAGE_MAP( CParamsList, CListCtrlEx )
	ON_CONTROL_RANGE ( EN_CHANGE,        1, 100, OnEditChange )
	ON_CONTROL_RANGE ( CBN_SELCHANGE,    1, 100, OnComboSelChange )
	ON_NOTIFY_REFLECT( LVN_ITEMCHANGING, OnItemSelect )
	ON_NOTIFY_REFLECT( LVN_ENDSCROLL,    OnScroll )
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CParamsList::PreSubclassWindow()
{
	InsertColumn( _T(" "), LVCFMT_LEFT );
	InsertColumn( _T("                                             "), LVCFMT_LEFT );

	SetItemHeight( 24 );

	m_brush.Detach();
	m_brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
}

int CParamsList::InsertParam( zetlab::_7xxx::device::config::page::param* param_, const bool& editable )
{
	::std::vector< zetlab::tstring > extra;
	zetlab::tstring value = param_->getValue( extra );

	int iItem = InsertItem( -1, -1, (param_->getName() + _T(":")).c_str(), (LPCTSTR)0 );

	if (-1 != iItem)
	{
		SetItemData( iItem, reinterpret_cast< DWORD_PTR >(param_) );

		switch (param_->getType())
		{
		case t_char:
		case t_short:
		case t_long:
		case t_unshort:
		case t_unlong:
		case t_float:
		case t_longhex:
		case t_longlong:
		case t_longlonghex:
		case t_mac:
		case t_type_sensor:
		case t_calibration:
		case t_bin_2:
		case t_bin_4:
		case t_bin_8:
		case t_bin_16:
		case t_bin_32:
		case t_password_hash:
			CreateEditBox( editable ? param_->isEditable() : editable, param_->getType() == t_password_hash);
			break;
		case t_time:
			// Запрещаем изменять в формате t_time, так как не работает конвертирование строки во время
			// Тем более что на данный момент в модулях нет изменяемых полей с таким типом
			CreateEditBox(false);
			break;

		case t_list_float:
		case t_list_long:
		case t_list_char:
		case t_list_char_index:
			if (CComboBox* combo = CreateComboBox(editable ? param_->isEditable() : editable))
			{
				for (size_t i = 0; i < extra.size(); i++)
					combo->AddString( extra[i].c_str() );
			}
			break;

		case t_ip:
			CreateIpAddrCtrl( editable ? param_->isEditable() : editable );
			break;
		}
	}

	return iItem;
}

bool CParamsList::UpdateParam( int nItem, param* param_, BOOL post_message )
{
	bool modified = param_->isModified();

	CString text;
	if (modified)
		text.Format(_T("%s:*"), param_->getName().c_str());
	else
		text.Format(_T("%s:"), param_->getName().c_str());
	SetItem( nItem, 0, LVIF_TEXT, text.LockBuffer(), 0, 0, 0, 0 );
	text.UnlockBuffer();

	if (post_message)
	{
		bool total_modified = false;
		for (int nItem = 0; nItem < GetItemCount(); nItem++)
		{
			param* param_= reinterpret_cast< param* >(GetItemData(nItem));
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

bool CParamsList::UpdateAllParams()
{
	bool modified = false;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		param* param_ = reinterpret_cast< param* >(GetItemData(nItem));

		if (UpdateParam( nItem, param_, FALSE ))
			modified = true;

		::std::vector< zetlab::tstring > extra;
		zetlab::tstring value = TranslateString( param_->getValue( extra ).c_str() );
		CWnd* control = controls[nItem].get();

		switch (param_->getType())
		{
		case t_char:
		case t_short:
		case t_long:
		case t_unshort:
		case t_unlong:
		case t_float:
		case t_time:
		case t_longhex:
		case t_longlong:
		case t_longlonghex:
		case t_mac:
		case t_type_sensor:
		case t_calibration:
		case t_bin_2:
		case t_bin_4:
		case t_bin_8:
		case t_bin_16:
		case t_bin_32:
		case t_password_hash:
			if (CEdit* edit = reinterpret_cast< CEdit* >(control))
				edit->SetWindowText( value.c_str() );
			break;

		case t_list_float:
		case t_list_long:
		case t_list_char:
		case t_list_char_index:
			if (CComboBox* combo = reinterpret_cast<CComboBox*>(control))
			{
				for (size_t i = 0; i < extra.size(); i++)
				{
					if (value == extra[i])
					{
						combo->SetCurSel( i );
						break;
					}
				}
			}
			break;

		case t_ip:
			if (CIPAddressCtrl* ip = reinterpret_cast<CIPAddressCtrl*>(control))
			{
				ip->SetAddress( boost::lexical_cast< DWORD >( value ) );
			}
			break;
		}
	}

//    GetParent()->PostMessage( WM_USER_MODIFY, 0, modified );

	return modified;
}

void CParamsList::RestoreAllParams()
{
	typedef zetlab::_7xxx::device::config::page::param param;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		param* param_ = reinterpret_cast< param* >(GetItemData(nItem));
		param_->restoreDefaultValue();
	}

	UpdateAllParams();
}

void CParamsList::CreateEditBox(BOOL bEditable, BOOL bPassword)
{
	CEdit* edit = new CEdit;
	edit->CreateEx(bEditable ? WS_EX_CLIENTEDGE : 0, WC_EDIT, _T(""), 
		WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | (bPassword ? ES_PASSWORD : 0),
		CRect(0, 0, 0, 0), this, controls.size() + 1);
	edit->SetFont(GetFont());
	edit->SetReadOnly(!bEditable);

	controls.push_back(boost::shared_ptr< CWnd >(edit));
}

CComboBox* CParamsList::CreateComboBox(BOOL bEditable)
{
	CComboBox* combo = new CComboBox;

	combo->Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
		CRect(0, 0, 0, 0), this, controls.size() + 1);
	combo->SetFont(GetFont());
	combo->EnableWindow(bEditable);

	controls.push_back(boost::shared_ptr< CWnd >(combo));
	return combo;
}

void CParamsList::CreateIpAddrCtrl(BOOL bEditable)
{
	CIPAddressCtrl* ipaddr = new CIPAddressCtrl();
	ipaddr->Create(WS_VISIBLE | WS_CHILD,
		CRect(0, 0, 0, 0), this, controls.size() + 1);
	ipaddr->SetFont(GetFont());
	ipaddr->EnableWindow(bEditable);

	controls.push_back(boost::shared_ptr< CWnd >(ipaddr));
}

void CParamsList::MoveControl( int nItem, int iSubItem, CWnd* pControl )
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

	CEdit* edit = dynamic_cast< CEdit* >(pControl);

	if (edit && (edit->GetStyle() & ES_READONLY))
	{
		rc.DeflateRect( m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 6 ),
						m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 2 ) );
	}
	else
	{
		rc.DeflateRect( m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 2 ),
						m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 2 ) );
	}

	pControl->SetWindowPos( &wndTop, rc.left, rc.top,
							rc.Width() - m_dpiAware.ScaleX( 5 ), rc.Height(),
							(nItem < GetTopIndex()) ?
							SWP_HIDEWINDOW : SWP_SHOWWINDOW |
							SWP_NOACTIVATE | SWP_NOZORDER );
	pControl->RedrawWindow();

	// CIPAddressCtrl resize problem fix
	CIPAddressCtrl* ipaddr = dynamic_cast< CIPAddressCtrl* >(pControl);
	if (ipaddr)
		ipaddr->SetFont( GetFont() );
}

void CParamsList::OnEditChange( UINT nID )
{
	if (!IsWindowVisible())
		return;

	typedef zetlab::_7xxx::device::config::page::param param;
	
	param* param_= reinterpret_cast< param* >(GetItemData(nID - 1));

	if (nullptr == param_)
		return;

	if (!param_->isEditable())
		return;

	CString value;

	if (t_ip == param_->getType())
	{
		DWORD dwAddr;
		((CIPAddressCtrl*)controls[nID - 1].get())->GetAddress( dwAddr );
		value = boost::lexical_cast< zetlab::tstring >( dwAddr ).c_str();
	}
	else
	{
		controls[nID - 1]->GetWindowText( value );
		CorrectParameterValue(param_, value);
	}

	if (CheckParameterValue(param_, nID))
	{
		param_->setValue((LPCTSTR)value);

		UpdateParam(nID - 1, param_);

		if (t_bin_2 == param_->getType() ||
			t_bin_4 == param_->getType() ||
			t_bin_8 == param_->getType() || 
			t_bin_16 == param_->getType() || 
			t_bin_32 == param_->getType())
		{
			controls[nID - 1]->GetWindowText(value);
			std::vector<zetlab::tstring> tmp;
			CString value2 = param_->getValue(tmp).c_str();
			if (value != value2)
				controls[nID - 1]->SetWindowText(value2);
		}
	}
	else
	{
		std::vector<zetlab::tstring> tmp;

		controls[nID - 1]->SetWindowText(param_->getValue(tmp).c_str());
	}
}

void CParamsList::OnComboSelChange( UINT nID )
{
	if (!IsWindowVisible())
		return;

	typedef zetlab::_7xxx::device::config::page::param param;

	param* param_ = reinterpret_cast< param* >(GetItemData(nID - 1));

	if (nullptr == param_)
		return;

	if (!param_->isEditable())
		return;

	CComboBox* combo = static_cast< CComboBox* >(controls[nID - 1].get());
	
	if (t_list_char_index == param_->getType())
	{
		param_->setValue( boost::lexical_cast< zetlab::tstring >( combo->GetCurSel() ));
	}
	else
	{
		CString value;
		auto curSel = combo->GetCurSel();
		if (curSel >= 0)
			combo->GetLBText(combo->GetCurSel(), value);
		else
			combo->GetWindowText(value);
		param_->setValue( (LPCTSTR)value );
	}

	UpdateParam( nID - 1, param_ );
}

void CParamsList::OnItemSelect( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMLISTVIEW pNMLV(reinterpret_cast< LPNMLISTVIEW >(pNMHDR));

	*pResult = CDRF_DODEFAULT;

	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
		*pResult = 1;
}

void CParamsList::OnScroll( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = CDRF_DODEFAULT;

	for (int i = 0, c = GetItemCount(); i < c; ++i)
	{
		MoveControl( i, 1, controls[i].get() );
	}
}

void CParamsList::OnSize( UINT nType, int cx, int cy )
{
	CListCtrlEx::OnSize( nType, cx, cy );

	if (!IsWindowVisible())
		return;

	for (int i = 0, c = GetItemCount(); i < c; ++i)
	{
		MoveControl( i, 1, controls[i].get() );
	}
}

HBRUSH CParamsList::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return m_brush;
}

bool CParamsList::CheckParameterValue(param* param_, UINT nID)
{
	bool retVal = true;

	if (param_->getStructType() == CHANNEL_PAR_STRUCT &&
		nID == 4)	// Если пытаемся приравнять имя канала пустой строке
	{
		CString value;
		controls[nID - 1]->GetWindowText(value);
		if (value.GetLength() < 1 || value.GetLength() > 32)
		{
			MessageBox(g_sParamWarning, g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);
			retVal = false;
		}	
		else if (value.Find(L';') != -1)
		{
			// проверка на ;
			MessageBox(g_sIncorrectSimbol + L".", g_sIncorrectData, MB_OKCANCEL | MB_ICONWARNING);
			retVal = false;
		}
	}

	return retVal;
}

void CParamsList::CorrectParameterValue(param* param_, CString& value_)
{
	int size_(0);
	auto type_(param_->getType());

	if (t_bin_2 == type_)
		size_ = 2;
	else if (t_bin_4 == type_)
		size_ = 4;
	else if (t_bin_8 == type_)
		size_ = 8;
	else if (t_bin_16 == type_)
		size_ = 16;
	else if (t_bin_32 == type_)
		size_ = 32;

	if (0 < size_)
	{
		CString value2;
		int nPos1(0), nPos2(0);
		while (1)
		{
			nPos1 = value_.Find(_T("0"), nPos1);
			nPos2 = value_.Find(_T("1"), nPos2);
			if (nPos1 != -1 || nPos2 != -1)
			{
				if (nPos1 != -1 && nPos2 != -1)
				{
					if (nPos1 < nPos2)
					{
						value2.Append(_T("0"));
						nPos2 = ++nPos1;
					}
					else
					{
						value2.Append(_T("1"));
						nPos1 = ++nPos2;
					}
				}
				else if (nPos1 != -1)
				{
					value2.Append(_T("0"));
					nPos2 = ++nPos1;
				}
				else if (nPos2 != -1)
				{
					value2.Append(_T("1"));
					nPos1 = ++nPos2;
				}
			}
			else
				break;
		}

		while (value2.GetLength() < size_)
			value2.Insert(0, _T("0"));

		value_ = value2.Left(size_);
	}
}

