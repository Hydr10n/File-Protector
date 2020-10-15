/*
 * ATTENTION: The test must be running as administrator. Besides, Visual Studio [Solution Platforms] must be set correctly before building ("Release" mode is recommended) (for example, if the test is going to be running on Windows x64 platform, change [Solution Platforms] to "x64" so that the test program can inject its DLL into 64-bit Windows Explorer).
*/

#include <iostream>
#include <conio.h>
#include "ShellUtils.h"
#include "FileUtils.h"
#include "../HookLib/HookLib.h"

using namespace std;

int wmain() {
	wstring moduleFileName;
	Hydr10n::FileUtils::GetModuleFileNameW(moduleFileName);
	(void)CoInitialize(NULL);
	Hydr10n::ShellUtils::SHOpenFolderAndSelectItem(moduleFileName.c_str());
	CoUninitialize();
	wcout << "Test: view the file of current process \"" << moduleFileName << "\" in Windows Explorer." << endl << endl
		<< "Test ready. Waiting for a key to start..." << endl;
	(void)_getwch();

	moduleFileName.erase(moduleFileName.find_last_of('\\'));

	SetGlobalWindowsHook();
	SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, moduleFileName.c_str(), NULL);
	wcout << "Test started. Waiting for a key to stop..." << endl;
	(void)_getwch();

	UnhookGlobalWindowsHook();
	SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, moduleFileName.c_str(), NULL);
	wcout << "Test stopped. Waiting for a key to quit..." << endl;
	(void)_getwch();
}