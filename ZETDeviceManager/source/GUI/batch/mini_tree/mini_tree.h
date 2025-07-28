#pragma once

#include "../../treectrl/treectrlex.h"

class CMiniTree : public CTreeCtrlEx
{
public:
    CMiniTree() {};
    virtual ~CMiniTree() {};

    virtual void LoadItemImages();

protected:
    afx_msg void    OnCustomDraw ( NMHDR* pNMHDR, LRESULT* pResult );

    DECLARE_MESSAGE_MAP()
};
