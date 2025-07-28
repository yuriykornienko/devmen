#pragma once

#include <Uxtheme.h>
#include <CDPI/cdpi.h>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/signals2.hpp>

#include <map>

#pragma comment(lib, "uxtheme.lib")

using namespace std;
using namespace boost;

class CListCtrlEx : public CListCtrl
{
public:
    CListCtrlEx( bool b7Style_ = true ) :
        m_7Style(b7Style_),
        m_canSmartWidth(true),
        m_contextMenu(nullptr),
        m_bkColor(GetSysColor( COLOR_WINDOW )),
		m_width(0),
		m_itlist(false),
		m_column(-1),
		m_bkColorcolumn(GetSysColor(COLOR_WINDOW))
    {
    };

    virtual ~CListCtrlEx()
    {
    };

public:
    int InsertColumn( LPCTSTR lpszText, int nFormat = LVCFMT_LEFT )
    {
        int width = GetStringWidth( lpszText ) + m_dpiAware.ScaleX( 16 );

        LVCOLUMN lvc;

        lvc.mask    = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
        lvc.pszText = const_cast< LPTSTR >( lpszText );
        lvc.cx      = width;
        lvc.fmt     = nFormat | HDF_STRING;

        int nColumn = CListCtrl::InsertColumn( 2147483647, &lvc );

        if (-1 != nColumn)
            m_neededWiths.insert( width_map_type::value_type(nColumn, width) );

        return nColumn;
    };

    int InsertColumn( UINT nIDText, int nFormat = LVCFMT_LEFT )
    {
        CString tmp;

        if (tmp.LoadString( nIDText ))
            return InsertColumn( tmp, nFormat );

        return -1;
    };

    int InsertItem( int iParent, int iImage, LPCTSTR lpszItem, ... )
    {
        CorrectNeededWidth( 0, lpszItem ); // корректировка ширины колонки

        LVITEM lvi;  //структура, используемая в CListCtrl для задания свойств элемента списка.

        if (-1 != iParent) //Если iParent не равен -1, значит, у элемента есть родитель, и нужно получить его отступ (indent).
        {
            // Get parent indentation
            lvi.iItem    = iParent;
            lvi.iSubItem = 0;
            lvi.mask     = LVIF_INDENT;

            GetItem( &lvi );
        }
        else   //Обработка случая без родителя (else)
        {
            iParent = 2147483636;
            lvi.iIndent = 0;
        }
        ///Настройка параметров нового элемента
        lvi.iItem      = iParent + 1;   ///позиция нового элемента (возможно, для вставки после родителя).
        lvi.iSubItem   = 0;  //основная колонка.
        lvi.pszText    = NULL;  //текст пока не задаётся.
        lvi.cchTextMax = 0;
        lvi.mask       = LVIF_IMAGE | LVIF_INDENT;  //указывает, что используются поля iImage и iIndent.
        lvi.iImage     = iImage;  // индекс иконки.
        lvi.iIndent    = lvi.iIndent + 1;  //увеличивается на 1 (отступ для дочернего элемента).

        int nItem(CListCtrl::InsertItem( &lvi )); //Вызывает стандартный InsertItem из CListCtrl, который добавляет элемент с заданными свойствами.

        //Возвращает индекс нового элемента(nItem).

        va_list v_arg;  //Обработка дополнительных подэлементов (колонок)
        LPCTSTR lpszText_;
        int     nSubItem(1);

        va_start( v_arg, lpszItem );

        SetItemText( nItem, 0, lpszItem ); //Устанавливает текст в основной колонке (0) нового элемента.
        for (;;nSubItem++)
        {
            lpszText_ = va_arg( v_arg, LPCTSTR );  //В цикле извлекаются строки из va_arg (пока не встретится NULL).

            if (!lpszText_) break;
			if (2147483651 != iParent && !CorrectNeededWidth(nSubItem, lpszText_)) break; //Если iParent не равен 2147483651 (магическое число?), проверяется ширина колонки через CorrectNeededWidth.

            SetItemText( nItem, nSubItem, lpszText_ ); //Для каждой колонки (nSubItem) устанавливается текст через SetItemText.
        }

        va_end( v_arg ); //Обязательное завершение работы с переменными аргументами.

        return nItem;  //Возвращает индекс только что добавленного элемента.
    };

	void CheckSize(long heightList = -1)
	{
		CRect rect;
		long height(0);

		for (int i = 0, c = GetItemCount() - 1; i < c; ++i)
		{
			GetItemRect(i, &rect, 0);
			height += rect.Height();
		}
		if (heightList == -1)
		{	
			GetWindowRect(rect);
			heightList = rect.Height();
		}

		if (heightList > height)
		{
			ScrollTop();
		}
	}

	void ScrollTop()
	{
		SCROLLINFO sinfo;
		sinfo.cbSize = sizeof(sinfo);
		sinfo.fMask = SIF_TRACKPOS | SIF_POS | SIF_RANGE | SIF_PAGE;
		::GetScrollInfo(m_hWnd, SB_VERT, &sinfo);
		if (sinfo.nPos != 0)
		{
			EnsureVisible(0, TRUE);
		}
	}
    BOOL DeleteItem( int iItem )
    {
        LVITEM lvi;	
		CheckSize();

        // Get this item indentation
        lvi.iItem = iItem;
        lvi.mask  = LVIF_INDENT;
        GetItem( &lvi );

        int iIndent = lvi.iIndent;
        int nCounter = iItem + 1;

        if (nCounter < GetItemCount())
        {
            do 
            {
                lvi.iItem = nCounter;
                GetItem( &lvi );

                if (lvi.iIndent > iIndent)
                    CListCtrl::DeleteItem( nCounter );
            }
            while (lvi.iIndent > iIndent && GetItemCount() > nCounter);
        }

        return CListCtrl::DeleteItem( iItem );
    };

    BOOL SetBackgroundColor( COLORREF cr )
    {
        m_bkColor = cr;
        return CListCtrl::SetBkColor( cr );
    };

	void SetBackgroundColorColumn(int Column, COLORREF cr)
	{
		m_column = Column;
		m_bkColorcolumn = cr;
	};

    void SetColumnWidth( int nCol, int width )
    {
        HDITEM hdi;

        hdi.mask= HDI_WIDTH;
        hdi.cxy = width;

        GetHeaderCtrl()->SetItem( nCol, &hdi );
    };

    BOOL DeleteAllColumns()
    {
        BOOL bResult = DeleteAllItems();

        for (int i = GetHeaderCtrl()->GetItemCount() - 1 ; i >= 0; --i)
        {
            bResult = bResult && DeleteColumn( i );
        }

        return bResult;
    };

    void SetItemHeight( int height )
    {
        m_imageList.Detach();
        m_imageList.Create( 1, m_dpiAware.ScaleY( height ), ILC_COLOR4, 1, 1 );
        SetImageList( &m_imageList, LVSIL_SMALL );
    };

    virtual void LoadItemImages()
    {
    };

    void SmartWidth( int width = -1 )
    {
        if (!m_hWnd )
            return;

        if (-1 == width)
        {
            CRect rc;

            GetClientRect( rc );
            width = rc.Width();
        }

	   // Needed width
		int neededWidth = 0;
       range::for_each( m_neededWiths | adaptors::map_values, [&]( int& w ) {	
            neededWidth += w;
        });

        double multi = double(width) / double(neededWidth);

        BOOL visible(IsWindowVisible());

        if (visible) ShowWindow( SW_HIDE );
        for (int i = 0, c = GetHeaderCtrl()->GetItemCount(); i < c; ++i)
        {
			SetColumnWidth(i, int(m_neededWiths[i] * multi));
        }
        if (visible) ShowWindow( SW_SHOW );
    };


	void SmartWidthForList(int width = -1)
	{
		if (!m_hWnd)
			return;

		if (-1 == width)
		{
			CRect rc;

			GetClientRect(rc);
			width = rc.Width();
		}

		// Needed width
		int neededWidth = m_neededWiths[0];
		range::for_each(m_neededWiths | adaptors::map_values, [&](int& w) {
			neededWidth += w;
		});

		double multi = double(width) / double(neededWidth);

		BOOL visible(IsWindowVisible());

		if (visible) ShowWindow(SW_HIDE);
		for (int i = 0, c = GetHeaderCtrl()->GetItemCount(); i < c; ++i)
		{
			if (i == 0)
			{
				//Remarks on MSDN: the column be the width of the column header plus 50 % . 
				SetColumnWidth(i, int(m_width * 2 * multi));
			}
			else
				SetColumnWidth(i, int(m_neededWiths[i] * multi));
		}
		if (visible) ShowWindow(SW_SHOW);
	};


protected:
    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* GetMessageMap() const
    {
        return GetThisMessageMap();
    };

    static const AFX_MSGMAP* PASCAL GetThisMessageMap()
    {
        typedef CListCtrlEx ThisClass;
        typedef CListCtrl TheBaseClass;
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            // Message map
            ON_WM_SIZE()
            ON_WM_CONTEXTMENU()
            ON_NOTIFY( HDN_BEGINTRACK, 0,     OnBeginTrack )
            ON_NOTIFY( HDN_ENDTRACK, 0,       OnEndTrack )
            ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnCustomDraw )

            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
        };

        static const AFX_MSGMAP messageMap = {
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0]
        };

        return &messageMap;
    };
    PTM_WARNING_RESTORE

    void OnSize( UINT nType, int cx, int cy )
    {
        CListCtrl::OnSize( nType, cx, cy );

		if (m_canSmartWidth)
		{
			if (!m_itlist)
				SmartWidth();
			else
				SmartWidthForList();
		}

    };	

    void OnBeginTrack( NMHDR *pNMHDR, LRESULT *pResult )
    {
        *pResult = CDRF_DODEFAULT;

        m_canSmartWidth = false;
    };

    void OnEndTrack( NMHDR *pNMHDR, LRESULT *pResult )
    {
        *pResult = CDRF_DODEFAULT;

        m_canSmartWidth = true;
    };

    void OnContextMenu( CWnd* pWnd, CPoint ptMousePos ) 
    {
        // if Shift-F10
        if (ptMousePos.x == -1 && ptMousePos.y == -1)
            ptMousePos = (CPoint)GetMessagePos();

        ScreenToClient( &ptMousePos );

        UINT uFlags(0);
        int  iItem;

        iItem = HitTest( ptMousePos, &uFlags );

        if (-1 == iItem)
            return;

        ClientToScreen( &ptMousePos );

        sig_prepare_cm( &m_contextMenu );
        if (!m_contextMenu || !m_contextMenu->m_hMenu)
            return;

        m_contextMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMousePos.x, ptMousePos.y, GetParent() );
    };

    void OnCustomDraw( NMHDR *pNMHDR, LRESULT *pResult )
    {
        LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>( pNMHDR );

        *pResult = CDRF_DODEFAULT;

		switch (pNMCD->nmcd.dwDrawStage)
		{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
			if (m_column != -1)
			{
				if (pNMCD->iSubItem == m_column)
					pNMCD->clrTextBk = m_bkColorcolumn;
			}
			else
				pNMCD->clrTextBk = m_bkColor;
			break;
		}
    };

    void PreSubclassWindow()
    {
        SetExtendedStyle( GetExtendedStyle() |
            LVS_EX_FULLROWSELECT |
            LVS_EX_INFOTIP |
            LVS_EX_DOUBLEBUFFER );

        #ifdef UNICODE
        if (m_7Style)
            SetWindowTheme( m_hWnd, L"explorer", 0 );
        #endif

        LoadItemImages();

        CListCtrl::PreSubclassWindow();
    };

    inline int CorrectNeededWidth( int nColumn, LPCTSTR lpszText )
    {
        int width = GetStringWidth( lpszText );
 
        if (!width || width > 1000)
            return 0;

        width += m_dpiAware.ScaleX( 16 );

        if (m_neededWiths[nColumn] < width)
            m_neededWiths[nColumn] = width;

		if (nColumn == 0)
		{
			if (width > m_width)
				m_width = width;
		}

        return width;
    };
protected:
    typedef std::map< int, int > width_map_type;

    bool                 m_7Style;
    CImageList           m_imageList;
    bool                 m_canSmartWidth;
    CMenu*               m_contextMenu;
    COLORREF             m_bkColor;
    COLORREF             m_bkColorcolumn;
    CDPI                 m_dpiAware;
    width_map_type       m_neededWiths;
	int					 m_width;
	int					 m_column;
	bool				 m_itlist;
public:
    void OnPrepareContextMenu( const boost::function< void(CMenu**) >& f )
        { sig_prepare_cm.connect( f ); }

	void setList(bool list) { m_itlist = list; }

private:
    boost::signals2::signal< void(CMenu**) > sig_prepare_cm;
};
