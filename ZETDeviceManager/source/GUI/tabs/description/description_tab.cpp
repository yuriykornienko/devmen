#include "stdafx.h"
#include "../../../application/application.h"
#include "../../user_messages.hpp"
#include "description_tab.h"
#include "resource\Phrases.h"

//#include <boost/make_shared.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/range/algorithm/for_each.hpp>

//#include <mathZet.h>

CDescriptionTab::CDescriptionTab(CWnd* pParent)
	: CTab(CDescriptionTab::IDD, pParent)
	,    device_(nullptr)
{
}

CDescriptionTab::~CDescriptionTab()
{
}

void CDescriptionTab::DoDataExchange(CDataExchange* pDX)
{
	CTab::DoDataExchange( pDX );

	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_description);
}

BEGIN_MESSAGE_MAP(CDescriptionTab, CTab)
	ON_MESSAGE_VOID(WM_USER_SAVE, OnUserSave)
END_MESSAGE_MAP()

BEGIN_ANCHOR_MAP(CDescriptionTab)
END_ANCHOR_MAP()

BOOL CDescriptionTab::OnInitDialog()
{
	CTab::OnInitDialog();
	GetDlgItem(IDC_GROUP_DESCRIPTION)->SetWindowTextW(g_sDescription);
	return TRUE;
}

void CDescriptionTab::SetAdapter(zetlab::std::device* const device__)
{
	device_  = device__;
	memento_ = boost::make_shared< zetlab::std::device_memento >( device__ );

	if (nullptr == device_)
		return;

	zetlab::tstring description;
	device_->readDescription(description);
	m_description = description.c_str();
	UpdateData(FALSE);
}

void CDescriptionTab::OnUserSave()
{
	UpdateData();

	if (nullptr == device_)
		return;

	zetlab::tstring description(m_description.GetString());
	if (m_description.GetLength() > 30)
	{
		m_description = m_description.Mid(0, 30);
		description = m_description.GetString();
	}

	device_->writeDescription(description);
}
