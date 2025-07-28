#pragma once

#include "../../types.hpp"
#include "../listctrl/listctrlex.h"

#include <Dialog_ZET\ConnectToZetTools.h>
#include "afxcmn.h"
#include "../../algo/saver/saver.hpp"


class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(::std::vector<::std::pair<zetlab::tstring, CZetTime>>& cfg, bool bSave = true, zetlab::factory_ptr factory = nullptr, CWnd* pParent = NULL);
	virtual ~CConfigDlg();

	zetlab::tstring m_cfgName;

	enum { IDD = IDD_CONFIG };
	long getCfgSize();
protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnEnChangeConfigEdit();
	afx_msg void OnNMClickConfigList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuSelect();
	DECLARE_MESSAGE_MAP()

	::std::vector<::std::pair<zetlab::tstring, CZetTime>> m_cfg;
	zetlab::factory_ptr m_factory;
	CListCtrlEx m_configList;
	CMenu       m_contextMenu;
	int			iItem;
	bool		m_bSave;
};
