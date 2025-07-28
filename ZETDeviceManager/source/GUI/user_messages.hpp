#pragma once

#define WM_USER_ADD			(WM_APP + 1)    // добавить устройство в список (wParam - item, lParam - &object)
#define WM_USER_REMOVE		(WM_APP + 2)    // удалить устройство из списка (lParam - &object)
#define WM_USER_SETITEM		(WM_APP + 3)    // установить порядковый номер в списке (wParam - item)
#define WM_USER_SAVE		(WM_APP + 5)    // сохранить настройки
#define WM_USER_CANCEL		(WM_APP + 6)    // отменить сохранение настроек
#define WM_USER_RESTORE		(WM_APP + 7)    // восстановить настройки
#define WM_USER_MODIFY		(WM_APP + 8)    // сообщить об изменениях (wParam - item, lParam - modified)
#define WM_USER_READY		(WM_APP + 9)    // сообщить о готовности структуры ZET SENSOR (wParam - &config)
#define WM_USER_REGISTRATOR (WM_APP + 10)   // сообщить о работе с регистратором
#define WM_USER_DOTAB		(WM_APP + 11)   // сообщить о смене вкладки
#define WM_ENABLE_DEVICE	(WM_APP + 12)   // сообщить об отключении устройства
#define WM_ENABLE_DATA		(WM_APP + 13)   // сообщить о том, что данные по линии можно получать
#define WM_UPDATE_DATA		(WM_APP + 14)   // обновить данные
#define WM_VIEW_ERROR		(WM_APP + 15)   // отобразить ошибку в общем tab-е
#define WM_DETECT_ZET76		(WM_APP + 16)   // выдать сообщение об обнаружении ZETSENSOR в сети
