#pragma once

#include "../../treectrl/treectrlex.h"

#include "../../../algo/object/object.hpp"

class CDevicesTree : public CTreeCtrlEx
{
public:
	CDevicesTree() 
		: hBlinkedItem(NULL)
		, blink_on_off(0)
		, _connectingImage(0)
	{}
	virtual ~CDevicesTree() {}

	virtual void LoadItemImages();

public:
	void Add( zetlab::object* object_, bool isSelect = false );  // ���������� �������� ������
	void AddRow037(zetlab::object* object_, bool isSelect /* = false*/);
	void Remove( zetlab::object* object_ );  // ������� �������, ��������������� ����������� �������.
	void Update( zetlab::object* object_ ); // ��������� ���������� �� ����������/������ � ������ (�����, ������).
	void SortAll( HTREEITEM startItem = TVI_ROOT );
	zetlab::object* GetSelectedItemObject(); // ���������� ��������� � ������ ������ (zetlab::object*).
	HTREEITEM GetSelectedHtreeItem(); // ���������� ��������� � ������ ������ (zetlab::object*).
	void SelectItem( zetlab::object* const object_ ); // �������� �������, ��������������� ����������� �������.
	void UpdateDeviceInfo(zetlab::object* const object_);  //��������� ���������� ������������� ���������.
	void UpdateChannelInfo( zetlab::object* const object_ ); //��������� ������ �������.
	void SetNewImage(HTREEITEM hItem, zetlab::object* const object_);
	void SetConnecting( bool started );  //�������� ����������� ��� ������������
	void ChangeName(zetlab::object* const object_, bool isModify); // ��������������� ������� � ����������� �� ��������� (��������, ���������� "*" ��� ��������� ��������).
	void ChangeNameAll(zetlab::object* const objectParent_, bool isModify); // ���������� ��������������� ��� �������� �������� ���������� ����.


private:
	void SortItems( HTREEITEM hItem );//: ���������� ��������� ������.
	HTREEITEM FindItem( zetlab::object* const object_, HTREEITEM startItem );  /// ����������� ����� �������� ������ �� ���������� �������.

protected:
	afx_msg void OnCustomDraw ( NMHDR* pNMHDR, LRESULT* pResult );  // ������������ ��������� ��������� ��������� (���� ������ ��� ����������� ���������, �������������� � �.�.).
	afx_msg void OnNeedToolTip( NMHDR* pNMHDR, LRESULT* pResult );  // ��������� ����� ��������� (tooltip) ��� �������� (IP-�����, ������, ���������� ������� � ��.).


	afx_msg void OnTimer( UINT_PTR nIDEvent ); 

	DECLARE_MESSAGE_MAP()

public:
	void startBlinking( zetlab::object* object_ );  //������� �������� ��� ��������.
	void stopBlinking ( zetlab::object* object_ );

protected:
	HTREEITEM hBlinkedItem;
	int       blink_on_off;

	int       _connectingImage;

	void UpdateConnecting(); //�������� ����������� ��� ������������
};
