#include "stdafx.h"
#include "../../application/application.h"
#include "ConfigDlg.h"
#include "resource/Phrases.h"
#include "../../algo/path/path.h"

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(::std::vector<::std::pair<zetlab::tstring, CZetTime>>& cfg, bool bSave, zetlab::factory_ptr factory, CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
	, m_bSave(bSave)
	, m_cfg(cfg)
	, iItem(0)
	, m_factory(factory)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIG_LIST, m_configList);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_EN_CHANGE(IDC_CONFIG_EDIT, &CConfigDlg::OnEnChangeConfigEdit)
	ON_NOTIFY(NM_CLICK, IDC_CONFIG_LIST, &CConfigDlg::OnNMClickConfigList)
	ON_NOTIFY(NM_RCLICK, IDC_CONFIG_LIST, &CConfigDlg::OnNMRClickList1)
	ON_COMMAND(IDM_DELETEMENU, OnMenuSelect)
END_MESSAGE_MAP()

BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_bSave ? g_sSaveConfig : g_sOpenConfig);
	SetDlgItemText(IDOK, m_bSave ? g_sSave : g_sOpen);
	SetDlgItemText(IDCANCEL, g_sCancel);

	GetDlgItem(IDC_CONFIG_EDIT)->EnableWindow(m_bSave);

	OnEnChangeConfigEdit();
	m_configList.InsertColumn(g_sConfig);
	m_configList.InsertColumn(g_sDateTime);
	m_configList.InsertColumn(L"", LVCFMT_CENTER);
	for (auto& cfg : m_cfg)
	{
		wchar_t s[20] = {0};
		cfg.second.ConvertToString(s, 20, 1., L"dd.MM.yyyy HH:mm:ss");
		auto i = m_configList.InsertItem(-1, -1, cfg.first.c_str(), (LPCTSTR)0);
		m_configList.SetItemText(i, 1, s);
		m_configList.SetItemText(i, 2, L"X");
		m_configList.SetBackgroundColorColumn(2, RGB(150, 0, 0));
	}	
	m_configList.SmartWidth();

	return TRUE;
}

void CConfigDlg::OnEnChangeConfigEdit()
{
	CString s;
	GetDlgItem(IDC_CONFIG_EDIT)->GetWindowText(s);
	GetDlgItem(IDOK)->EnableWindow(!s.IsEmpty());
}

void CConfigDlg::OnOK()
{
	
	CString s;
	GetDlgItem(IDC_CONFIG_EDIT)->GetWindowText(s);
	m_cfgName = s;

	CDialogEx::OnOK();
}

void CConfigDlg::OnNMClickConfigList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iSubItem == 2)
	{
		iItem = pNMItemActivate->iItem;
		OnMenuSelect();
	}
	else
	{
		auto s = m_configList.GetItemText(pNMItemActivate->iItem, 0);
		GetDlgItem(IDC_CONFIG_EDIT)->SetWindowText(s);
	}
	*pResult = 0;
}

void CConfigDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem > -1)
	{
		iItem = pNMItemActivate->iItem;
		POINT ptAction;
		GetCursorPos(&ptAction);
		m_contextMenu.DestroyMenu();
		m_contextMenu.CreatePopupMenu();

		m_contextMenu.AppendMenu(MF_BYCOMMAND | MF_ENABLED | MF_STRING, IDM_DELETEMENU, g_sDelete);
		m_contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptAction.x, ptAction.y, this);
	}
	*pResult = 0;
}

void CConfigDlg::OnMenuSelect()
{
	if (iItem >= 0 && iItem < (long)m_cfg.size())
	{
		if (::AfxMessageBox(g_sDeleteConfig + L"?", MB_SYSTEMMODAL | MB_OKCANCEL) == IDOK)
		{
			zetlab::saver saver_(m_factory);
			path    path_; path_.Create(this);
			BSTR	bstrDirConfig = _T("DirConfig");
			CString	cstrTabConfig = path_.ZetPath(&bstrDirConfig) + _T("devices.cfg.bak");
			if (saver_.deleteConfig((LPCTSTR)cstrTabConfig, m_cfg[iItem].first) == 1)
			{
				m_cfg.erase(m_cfg.begin() + iItem);
				m_configList.DeleteItem(iItem);
				GetDlgItem(IDC_CONFIG_EDIT)->SetWindowText(L"");
				m_cfgName = L"";
			}
		}
		else 
			m_configList.SetItemState(iItem, 0, LVIS_SELECTED);

		iItem = -1;
	}
}

long CConfigDlg::getCfgSize()
{
	return (long)m_cfg.size();
}