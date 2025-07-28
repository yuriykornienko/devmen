#pragma once

#include <map>

#include "../../types.hpp"
#include "../../../resource/phrases.h"
#include "../../../resource/resource.h"
#include "../../algo/utils/string_conv/string_conv.hpp"
#include "../../gui/user_messages.hpp"

class CTabs : public CTabCtrl
{
public:
	CTabs(): m_bCanChangeTab(true) {};
	virtual ~CTabs() {};

public:
	//
	template< typename T >
	inline LONG InsertItem(int nItem, T* dlg, int nImage, UINT idTabUser, BOOL modified = TRUE, CString Name = CString())
	{
		// Create window
		dlg->Create( T::IDD, this );

		// Tab info
		TabInfo info;
		info.modified = modified;

		zetlab::tstring name(L"");
		if (!Name.IsEmpty())
			name = Name;
		zetlab::tstring param = L"ZETDeviceManager.exe";
		zetlab::utils::translate(name, param);	

		info.name = name.c_str();
		info.idTabUser = idTabUser;

		// Insert tab and associate with window
		nItem = CTabCtrl::InsertItem(
			TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM,
			nItem, info.name, nImage,
			reinterpret_cast< LPARAM >(dlg) );		

		m_tabInfo.insert( std::pair< int, TabInfo >(nItem, info) );

		if (T::IDD == IDD_7XXX_TAB || T::IDD == IDD_CALIBRATION_TAB)
			dlg->SendMessage( WM_USER_SETITEM, nItem );

		return nItem;
	}

	BOOL DeleteItem( int nItem );

	int SetActiveTab( int nItem );

	BOOL SetTabName( int nItem, LPCTSTR lpszText );

	BOOL SetModified( int nItem, BOOL modified );

	int GetIndexItemTab(UINT idTabUser);

protected:
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnSelChangingTabs( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnSelChangeTabs( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg LRESULT OnUserModify( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUserRegistrator( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

protected:
	struct TabInfo
	{
		BOOL modified;
		CString name;
		UINT idTabUser;
	};
	std::map< int, TabInfo > m_tabInfo;

	bool m_bCanChangeTab;
};
