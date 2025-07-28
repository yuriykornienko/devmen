#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"
#include "../../../algo/7xxx/7xxx/device/device_7xxx.hpp"
#include "..\..\..\..\resource/phrases.h"
#include <Dialog_ZET\CDZetPath.h>
class CCommonTab : public CTab
{
public:
    CCommonTab( CWnd* pParent = NULL );
    virtual ~CCommonTab();
	enum {IDD = IDD_COMMON_TAB};

public:
    void SetAdapter( zetlab::std::device* const device_,
                     const zetlab::tstring& name );

    void SetAdapter( zetlab::_7xxx::device* const device_,
                     const zetlab::tstring& name );

	void Activate() {}
	void Disactivate() {}

	void ShowMessageError7xxx();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();

	CString retStrConvTime(unsigned long long Disksize, long freq);
	const char GetYearsPlural(uint32_t years);
	CDZetPath m_zPath;

	void ShiftControl(zetlab::std::device* const device_);

    DECLARE_MESSAGE_MAP()
    DECLARE_ANCHOR_MAP()

protected:
    CStatic m_icon;
    CString m_caption;
    CString m_typeStr;
    LONG    m_serial;
	CString m_version;
	CString m_flashState;
	CString m_fDiskState;
	CString m_batteryState;
	CString m_syncState;
};
