#pragma once

#include "../../algo/object/object.hpp"

#include "pages/start_page/start_page.h"
#include "pages/reuse_page/reuse_page.h"

class CWizardSheet :
    public CPropertySheet
{
public:
    CWizardSheet( zetlab::object* object__, CWnd* pParent = NULL );

protected:
    DECLARE_MESSAGE_MAP()

protected:
    CStartPage m_start;
    CReusePage m_reuse;

private:
    zetlab::object* object_;
};
