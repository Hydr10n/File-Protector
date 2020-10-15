/*
 * Heade File: FileProtector.h
 * Last Update: 2020/10/15
 *
 * Copyright (C) Hydr10n@GitHub. All Rights Reserved.
 */

#pragma once

#pragma warning(disable:4302)
#pragma warning(disable:4311)

#include <Windows.h>
#include <winternl.h>
#include "FileUtils.h"
#include "../DetoursHelpers/DetoursHelpers.h"
#include "../std_container_helpers/set_helper.h"

namespace Hydr10n {
	namespace FileUtils {
		class FileProtector final {
		public:
			static bool Hide(LPCWSTR lpcwPath) { return std_container_helpers::set_helper::modify(m_HiddenPaths, lpcwPath, false); }

			static bool Unhide(LPCWSTR lpcwPath) { return std_container_helpers::set_helper::modify(m_HiddenPaths, lpcwPath, true); }

		private:
			enum class FILE_INFORMATION_CLASS { FileIdBothDirectoryInformation = 37 };

			struct compare_less { bool operator()(const std::wstring& a, const std::wstring& b) const { return lstrcmpiW(a.c_str(), b.c_str()) < 0; } };

			static std::set<std::wstring, compare_less> m_HiddenPaths;
			static NTSTATUS(NTAPI* m_NtQueryDirectoryFile)(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass, BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan);

			static const struct static_constructor {
				static_constructor() {
					m_NtQueryDirectoryFile = (decltype(m_NtQueryDirectoryFile))DetourFindFunction("ntdll", "NtQueryDirectoryFile");
					DetoursHelpers::ChangeProcAddr((PVOID*)&m_NtQueryDirectoryFile, MyNtQueryDirectoryFile, FALSE);
				}

				~static_constructor() { DetoursHelpers::ChangeProcAddr((PVOID*)&m_NtQueryDirectoryFile, MyNtQueryDirectoryFile, TRUE); }
			} m_static_constructor;

			static NTSTATUS NTAPI MyNtQueryDirectoryFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass, BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan) {
				using std::wstring;
				const NTSTATUS ret = m_NtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
				if (NT_SUCCESS(ret) && FileInformationClass == FILE_INFORMATION_CLASS::FileIdBothDirectoryInformation)
					for (PFILE_ID_BOTH_DIR_INFO pCurrent = (PFILE_ID_BOTH_DIR_INFO)FileInformation, pPrevious = NULL; pCurrent != NULL; pCurrent = (PFILE_ID_BOTH_DIR_INFO)((PBYTE)pCurrent + pCurrent->NextEntryOffset)) {
						wstring path;
						if (GetFinalPathNameByHandleW(path, FileHandle) && !std_container_helpers::set_helper::contains(m_HiddenPaths, SkipUnicodeLongPathPrefix(path.c_str()) + ((path[path.size() - 1] == L'\\' ? L"" : L"\\") + wstring(pCurrent->FileName, pCurrent->FileNameLength / sizeof(*pCurrent->FileName)))))
							pPrevious = pCurrent;
						else if (pPrevious != NULL)
							pPrevious->NextEntryOffset = pCurrent->NextEntryOffset ? pPrevious->NextEntryOffset + pCurrent->NextEntryOffset : 0;
						if (!pCurrent->NextEntryOffset)
							break;
					}
				return ret;
			}
		};

		decltype(FileProtector::m_static_constructor) FileProtector::m_static_constructor;
		decltype(FileProtector::m_HiddenPaths) FileProtector::m_HiddenPaths;
		decltype(FileProtector::m_NtQueryDirectoryFile) FileProtector::m_NtQueryDirectoryFile;
	}
}