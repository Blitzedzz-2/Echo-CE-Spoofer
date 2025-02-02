#include <Windows.h>
#include <iostream>
#include <random>
#include <string>
#include <psapi.h>

DWORD targetPID = 0;

std::wstring generateRandomString(int length) {
    const std::wstring charset = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::wstring result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    for (int i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }

    return result;
}

DWORD getCheatEnginePID() {
    DWORD processID = 0;
    const std::wstring targetProcessNames[] = {
        L"cheatengine-i386.exe",
        L"cheatengine-x86_64.exe",
        L"cheatengine-x86_64-SSE4-AVX2.exe"
    };

    DWORD processes[1024];
    DWORD cbNeeded;
    DWORD processCount;
    if (EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        processCount = cbNeeded / sizeof(DWORD);

        for (DWORD i = 0; i < processCount; i++) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess) {
                wchar_t processName[MAX_PATH];
                if (GetModuleFileNameEx(hProcess, NULL, processName, sizeof(processName) / sizeof(wchar_t))) {
                    for (const auto& processNameStr : targetProcessNames) {
                        if (wcsstr(processName, processNameStr.c_str())) {
                            processID = processes[i];
                            CloseHandle(hProcess);
                            return processID;
                        }
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }

    return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processID = 0;
    GetWindowThreadProcessId(hwnd, &processID);

    if (processID == targetPID) {
        if (hwnd == NULL) {
            std::wcerr << L"Invalid window handle encountered!" << std::endl;
            return TRUE;
        }

        wchar_t windowTitle[256];
        int titleLength = GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

        std::wcout << L"Found window hwnd: " << hwnd << L" with title: " << windowTitle << std::endl;

        if (titleLength == 0) {
            std::wcerr << L"Failed to get window title for hwnd: " << hwnd << std::endl;
        }
        else {
            std::wstring randomString = generateRandomString(50);
            std::wstring newTitle = L"Echo CE Spoofer | " + randomString;

            if (!SetWindowTextW(hwnd, newTitle.c_str())) {
                std::wcerr << L"Failed to set window title for hwnd: " << hwnd << std::endl;
            }
            else {
                std::wcout << L"Window title spoofed to: " << newTitle << std::endl;
            }

            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
        }

        return FALSE;
    }

    return TRUE;
}

int main() {
    targetPID = getCheatEnginePID();
    if (targetPID != 0) {
        std::wcout << L"Found Cheat Engine with PID: " << targetPID << std::endl;

        EnumWindows(EnumWindowsProc, 0);
    }
    else {
        std::wcout << L"Cheat Engine process not found." << std::endl;
    }
    std::wcout << L"Made by blitzedzz." << std::endl;

    system("pause >nul");
    return 0;
}
