#pragma once

#define WM_USER_ADD			(WM_APP + 1)    // �������� ���������� � ������ (wParam - item, lParam - &object)
#define WM_USER_REMOVE		(WM_APP + 2)    // ������� ���������� �� ������ (lParam - &object)
#define WM_USER_SETITEM		(WM_APP + 3)    // ���������� ���������� ����� � ������ (wParam - item)
#define WM_USER_SAVE		(WM_APP + 5)    // ��������� ���������
#define WM_USER_CANCEL		(WM_APP + 6)    // �������� ���������� ��������
#define WM_USER_RESTORE		(WM_APP + 7)    // ������������ ���������
#define WM_USER_MODIFY		(WM_APP + 8)    // �������� �� ���������� (wParam - item, lParam - modified)
#define WM_USER_READY		(WM_APP + 9)    // �������� � ���������� ��������� ZET SENSOR (wParam - &config)
#define WM_USER_REGISTRATOR (WM_APP + 10)   // �������� � ������ � �������������
#define WM_USER_DOTAB		(WM_APP + 11)   // �������� � ����� �������
#define WM_ENABLE_DEVICE	(WM_APP + 12)   // �������� �� ���������� ����������
#define WM_ENABLE_DATA		(WM_APP + 13)   // �������� � ���, ��� ������ �� ����� ����� ��������
#define WM_UPDATE_DATA		(WM_APP + 14)   // �������� ������
#define WM_VIEW_ERROR		(WM_APP + 15)   // ���������� ������ � ����� tab-�
#define WM_DETECT_ZET76		(WM_APP + 16)   // ������ ��������� �� ����������� ZETSENSOR � ����
