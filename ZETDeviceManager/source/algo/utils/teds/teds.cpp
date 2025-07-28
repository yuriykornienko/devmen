#include "stdafx.h"
#include <ADCINFO.h>
#include "TEDS.h"
#include <Zadc.h>
#include "resource\Phrases.h"

using namespace teds_zetlab;

TEDS::TEDS()
	: m_address(0x00)
	, m_typeEeprom( EEPROM_ERROR )
	, m_chan(0)
	, m_DSP(0)
	, m_Type(0)
	, m_bCheckSumm(true)
	, m_statusRead(0)
	, m_tempalteId( TEMPLATE_ERROR )
	, hWnd( nullptr )
{
}


TEDS::~TEDS()
{
}
void TEDS::SetParent(HWND hwnd)
{
	hWnd = hwnd;
}
void TEDS::readData()
{
	// Начинаем читать с первой страницы
	m_address = 0x00;

	if ( !ZOpen(m_Type, m_DSP) )
	{
		try
		{
			unsigned short	Data[256] = {0};
			if ( !ZOpenTEDS(m_Type, m_DSP, m_chan, &Data) )
			{
				m_typeEeprom = detectedDevice_( Data[0] & 0xFF );
				// Если данный датчик поддерживается то читаем данные
				if ( m_typeEeprom != EEPROM_ERROR )
				{
					if (!m_DataFromTeds.empty())
						m_DataFromTeds.clear();

					m_DataFromTeds.reserve(32);

					long size(10);
					unsigned short	DataOut[4];
					
					for (UINT k = 0; k < 8; k++)
					{
						// Читаем данные с тедс
						if (ZReadTEDS(m_Type, m_DSP, m_address, &DataOut, &size) == 0)
						{
							for (auto i : DataOut)
							{
								m_DataFromTeds.push_back(  i & 0xFF );
								m_DataFromTeds.push_back( (i & 0xFF00) >> 8 );
							}
							m_address += 8;
							m_statusRead = 0;
						}
						// Если произошла ошибка то сообщаем об этом!
						else
						{
							if ( k == 0 )
								m_statusRead = -4;
							break;
						}
					}
				}
				// Если нет то выдаем ошибку
				else
				{
					m_statusRead = -3;
				}
			}
			else
			{
				m_statusRead = -2;
			}
			//---------------------------------------------------
			
		}
		catch (...)
		{
			m_statusRead = -1;
		}
	}
	else
	{
		m_statusRead = -1;
	}

	if (ZCloseTEDS(m_Type, m_DSP) != 0)
	{
		// Может не пройти с первого раза из-за конфликта доступа с другими программами
		for (int try_number = 0; try_number < 10; try_number++)
		{
			Sleep(100);
			if (ZCloseTEDS(m_Type, m_DSP) == 0)
				break;
		}
	}

	ZClose		( m_Type, m_DSP );

	m_statusRead = fillData_();

	Sleep(100);
	::PostMessage(hWnd, WM_STATE_TEDS,NULL,NULL);
}
int TEDS::parsingData_(_In_ const std::vector<uint32_t> & byte)
{
	if (byte.empty())
		return 4;

	//выходной вектор
	if (!m_DataFromTedsPars.empty())
		m_DataFromTedsPars.clear();

	m_DataFromTedsPars.resize(byte.size());

	int count_byte_all	( 0 );
	int index			( 0 );
	int count_byte		( 0 );
	int count_rest		( 0 );
	int shift_mask		( 0 );
	int shift_byte		( 0 );
	int iter			( 0 );
	
	uint32_t size(16);

	for (unsigned int i : byte)
	{
		count_byte_all += i;
	}

	// CheckCRC8
	uint32_t countFF(0);
	uint32_t uCheckSum(0);
	size = m_DataFromTeds.size();
	std::vector<uint32_t> _DataFromTeds( m_DataFromTeds );
	for (UINT i = 0; i < size; i++)
	{	
		uCheckSum += _DataFromTeds[i];

		if ( _DataFromTeds[i] == 0x00 )
			countFF++;
	}
	// Датчик пустой
	if ( countFF > size - 2 )
	{
		return 1;
	} 
	// Ошибка контрольной суммы
	if ( m_bCheckSumm && ( uCheckSum & 0xFF ) != 0x00 )
	{
		return 2;
	}
	// Удаляем данные контрольной суммы
	if ( m_bCheckSumm )
	{
		for (uint32_t i = 0; i < size - 4; i += 32)
			_DataFromTeds.erase( _DataFromTeds.begin() + i-- );
	}
	
	// Парсим полученные данные
	for (UINT i = 0; i < byte.size(); i++)
	{
		count_rest = byte[i];
		count_byte += byte[i];
		m_DataFromTedsPars[i] = ( _DataFromTeds[index] & get_mask_( shift_byte, count_rest > 8 ? 8 : count_rest ) ) >> shift_byte;
		count_rest -= (8 - shift_byte);
		int nop(0);
		while (count_byte >= 8)
		{
			index++;

			shift_mask = (count_rest / 8) >= 1 ? 8 : count_rest % 8;
			uint32_t mask = get_mask_(0, shift_mask);
			uint32_t tmp(byte[i] - count_rest);
			m_DataFromTedsPars[i] = m_DataFromTedsPars[i] | ( ( _DataFromTeds[index] & mask ) << tmp );
			count_byte -= 8;
			count_rest -= 8;
		}
		shift_byte = count_byte % 8;
	}

	return 0;
}
CString TEDS::getSensitivity()
{
	CString sVal;
	double dVal;
	RoundToN_SignificantDigits( m_dataForTeds.sensitivity._dData, dVal, 4, sVal );

	return sVal;
}
CString TEDS::getComment()
{
	return m_dataForTeds.name._sData;
}
CString TEDS::getUnitSense()
{
	return m_dataForTeds.unitSense;
}
CString TEDS::getUnit() 
{
	return m_dataForTeds.unit;
}
void TEDS::setAdress(uint32_t address)
{
	m_address = address;
}
TEMPLATE_ID TEDS::getTemplateID()
{
	return m_tempalteId;
}
TYPE_EEPROM TEDS::detectedDevice_( long device )
{
	switch (device)
	{
		case 0x2d:	//DS2431
		{
			return EEPROM_2431;
			break;
		}
		case 0x14:	//DS2430Aa
		{
			return EEPROM_2430;
			break;
		}
		default:
			ASSERT( false );  // new memory
	}
	return EEPROM_ERROR;
}

#pragma region Вспомогательные функции
void TEDS::setChan(uint32_t chan)
{
	m_chan = chan;
}
void TEDS::setType(long Type)
{
	m_Type = Type;
}
void TEDS::setDSP(long DSP)
{
	m_DSP = DSP;
}
uint32_t TEDS::get_mask_(uint32_t pos, uint32_t n)
{
	return ~(~0 << n) << pos;
}

TEMPLATE_ID TEDS::findId_()
{		
	switch ( m_typeEeprom )
	{
		case teds_zetlab::EEPROM_2430:	
		{
			m_bCheckSumm = false;
			/*Read2430 accelerometer*/	
			auto _ret = parsingData_( { 2, 8, 2 } );
			if ( _ret == 0 && m_DataFromTedsPars[1] == TEMPLATE_ACCELEROMETR
				 && m_DataFromTedsPars[2] == 0 )
			{
				return TEMPLATE_ACCELEROMETR;
			}

			/*Read2430 microphone legacy*/
			_ret = parsingData_( { 66,8 } );
			if ( _ret == 0 && m_DataFromTedsPars[1] == TEMPLATE_MICROPHONE_LEGACY )
			{
				return TEMPLATE_MICROPHONE_LEGACY;
			}
			break;
		}
		case teds_zetlab::EEPROM_2431:	
		{
			m_bCheckSumm = true;
			/*Read2431 accelerometer*/
			auto _ret = parsingData_( { 66, 8,2 } );
			if ( _ret == 0 && m_DataFromTedsPars[1] == TEMPLATE_ACCELEROMETR
				 && m_DataFromTedsPars[2] == 0 )
			{
				return TEMPLATE_ACCELEROMETR;
			}

			_ret = parsingData_( { 66, 8,1 } );
			if ( _ret == 0 && m_DataFromTedsPars[1] == TEMPLATE_MICROPHONE_BUILTIN_PREAMPLIFIERS
				 && m_DataFromTedsPars[2] == 0 )
			{
				return TEMPLATE_MICROPHONE_BUILTIN_PREAMPLIFIERS;
			}
			break;
		}
		default:
			break;
	}
	

	return TEMPLATE_ERROR;
}

long TEDS::fillData_()
{
	/*
	* Все шаблоны находятся по адресу
	* new template - https://standards.ieee.org/content/dam/ieee-standards/standards/web/documents/tutorials/teds.pdf
	* old template - https://www.ni.com/ru-ru/support/documentation/supplemental/06/ieee-1451-4-sensor-templates-overview.html
	*/
	if ( m_statusRead != 0 )
		return m_statusRead;
	m_tempalteId = findId_();

	long _ret( 0 );	
	switch ( m_tempalteId )
	{
		case TEMPLATE_ACCELEROMETR : 
		{ 
			switch ( m_typeEeprom )
			{
				////////////////////////////////// ACCELEROMETR 2430 ////////////////////////////////////////
				case teds_zetlab::EEPROM_2430:
				{
					_ret = parsingData_(std::vector<uint32_t>
					{
						//Template
						2, 8, 2, 16, 8, 2, 6, 1, 1, 8, 5, 16, 15, 12, 11, 2, 1,
						//name
						7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
					} );
					m_dataForTeds.sensitivity = COL_REL_RES( m_DataFromTedsPars[3], 0.0000005, 0.00015 );
					m_dataForTeds.name = ASCII_TEDS( std::vector<uint32_t>( m_DataFromTedsPars.begin() + 17, m_DataFromTedsPars.end() ) );
					break;
				}
				///////////////////////////////////// ACCELEROMETR 2431 /////////////////////////////////////
				case teds_zetlab::EEPROM_2431:	
				{
					_ret = parsingData_(std::vector<uint32_t>
					{
						// Basic Teds
						14, 15, 5, 6, 24,
						// Template
						2, 8, 2, 16, 8, 2, 6, 1, 1, 8, 5, 16, 15, 12, 11, 2, 1,
						// User Data
						7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
					} );
					m_dataForTeds.sensitivity = COL_REL_RES( m_DataFromTedsPars[8], 0.0000005, 0.00015 );
					m_dataForTeds.name = ASCII_TEDS( std::vector<uint32_t>( m_DataFromTedsPars.begin() + 22, m_DataFromTedsPars.end() ) ); 
					break;
				}
				//////////////////////////////////////////////////////////////////////////
				default:
					break;
			}			   	

			break;
		}
		//////////////////////////////// MICROPHONE LEGACY 2430 //////////////////////////////////////////
		case TEMPLATE_MICROPHONE_LEGACY:
		{
			_ret = parsingData_(std::vector<uint32_t>
			{
				// Basic Teds
				12, 16, 5, 6, 24,
				//Template
				3, 8, 16, 16, 1, 9
			} );
			m_dataForTeds.sensitivity = COL_REL_RES( m_DataFromTedsPars[8], 100e-6, 0.0001 );
			m_dataForTeds.name = ASCII_TEDS();

			break;
		}
		case TEMPLATE_MICROPHONE_BUILTIN_PREAMPLIFIERS:
		{
			_ret = parsingData_( std::vector<uint32_t>
			{
				// Basic Teds
				14, 15, 5, 6, 24,
				//Template
				2, 8, 1 , 16, 8, 2, 1, 1
			} );
			m_dataForTeds.sensitivity = COL_REL_RES( m_DataFromTedsPars[8], 10e-6, 0.0001 );
			auto refFreq = COL_REL_RES( m_DataFromTedsPars[9], 0.35, 0.0175 );

			break;
		}
		case TEMPLATE_ERROR:
		{
			_ret = 4;
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		default:
			ASSERT( false ); // add new sensor
			break;
	}

	getUnitById_();

	return _ret;
}

void TEDS::getUnitById_()
{
	switch ( m_tempalteId )
	{
		case teds_zetlab::TEMPLATE_ERROR:
			m_dataForTeds.unit = L"";
			m_dataForTeds.unitSense = g_sUNIT_V;
			break;
		case teds_zetlab::TEMPLATE_ACCELEROMETR:
			m_dataForTeds.unit = g_sUNIT_MPERS2;
			m_dataForTeds.unitSense = g_sUNIT_V;
			break;
		case teds_zetlab::TEMPLATE_MICROPHONE_LEGACY:
		case teds_zetlab::TEMPLATE_MICROPHONE_BUILTIN_PREAMPLIFIERS:
			m_dataForTeds.unit = g_sUNIT_PA;
			m_dataForTeds.unitSense = g_sUNIT_V;
			break;				  
		default:
			ASSERT( false );   // add new sensor
			break;
	}
}

#pragma endregion