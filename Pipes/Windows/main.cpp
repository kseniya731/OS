#ifdef UNICODE
#undef UNICODE
#endif

#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

const int N = 17;

std::string ReadPipe(HANDLE h) {
    char buf[256];
    DWORD read;
    std::string data;

    while (ReadFile(h, buf, sizeof(buf) - 1, &read, NULL) && read > 0) {
        buf[read] = '\0';
        data += buf;
    }

    return data;
}

void WritePipe(HANDLE h, const std::string& s) {
    DWORD written;
    WriteFile(h, s.c_str(), (DWORD)s.size(), &written, NULL);
}

void ProcessM(HANDLE in, HANDLE out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x * 7) << " ";
    }

    WritePipe(out, oss.str());
    CloseHandle(in);
    CloseHandle(out);
}

void ProcessA(HANDLE in, HANDLE out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x + N) << " ";
    }

    WritePipe(out, oss.str());
    CloseHandle(in);
    CloseHandle(out);
}

void ProcessP(HANDLE in, HANDLE out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x * x * x) << " ";
    }

    WritePipe(out, oss.str());
    CloseHandle(in);
    CloseHandle(out);
}

void ProcessS(HANDLE in, HANDLE out) {
    std::istringstream iss(ReadPipe(in));
    int sum = 0, x;

    while (iss >> x) {
        sum += x;
    }

    WritePipe(out, std::to_string(sum));
    CloseHandle(in);
    CloseHandle(out);
}

int main() {
    std::cout << "Pipe Chain" << std::endl;
    std::string inputStr = "11 2 3 4 8 7\n";
    std::cout << "N = " << N << "\nInput: " << inputStr << std::endl;

    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE r1, w1, r2, w2, r3, w3, rOut, wOut;

    CreatePipe(&r1, &w1, &sa, 0); // M->A
    CreatePipe(&r2, &w2, &sa, 0); // A->P
    CreatePipe(&r3, &w3, &sa, 0); // P->S
    CreatePipe(&rOut, &wOut, &sa, 0); // S->result

    HANDLE inRead, inWrite;
    CreatePipe(&inRead, &inWrite, &sa, 0);
    WritePipe(inWrite, inputStr);
    CloseHandle(inWrite);

    std::thread tS([&]() { ProcessS(r3, wOut); });
    std::thread tP([&]() { ProcessP(r2, w3); });
    std::thread tA([&]() { ProcessA(r1, w2); });
    std::thread tM([&]() { ProcessM(inRead, w1); });

    tM.join(); tA.join(); tP.join(); tS.join();

    std::string result = ReadPipe(rOut);
    std::cout << "Result: " << result << std::endl;

    std::stringstream ss(inputStr);
    int number;
    std::vector<int> numbers;

    while (ss >> number) {
        numbers.push_back(number);
    }

    long long expected = 0;
    for (int x : numbers) {
        long long val = x * 7 + N;
        expected += val * val * val;
    }
    std::cout << "Expected: " << expected << std::endl;

    if (!result.empty()) {
        try {
            long long result_num = std::stoll(result);
            std::cout << (result_num == expected ? "SUCCESS" : "FAIL") << std::endl;
        }
        catch (...) {
            std::cout << "FAIL: Invalid result format" << std::endl;
        }
    }
    else {
        std::cout << "FAIL: No result" << std::endl;
    }

    CloseHandle(rOut);
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}