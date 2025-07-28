#include "stdafx.h"
#include "sd_std.hpp"

#include "../../utils/recorder/recorder.hpp"

namespace zetlab {
	namespace std {

sd::sd()
{

}

sd::~sd()
{
	
}

void sd::launchSilentConverter(const tstring &path)
{
	CString _currPCDir(path.c_str());
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	CString cmd;

	// »щем папку ZET_DATA, она главнее
	hFind = FindFirstFile(_currPCDir + L"*", &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (CString(findFileData.cFileName) == L"ZET_DATA")
				{
					cmd = L"-flash ";
					break;
				}
			}
			else
			{
				if (zetlab::utils::check_convertible(findFileData.cFileName))
				{
					cmd = L"-path ";
					break;
				}
			}

		} while (FindNextFile(hFind, &findFileData) != NULL);
		FindClose(hFind);
	}

	if (!cmd.IsEmpty())
	{
		CString params;
		params.Format(L"%s %s", cmd.GetString(), path.c_str());
		ShellExecute(NULL, _T("open"), L"SilentConverter.exe", params, _T(""), SW_SHOW);
	}
}

}
} // namespace zetlab::std