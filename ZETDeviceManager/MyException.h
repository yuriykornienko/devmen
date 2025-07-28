#include "stdafx.h"
#include <iostream>
#include <string>
#include <exception> 

class MyException : public std::exception
{
private:
	std::string m_error;

public:
	MyException(std::string error)
		: m_error(error)
	{
	}

	const char* what() const { return m_error.c_str(); } 
};