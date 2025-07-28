#pragma once

#ifndef __AFXWIN_H__
#   error Включить stdafx.h до включения этого файла в PCH
#endif

#include "../../resource/resource.h"

#define SINGLE_INSTANCE_UUID _T("ZETPanel_WRITER_OR_READER")
#include <SingleInstance/singleinstance.h>

#define CONFIG_VERSION  1.2

class CZETDeviceManagerApp : public CWinApp
{
public:
    CZETDeviceManagerApp();

public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
    AVOID_MULTI_INSTANCE()
};

extern CZETDeviceManagerApp theApp;
