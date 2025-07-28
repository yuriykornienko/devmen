#pragma once

#include <Localized/Localized.h>
#include "../../../../algo/object/object.hpp"
#include "../../../../algo/std/device/device_std.hpp"

class CStartPage :
    public CLocalized,
    public CPropertyPage
{
public:
    CStartPage();
    virtual ~CStartPage();
    enum { IDD = IDD_WIZARD_START };

public:
    void SetAdapter( zetlab::object* const object__ );

private:
    void SetAdapterImpl( zetlab::std::device* const device_ );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();

    afx_msg void    OnShowWindow( BOOL bShow, UINT nStatus );

    DECLARE_MESSAGE_MAP()

protected:
    CStatic m_icon;
    CString m_type;
    LONG    m_serial;

private:
    zetlab::object* object_;
};
