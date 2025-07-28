#pragma once

//#include <minwinbase.h>

#ifndef __flashfileinfo
#define __flashfileinfo

//��������� ������ ������, ������� ������������ � ������ �� ������
struct flashFileInfoS
{
	wchar_t fileName[200]; //��� �����
	long size; //������ �����

	//����� �������� ����� (�� ������)
	SYSTEMTIME fcTime;				// file create time

	//������ �������� ����� ��������, ���� ����� ����� �� ����������
	wchar_t badTimeSign[2];
	flashFileInfoS() 
		: size(25000)
	{
		ZeroMemory(fileName, sizeof(fileName));
		fcTime = { 0 };
		ZeroMemory(badTimeSign, sizeof(badTimeSign));

	}
};

#endif