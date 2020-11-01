#include "HookLib.h"
#include "FileProtector.h"

#pragma data_seg("SharedData")
struct {
	HHOOK hHook;
	WCHAR szHookCallerModuleFileName[UNICODE_STRING_MAX_CHARS];
} sharedData{};
#pragma data_seg()
#pragma comment(linker, "/SECTION:SharedData,RWS")

HINSTANCE hInstance;

BOOL WINAPI SetGlobalWindowsHook() {
	BOOL ret = sharedData.hHook == NULL;
	if (ret) {
		GetModuleFileNameW(NULL, sharedData.szHookCallerModuleFileName, ARRAYSIZE(sharedData.szHookCallerModuleFileName));
		ret = (sharedData.hHook = SetWindowsHookExW(WH_GETMESSAGE, [](int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, nCode, wParam, lParam); }, hInstance, 0)) != NULL;
	}
	return ret;
}

BOOL WINAPI UnhookGlobalWindowsHook() {
	const BOOL ret = UnhookWindowsHookEx(sharedData.hHook);
	if (ret)
		sharedData.hHook = NULL;
	return ret;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	using Hydr10n::FileUtils::FileProtector;
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		if (sharedData.hHook != NULL)
			FileProtector::Hide(sharedData.szHookCallerModuleFileName);
		hInstance = hModule;
	}	break;
	}
	return TRUE;
}