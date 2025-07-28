#include "stdafx.h"
#include "CFGWorker.h"
#include <chrono>
#include <thread>
#include "source\GUI\user_messages.hpp"
#include <ZET7xxx/include/base/ZET7xxxFunctions.h>
#include <mutex>
#include "source/algo/7xxx/7xxx/device/config/config_7xxx.hpp"
//-----------------------------------------------------------------------------------------------------------------------------
std::map< uint64_t, std::recursive_mutex > g_mutexMap;
//-----------------------------------------------------------------------------------------------------------------------------
CCFGWorker::CCFGWorker()
	: m_pCurStructCfg(nullptr)
	, m_hWindowForMessage(nullptr)
	, m_msgError(_T(""))
	, m_sPathCfg(_T(""))
	, m_numNode(0)
	, m_serialParentCurItem(0)
	, m_flagEndThreadCompare(false)
	, m_statusThreadWrite(statusEnd)
	, m_isMaster(false)
{	
}
//-----------------------------------------------------------------------------------------------------------------------------
CCFGWorker::~CCFGWorker()
{
}
//-----------------------------------------------------------------------------------------------------------------------------
CString CCFGWorker::getSerialNumber(CString& nameFile)
{	
	CString sSerial(_T(""));
	int posNumber = nameFile.Find(_T("№")) + 1;
	int posPoint = nameFile.Find(_T("_"));

	return nameFile.Mid(posNumber, posPoint - posNumber);
}
//-----------------------------------------------------------------------------------------------------------------------------
_structCfg7xxx* CCFGWorker::getStructCfg7xxx(uint64_t serialNumber, bool isRefresh)
{
	if (serialNumber <= 0)
	{
		::PostMessage(getHwndWindowDistanation(), WM_VIEW_ERROR, NULL, NULL);
		return nullptr;
	}

	std::lock_guard<std::recursive_mutex> myGuard(g_mutexMap[serialNumber + m_numNode]);

	bool isOk(true);

	_structCfg7xxx curStruct;

	CString sFileNameDscr(_T(""));
	CString sFileNameDat(_T(""));

	CString sConfigPath(_T(""));
	if (m_sPathCfg != _T(""))
		sConfigPath = m_sPathCfg;
	else
	{
		sConfigPath = zpGetDirConfig().c_str();
		sConfigPath.Append(_T("ZET7xxx\\Configs"));
	}

	CString sConfigFile(_T(""));
	sConfigFile.Format(L"0x%I64x", serialNumber);
	
	// если есть нода, то дописываем ее к серийнику
	if (m_numNode > 1)
		sConfigFile.AppendFormat(_T("_%03d"), m_numNode);

	// проверяем, может уже есть данная структура в map
	auto it = checkStructInMap(sConfigFile);
	if (it != m_mapWithStructCfg.end())
			m_mapWithStructCfg.erase(it);

	CString sFindFile = sConfigPath + _T("\\*.*");

	// находим файлы dat и dsc
	WIN32_FIND_DATA win32_find_data;
	
	int countAttempt(30);				// пробуем 30 раз найти файлы (получится 30 сек) 
	// если кол-во файлов в директории 7xxx будет меняться
	// значит происходит первое включение modbusZetlab, много датчиков, следовательно ждем подольше
	// если кол-во файлов не меняется в течении 3 секунд, значит все загружено, а нужных нам может и не быть
	// в этом случае выходим из цикла и просим дозагрузки
	int countFilesOld(0);
	int countFilesNew(0);
	int countTime(0);

	bool isDatProblem(false);
	bool isDscrProblem(false);
	try
	{
		for (int i = 0; i < countAttempt; ++i)
		{
			HANDLE hFind = FindFirstFile(sFindFile, &win32_find_data);
			countFilesNew = 0;

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					++countFilesNew;
					CString sFileName(CString(win32_find_data.cFileName));
					if (!(win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
						if (makerStringUpper(sFileName).Find(makerStringUpper(sConfigFile)) != -1)
						{
							if (makerStringUpper(sFileName).Find(_T(".DAT")) != -1)
								sFileNameDat = sConfigPath + _T("\\") + sFileName;								
							
							if (makerStringUpper(sFileName).Find(_T(".DSCR")) != -1)							
								sFileNameDscr = sConfigPath + _T("\\") + sFileName;						

							if (!sFileNameDat.IsEmpty() && !sFileNameDscr.IsEmpty())
								break;
						}
					}
				} while (FindNextFile(hFind, &win32_find_data));
				FindClose(hFind);
			}

			if (i == 0)
				countFilesOld = countFilesNew;

			isDatProblem = sFileNameDat.IsEmpty();
			isDscrProblem = sFileNameDscr.IsEmpty();

			if (isDatProblem || isDscrProblem)
			{
				// каждые три секунды проверяем
				if (countTime == 3)
				{
					countTime = 0;
					if (countFilesOld != countFilesNew)
						countFilesOld = countFilesNew;
					else				
						break;					
				}
				
				std::this_thread::sleep_for(std::chrono::seconds(1));				
				++countTime;
			}
			else			
				break;			
		}

		if (isDatProblem || isDscrProblem)
			throw MyException("Files .dat or .dscr not Found");

		// открываем файлы и заполянем структуру
		FILE* pFile(nullptr);

		if (zfOpenFile(sFileNameDat, _T("rb"), &pFile) == 0)
		{
			int er1 = fseek(pFile, 0, SEEK_END);
			curStruct._sizeDev = ftell(pFile);
			int er2 = fseek(pFile, 0, SEEK_SET);

			curStruct._vDevStruct.resize(curStruct._sizeDev);

			fread(curStruct._vDevStruct.data(), sizeof(BYTE), curStruct._sizeDev, pFile);
			curStruct._vDevStructOriginal.assign(curStruct._vDevStruct.begin(), curStruct._vDevStruct.end());

			zfCloseFile(pFile);
			pFile = nullptr;
		}

		if (zfOpenFile(sFileNameDscr, _T("rb"), &pFile) == 0)
		{
			fseek(pFile, 0, SEEK_END);
			curStruct._sizeDefine = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			curStruct._vDefineStruct.resize(curStruct._sizeDefine);

			fread(curStruct._vDefineStruct.data(), sizeof(BYTE), curStruct._sizeDefine, pFile);
			zfCloseFile(pFile);
			pFile = nullptr;
		}

		isDatProblem = curStruct._vDevStruct.empty();
		isDscrProblem = curStruct._vDefineStruct.empty();					
	
		if (isDatProblem || isDscrProblem)
			throw MyException("Don't open files or empty files");
		
		curStruct._serialParent = m_serialParentCurItem;
		curStruct._isMaster = m_isMaster;
		curStruct._object = m_object;
		auto myPair = m_mapWithStructCfg.emplace(std::make_pair(sFileNameDat, curStruct));
		m_pCurStructCfg = &myPair.first->second;
//		m_pCurStructCfg = &m_mapWithStructCfg.at(sFileNameDat);
		return &myPair.first->second;		
	}
	catch (MyException &exception)
	{
		if (isDatProblem && isDscrProblem)
			setUpdateType(UpdateType::update_all);
		else if (isDatProblem)
			setUpdateType(UpdateType::update_dat);
		else
			setUpdateType(UpdateType::update_dscr);

		m_msgError = exception.what();
		std::thread thr(funcCloseWindowProperties, this);
		thr.detach();		
		return nullptr;
	}		
}
//-----------------------------------------------------------------------------------------------------------------------------
bool CCFGWorker::writeStructsToFile(uint64_t serialNumber, bool isChangeAllFiles/* = false*/)
{
	bool isOk(true);
	
	std::thread thr(funcThreadWrite, this, serialNumber, isChangeAllFiles);
	thr.detach();

	return isOk;
}
//-----------------------------------------------------------------------------------------------------------------------------
bool CCFGWorker::removeFileConfiguration(uint64_t serialNumber)
{
	bool isOk(true);

	if (serialNumber <= 0)
		return false;

	CString sConfigPath(L"");
	if (m_sPathCfg != L"")
		sConfigPath = m_sPathCfg;
	else
	{
		sConfigPath = zpGetDirConfig().c_str();
		sConfigPath.Append(L"ZET7xxx\\Configs");
	}

	CString sConfigFile(L"");
	sConfigFile.Format(L"0x%I64x", serialNumber);

	// если есть нода, то дописываем ее к серийнику
	if (m_numNode > 1)
		sConfigFile.AppendFormat(L"_%03d", m_numNode);

	CString sFindFile = sConfigPath + _T("\\*.*");

	// находим файлы dat и dsc
	WIN32_FIND_DATA win32_find_data;

	CString sFileNameDat(L"");
	HANDLE hFind = FindFirstFile(sFindFile, &win32_find_data);
			
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString sFileName(CString(win32_find_data.cFileName));
			if (!(win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (makerStringUpper(sFileName).Find(makerStringUpper(sConfigFile)) != -1)
				{
					if (makerStringUpper(sFileName).Find(_T(".DAT")) != -1)
						sFileNameDat = sConfigPath + _T("\\") + sFileName;						
					
					if (!sFileNameDat.IsEmpty())
						break;
				}
			}
		} while (FindNextFile(hFind, &win32_find_data));
		FindClose(hFind);
	}

	if (!sFileNameDat.IsEmpty())
		isOk = DeleteFile(sFileNameDat) != 0;
	else
		isOk = false;
		
	return isOk;
}
//-----------------------------------------------------------------------------------------------------------------------------
std::map< CString, _structCfg7xxx >::iterator CCFGWorker::checkStructInMap(CString& sSerialNumber)
{
	std::map< CString, _structCfg7xxx >::iterator itResult(m_mapWithStructCfg.end());
	for (auto it = m_mapWithStructCfg.begin(); it != m_mapWithStructCfg.end(); ++it)
		if (it->first.Find(sSerialNumber) != -1)
		{
			itResult = it;
			break;
		}

	return itResult;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::removeStructFromMap(uint64_t serialNumber)
{
	bool isNeedRunData(true);
	CString sSerial(_T(""));
	sSerial.Format(_T("0x%I64x"), serialNumber);

	CString sNode(L"");
	long numNode = getNodeDevice();
	if (numNode >= 1)
		sNode.AppendFormat(L"_%03d.", numNode);

	for (auto it = m_mapWithStructCfg.begin(); it != m_mapWithStructCfg.end(); ++it)
		if (it->first.Find(sSerial) != -1 && it->first.Find(sNode) != -1)
		{
			if (m_mapWithStructCfg.size() > 1)
			{
				// проверим есть ли изменения в линии
				// в которой находится данный датчик				
				bool isFlag(false);
				for (auto& it2 : m_mapWithStructCfg)
				{
					isFlag = it->first != it2.first;					// не проверять тот же элемент
					if (isFlag)
					{
						isFlag = ((it->second._isMaster && serialNumber == it2.second._serialParent) ||					// если мастер, то проверяем по переданному серийнику
								 (!it->second._isMaster && it->second._serialParent == it2.second._serialParent)) &&	// если не мастер, то по серийникам родителей
								 !it2.second._isRunData;

						if (isFlag)
						{
							isNeedRunData = false;						// пока не запускаем данные
							break;
						}
					}
				}					
			}

			if (isNeedRunData)
			{
				uint64_t serial = it->second._isMaster ? serialNumber : it->second._serialParent;
				sendMessageRunData(serial);						
			}

			it->second._isRunData = true;

// 			m_mapWithStructCfg.erase(it);
// 			m_pCurStructCfg = nullptr;
			break;
		}	
}
//-----------------------------------------------------------------------------------------------------------------------------
_structCfg7xxx* CCFGWorker::getPtrCurStructCfg()
{
	return m_pCurStructCfg;
}
//-----------------------------------------------------------------------------------------------------------------------------
BOOL CCFGWorker::checkItemTabForModify(int number)
{
	if (m_pCurStructCfg == nullptr)
		return FALSE;

	return m_pCurStructCfg->_mParamModify.find(number) != m_pCurStructCfg->_mParamModify.end() ? TRUE : FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::addNumberModifiedItemTab(int number, const std::vector<CompareParametres>& inVector)
{
	if (m_pCurStructCfg == nullptr)
		return;

		//	m_pCurStructCfg->_setNumItemTab.insert(number);
	m_pCurStructCfg->_mParamModify.emplace(number, inVector);
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::removeNumberModifiedItemTab(int number)
{
	if (m_pCurStructCfg == nullptr)
		return;

	auto it = m_pCurStructCfg->_mParamModify.find(number);
	if (it != m_pCurStructCfg->_mParamModify.end())
		m_pCurStructCfg->_mParamModify.erase(it);
}
//-----------------------------------------------------------------------------------------------------------------------------
BOOL CCFGWorker::checkModifyTabs()
{
	if (m_mapWithStructCfg.empty())
		return FALSE;

	for (auto it : m_mapWithStructCfg)
		if (!it.second._mParamModify.empty())
			return TRUE;

	return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------------
BOOL CCFGWorker::checkModifyDevice()
{
	if (m_pCurStructCfg == nullptr)
		return FALSE;

	if (m_pCurStructCfg->_mParamModify.empty())
		return FALSE;

	return TRUE;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::changeParamModify(int number, std::vector<CompareParametres>& inVector)
{
	if (!m_pCurStructCfg)
		return;
	auto it = m_pCurStructCfg->_mParamModify.find(number);
	if (it != m_pCurStructCfg->_mParamModify.end())
		//std::swap(inSet, it->second);
	{		
		it->second.clear();
		it->second.assign(inVector.begin(), inVector.end());
	}
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setSerialParent(unsigned long long inSerialParent)
{
	m_serialParentCurItem = inSerialParent;
}
//-----------------------------------------------------------------------------------------------------------------------------
CString CCFGWorker::makerStringUpper(CString& sMakeToUp)
{
	CString temp(sMakeToUp);

	return temp.MakeUpper();
}
//-----------------------------------------------------------------------------------------------------------------------------
HWND CCFGWorker::getHwndWindowDistanation()
{
	return m_hWindowForMessage;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::SetHwndWindowDistanation(HWND inHwnd)
{
	m_hWindowForMessage = inHwnd;
}
//-----------------------------------------------------------------------------------------------------------------------------
CString& CCFGWorker::getMsgError()
{
	return m_msgError;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setMsgError(CString& inErr)
{
	m_msgError = inErr;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setStatusThread(const statusWorkThreadWrite newStatus)
{
	m_statusThreadWrite = newStatus;
}
//-----------------------------------------------------------------------------------------------------------------------------
statusWorkThreadWrite CCFGWorker::getStatusThread() const
{
	return m_statusThreadWrite;
}
//-----------------------------------------------------------------------------------------------------------------------------
std::map< CString, _structCfg7xxx >* CCFGWorker::getPtrMapStruct()
{
	return &m_mapWithStructCfg;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setPromiseEndThreadWrite()
{
//	m_promiseEndThreadWrite.set_value();
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setPathCfgDevice(const wchar_t* sPath)
{
	m_sPathCfg = sPath;
}
//-----------------------------------------------------------------------------------------------------------------------------
CString CCFGWorker::getPathCfgDevice()
{
	return m_sPathCfg;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setNodeDevice(long numNode)
{
	m_numNode = numNode;
}
//-----------------------------------------------------------------------------------------------------------------------------
long CCFGWorker::getNodeDevice()
{
	return m_numNode;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setFlagEndThreadCompare(bool inFlag)
{
	m_flagEndThreadCompare = inFlag;
}
//-----------------------------------------------------------------------------------------------------------------------------
bool CCFGWorker::getFlagEndThreadCompare()
{
	return m_flagEndThreadCompare;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setFlagMaster(bool flag)
{
	m_isMaster = flag;
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::sendMessageRunData(uint64_t& serial)
{
	long rightPart = serial & 0xffffffff;
	long leftPart = serial >> 32 & 0xffffffff;
	PostMessage(m_hMainHwnd, WM_ENABLE_DATA, leftPart, rightPart);
}
//-----------------------------------------------------------------------------------------------------------------------------
void CCFGWorker::setUpdateType(const UpdateType type)
{
	m_typeUpdate = type;
}
//-----------------------------------------------------------------------------------------------------------------------------
UpdateType CCFGWorker::getUpdateType() const
{
	return m_typeUpdate;
}
//-----------------------------------------------------------------------------------------------------------------------------
// функция потока закрытия окна свойств в случае ошибки
//-----------------------------------------------------------------------------------------------------------------------------
void funcCloseWindowProperties(CCFGWorker* myClass)
{
	if (myClass->getHwndWindowDistanation() && myClass->getMsgError() != _T(""))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));	// подождем немного
 //		AfxMessageBox(myClass->getMsgError());
 		::PostMessage(myClass->getHwndWindowDistanation(), WM_UPDATE_DATA, static_cast<WPARAM>(myClass->getUpdateType()), NULL);
	}	
}
//-----------------------------------------------------------------------------------------------------------------------------
// функция потока записи
//-----------------------------------------------------------------------------------------------------------------------------
void funcThreadWrite(CCFGWorker* myClass, uint64_t serialNumber, bool isChangeAllFiles)
{	
	std::lock_guard<std::recursive_mutex> myGuard(g_mutexMap[serialNumber + myClass->getNodeDevice()]);

	structForCheckFile structCheckFile;
	WIN32_FILE_ATTRIBUTE_DATA attrib;
	std::vector< structForCheckFile > vCheckFile;

	CString sFileNameDat(_T(""));

	CString sConfigPath(_T(""));
	if (myClass->getPathCfgDevice() != _T(""))
		sConfigPath = myClass->getPathCfgDevice();
	else
	{
		sConfigPath = zpGetDirConfig().c_str();
		sConfigPath.Append(_T("ZET7xxx\\Configs"));
	}

	CString sFindFile = sConfigPath + _T("\\*.*");

	CString sSerialForCheckWrite(_T(""));

	sSerialForCheckWrite.Format(_T("%I64x"), serialNumber);

	auto mapStructCfg = myClass->getPtrMapStruct();
		
	// находим файлы dat
	WIN32_FIND_DATA win32_find_data;
	HANDLE hFind = FindFirstFile(sFindFile, &win32_find_data);

	CString sNode(L"");
	long numNode = myClass->getNodeDevice();
	if (numNode >= 1)
		sNode.AppendFormat(L"_%03d.", numNode);

	// процесс записи
	myClass->setStatusThread(statusWtire);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString sFileName(CString(win32_find_data.cFileName));
			if (!(win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// находим файл в map
				auto findStruct = mapStructCfg->find(sConfigPath + _T("\\") + sFileName);
				if (findStruct != mapStructCfg->end() && findStruct->first.Find(sNode) != -1)
				{
					FILE* pFile(nullptr);

					bool flagCheckOneChangeFile = !isChangeAllFiles &&											// перезаписываем только один файл
						sFileName.Find(sSerialForCheckWrite) == -1;												// номера серийника нет в имени файла

					bool flagCheckParent =	isChangeAllFiles							&&						// перезаписываем не только один файл
											findStruct->second._serialParent != 0		&&						// поле parent не пустое 
											findStruct->second._serialParent != serialNumber;					// серийник родителя не совпадает с мастером

					if (flagCheckOneChangeFile || flagCheckParent)
						continue;

					if (!findStruct->second._mParamModify.empty() && zfOpenFile(findStruct->first, _T("wb"), &pFile) == 0)
					{		
						fwrite(findStruct->second._vDevStruct.data(), sizeof(BYTE), findStruct->second._sizeDev, pFile);
						zfCloseFile(pFile);
						PostMessage(myClass->m_hMainHwnd, WM_ENABLE_DEVICE, 1, (LPARAM)(LPVOID)findStruct->second._object);
						// запомнили время записи
						// кидаем данные в вектор для потока проверки
						GetFileAttributesEx(findStruct->first,
							GetFileExInfoStandard, &attrib);
						structCheckFile._lastWriteTime = attrib.ftLastWriteTime;
						structCheckFile._nameFile = findStruct->first;
						structCheckFile._object_ptr = findStruct->second._object;						
						vCheckFile.emplace_back(structCheckFile);				
						
						pFile = nullptr;						
					}

					if (!isChangeAllFiles)
						break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (FindNextFile(hFind, &win32_find_data));
		FindClose(hFind);
	}

	// закончили запись, начинам сравнивать	
	myClass->setStatusThread(statusCompare);

	_structCfg7xxx tempStruct;
	size_t exam(0);
	auto sleepTime = std::chrono::milliseconds(20);
	std::chrono::milliseconds timeExam = std::chrono::milliseconds(0);

	bool isProblem(false);
	// берем серийник родителя
	unsigned long long serialParent(0);

	while (vCheckFile.size() != exam)			// ждем две минуты если проблема
	{
		if (myClass->getFlagEndThreadCompare() || timeExam > std::chrono::milliseconds(10000))		// если в течении 10 секунд ни одна конфигурация не изменилась выходим из цикла
		{
			isProblem = true;
			break;
		}
		// перебираем вектор записанных файлов
		// проверяем перезаписались ли они		
		for (auto &it : vCheckFile)
		{
			if (GetFileAttributesEx(it._nameFile,
				GetFileExInfoStandard, &attrib) != FALSE)
			{
				if (CompareFileTime(&attrib.ftLastWriteTime, &it._lastWriteTime) == 1)
				{
					it._lastWriteTime = attrib.ftLastWriteTime;
					// файл был перезаписан!			
					FILE* pFile(nullptr);

					// читаем dat файл
					if (zfOpenFile(it._nameFile, _T("rb"), &pFile) == 0)
					{
						int er1 = fseek(pFile, 0, SEEK_END);
						tempStruct._sizeDev = ftell(pFile);
						int er2 = fseek(pFile, 0, SEEK_SET);

						tempStruct._vDevStruct.resize(tempStruct._sizeDev);

						fread(tempStruct._vDevStruct.data(), sizeof(BYTE), tempStruct._sizeDev, pFile);
						zfCloseFile(pFile);
						pFile = nullptr;
					}
									
					using namespace zet7xxx;
					auto itOld = mapStructCfg->find(it._nameFile);
					if (!tempStruct._vDevStruct.empty() && itOld != mapStructCfg->end())
					{
						// начинаем сравнивать
						bool isOk(true);

						if (!itOld->second._mParamModify.empty())
						{
							typedef zetlab::_7xxx::device::config::page::param param;
							long posDev;
							std::vector< std::wstring > extra;
							std::wstring sValueOld, sValueNew;
						
							bool isEqual(true);
							extra.reserve(10);
							
							for (auto& itComp : itOld->second._mParamModify)
							{
								if (!itComp.second.empty())
								{
									for (auto& itVec : itComp.second)
									{
										posDev = static_cast<BYTE*>(itVec._addr) - itOld->second._vDevStruct.data();
										
										param param_(L"", itVec._type, itVec._addr, itVec._addr2, itVec._size, itVec._editable, L"", 0, itVec._structType);

										// страрое значение															
										sValueOld = param_.getValue(extra);
										
										// новое значение
										// передаем новый адрес в параметры
										param_.setAddr(tempStruct._vDevStruct.data() + posDev);
																			
										sValueNew = param_.getValue(extra);

										if (sValueNew != sValueOld)
										{
											isOk = false;
											
											// восстанавливаем оригинальные данные, если они не записались
											std::wstring sOriginal;
											param_.setAddr(itOld->second._vDevStructOriginal.data() + posDev);
											
											sOriginal = param_.getValue(extra);

											param_.setAddr(tempStruct._vDevStruct.data() + posDev);

											param_.setValue(sOriginal);
										}									
									}									
								}
							}
						}

						if (!isOk)
						{
							// записываем измененную структуру
							if (zfOpenFile(it._nameFile, _T("wb"), &pFile) == 0)
							{
								fwrite(tempStruct._vDevStruct.data(), sizeof(BYTE), tempStruct._sizeDev, pFile);
								zfCloseFile(pFile);					

								pFile = nullptr;
							}
						}

						int flagView(0);
						flagView = isOk ? 0 : 2;
//						SendMessage(myClass->m_hMainHwnd, WM_ENABLE_DEVICE, flagView, (LPARAM)(LPVOID)it._object_ptr);

						// запомним серийник мастера для отправки message на чтение данных
						serialParent = itOld->second._isMaster ? serialNumber : itOld->second._serialParent;
	
						PostMessage(myClass->m_hMainHwnd, WM_ENABLE_DEVICE, flagView, (LPARAM)(LPVOID)it._object_ptr);

						itOld->second._isRunData = true;

						timeExam = std::chrono::milliseconds(0);				// сбрасываем проверочный временной интервал
					}

					++exam;
				}
			}
		}
		
		timeExam += sleepTime;
		std::this_thread::sleep_for(sleepTime);
	}
		
	// сравение прошло
	auto clearMap = [&mapStructCfg, &vCheckFile, &serialParent, &serialNumber, &myClass](){
		for (auto & it : vCheckFile)
		{
			auto it1 = mapStructCfg->find(it._nameFile);
			if (it1 != mapStructCfg->end())
			{
				// если была проблема при записи конфигурации
				// нужно запустить чтение данных по линии, а значит запомнить серийник мастера
				if (serialParent == 0)
					serialParent = it1->second._isMaster ? serialNumber : it1->second._serialParent;

				PostMessage(myClass->m_hMainHwnd, WM_ENABLE_DEVICE, 2, (LPARAM)(LPVOID)it1->second._object);
				it1->second._isRunData = true;
//				mapStructCfg->erase(it1);
			}
		}
	};

	if (isProblem)
		clearMap();

	// отправляем сообщение на запуск чтения данных
	if (serialParent > 0)
	{
		bool isNeedRunData(true);
		CString sSerial(_T(""));
		sSerial.Format(_T("0x%I64x"), serialParent);
		// нужно проверить есть ли изменения в этой линии
		if (!mapStructCfg->empty())
			for (auto& it : *mapStructCfg)
				if ((it.second._serialParent == serialParent || it.first.Find(sSerial) != -1) && !it.second._isRunData)
				{
					isNeedRunData = false;
					break;
				}

		if (isNeedRunData)
			myClass->sendMessageRunData(serialParent);
	}
		
	// конец потока
	std::this_thread::sleep_for(std::chrono::seconds(1)); // нужно чтобы object отправленный через SendMessage не пропал
	myClass->setStatusThread(statusEnd);
}
//-----------------------------------------------------------------------------------------------------------------------------
bool CCFGWorker::CheckSettings()
{
	for (auto &itt : m_pCurStructCfg->_mParamModify)
	{
		auto find = std::find_if(itt.second.begin(), itt.second.end(), [](CompareParametres data) {
			return data._structType == IDENTIFICATION_PAR_STRUCT;
		});

		if (find != itt.second.end())
		{
			BYTE* temp = static_cast<BYTE*>(find->_addr);
			CString strCheck(temp);
			if (!strCheck.IsEmpty())
			{
				for (auto &xmlTree : m_xmlWorker.m_mapDeviceForTree)
				{
					if (xmlTree.first._statusMaster == 1 &&
						xmlTree.first._identification == strCheck)
						return false;
				}
			}
		}
	}
	return true;
}
