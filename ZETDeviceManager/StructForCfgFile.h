#pragma once
#include <set>
#include <vector>
#include <windows.h>
#include <ModbusDefinitions.h>

// новая структура 7xxx, уход от разделяемой памяти
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
	
	std::vector< BYTE > _vDevStruct;				// структура данных
	std::vector< BYTE > _vDevStructOriginal;		// для хранения исходной структуры
	std::vector< BYTE > _vDefineStruct;				// структура описателей

	std::map< int, std::vector<CompareParametres>> _mParamModify;	// ключи - измененный таб, значение - контейнер (адрес значения, его тип)
	unsigned long long _serialParent;								// серийный номер родителя (мастера)
	long _sizeDev;													// размер структуры данных
	long _sizeDefine;												// размер структуры описателей	
	bool _isMaster;													// флаг того, что работаем с мастером
	bool _isRunData;												// флаг того, что процесс записи завершен
		
	void* _object;													// объект в deviceManager;
};

