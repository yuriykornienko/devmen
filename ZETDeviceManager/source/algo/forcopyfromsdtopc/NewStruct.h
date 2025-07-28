#define NUM_DEVICES		3
#pragma pack(push, 1)
#include <NEW_CHREC.h>

struct NEW_ADC_INFO_USB
{
	// Команды DSP
	unsigned short	Command;				//Команда
	unsigned short	TypeADC;				//Режим работы DSP: 0 - ADC, 1 - DAC
	signed short	Start;					//состояние ввода данных 1- ввод
	signed short	StartDAC;				//состояние ввода данных 1- ввод

	// Аппаратная конфигурация DSP
	unsigned short	EnableADC;				//Наличие АЦП
	unsigned short	EnableDAC;				//Наличие ЦАП
	unsigned short	EnableExtDevice;		//0 - ничего нет, 1 - PRUS, 2 - HCP
	unsigned short	QuantityChannelADC;		//Максимальное количество каналов в модуле АЦП
	unsigned short	QuantityChannelDAC;		//Максимальное количество каналов в модуле ЦАП
	unsigned short  abcd;                    //выравнивание
	// Конфигурация каналов
	unsigned long	Channel;				//битовая комбинация вводимых каналов
	unsigned long	ChannelDAC;				//битовая комбинация выводимых каналов
	unsigned long	HCPChannel;				//битовая комбинация включенных каналов HCP
	unsigned long	OverloadChannel;		//битовая комбинация перегруженных каналов
	unsigned short	WorkChADC;				//количество работающих каналов АЦП
	unsigned short	WorkChDAC;				//количество работающих каналов ЦАП

	// Конфигурация коэф. усиления
	//	unsigned short	CodAmplify[32];			//коды усиления на АЦП		

	// new	
	unsigned short 	CodAmplify[20];			//коды усиления на АЦП
	unsigned char 	conv[16];				// единицы измерения
	float 			sense;					// чувствительность датчика В/ед.изм.
	float 			refer;					// опорное значение для вычисления значения в дБ
	// end new

	unsigned short	PRUS[32];				//коды усиления усилителя заряда / вкл. (выкл.) HCP
	unsigned short	Atten[4];				//коды аттенюатора ЦАП

	// Конфигурация частот
	unsigned short	EnaExtStart;			//разрешение внешнего старта 0-внутр. 1-внеш
	unsigned short	EnaExtFreq;				//разрешение внешней частоты 0-внутр. 1-внеш
	unsigned short	EnaCycle;				//разрешение циклического накопления 0-однократно, 1-циклически
	unsigned short	EnaCycleDAC;			//разрешение циклического накопления 0-однократно, 1-циклически
	unsigned short	ExtCycleDAC;			//0 - генерация сигнала из внутренней памяти DSP
	//1 - генерация сигнала из внешней памяти с подкачкой по прерываниям
	unsigned short	ModaADC;				//мода работы АЦП 0-1, 1-2, 2-4, 3-5, 4-8, 5-10 прореживание
	unsigned short	Rate;					//коэффициент деления опорной частоты
	unsigned short	RateDAC;				//коэффициент деления опорной частоты

	// Конфигурация буферов
	unsigned short	SizeInterrupt;			//размер для перекачки данных при каждом прерывании
	unsigned short	SizeInterruptDAC;			//размер для перекачки данных при каждом прерывании
	unsigned short	PointerDSP;				//индекс прошедшего накопления в DSP
	unsigned short  efgh;                    //выравнивание

	// Переменные отслеживающие временные параметры
	unsigned long		Flag;					//количество прерываний
	unsigned long       FlagExt;
	// Прочие переменные
	unsigned short	AdrDSP;					//Adress in DSP
	unsigned short	Count;					//Количество слов для передачи
	unsigned short	NumberChannel;			//Номер канала (служебная переменная)
	unsigned short  ijkl;                    //выравнивание
	unsigned long	DigitalInput;			//Данные цифрового порта
	unsigned long	DigitalOutput;			//Данные цифрового порта
	unsigned short	TestCode[6];			//Тестовые проверки
	char			VersionDSP[64];			//Версия DSP
	unsigned long	SerialNumber;			//Серийный номер DSP
	unsigned long	ModelNumber;			//Тип платы с DSP
	unsigned long	ProgramCode;			//лицензия программного кода
	unsigned long	ChannelDiff;			//битовая комбинация дифференциальных каналов
	unsigned long	DigitalOutEnable;		//битовая комбинация выходов цифрового порта

	float	DigitalResolutionLowADC;		//вес младшего разряда АЦП 
	float	DigitalResolutionHighADC;		//вес младшего разряда АЦП 
	float	DigitalResolutionLowDAC;		//вес младшего разряда ЦАП 
	float	DigitalResolutionHighDAC;		//вес младшего разряда ЦАП 

	float	DigitalResolChanADC[32];		//откалиброванный вес младшего разряда АЦП
	float	DigitalResolChanDAC[4];			//откалиброванный вес младшего разряда ЦАП
	unsigned long	SystemTime;				//системное время формата UTC (структура FILETIME)
	unsigned long	SystemTimeExt;

	unsigned short	Reserved[12];			//Зарезервировано
};


struct OLD_PARAM		//параметры сохраняются на флэшке при включении и выключении питания
{
	short int
		//	contrast,		//контрастность
		sizeSelf,		//размер этой структуры
		timereg,		//время записи
		porog,			//порог срабатывания 1/20 - 20/20 от верхнего уровня
		typecalc,		//тип вычисляемых величин скз,пик
		ICP,			//0-ICP выключен, 1-ICP включен,
		fast,			//0-slow 1-fast усреднение
		write,			//1-включить запись, -1 выключить запись
		enazap,			//0 - запись по включению, 1 - по порогу,
		digana,			//0 - 1, цифровой - аналоговый запуск,
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
		ModeSD;			//младший байт: 0 - ожидание скачивания данных отключено
	//				1 - ожидание скачивания по USB, 2 - по Ethernet
	//старший байт: 0 бит - разбивка по суткам, 1 бит - замена старых файлов, 2 бит - непрерывная запись
};

struct NEW_PARAM		//параметры сохраняются на флэшке при включении и выключении питания
{
	unsigned int sizeSelf;		// размер этой структуры
	char Signature[16];			// сигнатура
	short int
		contrast,		//контрастность
		timereg,		//время записи
		porog,			//порог срабатывания 1/20 - 20/20 от верхнего уровня // порог срабатывания авто записи // старшее слово порога
		typecalc,		//тип вычисляемых величин 0 - пик, 1 - скз, 2 - пост
		fast,			//0-slow 1-fast, 2-Imp временные характеристики
		write,			//1-включить запись, -1 выключить запись
		enazap,			//0 - запись по включению, 1 - по событию,
		digana,			//0 - цифровой,1 - аналоговый запуск, 2 - время
		numbit,			// номер бита или канала для запуска
		level,			// 0-1:  0->1, 1->0, положительный или отрицательный фронт
		dBON,			// пересчет измерений в dB; 1 -- dB; 0 -- не dB
		ModeRec,		// режим записи 0 (MOMENT) - мгновенного значения, 1 (RMS+PEAK) - среднеквадратичного значения + пикового значения
		FDiap,			// частотный диапазон (0 - 2Гц..1КГц; 1 - 10Гц..1КГц; 2 - 10Гц..2КГц) для виброметра
		FltrNM,			// фильтр шумомера A -- 0, C -- 1, LIN -- 2  // + порог
		CVel,			// измеряемая величина (0 - виброускорение; 1 - виброскорость; 2 - виброперемещение)
		MaxTimeWrite,	// максимальное время записи для одного измерения ?
		CurrCal[NUM_DEVICES],		// номер текущего датчика для каждого прибора 0..4
		currTypeDevice, // Текущий прибор 0..2
		TestCode[3],	// тестовый код для проверки целостности
		low_timereg,	// длительность записи
		high_timereg,	// длительность записи
		preDelay;		// задержка передзаписью
	ULONG64
		StartTime;		// время старта
	char
		AdjustFreq;		// 0 -  нет подстройки, 1 - GPS
};

struct NEW_CALIBR		//параметры читаются из флэшки при включении питания
{
	unsigned long sizeSelf;		// размер этой структуры
	char Signature[16];			// сигнатура
	short ICP;					// 0-ICP выключен, 1-ICP включен,
	short TestCode[3];			// тестовый код для проверки целостности + выравнивание
	char name[128];				// название канала
	char conv[16];				// единицы измерения
	float sense;				// чувствительность датчика В/ед.изм.
	float DC;					// смещение нуля
	float refer;				// опорное значение для вычисления значения в дБ
	float ExtAmp;				// КУ внешнего усилителя
};

#pragma pack(pop)