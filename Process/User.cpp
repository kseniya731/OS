#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <algorithm>

bool findProcessByName(const std::string& processName, std::vector<DWORD>& processIds) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    processIds.clear();

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            std::wstring wName(pe32.szExeFile);
            std::string currentProcessName(wName.begin(), wName.end());

            if (_stricmp(currentProcessName.c_str(), processName.c_str()) == 0) {
                processIds.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return !processIds.empty();
}

bool processExists(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (hProcess != NULL) {
        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            CloseHandle(hProcess);
            return exitCode == STILL_ACTIVE;
        }
        CloseHandle(hProcess);
    }
    return false;
}

bool launchProcess(const std::string& command, bool wait = false) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    char cmd[1024];
    strcpy_s(cmd, command.c_str());

    if (!CreateProcessA(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
        return false;
    }

    if (wait) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

bool launchProcessAndWait(const std::string& command) {
    return launchProcess(command, true);
}

bool setEnvironmentVariable(const std::string& name, const std::string& value) {
    if (SetEnvironmentVariableA(name.c_str(), value.c_str())) {
        std::cout << "Environment variable " << name << " set to: " << value << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to set environment variable. Error: " << GetLastError() << std::endl;
        return false;
    }
}

bool deleteEnvironmentVariable(const std::string& name) {
    if (SetEnvironmentVariableA(name.c_str(), NULL)) {
        std::cout << "Environment variable " << name << " deleted." << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to delete environment variable. Error: " << GetLastError() << std::endl;
        return false;
    }
}

void cleanupProcesses(const std::string& processName) {
    std::vector<DWORD> pids;
    if (findProcessByName(processName, pids)) {
        std::cout << "\nCleaning up " << processName << " processes..." << std::endl;
        for (DWORD pid : pids) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
                std::cout << "  Terminated process with ID: " << pid << std::endl;
            }
        }
        Sleep(500);
    }
}

void demonstrateByName(const std::string& testProcessName) {
    std::cout << "\nDemonstration: Killing process by name" << std::endl;

    cleanupProcesses("Calculator.exe");
    cleanupProcesses("CalculatorApp.exe");
    cleanupProcesses("calc.exe");

    std::cout << "\nLaunching Calculator..." << std::endl;
    if (launchProcess("calc.exe")) {
        Sleep(2000);

        std::vector<DWORD> processIds;
        if (findProcessByName(testProcessName, processIds)) {
            std::cout << "Found " << processIds.size() << " process(es) named "
                << testProcessName << " with IDs: ";
            for (size_t i = 0; i < processIds.size(); i++) {
                std::cout << processIds[i];
                if (i < processIds.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;

            std::string command = "killer.exe --name " + testProcessName;
            std::cout << "Running: " << command << std::endl;

            if (launchProcessAndWait(command)) {
                Sleep(1000);

                std::vector<DWORD> remainingIds;
                if (!findProcessByName(testProcessName, remainingIds) || remainingIds.empty()) {
                    std::cout << "SUCCESS: All " << testProcessName << " processes were terminated." << std::endl;
                }
                else {
                    std::cout << "FAIL: " << remainingIds.size() << " " << testProcessName << " process(es) still exist." << std::endl;
                }
            }
            else {
                std::cerr << "Failed to run killer." << std::endl;
            }
        }
        else {
            std::cout << "Test process " << testProcessName << " not found." << std::endl;
        }
    }
}

void demonstrateById() {
    std::cout << "\nDemonstration: Killing process by ID" << std::endl;

    cleanupProcesses("notepad.exe");

    std::cout << "\nLaunching Notepad..." << std::endl;
    if (launchProcess("notepad.exe")) {
        Sleep(1500);

        std::vector<DWORD> processIds;
        if (findProcessByName("notepad.exe", processIds) && !processIds.empty()) {
            DWORD processId = processIds[0];
            std::cout << "Found notepad.exe with ID: " << processId << std::endl;

            std::string command = "killer.exe --id " + std::to_string(processId);
            std::cout << "Running: " << command << std::endl;

            if (launchProcessAndWait(command)) {
                Sleep(1000);

                if (!processExists(processId)) {
                    std::cout << "✓SUCCESS: Process with ID " << processId << " was terminated." << std::endl;
                }
                else {
                    std::cout << "FAIL: Process with ID " << processId << " still exists." << std::endl;
                }
            }
            else {
                std::cerr << "Failed to run killer." << std::endl;
            }
        }
        else {
            std::cout << "Test process notepad.exe not found." << std::endl;
        }
    }
}

void demonstrateByEnvironment() {
    std::cout << "\nDemonstration: Killing processes from PROC_TO_KILL" << std::endl;

    cleanupProcesses("notepad.exe");
    cleanupProcesses("Calculator.exe");
    cleanupProcesses("CalculatorApp.exe");
    cleanupProcesses("calc.exe");
    cleanupProcesses("mspaint.exe");

    std::string envValue = "\"notepad.exe,Calculator.exe,mspaint.exe\"";
    if (!setEnvironmentVariable("PROC_TO_KILL", envValue)) {
        return;
    }

    std::cout << "\nLaunching test processes..." << std::endl;
    launchProcess("notepad.exe");
    launchProcess("calc.exe");
    launchProcess("mspaint.exe");

    Sleep(2500);

    std::cout << "\nChecking if test processes are running:" << std::endl;
    std::vector<std::string> testProcesses = { "notepad.exe", "Calculator.exe", "mspaint.exe" };
    std::vector<DWORD> allPids;

    for (const auto& proc : testProcesses) {
        std::vector<DWORD> pids;
        if (findProcessByName(proc, pids)) {
            std::cout << "  Found " << pids.size() << " process(es) named " << proc << std::endl;
            allPids.insert(allPids.end(), pids.begin(), pids.end());
        }
        else {
            std::cout << "  " << proc << " not found." << std::endl;
        }
    }

    std::cout << "\nRunning killer without parameters..." << std::endl;
    if (launchProcessAndWait("killer.exe")) {
        Sleep(1500);

        std::cout << "\nVerifying that test processes were terminated:" << std::endl;
        bool allTerminated = true;

        for (const auto& proc : testProcesses) {
            std::vector<DWORD> remainingPids;
            if (!findProcessByName(proc, remainingPids) || remainingPids.empty()) {
                std::cout << "All " << proc << " processes were terminated." << std::endl;
            }
            else {
                std::cout << remainingPids.size() << " " << proc << " process(es) still exist." << std::endl;
                allTerminated = false;
            }
        }

        if (allTerminated) {
            std::cout << "\nSUCCESS: All processes from PROC_TO_KILL were terminated." << std::endl;
        }
        else {
            std::cout << "\nFAIL: Not all processes were terminated." << std::endl;
        }
    }

    std::cout << "\nCleaning up..." << std::endl;
    deleteEnvironmentVariable("PROC_TO_KILL");

    cleanupProcesses("notepad.exe");
    cleanupProcesses("Calculator.exe");
    cleanupProcesses("CalculatorApp.exe");
    cleanupProcesses("calc.exe");
    cleanupProcesses("mspaint.exe");
}

int main() {
    std::cout << "User Application (demonstrates the functionality of the Killer process)" << std::endl;

    demonstrateByName("Calculator.exe");

    demonstrateById();

    demonstrateByEnvironment();

    std::cout << "\nFinal cleanup" << std::endl;
    std::vector<std::string> processesToClean = {
        "notepad.exe",
        "Calculator.exe",
        "CalculatorApp.exe",
        "mspaint.exe"
    };

    for (const auto& proc : processesToClean) {
        cleanupProcesses(proc);
    }

    std::cout << "\nDemonstration complete" << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}