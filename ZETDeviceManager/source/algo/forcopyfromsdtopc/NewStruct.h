#define NUM_DEVICES		3
#pragma pack(push, 1)
#include <NEW_CHREC.h>

struct NEW_ADC_INFO_USB
{
	// ������� DSP
	unsigned short	Command;				//�������
	unsigned short	TypeADC;				//����� ������ DSP: 0 - ADC, 1 - DAC
	signed short	Start;					//��������� ����� ������ 1- ����
	signed short	StartDAC;				//��������� ����� ������ 1- ����

	// ���������� ������������ DSP
	unsigned short	EnableADC;				//������� ���
	unsigned short	EnableDAC;				//������� ���
	unsigned short	EnableExtDevice;		//0 - ������ ���, 1 - PRUS, 2 - HCP
	unsigned short	QuantityChannelADC;		//������������ ���������� ������� � ������ ���
	unsigned short	QuantityChannelDAC;		//������������ ���������� ������� � ������ ���
	unsigned short  abcd;                    //������������
	// ������������ �������
	unsigned long	Channel;				//������� ���������� �������� �������
	unsigned long	ChannelDAC;				//������� ���������� ��������� �������
	unsigned long	HCPChannel;				//������� ���������� ���������� ������� HCP
	unsigned long	OverloadChannel;		//������� ���������� ������������� �������
	unsigned short	WorkChADC;				//���������� ���������� ������� ���
	unsigned short	WorkChDAC;				//���������� ���������� ������� ���

	// ������������ ����. ��������
	//	unsigned short	CodAmplify[32];			//���� �������� �� ���		

	// new	
	unsigned short 	CodAmplify[20];			//���� �������� �� ���
	unsigned char 	conv[16];				// ������� ���������
	float 			sense;					// ���������������� ������� �/��.���.
	float 			refer;					// ������� �������� ��� ���������� �������� � ��
	// end new

	unsigned short	PRUS[32];				//���� �������� ��������� ������ / ���. (����.) HCP
	unsigned short	Atten[4];				//���� ����������� ���

	// ������������ ������
	unsigned short	EnaExtStart;			//���������� �������� ������ 0-�����. 1-����
	unsigned short	EnaExtFreq;				//���������� ������� ������� 0-�����. 1-����
	unsigned short	EnaCycle;				//���������� ������������ ���������� 0-����������, 1-����������
	unsigned short	EnaCycleDAC;			//���������� ������������ ���������� 0-����������, 1-����������
	unsigned short	ExtCycleDAC;			//0 - ��������� ������� �� ���������� ������ DSP
	//1 - ��������� ������� �� ������� ������ � ��������� �� �����������
	unsigned short	ModaADC;				//���� ������ ��� 0-1, 1-2, 2-4, 3-5, 4-8, 5-10 ������������
	unsigned short	Rate;					//����������� ������� ������� �������
	unsigned short	RateDAC;				//����������� ������� ������� �������

	// ������������ �������
	unsigned short	SizeInterrupt;			//������ ��� ��������� ������ ��� ������ ����������
	unsigned short	SizeInterruptDAC;			//������ ��� ��������� ������ ��� ������ ����������
	unsigned short	PointerDSP;				//������ ���������� ���������� � DSP
	unsigned short  efgh;                    //������������

	// ���������� ������������� ��������� ���������
	unsigned long		Flag;					//���������� ����������
	unsigned long       FlagExt;
	// ������ ����������
	unsigned short	AdrDSP;					//Adress in DSP
	unsigned short	Count;					//���������� ���� ��� ��������
	unsigned short	NumberChannel;			//����� ������ (��������� ����������)
	unsigned short  ijkl;                    //������������
	unsigned long	DigitalInput;			//������ ��������� �����
	unsigned long	DigitalOutput;			//������ ��������� �����
	unsigned short	TestCode[6];			//�������� ��������
	char			VersionDSP[64];			//������ DSP
	unsigned long	SerialNumber;			//�������� ����� DSP
	unsigned long	ModelNumber;			//��� ����� � DSP
	unsigned long	ProgramCode;			//�������� ������������ ����
	unsigned long	ChannelDiff;			//������� ���������� ���������������� �������
	unsigned long	DigitalOutEnable;		//������� ���������� ������� ��������� �����

	float	DigitalResolutionLowADC;		//��� �������� ������� ��� 
	float	DigitalResolutionHighADC;		//��� �������� ������� ��� 
	float	DigitalResolutionLowDAC;		//��� �������� ������� ��� 
	float	DigitalResolutionHighDAC;		//��� �������� ������� ��� 

	float	DigitalResolChanADC[32];		//��������������� ��� �������� ������� ���
	float	DigitalResolChanDAC[4];			//��������������� ��� �������� ������� ���
	unsigned long	SystemTime;				//��������� ����� ������� UTC (��������� FILETIME)
	unsigned long	SystemTimeExt;

	unsigned short	Reserved[12];			//���������������
};


struct OLD_PARAM		//��������� ����������� �� ������ ��� ��������� � ���������� �������
{
	short int
		//	contrast,		//�������������
		sizeSelf,		//������ ���� ���������
		timereg,		//����� ������
		porog,			//����� ������������ 1/20 - 20/20 �� �������� ������
		typecalc,		//��� ����������� ������� ���,���
		ICP,			//0-ICP ��������, 1-ICP �������,
		fast,			//0-slow 1-fast ����������
		write,			//1-�������� ������, -1 ��������� ������
		enazap,			//0 - ������ �� ���������, 1 - �� ������,
		digana,			//0 - 1, �������� - ���������� ������,
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
		ModeSD;			//������� ����: 0 - �������� ���������� ������ ���������
	//				1 - �������� ���������� �� USB, 2 - �� Ethernet
	//������� ����: 0 ��� - �������� �� ������, 1 ��� - ������ ������ ������, 2 ��� - ����������� ������
};

struct NEW_PARAM		//��������� ����������� �� ������ ��� ��������� � ���������� �������
{
	unsigned int sizeSelf;		// ������ ���� ���������
	char Signature[16];			// ���������
	short int
		contrast,		//�������������
		timereg,		//����� ������
		porog,			//����� ������������ 1/20 - 20/20 �� �������� ������ // ����� ������������ ���� ������ // ������� ����� ������
		typecalc,		//��� ����������� ������� 0 - ���, 1 - ���, 2 - ����
		fast,			//0-slow 1-fast, 2-Imp ��������� ��������������
		write,			//1-�������� ������, -1 ��������� ������
		enazap,			//0 - ������ �� ���������, 1 - �� �������,
		digana,			//0 - ��������,1 - ���������� ������, 2 - �����
		numbit,			// ����� ���� ��� ������ ��� �������
		level,			// 0-1:  0->1, 1->0, ������������� ��� ������������� �����
		dBON,			// �������� ��������� � dB; 1 -- dB; 0 -- �� dB
		ModeRec,		// ����� ������ 0 (MOMENT) - ����������� ��������, 1 (RMS+PEAK) - ������������������� �������� + �������� ��������
		FDiap,			// ��������� �������� (0 - 2��..1���; 1 - 10��..1���; 2 - 10��..2���) ��� ����������
		FltrNM,			// ������ �������� A -- 0, C -- 1, LIN -- 2  // + �����
		CVel,			// ���������� �������� (0 - ��������������; 1 - �������������; 2 - ����������������)
		MaxTimeWrite,	// ������������ ����� ������ ��� ������ ��������� ?
		CurrCal[NUM_DEVICES],		// ����� �������� ������� ��� ������� ������� 0..4
		currTypeDevice, // ������� ������ 0..2
		TestCode[3],	// �������� ��� ��� �������� �����������
		low_timereg,	// ������������ ������
		high_timereg,	// ������������ ������
		preDelay;		// �������� ������������
	ULONG64
		StartTime;		// ����� ������
	char
		AdjustFreq;		// 0 -  ��� ����������, 1 - GPS
};

struct NEW_CALIBR		//��������� �������� �� ������ ��� ��������� �������
{
	unsigned long sizeSelf;		// ������ ���� ���������
	char Signature[16];			// ���������
	short ICP;					// 0-ICP ��������, 1-ICP �������,
	short TestCode[3];			// �������� ��� ��� �������� ����������� + ������������
	char name[128];				// �������� ������
	char conv[16];				// ������� ���������
	float sense;				// ���������������� ������� �/��.���.
	float DC;					// �������� ����
	float refer;				// ������� �������� ��� ���������� �������� � ��
	float ExtAmp;				// �� �������� ���������
};

#pragma pack(pop)