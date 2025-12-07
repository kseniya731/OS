#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>
#include <limits>

const int N = 17;

std::string ReadPipe(int fd) {
    char buf[256];
    std::string data;
    ssize_t read_bytes;

    while ((read_bytes = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[read_bytes] = '\0';
        data += buf;
    }

    return data;
}

void WritePipe(int fd, const std::string& s) {
    write(fd, s.c_str(), s.size());
}

void ProcessM(int in, int out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x * 7) << " ";
    }

    WritePipe(out, oss.str());
    close(in);
    close(out);
}

void ProcessA(int in, int out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x + N) << " ";
    }

    WritePipe(out, oss.str());
    close(in);
    close(out);
}

void ProcessP(int in, int out) {
    std::istringstream iss(ReadPipe(in));
    std::ostringstream oss;
    int x;

    while (iss >> x) {
        oss << (x * x * x) << " ";
    }

    WritePipe(out, oss.str());
    close(in);
    close(out);
}

void ProcessS(int in, int out) {
    std::istringstream iss(ReadPipe(in));
    int sum = 0, x;

    while (iss >> x) {
        sum += x;
    }

    WritePipe(out, std::to_string(sum));
    close(in);
    close(out);
}

int main() {
    std::cout << "Pipe Chain" << std::endl;
    std::string inputStr = "11 2 3 4 8 7\n";
    std::cout << "N = " << N << "\nInput: " << inputStr << std::endl;

    // M->A
    int r1, w1;
    int pipe_ma[2];
    if (pipe(pipe_ma) == -1) {
        std::cerr << "Failed to create pipe M->A: " << strerror(errno) << std::endl;
        return 1;
    }
    r1 = pipe_ma[0];
    w1 = pipe_ma[1];

    // A->P
    int r2, w2;
    int pipe_ap[2];
    if (pipe(pipe_ap) == -1) {
        std::cerr << "Failed to create pipe A->P: " << strerror(errno) << std::endl;
        close(r1); close(w1);
        return 1;
    }
    r2 = pipe_ap[0];
    w2 = pipe_ap[1];

    // P->S
    int r3, w3;
    int pipe_ps[2];
    if (pipe(pipe_ps) == -1) {
        std::cerr << "Failed to create pipe P->S: " << strerror(errno) << std::endl;
        close(r1); close(w1); close(r2); close(w2);
        return 1;
    }
    r3 = pipe_ps[0];
    w3 = pipe_ps[1];

    // S->result
    int rOut, wOut;
    int pipe_result[2];
    if (pipe(pipe_result) == -1) {
        std::cerr << "Failed to create pipe S->result: " << strerror(errno) << std::endl;
        close(r1); close(w1); close(r2); close(w2); close(r3); close(w3);
        return 1;
    }
    rOut = pipe_result[0];
    wOut = pipe_result[1];

    // input pipe
    int inRead, inWrite;
    int pipe_input[2];
    if (pipe(pipe_input) == -1) {
        std::cerr << "Failed to create input pipe: " << strerror(errno) << std::endl;
        close(r1); close(w1); close(r2); close(w2); close(r3); close(w3);
        close(rOut); close(wOut);
        return 1;
    }
    inRead = pipe_input[0];
    inWrite = pipe_input[1];

    WritePipe(inWrite, inputStr);
    close(inWrite);

    std::thread tS([&]() { ProcessS(r3, wOut); });
    std::thread tP([&]() { ProcessP(r2, w3); });
    std::thread tA([&]() { ProcessA(r1, w2); });
    std::thread tM([&]() { ProcessM(inRead, w1); });

    tM.join(); 
    tA.join(); 
    tP.join(); 
    tS.join();

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
        long long val = static_cast<long long>(x) * 7 + N;
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

    close(rOut);
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    
    return 0;
}