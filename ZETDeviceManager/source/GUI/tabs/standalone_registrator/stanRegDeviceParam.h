// Здесь описаны стандартные структуры хранящие настройки устройств используемые для работы автономного регистратора

#ifndef STAND_REG_DEVICE_PARAM_HEADER
#define STAND_REG_DEVICE_PARAM_HEADER

#include <CALIBR.h>

struct currCopyProgressState
{
	currCopyProgressState()
		: _name(L"")
		, _currSendCurrFSize(100)
		, _rate(0)
	{
	}
	CString _name;
	long    _currSendCurrFSize;
	unsigned long _rate;	
};

#define ZET_DEVICE_210 10
#define ZET_DEVICE_110 12
#define ZET_DEVICE_A17U8  14
#define ZET_DEVICE_A17U2  15
#define ZET_DEVICE_220 16
#define ZET_DEVICE_230 17
#define ZET_DEVICE_ZET048 18
#define ZET_DEVICE_240 19

typedef struct _PARAMEXT		//параметры сохраняются на флэшке при включении и выключении питания
{
	unsigned int sizeSelf;		// размер этой структуры
	char Signature[16];			// сигнатура
	short int 
		contrast,		//контрастность
		timereg,		//время записи
		porog,			//порог срабатывания 1/20 - 20/20 от верхнего уровня // порог срабатывания авто записи
		typecalc,		//тип вычисляемых величин 0 - пик, 1 - скз, 2 - пост
		fast,			//0-slow 1-fast (8 Гц) индикация 
		write,			//1-включить запись, -1 выключить запись
		enazap,			//0 - запись по включению, 1 - по порогу,
		digana,			//0 - 1, цифровой - аналоговый запуск, 2 - по времени
		numbit,			// номер бита или канала для запуска
		level,			// 0-1:  0->1, 1->0, положительный или отрицательный фронт
		dBON,			// пересчет измерений в dB; 1 -- dB; 0 -- не dB
		ModeRec,		// режим записи 0 (MOMENT) - мгновенного значения, 1 (RMS+PEAK) - среднеквадратичного значения + пикового значения
		FDiap,			// частотный диапазон (0 - 2Гц..1КГц; 1 - 10Гц..1КГц; 2 - 10Гц..2КГц) для виброметра
		FltrNM,			// фильтр шумомера A -- 0, C -- 1, LIN -- 2  // + порог
		CVel,			// измеряемая величина (0 - виброускорение; 1 - виброскорость; 2 - виброперемещение)
		MaxTimeWrite,	// максимальное время записи для одного измерения ?
		CurrCal[3],		// номер текущего датчика для прибора 0..4
		currTypeDevice, // Текущий прибор 0..2
		TestCode[3],	// тестовый код для проверки целостности
		low_timereg,	// длительность записи
		high_timereg,	// длительность записи
		preDelay;		// задержка перед записью
	_PARAMEXT()
		: sizeSelf(2500)
		, contrast(0)
		, timereg(0)
		, porog(0)
		, typecalc(0)
		, fast(0)
		, write(0)
		, enazap(0)
		, digana(0)
		, numbit(0)
		, level(0)
		, dBON(0)
		, ModeRec(0)
		, FDiap(0)
		, FltrNM(0)
		, CVel(0)
		, MaxTimeWrite(0)
		, currTypeDevice(0)
		, low_timereg(0)
		, high_timereg(0)
		, preDelay(0)
	{
		ZeroMemory(Signature, sizeof(Signature));
		ZeroMemory(CurrCal, sizeof(CurrCal));
		ZeroMemory(TestCode, sizeof(TestCode));
	}
} PARAMEXT, *PPARAMEXT;

typedef struct	_PARAM	//параметры сохраняются на флэшке при включении и выключении питания
{					//файл config.par
	short int 
		//	contrast,		//контрастность
		sizeSelf,		//размер этой структуры
		timereg,		//время записи
		porog,			//порог срабатывания 1/20 - 20/20 от верхнего уровня
		typecalc,		//тип вычисляемых величин скз,пик
		ICP,			//0-ICP выключен, 1-ICP включен,
		fast,			//0-slow 1-fast усреднение
		write,			//1-включить запись, -1 выключить запись
		enazap,			//0 - запись по включению, 1 - по событию,
		digana,			//0 - цифровой,1 - аналоговый запуск, 2 - время
		numbit,			//номер бита или канала для запуска
		level,			//0-1   0->1, 1->0, положительный или отрицательный фронт для внешнего запуска
		enagetlevel,	//0 - не отслеживать уровень, 1 - отслеживать
		num_dig_port,	//номер цифрового бита 0 - 13
		num_ana_port,	//номер аналогового входа 0 - 15
		front,			//фронт 0 - возрастающий, 1 - нисходящий
		level_ana,		//уровень аналогового сигнала +- 32767
		level_activ,	//уровнь активного сигнала (когда включился) 0 - 0 В - 1 - 3.3 В
		low_porog,		//порог срабатывания 32 разрядный, младшая часть
		high_porog,		//порог срабатывания 32 разрядный, старшая часть
		low_timereg,	//длительность записи 32 разрядный, младшая часть
		high_timereg,	//длительность записи 32 разрядный, старшая часть
		enaAutoGen;		//битовое поле регистратора (младший бит справа ) 0 bit -- если 1, то автономный генератор включен
	float
		AGSygnalType,	// Тип сигнала
		AGSygnalParam1,	// парам 1
		AGSygnalParam2,	// парам 2
		AGSygnalParam3,	// парам 3
		AGSygnalParam4;	// парам 4
	ULONG64
		StartTime;		// время старта
	short 
		precision,		// -1 не точный, 1 - GPS, 2 - Ethernet, 3 - USB
		ModeSD,			//младший байт: 0 - ожидание скачивания данных отключено
		//				1 - ожидание скачивания по USB, 2 - по Ethernet
		//старший байт: 0 бит - разбивка по суткам, 1 бит - замена старых файлов, 2 бит - непрерывная запись
		MultiThreshold; //1 - многократное срабатывание по порогу
	_PARAM() 
		: sizeSelf(2500)
		, timereg(0)
		, porog(0)
		, typecalc(0)
		, ICP(0)
		, fast(0)
		, write(0)
		, enazap(0)
		, digana(0)
		, numbit(0)
		, level(0)
		, enagetlevel(0)
		, num_dig_port(0)
		, num_ana_port(0)
		, front(0)
		, level_ana(0)
		, level_activ(0)
		, low_porog(0)
		, high_porog(0)
		, low_timereg(0)
		, high_timereg(0)
		, enaAutoGen(0)
		, AGSygnalType(0.f)
		, AGSygnalParam1(0.f)
		, AGSygnalParam2(0.f)
		, AGSygnalParam3(0.f)
		, AGSygnalParam4(0.f)
		, StartTime(0)
		, precision(0)
		, ModeSD(0)
		, MultiThreshold(0)
	{
	}
} PARAM, *PPARAM;
/*
typedef struct _CALIBR		//параметры читаются из флэшки при включении питания
{
	unsigned long sizeSelf;		// размер этой структуры
	char Signature[16];			// сигнатура
	short ICP;					// 0-ICP выключен, 1-ICP включен,
	short TestCode[3];			// тестовый код для проверки целостности + выравнивание
	char name[128];				// название канала
	char conv[16];				// единицы измерения
	float sense;				// вес младшего разряда
	float DC;					// смещение нуля
	float refer;				// опорное значение для вычисления значения в дБ
	float ExtAmp;				// КУ внешнего усилителя
} CALIBR, *PCALIBR;
*/
#endif	// конец STAND_REG_DEVICE_PARAM_HEADER