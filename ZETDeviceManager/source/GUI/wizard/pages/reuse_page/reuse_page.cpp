#include "stdafx.h"
#include "../../../../application/application.h"
#include "reuse_page.h"

CReusePage::CReusePage() :
    CPropertyPage(CReusePage::IDD, 0, CReusePage::IDS, 0),
    object_(nullptr)
{
}

CReusePage::~CReusePage()
{
}

void CReusePage::DoDataExchange( CDataExchange* pDX )
{
    CPropertyPage::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CReusePage, CPropertyPage )
END_MESSAGE_MAP()

BOOL CReusePage::OnInitDialog()
{

    CPropertyPage::OnInitDialog();

    return TRUE;
}

void CReusePage::SetAdapter( zetlab::object* const object__ )
{
    if (nullptr == object__)
        return;

    object_ = object__;
}

BOOL CReusePage::OnSetActive()
{
    if (nullptr == object_)
        return FALSE;

    ((CPropertySheet*)GetParent())->SetWizardButtons( PSWIZB_BACK | PSWIZB_CANCEL );

    return CPropertyPage::OnSetActive();
}
