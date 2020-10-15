# File Protector

This project is a demonstration of how to hide files by using Microsoft Detours library and other techniques such as DLL injection and windows hook on Windows platform. Intercepting the Windows functions ```NtQueryDirectoryFile``` is the key.

A simple library is provided to make the task above easier. See README.md files in HookLib and FileUtils directories for details.