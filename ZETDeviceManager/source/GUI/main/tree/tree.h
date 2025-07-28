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
	void Add( zetlab::object* object_, bool isSelect = false );  // добавление элемента дерева
	void AddRow037(zetlab::object* object_, bool isSelect /* = false*/);
	void Remove( zetlab::object* object_ );  // Удаляет элемент, соответствующий переданному объекту.
	void Update( zetlab::object* object_ ); // Обновляет информацию об устройстве/канале в дереве (текст, иконку).
	void SortAll( HTREEITEM startItem = TVI_ROOT );
	zetlab::object* GetSelectedItemObject(); // Возвращает выбранный в дереве объект (zetlab::object*).
	HTREEITEM GetSelectedHtreeItem(); // Возвращает выбранный в дереве объект (zetlab::object*).
	void SelectItem( zetlab::object* const object_ ); // Выделяет элемент, соответствующий переданному объекту.
	void UpdateDeviceInfo(zetlab::object* const object_);  //Обновляет визуальное представление устройств.
	void UpdateChannelInfo( zetlab::object* const object_ ); //Обновляет данные каналов.
	void SetNewImage(HTREEITEM hItem, zetlab::object* const object_);
	void SetConnecting( bool started );  //Анимация подключения для контроллеров
	void ChangeName(zetlab::object* const object_, bool isModify); // Переименовывает элемент в зависимости от состояния (например, добавление "*" для изменённых объектов).
	void ChangeNameAll(zetlab::object* const objectParent_, bool isModify); // Рекурсивно переименовывает все дочерние элементы указанного узла.


private:
	void SortItems( HTREEITEM hItem );//: Сортировка элементов дерева.
	HTREEITEM FindItem( zetlab::object* const object_, HTREEITEM startItem );  /// Рекурсивный поиск элемента дерева по связанному объекту.

protected:
	afx_msg void OnCustomDraw ( NMHDR* pNMHDR, LRESULT* pResult );  // Обрабатывает кастомную отрисовку элементов (цвет текста для отключенных устройств, предупреждений и т.д.).
	afx_msg void OnNeedToolTip( NMHDR* pNMHDR, LRESULT* pResult );  // Формирует текст подсказки (tooltip) для элемента (IP-адрес, статус, координаты каналов и др.).


	afx_msg void OnTimer( UINT_PTR nIDEvent ); 

	DECLARE_MESSAGE_MAP()

public:
	void startBlinking( zetlab::object* object_ );  //Мигание элемента при событиях.
	void stopBlinking ( zetlab::object* object_ );

protected:
	HTREEITEM hBlinkedItem;
	int       blink_on_off;

	int       _connectingImage;

	void UpdateConnecting(); //Анимация подключения для контроллеров
};
