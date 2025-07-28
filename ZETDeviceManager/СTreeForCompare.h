#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <7xxxXmlWorker\XMLWorker.h>
#include "resource\resource.h"
#include <memory>
#include <WndResizer\WndResizer.h>
#include "CFGWorker.h"
#include "source/algo/object/object.hpp"
#include "source/GUI/main/tree/tree.h"
//-----------------------------------------------------------------------------------------------------------------------------
namespace zetlab{
enum class timer{
	timerUpdateTree = 0
};
class ÑTreeForCompare : public CDialogEx
{
	DECLARE_DYNAMIC(ÑTreeForCompare)
public:
	typedef boost::unordered_map< tstring, object_ptr > objects_type;
	typedef ::std::pair< tstring, object_ptr >          objects_pair;

public:
	ÑTreeForCompare(CWnd* pParent = NULL);  
	virtual ~ÑTreeForCompare();

	enum { IDD = IDD_TREE_FOR_COMPARE };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);  

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSizing(UINT wParam, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnPropertiesSaveAll();
	afx_msg void    OnPropertiesRequest();
	afx_msg void    OnPropertiesSave();
	afx_msg void    OnTimer(UINT_PTR nIDEvent);

protected:
	bool initResizer();

	object* const add(	const tstring& full_path,
						object* parent,
						object_type::values type);
	const bool exists(const tstring& full_path) const;
	void clearTreeForUpdate();
	void PrepareContextMenu(CMenu** context_menu);
	void startTimer(timer typeTimer, long time);
	long getNodeFromShotName(CString sName);
	
public:
	CString getPathXml();
	void setPathXml(const wchar_t * sPath);

	bool updateTree();	

private:
	enum sizeWindow{
		widthMainWindow = 400,
		heightMainWindow = 300
	};	

private:
	CWndResizer m_resizer;
	CXMLWorker m_xmlWorker;
	CCFGWorker m_cfgWorker;

	objects_type  _objects;
	object_ptr    _root;
	CDevicesTree  m_tree;

	CButton m_btnOk;
	CButton m_btnCancel;
	RECT m_rectOld;
	
	CString m_pathToXml;

	CMenu m_contextMenu;
	HICON m_hIcon;
};
}