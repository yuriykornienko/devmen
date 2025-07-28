#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

#include "grayable_groupbox/grayable_groupbox.h"
#include "afxwin.h"

class CSyncTab : public CTab
{
public:
	CSyncTab( CWnd* pParent = NULL );
	virtual ~CSyncTab();
	enum { IDD = IDD_SYNC_TAB };

public:
	void SetAdapter( zetlab::std::device* const device__ );

	void Activate();
	void Disactivate();

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();
	virtual void InitToolTips();

	afx_msg void    OnTimer( UINT_PTR nIDEvent );
	afx_msg void    OnChange( UINT nID );
	afx_msg void    OnUserSave();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()
	
private:
	void HandleVisible( int type );
	void HandleGpsEnabled();
	void MoveUp( CWnd* pWnd, int cy );

protected:
	CString m_Adjustment;
	CString m_time;
	int     m_synced;
	int     m_enableMaster2;
	int     m_enableSlave2;
	int     m_satellites;
	int     m_pps;
	CStatic m_ppsIcon;
	int     m_enableMaster;
	int     m_enableSlave;
	int     m_domain;
	int     m_absolutePriority;
	int     m_relativePriority;

	CGrayableGroupBox m_groupGps;

private:
	zetlab::std::device* device_;
public:
};
