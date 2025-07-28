#pragma once
#include "source/algo/std/channel/channel.hpp"
#include "resource/resource.h"

class CTensoIcpDlg : public CDialog
{
	DECLARE_DYNAMIC(CTensoIcpDlg)

public:
	CTensoIcpDlg(zetlab::std::channel* channel, CWnd* pParent = nullptr); 
	virtual ~CTensoIcpDlg();

	enum { IDD = IDD_TENSO_ICP_DLG };

protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;
	void OnCancel() override;

	afx_msg void OnResistanceChangeRequest();
	afx_msg void OnCoefficientChangeRequest();
	afx_msg void OnYoungModChangeRequest();
	afx_msg void OnUnitSelect();

	DECLARE_MESSAGE_MAP()

	zetlab::std::channel* m_channel;

	CString m_resistance;
	CString m_coefficient;
	CString m_youngMod;

	CComboBox m_unitCombo;

private:
	zetlab::std::channel_memento_ptr memento_;
};
