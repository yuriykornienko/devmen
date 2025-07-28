// ����� ������� ����������� ��������� �������� ��������� ��������� ������������ ��� ������ ����������� ������������

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

typedef struct _PARAMEXT		//��������� ����������� �� ������ ��� ��������� � ���������� �������
{
	unsigned int sizeSelf;		// ������ ���� ���������
	char Signature[16];			// ���������
	short int 
		contrast,		//�������������
		timereg,		//����� ������
		porog,			//����� ������������ 1/20 - 20/20 �� �������� ������ // ����� ������������ ���� ������
		typecalc,		//��� ����������� ������� 0 - ���, 1 - ���, 2 - ����
		fast,			//0-slow 1-fast (8 ��) ��������� 
		write,			//1-�������� ������, -1 ��������� ������
		enazap,			//0 - ������ �� ���������, 1 - �� ������,
		digana,			//0 - 1, �������� - ���������� ������, 2 - �� �������
		numbit,			// ����� ���� ��� ������ ��� �������
		level,			// 0-1:  0->1, 1->0, ������������� ��� ������������� �����
		dBON,			// �������� ��������� � dB; 1 -- dB; 0 -- �� dB
		ModeRec,		// ����� ������ 0 (MOMENT) - ����������� ��������, 1 (RMS+PEAK) - ������������������� �������� + �������� ��������
		FDiap,			// ��������� �������� (0 - 2��..1���; 1 - 10��..1���; 2 - 10��..2���) ��� ����������
		FltrNM,			// ������ �������� A -- 0, C -- 1, LIN -- 2  // + �����
		CVel,			// ���������� �������� (0 - ��������������; 1 - �������������; 2 - ����������������)
		MaxTimeWrite,	// ������������ ����� ������ ��� ������ ��������� ?
		CurrCal[3],		// ����� �������� ������� ��� ������� 0..4
		currTypeDevice, // ������� ������ 0..2
		TestCode[3],	// �������� ��� ��� �������� �����������
		low_timereg,	// ������������ ������
		high_timereg,	// ������������ ������
		preDelay;		// �������� ����� �������
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

typedef struct	_PARAM	//��������� ����������� �� ������ ��� ��������� � ���������� �������
{					//���� config.par
	short int 
		//	contrast,		//�������������
		sizeSelf,		//������ ���� ���������
		timereg,		//����� ������
		porog,			//����� ������������ 1/20 - 20/20 �� �������� ������
		typecalc,		//��� ����������� ������� ���,���
		ICP,			//0-ICP ��������, 1-ICP �������,
		fast,			//0-slow 1-fast ����������
		write,			//1-�������� ������, -1 ��������� ������
		enazap,			//0 - ������ �� ���������, 1 - �� �������,
		digana,			//0 - ��������,1 - ���������� ������, 2 - �����
		numbit,			//����� ���� ��� ������ ��� �������
		level,			//0-1   0->1, 1->0, ������������� ��� ������������� ����� ��� �������� �������
		enagetlevel,	//0 - �� ����������� �������, 1 - �����������
		num_dig_port,	//����� ��������� ���� 0 - 13
		num_ana_port,	//����� ����������� ����� 0 - 15
		front,			//����� 0 - ������������, 1 - ����������
		level_ana,		//������� ����������� ������� +- 32767
		level_activ,	//������ ��������� ������� (����� ���������) 0 - 0 � - 1 - 3.3 �
		low_porog,		//����� ������������ 32 ���������, ������� �����
		high_porog,		//����� ������������ 32 ���������, ������� �����
		low_timereg,	//������������ ������ 32 ���������, ������� �����
		high_timereg,	//������������ ������ 32 ���������, ������� �����
		enaAutoGen;		//������� ���� ������������ (������� ��� ������ ) 0 bit -- ���� 1, �� ���������� ��������� �������
	float
		AGSygnalType,	// ��� �������
		AGSygnalParam1,	// ����� 1
		AGSygnalParam2,	// ����� 2
		AGSygnalParam3,	// ����� 3
		AGSygnalParam4;	// ����� 4
	ULONG64
		StartTime;		// ����� ������
	short 
		precision,		// -1 �� ������, 1 - GPS, 2 - Ethernet, 3 - USB
		ModeSD,			//������� ����: 0 - �������� ���������� ������ ���������
		//				1 - �������� ���������� �� USB, 2 - �� Ethernet
		//������� ����: 0 ��� - �������� �� ������, 1 ��� - ������ ������ ������, 2 ��� - ����������� ������
		MultiThreshold; //1 - ������������ ������������ �� ������
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
typedef struct _CALIBR		//��������� �������� �� ������ ��� ��������� �������
{
	unsigned long sizeSelf;		// ������ ���� ���������
	char Signature[16];			// ���������
	short ICP;					// 0-ICP ��������, 1-ICP �������,
	short TestCode[3];			// �������� ��� ��� �������� ����������� + ������������
	char name[128];				// �������� ������
	char conv[16];				// ������� ���������
	float sense;				// ��� �������� �������
	float DC;					// �������� ����
	float refer;				// ������� �������� ��� ���������� �������� � ��
	float ExtAmp;				// �� �������� ���������
} CALIBR, *PCALIBR;
*/
#endif	// ����� STAND_REG_DEVICE_PARAM_HEADER