#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "7xxx_tab.h"

C7xxxTab::C7xxxTab(CCFGWorker* pCfgWorker, CWnd* pParent)
	: CTab( C7xxxTab::IDD, pParent )
	, m_item(0)
	, m_pCfgWorker(pCfgWorker)
{
}

C7xxxTab::~C7xxxTab()
{
}

void C7xxxTab::DoDataExchange( CDataExchange* pDX )
{
    CTab::DoDataExchange( pDX );

    DDX_Text   ( pDX, IDC_LABEL_DESC, m_desc );
    DDX_Control( pDX, IDC_LIST,       m_list );
}

BEGIN_MESSAGE_MAP( C7xxxTab, CTab )
    ON_MESSAGE     ( WM_USER_MODIFY,  OnUserModify )
    ON_MESSAGE     ( WM_USER_SETITEM, OnUserSetItem )
    ON_MESSAGE_VOID( WM_USER_RESTORE, OnUserRestore )
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP( C7xxxTab )
    ANCHOR_MAP_ENTRY( NULL, ANF_AUTOMATIC )
END_ANCHOR_MAP()

BOOL C7xxxTab::OnInitDialog()
{
    CTab::OnInitDialog();

    LOGFONT lfont;

    GetFont()->GetLogFont( &lfont );
    lfont.lfWeight = FW_BOLD;

    m_descFont.CreateFontIndirect( &lfont );
    GetDlgItem( IDC_LABEL_DESC )->SetFont( &m_descFont );

    return TRUE;
}

void C7xxxTab::SetAdapter( const zetlab::_7xxx::device::config::page_ptr& page_, const bool& editable )
{
    page_holder = page_;

    m_desc = page_->getDesc().c_str();
    for (auto it = page_->begin(), end = page_->end(); it != end; ++it)
        m_list.InsertParam( it->get(), editable );

    m_list.UpdateAllParams();

    UpdateData( FALSE );
}

LRESULT C7xxxTab::OnUserModify( WPARAM wParam, LPARAM lParam )
{
	m_vParamModify.clear();
	for (int nItem = 0; nItem < m_list.GetItemCount(); nItem++)
	{
		param* param_ = reinterpret_cast<param*>(m_list.GetItemData(nItem));
		if (param_->isModified())		
			m_vParamModify.emplace_back(CompareParametres(	param_->getType(), param_->getAddr(), 
															param_->getAddr2(), param_->getSize(), 
															param_->isEditable(), param_->getStructType()	));		
	}

// 	if (static_cast<BOOL>(lParam) == TRUE)
// 		m_pCfgWorker->addNumberModifiedItemTab(static_cast<int>(m_item));
// 	else
// 		m_pCfgWorker->removeNumberModifiedItemTab(static_cast<int>(m_item));
	
	m_pCfgWorker->changeParamModify(m_item, m_vParamModify);

    GetParent()->SendMessage( WM_USER_MODIFY, m_item, lParam );
    return 0;
}

LRESULT C7xxxTab::OnUserSetItem( WPARAM wParam, LPARAM lParam )
{
    m_item = static_cast< int >(wParam);
    return 0;
}

void C7xxxTab::OnUserRestore()
{
    m_list.RestoreAllParams();
}
