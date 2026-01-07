#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <memory>
#include <set>
#include <csignal>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <unistd.h>

bool killProcessById(pid_t processId) {
    if (kill(processId, SIGTERM) == 0) {
        std::cout << "Successfully sent SIGTERM to process with ID: " << processId << std::endl;
        return true;
    } else {
        std::cerr << "Failed to terminate process with ID " << processId << ". Error: " << strerror(errno) << std::endl;
        return false;
    }
}

bool killProcessByIdForce(pid_t processId) {
    if (kill(processId, SIGKILL) == 0) {
        std::cout << "Successfully sent SIGKILL to process with ID: " << processId << std::endl;
        return true;
    } else {
        std::cerr << "Failed to kill process with ID " << processId 
                  << ". Error: " << strerror(errno) << std::endl;
        return false;
    }
}

std::vector<pid_t> findAllProcessesByName(const std::string& processName) {
    std::vector<pid_t> pids;
    DIR* procDir = opendir("/proc");
    
    if (!procDir) {
        std::cerr << "Failed to open /proc directory. Error: " << strerror(errno) << std::endl;
        return pids;
    }
    
    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        bool isPid = true;
        for (int i = 0; entry->d_name[i] != '\0'; i++) {
            if (!isdigit(entry->d_name[i])) {
                isPid = false;
                break;
            }
        }
        
        if (!isPid) continue;
        
        pid_t pid = atoi(entry->d_name);
        
        std::string cmdlinePath = std::string("/proc/") + entry->d_name + "/cmdline";
        std::ifstream cmdlineFile(cmdlinePath);
        
        if (cmdlineFile.is_open()) {
            std::string cmdline;
            std::getline(cmdlineFile, cmdline);
            
            if (!cmdline.empty()) {
                size_t lastSlash = cmdline.find_last_of('/');
                std::string exeName = (lastSlash != std::string::npos) ? cmdline.substr(lastSlash + 1) : cmdline;
                exeName.erase(std::remove(exeName.begin(), exeName.end(), '\0'), exeName.end());
                
                if (exeName == processName) {
                    pids.push_back(pid);
                }
            }
            cmdlineFile.close();
        }
    }
    
    closedir(procDir);
    return pids;
}

bool killProcessesByName(const std::string& processName) {
    std::vector<pid_t> pids = findAllProcessesByName(processName);
    
    if (pids.empty()) {
        std::cout << "No processes found with name: " << processName << std::endl;
        return false;
    }
    
    std::cout << "Found " << pids.size() << " process(es) with name: " << processName << std::endl;
    bool allKilled = true;
    
    for (pid_t pid : pids) {
        std::cout << "  Process ID: " << pid << std::endl;
        if (!killProcessById(pid)) {
            sleep(1);
            if (!killProcessByIdForce(pid)) {
                allKilled = false;
            }
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
    char* value = getenv(varName);
    if (value != nullptr) {
        return std::string(value);
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
              << "  killer --name firefox\n"
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
                pid_t processId = static_cast<pid_t>(std::stoi(argv[2]));
                if (!killProcessById(processId)) {
                    sleep(1);
                    killProcessByIdForce(processId);
                }
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
