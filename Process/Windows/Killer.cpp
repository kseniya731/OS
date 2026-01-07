#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <memory>
#include <set>

bool killProcessById(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process with ID " << processId
            << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    if (TerminateProcess(hProcess, 0)) {
        std::cout << "Successfully terminated process with ID: " << processId << std::endl;
        CloseHandle(hProcess);
        return true;
    }
    else {
        std::cerr << "Failed to terminate process with ID " << processId
            << ". Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }
}

// Функция для поиска всех процессов по имени
std::vector<DWORD> findAllProcessesByName(const std::string& processName) {
    std::vector<DWORD> pids;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot. Error: " << GetLastError() << std::endl;
        return pids;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            std::wstring wName(pe32.szExeFile);
            std::string currentProcessName(wName.begin(), wName.end());

            if (_stricmp(currentProcessName.c_str(), processName.c_str()) == 0) {
                pids.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return pids;
}

bool killProcessesByName(const std::string& processName) {
    std::vector<DWORD> pids = findAllProcessesByName(processName);

    if (pids.empty()) {
        std::cout << "No processes found with name: " << processName << std::endl;
        return false;
    }

    std::cout << "Found " << pids.size() << " process(es) with name: " << processName << std::endl;
    bool allKilled = true;

    for (DWORD pid : pids) {
        std::cout << "  Process ID: " << pid << std::endl;
        if (!killProcessById(pid)) {
            allKilled = false;
        }
    }

    return allKilled;
}

std::vector<std::string> parseProcessNames(const std::string& envVar) {
    std::vector<std::string> names;
    std::stringstream ss(envVar);
    std::string name;

    while (std::getline(ss, name, ',')) {
        name.erase(std::remove(name.begin(), name.end(), '\"'), name.end());
        name.erase(std::remove(name.begin(), name.end(), '\''), name.end());
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

        if (!name.empty()) {
            names.push_back(name);
        }
    }

    return names;
}

std::string getEnvironmentVariable(const char* varName) {
    char* buffer = nullptr;
    size_t size = 0;

    if (_dupenv_s(&buffer, &size, varName) == 0 && buffer != nullptr) {
        std::string result(buffer);
        free(buffer);
        return result;
    }

    return "";
}

void killProcessesFromEnvironment() {
    std::string envValue = getEnvironmentVariable("PROC_TO_KILL");

    if (envValue.empty()) {
        std::cout << "Environment variable PROC_TO_KILL not found or empty." << std::endl;
        return;
    }

    std::cout << "Found PROC_TO_KILL variable with value: " << envValue << std::endl;

    std::vector<std::string> processNames = parseProcessNames(envValue);

    if (processNames.empty()) {
        std::cout << "No valid process names found in PROC_TO_KILL." << std::endl;
        return;
    }

    std::cout << "Parsed process names:" << std::endl;
    for (const auto& name : processNames) {
        std::cout << "  - " << name << std::endl;
    }

    std::cout << "\nAttempting to kill processes from PROC_TO_KILL..." << std::endl;
    for (const auto& name : processNames) {
        killProcessesByName(name);
    }
}

void printUsage() {
    std::cout << "Usage: killer [OPTIONS]\n"
        << "Options:\n"
        << "  --id <process_id>     Kill process by ID\n"
        << "  --name <process_name> Kill all processes by name\n"
        << "  (no arguments)        Kill processes from PROC_TO_KILL environment variable\n"
        << "\nExamples:\n"
        << "  killer --id 1234\n"
        << "  killer --name notepad.exe\n"
        << "  killer\n" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        killProcessesFromEnvironment();
    }
    else if (argc == 3) {
        std::string option = argv[1];

        if (option == "--id") {
            try {
                DWORD processId = std::stoul(argv[2]);
                killProcessById(processId);
            }
            catch (const std::exception& e) {
                std::cerr << "Invalid process ID: " << argv[2] << std::endl;
                return 1;
            }
        }
        else if (option == "--name") {
            std::string processName = argv[2];
            killProcessesByName(processName);
        }
        else {
            std::cerr << "Unknown option: " << option << std::endl;
            printUsage();
            return 1;
        }
    }
    else {
        std::cerr << "Invalid number of arguments." << std::endl;
        printUsage();
        return 1;
    }

    return 0;
}