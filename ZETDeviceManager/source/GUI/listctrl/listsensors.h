#pragma once
#ifndef ListSensors_h__
#define ListSensors_h__

#include <string>

// Define std::tstring
namespace std {
#if UNICODE
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
}

#include "../../algo/sensors/sensors.hpp"

#include <Uxtheme.h>
#include <CDPI/cdpi.h>
#include <map>
#include <vector>
#include <boost/assign/std/map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/lexical_cast.hpp>
//#include <mathZet.h>
#include "stdafx.h"
#include "resource\Phrases.h"

class CListSensors : public CListCtrl
{
public:
    CListSensors( bool b7Style_ = true ) :
        m_b7Style(b7Style_),
        m_canSmartWidth(true),
        pointEdit(-1, -1)
    {
    }

    virtual ~CListSensors()
    {
    }

    typedef zetlab::sensors::sensors_type adapter_type;

public:
    void SetAdapter( const adapter_type& adapter_ )
    {
        m_adapter.clear();
        m_adapter = adapter_;

        SetItemCountEx( m_adapter.size(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL );
        SmartWidth();
    }

    int InsertColumn( LPCTSTR lpszText, int nFormat = LVCFMT_LEFT )
    {
        int width = GetStringWidth( lpszText ) + m_dpiAware.ScaleX( 16 );

        LVCOLUMN lvc;

        lvc.mask    = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
        lvc.pszText = const_cast< LPTSTR >( lpszText );
        lvc.cx      = width;
		lvc.fmt = LVCFMT_CENTER;//HDF_STRING;
		
        int nColumn = CListCtrl::InsertColumn( 2147483647, &lvc );

        if (-1 != nColumn)
            // Insert to map
            boost::assign::insert( m_neededWiths )( nColumn, width );

        return nColumn;
    }

    void SetItemsHeight( int height )
    {
        // Destroy image list
        m_fakeImageList.DeleteImageList();

        // Initialize images
        m_fakeImageList.Create( 1, m_dpiAware.ScaleY( height ), ILC_COLOR4, 1, 1 );

        // Associate image list with control
        SetImageList( &m_fakeImageList, LVSIL_SMALL );
    }

    void SetColumnWidth( int nCol, int width )
    {
        HDITEM hdi;

        hdi.mask= HDI_WIDTH;
        hdi.cxy = width;

        // Set item width
        GetHeaderCtrl()->SetItem( nCol, &hdi );
    }

    BOOL SetContextMenu( UINT nID )
    {
        return m_contextMenu.LoadMenu( nID );
    }

protected:
    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* GetMessageMap() const
    {
        return GetThisMessageMap();
    }

    static const AFX_MSGMAP* PASCAL GetThisMessageMap()
    {
        typedef CListSensors ThisClass;
        typedef CListCtrl TheBaseClass;
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            // Message map
            ON_WM_SIZE()
            ON_NOTIFY( HDN_BEGINTRACK, 0, OnBeginTrack )
            ON_NOTIFY( HDN_ENDTRACK, 0, OnEndTrack )
            ON_NOTIFY_REFLECT( LVN_GETDISPINFO, OnGetDispInfo )
            ON_EN_KILLFOCUS( 1, OnEditLostFocus )
            ON_CBN_KILLFOCUS( 1, OnComboKillFocus )
//             ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnListCtrlCustomDraw )
            ON_NOTIFY_REFLECT( NM_DBLCLK, OnLDoubleClick )
            ON_NOTIFY_REFLECT(NM_CLICK, OnLClick )
//             ON_NOTIFY_REFLECT( NM_RCLICK, OnListCtrlRightClick )

            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
        };

        static const AFX_MSGMAP messageMap = {
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0]
        };

        return &messageMap;
    }
    PTM_WARNING_RESTORE

    //
    void OnSize( UINT nType, int cx, int cy )
    {
        // Default
        CListCtrl::OnSize( nType, cx, cy );

        if (m_canSmartWidth)
            SmartWidth();
    }

    void OnBeginTrack( NMHDR *pNMHDR, LRESULT *pResult )
    {
        *pResult = CDRF_DODEFAULT;

        m_canSmartWidth = false;
    }

    void OnEndTrack( NMHDR *pNMHDR, LRESULT *pResult )
    {
        *pResult = CDRF_DODEFAULT;

        m_canSmartWidth = true;
    }

    void OnGetDispInfo( NMHDR *pNMHDR, LRESULT *pResult )
    {
        NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>( pNMHDR );

        *pResult = CDRF_DODEFAULT;

        LVITEM* pItem = &(pDispInfo)->item; 

        if (pItem->mask & LVIF_TEXT)
        {
            _tcscpy_s( pItem->pszText, MAX_PATH, GetDispInfo( pItem->iItem, pItem->iSubItem ).c_str() );
        }
    }

    std::tstring GetDispInfo( int iItem, int iSubItem )
    {
        switch (iSubItem)
        {
        case 0:
            return m_adapter[iItem]->name;

        case 1:
            {
                CString tmp;
                tmp.Format( _T("%g "), m_adapter[iItem]->sense );

				if (!m_adapter[iItem]->unitSense.empty())
					tmp += m_adapter[iItem]->unitSense.c_str();
				else
					tmp += g_sUNIT_V;
				tmp += L"/";

				if (!m_adapter[iItem]->unit.empty())
					tmp += m_adapter[iItem]->unit.c_str();
				else
					tmp += g_sUNIT_MV;

                return std::tstring(tmp);
            }

        case 2:
			return (m_adapter[iItem]->icp == 2) ? 
                std::tstring(TranslateString(L"ICP").GetString()) : ((m_adapter[iItem]->icp == 1) ?
                    std::tstring(g_sPreamplifier) : std::tstring(g_sNo));

        case 3:
            {
                CString tmp;
                tmp.Format( _T("%g"), m_adapter[iItem]->amplify );
                return std::tstring(tmp);
            }

        case 4:
            {
                CString tmp;
                tmp.Format( _T("%g"), m_adapter[iItem]->reference );
                return std::tstring(tmp);
            }

        case 5:
            {
                CString tmp;
                tmp.Format( _T("%g"), m_adapter[iItem]->shift );
                return std::tstring(tmp);
            }

        case 6:
            return m_adapter[iItem]->afch;

		case 7:
			return (m_adapter[iItem]->hpf == 1) ? 
                std::tstring(g_sAc) : std::tstring(g_sDc);

		case 8:
			return (m_adapter[iItem]->charge == 1) ?
                std::tstring(g_sYes) : std::tstring(g_sNo);
        }

        return _T("");
    }

    void OnListCtrlCustomDraw( NMHDR *pNMHDR, LRESULT *pResult )
    {
        LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>( pNMHDR );

        *pResult = CDRF_DODEFAULT;

        if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
        {
            *pResult = CDRF_NOTIFYITEMDRAW;
        }
        else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
        {
            *pResult = CDRF_NOTIFYSUBITEMDRAW;
        }
        else if ((CDDS_SUBITEM | CDDS_ITEMPREPAINT) == pNMCD->nmcd.dwDrawStage)
        {
            *pResult = CDRF_DODEFAULT;
        }
    }

    void OnLDoubleClick( NMHDR* pNMHDR, LRESULT* pResult )
    {
        LPNMITEMACTIVATE pNMAC = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

        *pResult = CDRF_DODEFAULT;

        EndEdit();
        BeginEdit( pNMAC->iItem, pNMAC->iSubItem );
    }

	void OnLClick(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMITEMACTIVATE pNMAC = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

		if (comboForSense.m_hWnd && combo.m_hWnd)
			EndEdit();
	}

    void OnEditLostFocus()
    {
        EndEdit();
    }

    void OnComboKillFocus()
    {
        EndEdit();
    }

    void OnListCtrlRightClick( NMHDR *pNMHDR, LRESULT *pResult )
    {
        LPNMLISTVIEW pNMLV = reinterpret_cast< LPNMLISTVIEW >(pNMHDR);

        *pResult = CDRF_DODEFAULT;

        if (-1 != pNMLV->iItem && m_contextMenu.m_hMenu)
        {
            POINT ptAction = pNMLV->ptAction;
            ClientToScreen( &ptAction );

            m_contextMenu.GetSubMenu(0)->
                TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptAction.x, ptAction.y, GetParent() );
        }
    }

    void PreSubclassWindow()
    {
        // Full row select and tooltips by default
        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );

        if (m_b7Style)
            // Set 7even style
            SetWindowTheme( m_hWnd, L"explorer", 0 );

        // Default
        CListCtrl::PreSubclassWindow();
    }

    inline void CorrectNeededWidth( int nColumn, LPCTSTR lpszText )
    {
        int width = GetStringWidth( lpszText ) + m_dpiAware.ScaleX( 16 );

        if (m_neededWiths[nColumn] < width)
            m_neededWiths[nColumn] = width;
    }

    void SmartWidth( int width = -1 )
    {
        if (!m_hWnd )
            return;

        if (-1 == width)
        {
            CRect rc;

            // Get boundings
            GetClientRect( rc );

            // Width
            width = rc.Width();
        }

        // Needed width
        int neededWidth = 0;
        boost::range::for_each( m_neededWiths | boost::adaptors::map_values, (neededWidth += boost::lambda::_1) );

        double multi = double(width) / double(neededWidth);


		if (multi > 0.99) {
			ShowWindow(SW_HIDE);
			for (int i = 0, c = GetHeaderCtrl()->GetItemCount(); i < c; ++i)
			{
				SetColumnWidth(i, int(m_neededWiths[i] * multi));
			}
		} else {
			ShowWindow(SW_HIDE);
			for (int i = 0, c = GetHeaderCtrl()->GetItemCount(); i < c; ++i)
			{
				SetColumnWidth(i, int(m_neededWiths[i]));
			}
		}
        ShowWindow( SW_SHOW );
    }

    void BeginEdit( int iItem, int iSubItem )
    {
        if (iItem < 0)
            return;

        pointEdit.x = iSubItem;
        pointEdit.y = iItem;

        CWnd* pControl = NULL;

		CWnd* pControlSense1 = NULL;
		CWnd* pControlSense2 = NULL;

        CString tmp = L"";

        switch (iSubItem)
        {
        case 0:
        case 3:
        case 4:
        case 5:
        case 6:
            edit.CreateEx( WS_EX_CLIENTEDGE, WC_EDIT, _T(""), WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 1 );
            pControl = &edit;
            break;

		case 1:
			edit.CreateEx(WS_EX_CLIENTEDGE, WC_EDIT, _T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1);
			pControl = &edit;


			comboForSense.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, CRect(0, 0, 0, 0), this, 3);
			InitUnitsSense(&comboForSense);
			pControlSense1 = &comboForSense;

			combo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, CRect(0, 0, 0, 0), this, 2);
			InitUnits(&combo);
			pControlSense2 = &combo;

			break;

        case 2:
            combo.Create( WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, CRect( 0, 0, 0, 0 ), this, 1 );
            InitAmplifiers( &combo );
            combo.SetCurSel( m_adapter[iItem]->icp );
            pControl = &combo;
            break;

		case 7:
			combo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, CRect(0, 0, 0, 0), this, 1);
			InitHpfs(&combo);
			combo.SetCurSel(m_adapter[iItem]->hpf);
			pControl = &combo;
			break;

		case 8:
			combo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, CRect(0, 0, 0, 0), this, 1);
			InitCharges(&combo);
			combo.SetCurSel(m_adapter[iItem]->charge);
			pControl = &combo;
			break;
        }

        pControl->SetFont( GetFont() );

		if (pControlSense1 != NULL && pControlSense2 != NULL)
		{
			CString tmp(L"");
			tmp.Format(_T("%g "), m_adapter[iItem]->sense);
			pControl->SetWindowText(tmp);

			pControlSense1->SetFont(GetFont());
			tmp = m_adapter[iItem]->unitSense.c_str();
			if (tmp.IsEmpty())
				tmp = g_sUNIT_V;
			pControlSense1->SetWindowText(tmp);

			pControlSense2->SetFont(GetFont());
			pControlSense2->SetWindowText(m_adapter[iItem]->unit.c_str());

			MoveControl(iItem, iSubItem, pControl,		 0);
			MoveControl(iItem, iSubItem, pControlSense1, 1);
			MoveControl(iItem, iSubItem, pControlSense2, 2);
		}
		else
		{
			pControl->SetWindowText(GetDispInfo(iItem, iSubItem).c_str());
			pControl->SetFocus();
			MoveControl(iItem, iSubItem, pControl);
		}
    }

    void EndEdit()
    {
        if (-1 != pointEdit.x && -1 != pointEdit.y)
        {
            CString cstrText(L"");
            CString cstrTextCombo(L"");
            CString cstrTextComboForSense(L"");


            int nSel(CB_ERR);
            

			if (comboForSense.m_hWnd)
			{
				edit.GetWindowText(cstrText);
				combo.GetWindowText(cstrTextCombo);
				comboForSense.GetWindowText(cstrTextComboForSense);

				// Destroy
				edit.DestroyWindow();
				combo.DestroyWindow();
				comboForSense.DestroyWindow();
			}


            if (edit.m_hWnd)
            {
                edit.GetWindowText( cstrText );

                // Destroy
                edit.DestroyWindow();
            }
			else if (combo.m_hWnd)
            {
                combo.GetWindowText( cstrText );
                nSel = combo.GetCurSel();

                // Destroy
                combo.DestroyWindow();
            }


            try
            {
                switch (pointEdit.x)
                {
                case 0:
                    m_adapter[pointEdit.y]->name = std::tstring(cstrText);
                    break;

                case 1:
					m_adapter[pointEdit.y]->unit = std::tstring(cstrTextCombo);
					m_adapter[pointEdit.y]->unitSense = std::tstring(cstrTextComboForSense);
                    m_adapter[pointEdit.y]->sense = boost::lexical_cast< double >( (LPCTSTR)cstrText );
                    break;

                case 2:
                    m_adapter[pointEdit.y]->icp = long(nSel);
                    break;

                case 3:
                    m_adapter[pointEdit.y]->amplify = boost::lexical_cast< double >( (LPCTSTR)cstrText );
                    break;

                case 4:
                    m_adapter[pointEdit.y]->reference = boost::lexical_cast< double >( (LPCTSTR)cstrText );
                    break;

                case 5:
                    m_adapter[pointEdit.y]->shift = boost::lexical_cast< double >( (LPCTSTR)cstrText );
                    break;

                case 6:
                    m_adapter[pointEdit.y]->afch = std::tstring(cstrText);
                    break;

				case 7:
					m_adapter[pointEdit.y]->hpf = uint16_t(nSel);
					break;

				case 8:
					m_adapter[pointEdit.y]->charge = uint16_t(nSel);
                }
            }
            catch (boost::bad_lexical_cast&) {}
        }

        pointEdit.x = pointEdit.y = -1;
    }

    void MoveControl( int nItem, int iSubItem, CWnd* pControl, int multipleItem = -1)
    {
        if (!pControl) return;

        CRect rc;

        // Get item bounding
        GetSubItemRect( nItem, iSubItem, LVIR_BOUNDS, rc );

        if (0 == iSubItem)
        {
            CRect rc2;

            GetSubItemRect( nItem, iSubItem + 1, LVIR_BOUNDS, rc2 );

            rc.right = rc2.left;
        }

        // Reduce rectangle
        rc.DeflateRect( m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 1 ), m_dpiAware.ScaleX( 1 ), m_dpiAware.ScaleY( 1 ) );

		int Width(rc.Width());

		switch (multipleItem)
		{
		case 0:
			rc.right = rc.left + Width /3;
			break;
		case 1:
			rc.left = rc.left + Width / 3;
			rc.right = rc.left + Width / 3;
			break;
		case 2:
			rc.left = (Width / 3) * 2 + rc.left;
			break;
		default:
			break;
		}

        // Move control
        pControl->SetWindowPos( &wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER );
        pControl->RedrawWindow();
    }

    void InitUnits( CComboBox* combo )
    {
        combo->ResetContent();

		combo->AddString(g_sUNIT_G);
		combo->AddString(g_sUNIT_MV);
		combo->AddString(g_sUNIT_V);
		combo->AddString(g_sUNIT_MA);
		combo->AddString(g_sUNIT_A);
		combo->AddString(g_sUNIT_H);
		combo->AddString(g_sUNIT_KH);
		combo->AddString(g_sUNIT_PA);
		combo->AddString(g_sUNIT_KPA);
		combo->AddString(g_sUNIT_MPA);
		combo->AddString(g_sUNIT_MM);
		combo->AddString(g_sUNIT_MMPERS);
		combo->AddString(g_sUNIT_MPERS);
		combo->AddString(g_sUNIT_MPERS2);
		combo->AddString(g_sUNIT_ATM);
		combo->AddString(g_sUNIT_CEL);
    }

	void InitUnitsSense(CComboBox* combo)
	{
		comboForSense.AddString(g_sUNIT_MKV);
		comboForSense.AddString(g_sUNIT_MV);
		comboForSense.AddString(g_sUNIT_V);
		comboForSense.AddString(g_sUNIT_KV);
	}

    void InitAmplifiers( CComboBox* combo )
    {
        CString csRes;

		if (csRes = g_sNo) combo->AddString(csRes);
		if (csRes = g_sPreamplifier) combo->AddString(csRes);
		if (csRes = g_sYes) combo->AddString(csRes);
    }

	void InitHpfs(CComboBox* combo)
	{
		CString csRes;

		if (csRes = g_sNo) combo->AddString(csRes);
		if (csRes = g_sYes) combo->AddString(csRes);
	}

	void InitCharges(CComboBox* combo)
	{
		CString csRes;

		if (csRes = g_sNo) combo->AddString(csRes);
		if (csRes = g_sYes) combo->AddString(csRes);
	}

protected:
    // Extended Windows 7even style
    bool m_b7Style;

    //
    bool m_canSmartWidth;

    // Context menu
    CMenu m_contextMenu;

    // Used for SetItemHeight
    CImageList m_fakeImageList;

    // Provides DPI aware
    CDPI m_dpiAware;

    //
    CEdit edit;

    //
    CComboBox combo;

	//
	CComboBox comboForSense;
    //
    CPoint pointEdit;

    //
    std::map< int, int > m_neededWiths;

    //
    adapter_type m_adapter;
};

#endif // ListSensors_h__
