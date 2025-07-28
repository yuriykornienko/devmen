#pragma once

#include "../../../../algo/object/object.hpp"

class CReusePage :
    public CPropertyPage
{
public:
    CReusePage();
    virtual ~CReusePage();
    enum { IDD = IDD_WIZARD_REUSE };
	enum { IDS = IDS_IDD_WIZARD_REUSE };		// ���? <- "� �� ����� ������ ��� �������"

public:
    void SetAdapter( zetlab::object* const object__ );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();

    DECLARE_MESSAGE_MAP()

private:
    zetlab::object* object_;
};
