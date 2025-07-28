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
	void Add( zetlab::object* object_, bool isSelect = false ); // Добавляет устройство или канал в список. Формирует строки с параметрами (чувствительность, частота, ICP и т.д.).
	void AddRow037(zetlab::object* object_, bool isSelect /*= false*/); // добавление пустой строки
	void Remove( zetlab::object* object_ );  //Удаляет устройство или канал из списка
	void Update( zetlab::object* object_ );  //Обновляет информацию об устройстве или канале в списке (вызывает UpdateDeviceInfo и UpdateChannelInfo).
	void SortAll();  // Сортирует элементы списка по иерархии и другим критериям (использует callback-функцию Sort).
	zetlab::object* GetSelectedItemObject();  // Возвращает выбранный в списке объект (устройство или канал).
	zetlab::object* GetItemObject(const uint32_t type_, const uint64_t serial_);
	void SelectItem( zetlab::object* const object_ );
	void UpdateDeviceInfo(zetlab::object* const object_); // Обновляет информацию об устройстве (название, описание, иконку).
	void UpdateChannelInfo( zetlab::object* const object_ ); // Обновляет параметры канала (чувствительность, частоту, координаты и т.д.).
	void GetItemImage(int iItem, int &iImage);
	void SetItemImage(int iItem, int iImage);
	void SetNewImage(int iItem, zetlab::object* const object_);
	void SetConnecting( bool started );  // Управляет анимацией подключения (переключение между иконками connecting1 – connecting4).
	zetlab::object* EnumerationAllChannelsFromDevice(int Item); // Возвращает объект канала по индексу в списке (используется для перебора каналов устройства).
	CString ConvertToFreq(const float& freq_);  // Форматирует частоту в строку с единицами измерения (Гц или кГц).
	void ChangeName(zetlab::object* const object_, bool isModify);
	void ChangeNameAll(zetlab::object* const objectParent_, bool isModify); // Обновляет отображаемое имя устройства (например, при редактировании пользователем).


private:
	int FindItem( zetlab::object* const object_ );

protected:
	afx_msg void    OnCustomDraw( NMHDR *pNMHDR, LRESULT *pResult ); // Обрабатывает кастомное отображение элементов списка (цвет текста для неактивных устройств).
	afx_msg BOOL	OnNeedToolTip(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);   // Формирует всплывающие подсказки для элементов списка (описание канала, IP-адрес устройства и т.д.).
	afx_msg void    OnTimer( UINT_PTR nIDEvent );

	DECLARE_MESSAGE_MAP()
public:
	void startBlinking( zetlab::object* object_ );
	void stopBlinking ( zetlab::object* object_ );  // Включает/выключает анимацию мигания иконки устройства (например, при подключении).
	CString cstrText;
protected:
	int iBlinkedItem;
	int blink_on_off;

	int _connectingImage;

	void UpdateConnecting(); // Управляет анимацией подключения (переключение между иконками connecting1 – connecting4).
	void SendErrorInfo(long error_, const zetlab::object* object_, bool arose_ = true) const;  // Отправляет информацию об ошибке в главное окно программы ZETLab.
	long ConvertToCoordinates(zetlab::std::channel* channel_, CString* X, CString* Y, CString* Z, CString* P, bool strict = false); // Преобразует координаты канала (X, Y, Z) и ориентацию (P) в строковый формат.


};
