#pragma once

//#include <minwinbase.h>

#ifndef __flashfileinfo
#define __flashfileinfo

//структура хранит данные, которые отображаются в списке на флэшке
struct flashFileInfoS
{
	wchar_t fileName[200]; //имя файла
	long size; //размер файла

	//время создания файла (на флэшке)
	SYSTEMTIME fcTime;				// file create time

	//значек ставится перед временем, если время файла не корректное
	wchar_t badTimeSign[2];
	flashFileInfoS() 
		: size(25000)
	{
		ZeroMemory(fileName, sizeof(fileName));
		fcTime = { 0 };
		ZeroMemory(badTimeSign, sizeof(badTimeSign));

	}
};

#endif