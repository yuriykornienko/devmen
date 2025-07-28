#pragma once

#include "../default_tab.h"

#include "../../../algo/std/device/device_std.hpp"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <vector>

class CFrequencyTab : public CTab
{
public:
	CFrequencyTab( CWnd* pParent = NULL );
	virtual ~CFrequencyTab();
	enum { IDD = IDD_FREQ_TAB };
public:
	void SetAdapter( zetlab::std::device* const device__ );

	void Activate() {}
	void Disactivate() {}

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();
	void InsertFrequencies( const boost::function< float(::std::vector< float >&) >& f,
							CComboBox* combo );

	afx_msg void    OnAdcChangeRequest();
	afx_msg void    OnDacChangeRequest();
	afx_msg void    OnCancel();
	afx_msg void    OnUserCancel();

	DECLARE_MESSAGE_MAP()
	DECLARE_ANCHOR_MAP()

protected:
	CComboBox m_comboAdc;
	float     m_adc_value;
	CComboBox m_comboDac;
	float     m_dac_value;

private:
	zetlab::std::device*            device_;
	zetlab::std::device_memento_ptr memento_;
};
