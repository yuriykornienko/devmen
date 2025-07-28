#pragma once

#include "../../listctrl/listctrlex.h"

#include "../../../algo/object/object.hpp"

class CDevicesList : public CListCtrlEx
{
public:
	CDevicesList() 
		: iBlinkedItem(-1)
		, blink_on_off(0)
		, _connectingImage(0)
	{
	}
	virtual ~CDevicesList() {}

	virtual void LoadItemImages();

public:
	void Add( zetlab::object* object_, bool isSelect = false ); // ��������� ���������� ��� ����� � ������. ��������� ������ � ����������� (����������������, �������, ICP � �.�.).
	void AddRow037(zetlab::object* object_, bool isSelect /*= false*/); // ���������� ������ ������
	void Remove( zetlab::object* object_ );  //������� ���������� ��� ����� �� ������
	void Update( zetlab::object* object_ );  //��������� ���������� �� ���������� ��� ������ � ������ (�������� UpdateDeviceInfo � UpdateChannelInfo).
	void SortAll();  // ��������� �������� ������ �� �������� � ������ ��������� (���������� callback-������� Sort).
	zetlab::object* GetSelectedItemObject();  // ���������� ��������� � ������ ������ (���������� ��� �����).
	zetlab::object* GetItemObject(const uint32_t type_, const uint64_t serial_);
	void SelectItem( zetlab::object* const object_ );
	void UpdateDeviceInfo(zetlab::object* const object_); // ��������� ���������� �� ���������� (��������, ��������, ������).
	void UpdateChannelInfo( zetlab::object* const object_ ); // ��������� ��������� ������ (����������������, �������, ���������� � �.�.).
	void GetItemImage(int iItem, int &iImage);
	void SetItemImage(int iItem, int iImage);
	void SetNewImage(int iItem, zetlab::object* const object_);
	void SetConnecting( bool started );  // ��������� ��������� ����������� (������������ ����� �������� connecting1 � connecting4).
	zetlab::object* EnumerationAllChannelsFromDevice(int Item); // ���������� ������ ������ �� ������� � ������ (������������ ��� �������� ������� ����������).
	CString ConvertToFreq(const float& freq_);  // ����������� ������� � ������ � ��������� ��������� (�� ��� ���).
	void ChangeName(zetlab::object* const object_, bool isModify);
	void ChangeNameAll(zetlab::object* const objectParent_, bool isModify); // ��������� ������������ ��� ���������� (��������, ��� �������������� �������������).


private:
	int FindItem( zetlab::object* const object_ );

protected:
	afx_msg void    OnCustomDraw( NMHDR *pNMHDR, LRESULT *pResult ); // ������������ ��������� ����������� ��������� ������ (���� ������ ��� ���������� ���������).
	afx_msg BOOL	OnNeedToolTip(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);   // ��������� ����������� ��������� ��� ��������� ������ (�������� ������, IP-����� ���������� � �.�.).
	afx_msg void    OnTimer( UINT_PTR nIDEvent );

	DECLARE_MESSAGE_MAP()
public:
	void startBlinking( zetlab::object* object_ );
	void stopBlinking ( zetlab::object* object_ );  // ��������/��������� �������� ������� ������ ���������� (��������, ��� �����������).
	CString cstrText;
protected:
	int iBlinkedItem;
	int blink_on_off;

	int _connectingImage;

	void UpdateConnecting(); // ��������� ��������� ����������� (������������ ����� �������� connecting1 � connecting4).
	void SendErrorInfo(long error_, const zetlab::object* object_, bool arose_ = true) const;  // ���������� ���������� �� ������ � ������� ���� ��������� ZETLab.
	long ConvertToCoordinates(zetlab::std::channel* channel_, CString* X, CString* Y, CString* Z, CString* P, bool strict = false); // ����������� ���������� ������ (X, Y, Z) � ���������� (P) � ��������� ������.


};
