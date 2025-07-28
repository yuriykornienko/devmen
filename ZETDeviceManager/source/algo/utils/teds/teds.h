#pragma once

//////////////////////////////////////////////////////////////////////////
#include <Teds/teds_struct.h>
#include <mathZet.h>
//////////////////////////////////////////////////////////////////////////
#include <vector>					 

#define WM_STATE_TEDS		(WM_APP + 12)   // сообщить о состоянии считывания TEDS

class TEDS
{
public:
	TEDS();
	~TEDS();

	void SetParent				( HWND hwnd );

	/* Функция чтения данных */
	void readData				();

private:
	teds_zetlab::TYPE_EEPROM detectedDevice_	( long device );
	int parsingData_			( _In_ const std::vector<uint32_t> & byte );	
	teds_zetlab::TEMPLATE_ID findId_			();
	long fillData_				();

	uint32_t get_mask_			( uint32_t pos, uint32_t n );
	void getUnitById_			();
public:
	// 0 - Данные прочитаны 
	// 1 - Данные пустые 
	// 2 - не совпадает контрольная сумма 
	// 3 - не поддерживается данный датчик 
	// 4 - Данный шаблон не поддерживается
	//-1 - Ошибка открытия ZOpen
	//-2 - Ошибка чтения TEDS
	//-3 - Датчик не поддерживается
	//-4 - ошибка чтения
	long getStatusRead(){ return m_statusRead; }

	void setChan		( uint32_t chan );
	void setType		( long Type );
	void setDSP			( long DSP );
	void setAdress		( uint32_t adress );
	teds_zetlab::TEMPLATE_ID getTemplateID();
	CString getSensitivity	();

	CString getComment	();
	CString getUnitSense();
	CString getUnit();
	//--------------- Поля ------------------
private:
	std::vector<uint32_t> m_DataFromTedsPars;	// Распарсенные данные с ТЕДС
	std::vector<uint32_t> m_DataFromTeds;		// ДАнные с ТЕДС
	uint32_t m_address;
	uint32_t m_chan;
	long m_Type;
	long m_DSP;
	HWND hWnd;
	long m_statusRead;
	bool m_bCheckSumm;

	teds_zetlab::TEMPLATE_ID m_tempalteId;
	teds_zetlab::TYPE_EEPROM m_typeEeprom;
	teds_zetlab::TemplateTEDS m_dataForTeds;
public:
};

