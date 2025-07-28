#pragma once

//////////////////////////////////////////////////////////////////////////
#include <Teds/teds_struct.h>
#include <mathZet.h>
//////////////////////////////////////////////////////////////////////////
#include <vector>					 

#define WM_STATE_TEDS		(WM_APP + 12)   // �������� � ��������� ���������� TEDS

class TEDS
{
public:
	TEDS();
	~TEDS();

	void SetParent				( HWND hwnd );

	/* ������� ������ ������ */
	void readData				();

private:
	teds_zetlab::TYPE_EEPROM detectedDevice_	( long device );
	int parsingData_			( _In_ const std::vector<uint32_t> & byte );	
	teds_zetlab::TEMPLATE_ID findId_			();
	long fillData_				();

	uint32_t get_mask_			( uint32_t pos, uint32_t n );
	void getUnitById_			();
public:
	// 0 - ������ ��������� 
	// 1 - ������ ������ 
	// 2 - �� ��������� ����������� ����� 
	// 3 - �� �������������� ������ ������ 
	// 4 - ������ ������ �� ��������������
	//-1 - ������ �������� ZOpen
	//-2 - ������ ������ TEDS
	//-3 - ������ �� ��������������
	//-4 - ������ ������
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
	//--------------- ���� ------------------
private:
	std::vector<uint32_t> m_DataFromTedsPars;	// ������������ ������ � ����
	std::vector<uint32_t> m_DataFromTeds;		// ������ � ����
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

