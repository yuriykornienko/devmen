#pragma once

// Dpi aware
#include <CDPI/cdpi.h>

class CClipboardToolBar : public CToolBar
{
public:
    CClipboardToolBar();
    virtual ~CClipboardToolBar();

    // Returns toolbar height
    virtual int Create( CWnd* pParentWnd, UINT nIDResource );

public:
	BOOL SetButtonText(int nIndex, CString  uResID);
    
    void SetPasteAvailable( BOOL bAvailable = TRUE );

protected:
    afx_msg BOOL    OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult );

    DECLARE_MESSAGE_MAP()

protected:
    CDPI          m_dpiAware;
    CImageList    m_imageList;
};
