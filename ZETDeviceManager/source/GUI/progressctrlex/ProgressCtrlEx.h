#pragma once

class CProgressCtrlEx : public CProgressCtrl
{
	DECLARE_DYNAMIC( CProgressCtrlEx )

public:
	CProgressCtrlEx();
	virtual ~CProgressCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void	SetShowText( CString c_str );
	CString	GetShowText();
	void	SetZeroRange( int range );
	void	SetPosition( int pos );
	void	SetIndeterminate( BOOL bInf = TRUE );
	void	Pause();
	void	Error();

protected:
	afx_msg void OnPaint();

	CString			showText;
	ITaskbarList3*	pTaskbarList;
};
