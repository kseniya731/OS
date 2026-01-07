#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <csignal>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

bool findProcessByName(const std::string& processName, std::vector<pid_t>& processIds) {
    processIds.clear();
    DIR* procDir = opendir("/proc");
    
    if (!procDir) {
        return false;
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
        
        if (!isPid) {
            continue;
        }

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
                    processIds.push_back(pid);
                }
            }
            cmdlineFile.close();
        }
    }
    
    closedir(procDir);
    return !processIds.empty();
}

bool processExists(pid_t processId) {
    std::string procPath = "/proc/" + std::to_string(processId);
    DIR* dir = opendir(procPath.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

bool launchProcess(const std::string& command, bool wait = false) {
    pid_t pid = fork();
    
    if (pid == 0) {
        std::vector<std::string> args;
        std::stringstream ss(command);
        std::string arg;
        
        while (ss >> arg) {
            args.push_back(arg);
        }
        
        std::vector<char*> argv;
        for (auto& a : args) {
            argv.push_back(&a[0]);
        }
        argv.push_back(nullptr);
        
        execvp(argv[0], argv.data());
        
        std::cerr << "Failed to execute command: " << command 
                  << ". Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    else if (pid > 0) {
        if (wait) {
            int status;
            waitpid(pid, &status, 0);
            return WIFEXITED(status) && WEXITSTATUS(status) == 0;
        }
        return true;
    }
    else {
        std::cerr << "Failed to fork process. Error: " << strerror(errno) << std::endl;
        return false;
    }
}

bool launchProcessAndWait(const std::string& command) {
    return launchProcess(command, true);
}

bool setEnvironmentVariable(const std::string& name, const std::string& value) {
    if (setenv(name.c_str(), value.c_str(), 1) == 0) {
        std::cout << "Environment variable " << name << " set to: " << value << std::endl;
        return true;
    } else {
        std::cerr << "Failed to set environment variable. Error: " << strerror(errno) << std::endl;
        return false;
    }
}

bool deleteEnvironmentVariable(const std::string& name) {
    if (unsetenv(name.c_str()) == 0) {
        std::cout << "Environment variable " << name << " deleted." << std::endl;
        return true;
    } else {
        std::cerr << "Failed to delete environment variable. Error: " << strerror(errno) << std::endl;
        return false;
    }
}

void cleanupProcesses(const std::string& processName) {
    std::vector<pid_t> pids;
    if (findProcessByName(processName, pids)) {
        std::cout << "\nCleaning up " << processName << " processes..." << std::endl;
        for (pid_t pid : pids) {
            if (kill(pid, SIGTERM) == 0) {
                std::cout << "  Sent SIGTERM to process with ID: " << pid << std::endl;
            }
        }
        usleep(500000);
    }
}

void demonstrateByName(const std::string& testProcessName) {
    std::cout << "\nDemonstration: Killing process by name" << std::endl;
    
    cleanupProcesses("xclock");
    
    std::cout << "\nLaunching xclock (X11 clock utility)..." << std::endl;
    if (launchProcess("xclock")) {
        sleep(2);
        
        std::vector<pid_t> processIds;
        if (findProcessByName(testProcessName, processIds)) {
            std::cout << "Found " << processIds.size() << " process(es) named "
                      << testProcessName << " with IDs: ";
            for (size_t i = 0; i < processIds.size(); i++) {
                std::cout << processIds[i];
                if (i < processIds.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            
            std::string command = "./killer --name " + testProcessName;
            std::cout << "Running: " << command << std::endl;
            
            if (launchProcessAndWait(command)) {
                sleep(1);
                
                std::vector<pid_t> remainingIds;
                if (!findProcessByName(testProcessName, remainingIds) || remainingIds.empty()) {
                    std::cout << "SUCCESS: All " << testProcessName << " processes were terminated." << std::endl;
                }
                else {
                    std::cout << "FAIL: " << remainingIds.size() << " " 
                              << testProcessName << " process(es) still exist." << std::endl;
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
    
    cleanupProcesses("gedit");
    
    std::cout << "\nLaunching xeyes (X11 utility)..." << std::endl;
    if (launchProcess("xeyes")) {
        sleep(1);
        
        std::vector<pid_t> processIds;
        if (findProcessByName("xeyes", processIds) && !processIds.empty()) {
            pid_t processId = processIds[0];
            std::cout << "Found xeyes with ID: " << processId << std::endl;
            
            std::string command = "./killer --id " + std::to_string(processId);
            std::cout << "Running: " << command << std::endl;
            
            if (launchProcessAndWait(command)) {
                sleep(1);
                
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
            std::cout << "Test process xeyes not found." << std::endl;
        }
    }
}

void demonstrateByEnvironment() {
    std::cout << "\nDemonstration: Killing processes from PROC_TO_KILL" << std::endl;
    
    cleanupProcesses("xclock");
    cleanupProcesses("xeyes");
    cleanupProcesses("xcalc");
    
    std::string envValue = "\"xclock,xeyes,xcalc\"";
    if (!setEnvironmentVariable("PROC_TO_KILL", envValue)) {
        return;
    }
    
    std::cout << "\nLaunching test processes..." << std::endl;
    launchProcess("xclock");
    launchProcess("xeyes");
    launchProcess("xcalc");
    
    sleep(2);
    
    std::cout << "\nChecking if test processes are running:" << std::endl;
    std::vector<std::string> testProcesses = {"xclock", "xeyes", "xcalc"};
    std::vector<pid_t> allPids;
    
    for (const auto& proc : testProcesses) {
        std::vector<pid_t> pids;
        if (findProcessByName(proc, pids)) {
            std::cout << "  Found " << pids.size() << " process(es) named " << proc << std::endl;
            allPids.insert(allPids.end(), pids.begin(), pids.end());
        }
        else {
            std::cout << "  " << proc << " not found." << std::endl;
        }
    }
    
    std::cout << "\nRunning killer without parameters..." << std::endl;
    if (launchProcessAndWait("./killer")) {
        sleep(1);
        
        std::cout << "\nVerifying that test processes were terminated:" << std::endl;
        bool allTerminated = true;
        
        for (const auto& proc : testProcesses) {
            std::vector<pid_t> remainingPids;
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
    
    cleanupProcesses("xclock");
    cleanupProcesses("xeyes");
    cleanupProcesses("xcalc");
}

int main() {
    std::cout << "User Application (demonstrates the functionality of the Killer process)" << std::endl;
    
    demonstrateByName("xclock");
    
    demonstrateById();
    
    demonstrateByEnvironment();
    
    std::cout << "\nFinal cleanup" << std::endl;
    std::vector<std::string> processesToClean = {
        "xclock",
        "xeyes",
        "xcalc",
        "gedit"
    };
    
    for (const auto& proc : processesToClean) {
        cleanupProcesses(proc);
    }
    
    std::cout << "\nDemonstration complete" << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return 0;
}
