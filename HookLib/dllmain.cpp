#include "HookLib.h"
#include "FileProtector.h"

#pragma data_seg("SharedData")
HHOOK hHook{};
struct { WCHAR szHookCallerFileName[UNICODE_STRING_MAX_CHARS]; } sharedData{};
#pragma data_seg()
#pragma comment(linker, "/SECTION:SharedData,RWS")

HINSTANCE hInstance;

BOOL WINAPI SetGlobalWindowsHook() {
	std::wstring moduleFileName;
	if (Hydr10n::FileUtils::GetModuleFileNameW(moduleFileName))
		(void)lstrcpynW(sharedData.szHookCallerFileName, moduleFileName.c_str(), ARRAYSIZE(sharedData.szHookCallerFileName));
	return hHook == NULL ? (hHook = SetWindowsHookExW(WH_GETMESSAGE, [](int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, nCode, wParam, lParam); }, hInstance, 0)) != NULL : FALSE;
}

BOOL WINAPI UnhookGlobalWindowsHook() {
	const BOOL ret = UnhookWindowsHookEx(hHook);
	if (ret)
		hHook = NULL;
	return ret;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	using namespace Hydr10n::FileUtils;
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		FileProtector::Hide(sharedData.szHookCallerFileName);
		hInstance = hModule;
	}	break;
	}
	return TRUE;
}