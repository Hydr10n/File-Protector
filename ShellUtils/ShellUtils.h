#pragma once

#include <ShlObj.h>

namespace Hydr10n {
	namespace ShellUtils {
		HRESULT WINAPI SHOpenFolderAndSelectItem(LPCWSTR lpcwName) {
			LPITEMIDLIST lpItemIdList;
			HRESULT ret = SHParseDisplayName(lpcwName, NULL, &lpItemIdList, 0, NULL);
			if (SUCCEEDED(ret)) {
				ret = SHOpenFolderAndSelectItems(lpItemIdList, 0, NULL, 0);
				CoTaskMemFree(lpItemIdList);
			}
			return ret;
		}
	}
}