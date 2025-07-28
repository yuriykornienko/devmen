#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "calibration_tab.h"

#include "../../../algo/utils/double_bubble_sort/double_bubble_sort.hpp"

#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>

CCalibrationTab::CCalibrationTab( CWnd* pParent ) 
	: CTab( CCalibrationTab::IDD, pParent )
	, m_item(0)
{
}

CCalibrationTab::~CCalibrationTab()
{
}

void CCalibrationTab::DoDataExchange( CDataExchange* pDX )
{
    CTab::DoDataExchange( pDX );

    DDX_Text   ( pDX, IDC_LABEL_DESC, m_desc );
    DDX_Control( pDX, IDC_LIST,       m_table );
    DDX_Control( pDX, IDC_GRID,       m_gridHolder );
}

BEGIN_MESSAGE_MAP( CCalibrationTab, CTab )
    ON_MESSAGE     ( WM_USER_MODIFY,  OnUserModify )
    ON_MESSAGE     ( WM_USER_SETITEM, OnUserSetItem )
    ON_MESSAGE_VOID( WM_USER_RESTORE, OnUserRestore )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( CCalibrationTab )
    ANCHOR_MAP_ENTRY( NULL, ANF_AUTOMATIC )
END_ANCHOR_MAP()

BOOL CCalibrationTab::OnInitDialog()
{
    CTab::OnInitDialog();

    m_gridHolder.SetAx( &m_grid );
    m_grid.Create     ( _T(""), WS_CHILD | WS_VISIBLE, CRect(), &m_gridHolder, 1000 );
    m_grid.SetClrFon  ( GetSysColor( COLOR_WINDOW ) );
    m_grid.SetClrGrd  ( RGB(   0, 138, 194 ) );
    m_grid.SetClrDig  ( RGB(   0, 138, 194 ) );
    m_grid.SetClrCrs  ( RGB( 215, 245, 255 ) );
    m_grid.SetClrGrf  ( RGB( 255,  55,   0 ) );
    m_grid.SetTypeAbs ( 1 | 2 | 4 | 8 );
    m_grid.SetTypeLine( 2 );
    m_grid.FormatX    ( _T("") );
    m_grid.FormatY    ( _T("") );

    LOGFONT lfont;

    GetFont()->GetLogFont( &lfont );
    lfont.lfWeight = FW_BOLD;

    m_descFont.CreateFontIndirect( &lfont );
    GetDlgItem( IDC_LABEL_DESC )->SetFont( &m_descFont );

    return TRUE;
}

void CCalibrationTab::SetAdapter( const zetlab::_7xxx::device::config::page_ptr& page_, const bool& editable )
{
    page_holder = page_;

    m_desc = page_->getDesc().c_str();
    for (auto it = page_->begin(), end = page_->end(); it != end; ++it)
    {
        m_table.InsertParam( (*it).get(), editable );
    }

    UpdateGrid();

    UpdateData( FALSE );
}

void CCalibrationTab::UpdateGrid()
{
    auto size(page_holder->size());
    int  i(0);

    boost::scoped_array< float > x(new float[size]);
    boost::scoped_array< float > y(new float[size]);

    for (auto it = page_holder->begin(), end = page_holder->end(); it != end; ++it, ++i)
    {
        ::std::vector< zetlab::tstring > extra;
        
        (*it)->getValue( extra );

        try
        {
            x[i] = boost::lexical_cast< float >( extra[0] );
            y[i] = boost::lexical_cast< float >( extra[1] );
        }
        catch (boost::bad_lexical_cast&)
        {
            x[i] = 0.0;
            y[i] = 0.0;
        }
    }

    float ly(*std::min_element       ( y.get(), y.get() + size ));
    float dy(*std::max_element       ( y.get(), y.get() + size ) - ly);
    zetlab::utils::double_bubble_sort( x.get(), y.get(),  size );

    m_grid.SetSize  ( size );
    m_grid.AltC     ( x.get() );
    m_grid.Paint    ( y.get() );
    m_grid.SetMathly( ly - dy * 0.1 );
    m_grid.SetMathdy( dy + dy * 0.2 );
}

LRESULT CCalibrationTab::OnUserModify( WPARAM wParam, LPARAM lParam )
{
    GetParent()->SendMessage( WM_USER_MODIFY, m_item, lParam );
    return 0;
}

LRESULT CCalibrationTab::OnUserSetItem( WPARAM wParam, LPARAM lParam )
{
    m_item = static_cast< int >(wParam);
    return 0;
}

void CCalibrationTab::OnUserRestore()
{
    m_table.RestoreAllParams();
}