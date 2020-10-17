#include "HookLib.h"
#include "FileProtector.h"

#pragma data_seg("SharedData")
HHOOK hHook{};
struct { WCHAR szHookCallerFileName[UNICODE_STRING_MAX_CHARS]; } sharedData{};
#pragma data_seg()
#pragma comment(linker, "/SECTION:SharedData,RWS")

HINSTANCE hInstance;

BOOL WINAPI SetGlobalWindowsHook() {
	GetModuleFileNameW(NULL, sharedData.szHookCallerFileName, ARRAYSIZE(sharedData.szHookCallerFileName));
	return hHook == NULL ? (hHook = SetWindowsHookExW(WH_GETMESSAGE, [](int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, nCode, wParam, lParam); }, hInstance, 0)) != NULL : FALSE;
}

BOOL WINAPI UnhookGlobalWindowsHook() {
	const BOOL ret = UnhookWindowsHookEx(hHook);
	if (ret)
		hHook = NULL;
	return ret;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	using Hydr10n::FileUtils::FileProtector;
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		FileProtector::Hide(sharedData.szHookCallerFileName);
		hInstance = hModule;
	}	break;
	}
	return TRUE;
}