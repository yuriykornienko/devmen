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
	// ������ � ������� ������������
	_structCfg7xxx* getStructCfg7xxx(uint64_t serialNumber, bool isRefresh);					// �������� ��������� �� ���������
	bool writeStructsToFile(uint64_t serialNumber, bool isChangeAllFiles = false);				// �������� ���������� ���������	
	bool removeFileConfiguration(uint64_t serialNumber);																// �������� ����� ������������
	
	// ��������������� �������
	CString getSerialNumber(CString& nameFile);													// �������� �������� ����� �� �������� �����
	_structCfg7xxx* getPtrCurStructCfg();														// �������� ��������� �� ������� ���������������� ���������
	CString makerStringUpper(CString& sMakeToUp);												// �������� ������� �������

	// ��� ������ � map
	std::map< CString, _structCfg7xxx >::iterator checkStructInMap(CString& sSerialNumber);			// ��������� ��������� ��� ������� � ���������������� ���������
	std::map< CString, _structCfg7xxx >* getPtrMapStruct();											// ��������� �� map �� �����������
	void removeStructFromMap(uint64_t serialNumber);											// �������� �������� �� map �� ���������

	// ������ � ������
	void addNumberModifiedItemTab(int number, const std::vector<CompareParametres>& inVector);		// �������� ����� ����������� ����
	void removeNumberModifiedItemTab(int number);												// ������� ����� ����	
	BOOL checkItemTabForModify(int number);														// ��������� ��������� �� ���

	// �������� ���������
	BOOL checkModifyTabs();																		// ��������� ���� �� ���� ���� ���������� � ����������� ����������
	BOOL checkModifyDevice();																	// ��������� ���� �� ��������� ������� �� �������� ����������

	void changeParamModify(int number, std::vector<CompareParametres>& inVector);						// ���������� �������� � ���� �������


	void setSerialParent(unsigned long long inSerialParent);									// �������� ��������
	void setFlagMaster(bool flag);																// ���������� ����, ��� ������� ���������� ������

	HWND getHwndWindowDistanation();
	void SetHwndWindowDistanation(HWND inHwnd);

	CString& getMsgError();
	void setMsgError(CString& inErr);

//	future<void>* stopThreadWrite();															// ��� �������� ��������� ������ ������

	statusWorkThreadWrite getStatusThread() const;
	void setStatusThread(const statusWorkThreadWrite newStatus);

	void setPromiseEndThreadWrite();

	void setPathCfgDevice(const wchar_t* sPath);
	CString getPathCfgDevice();

	void setNodeDevice(long numNode);															// ����� ����, ��� ������� ��������� � ����� � ������ ���������� ���� � �������
	long getNodeDevice();

	void setFlagEndThreadCompare(bool inFlag);													// ����� ����, ��� ������� ��������� � ����� � ������ ���������� ���� � �������
	bool getFlagEndThreadCompare();

	void sendMessageRunData(uint64_t& serial);	

	void setUpdateType(const UpdateType type);
	UpdateType getUpdateType() const;

	bool CheckSettings();
private:
	std::map < CString, _structCfg7xxx > m_mapWithStructCfg;											// map �� ����������� �������	
	_structCfg7xxx* m_pCurStructCfg;															// ��������� �� ������� ���������	

	unsigned long long m_serialParentCurItem;													// �������� �������� ��������	
	long m_numNode;

	CString m_msgError;																			// ����� ������
	CString m_sPathCfg;
	
	HWND m_hWindowForMessage;																	// hwnd ���� ��� �������� ���������
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