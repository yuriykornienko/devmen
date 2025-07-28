#pragma once

#include "stdafx.h"
#include <wchar.h>

//предназначен для замены FILE* 
//автоматически закрывает ненулевой указатель при разрушении объекта
struct FileHandler
{
	FILE* f;
	FileHandler(FILE* file = NULL) :f(file){}
	~FileHandler(){ try{ Close(); } catch (...){} }
	FILE** GetPtr(){ return &f; }
	void Close(){ if (f) fclose(f); f = NULL; }
	FILE* Detach(){ FILE* ff = f; f = NULL; return ff; }
	FileHandler& operator=(FILE* file){ Close(); f = file; }
	FileHandler& operator=(FileHandler& file){ Close(); f = file.Detach(); }
	operator FILE* (){ return f; }
};