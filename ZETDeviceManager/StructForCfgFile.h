#pragma once
#include <set>
#include <vector>
#include <windows.h>
#include <ModbusDefinitions.h>

// ����� ��������� 7xxx, ���� �� ����������� ������
struct _structCfg7xxx
{
	_structCfg7xxx()
		: _sizeDev(0)
		, _sizeDefine(0)
		, _serialParent(0)
		, _isMaster(false)
		, _isRunData(false)
		, _object(nullptr)	
	{
	}
	
	std::vector< BYTE > _vDevStruct;				// ��������� ������
	std::vector< BYTE > _vDevStructOriginal;		// ��� �������� �������� ���������
	std::vector< BYTE > _vDefineStruct;				// ��������� ����������

	std::map< int, std::vector<CompareParametres>> _mParamModify;	// ����� - ���������� ���, �������� - ��������� (����� ��������, ��� ���)
	unsigned long long _serialParent;								// �������� ����� �������� (�������)
	long _sizeDev;													// ������ ��������� ������
	long _sizeDefine;												// ������ ��������� ����������	
	bool _isMaster;													// ���� ����, ��� �������� � ��������
	bool _isRunData;												// ���� ����, ��� ������� ������ ��������
		
	void* _object;													// ������ � deviceManager;
};

