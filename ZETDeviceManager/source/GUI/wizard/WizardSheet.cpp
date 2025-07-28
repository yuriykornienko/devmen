#include "stdafx.h"
#include "../../application/application.h"
#include "WizardSheet.h"
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

CWizardSheet::CWizardSheet( zetlab::object* object__, CWnd* pParent ) :
    object_(object__),
	CPropertySheet(g_sEquipmentSetupWizard, pParent, 0)
{
    m_psh.dwFlags |= PSH_AEROWIZARD | PSH_USEHICON;
    m_psh.hIcon    = AfxGetApp()->LoadIcon( IDI_WIZARD );

    if (nullptr == object_->getAsDevice() || zetlab::std::demo == object_->getAsDevice()->getConnectionType())
        return;

    AddPage( &m_start );
    AddPage( &m_reuse );

    m_start.SetAdapter( object_ );
    m_reuse.SetAdapter( object_ );
}

BEGIN_MESSAGE_MAP( CWizardSheet, CPropertySheet )
END_MESSAGE_MAP()
