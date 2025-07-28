#pragma once
#include <atomic>
#include <memory>
#include <functional>
#include <iostream>
#include <map>
#include <ZETFile\include\ZETFile.h>
#include "StructForCfgFile.h"
#include <7xxxXmlWorker\XMLWorker.h>
#include <ModbusDefinitions.h>
#include "MyException.h"
//-----------------------------------------------------------------------------------------------------------------------------
enum stateFileCfg
{
	fileClose,
	fileOpen,
	fileChanged,
};
//-----------------------------------------------------------------------------------------------------------------------------
struct structForCheckFile
{
	structForCheckFile()
		: _nameFile(_T(""))
		, _lastWriteTime({})
		, _object_ptr(nullptr)
	{}

	~structForCheckFile()
	{
		_object_ptr = nullptr;
	}

	structForCheckFile(const structForCheckFile& inStruct)
	{
		*this = inStruct;
	}

	structForCheckFile& operator = (const structForCheckFile& inStruct)
	{
		_nameFile = inStruct._nameFile;
		_lastWriteTime = inStruct._lastWriteTime;
		_object_ptr = inStruct._object_ptr;
		return *this;
	}

	CString _nameFile;
	FILETIME _lastWriteTime;
	void* _object_ptr;
};
//-----------------------------------------------------------------------------------------------------------------------------
enum statusWorkThreadWrite
{
	statusWtire = 0,
	statusCompare,
	statusEnd
};

class CCFGWorker
{
public:
	CCFGWorker();
	~CCFGWorker();

public:
	// работа с файлами конфигураций
	_structCfg7xxx* getStructCfg7xxx(uint64_t serialNumber, bool isRefresh);					// получить структуру по серийнику
	bool writeStructsToFile(uint64_t serialNumber, bool isChangeAllFiles = false);				// записать измененные структуры	
	bool removeFileConfiguration(uint64_t serialNumber);																// удаление файла конфигураций
	
	// вспомогательные функции
	CString getSerialNumber(CString& nameFile);													// получить серийный номер из названия файла
	_structCfg7xxx* getPtrCurStructCfg();														// получить указатель на текущую конфигурационную структуру
	CString makerStringUpper(CString& sMakeToUp);												// получить верхний регистр

	// для работы с map
	std::map< CString, _structCfg7xxx >::iterator checkStructInMap(CString& sSerialNumber);			// получение итератора для доступа к конфигурационной структуре
	std::map< CString, _structCfg7xxx >* getPtrMapStruct();											// указатель на map со структурами
	void removeStructFromMap(uint64_t serialNumber);											// удаление элемента из map по серийнику

	// работа с табами
	void addNumberModifiedItemTab(int number, const std::vector<CompareParametres>& inVector);		// добавить номер измененного таба
	void removeNumberModifiedItemTab(int number);												// удалить номер таба	
	BOOL checkItemTabForModify(int number);														// проверить изменялся ли таб

	// проверка изменений
	BOOL checkModifyTabs();																		// проверить есть ли хоть одно устройство с измененными свойствами
	BOOL checkModifyDevice();																	// проверить есть ли изменение свойств по текущему устройству

	void changeParamModify(int number, std::vector<CompareParametres>& inVector);						// изменились значения в окне свойств


	void setSerialParent(unsigned long long inSerialParent);									// серийник родителя
	void setFlagMaster(bool flag);																// установить флаг, что текущее устройство мастер

	HWND getHwndWindowDistanation();
	void SetHwndWindowDistanation(HWND inHwnd);

	CString& getMsgError();
	void setMsgError(CString& inErr);

//	future<void>* stopThreadWrite();															// для проверки остановки потока записи

	statusWorkThreadWrite getStatusThread() const;
	void setStatusThread(const statusWorkThreadWrite newStatus);

	void setPromiseEndThreadWrite();

	void setPathCfgDevice(const wchar_t* sPath);
	CString getPathCfgDevice();

	void setNodeDevice(long numNode);															// номер ноды, для точного обращения к файлу в случае нескольких осей у датчика
	long getNodeDevice();

	void setFlagEndThreadCompare(bool inFlag);													// номер ноды, для точного обращения к файлу в случае нескольких осей у датчика
	bool getFlagEndThreadCompare();

	void sendMessageRunData(uint64_t& serial);	

	void setUpdateType(const UpdateType type);
	UpdateType getUpdateType() const;

	bool CheckSettings();
private:
	std::map < CString, _structCfg7xxx > m_mapWithStructCfg;											// map со структурами свойств	
	_structCfg7xxx* m_pCurStructCfg;															// указатель на текущую структуру	

	unsigned long long m_serialParentCurItem;													// родитель текущего элемента	
	long m_numNode;

	CString m_msgError;																			// текст ошибки
	CString m_sPathCfg;
	
	HWND m_hWindowForMessage;																	// hwnd окна для отправки сообщения
	bool m_flagEndThreadCompare;
	bool m_isMaster;

	UpdateType m_typeUpdate;																	

private:
//	promise<void> m_promiseEndThreadWrite;
	std::atomic<statusWorkThreadWrite> m_statusThreadWrite;

public:
	CXMLWorker m_xmlWorker;
	HWND m_hMainHwnd;
	void* m_object;		
};
//-----------------------------------------------------------------------------------------------------------------------------
void funcCloseWindowProperties(CCFGWorker* myClass);
void funcThreadWrite(CCFGWorker* myClass, uint64_t serialNumber, bool isChangeAllFiles);
//-----------------------------------------------------------------------------------------------------------------------------