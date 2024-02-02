#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>

class cpu {
public:

    std::string lastOp;
    bool Running = false;
    int returnCode = 0;

    int Run(std::vector<std::vector<std::string>> *comp) {
        Running = true;
        std::cout << "RUNNING PROGRAM" << std::endl;
        for (eax = 0; eax < comp->size(); eax++) {
            if (!Running)
                return returnCode;
            Execute(&comp->at(eax));
        }
        return returnCode;
    }

    int Execute(std::vector<std::string> *line) {
        lastOp = "";
        for (int i = 0; i < line->size(); i++) {
            lastOp += line->at(i) + " ";
        }
        std::string ins = line->at(0);
        //std::cout << "Processor EXEC: " << line[0] << line[1] << line[2] << std::endl;
        if (ins == "#") {
            return 0;
        }
        if (ins == "mv") {
            Move(line->at(1), line->at(2));
        }
        if (ins == "add") {
            int var = ReadReg(line->at(2)) + ReadReg(line->at(3));
            WriteReg(line->at(1), var);
        }
        if (ins == "sub") {
            int var = ReadReg(line->at(2)) - ReadReg(line->at(3));
            WriteReg(line->at(1), var);
        }
        if (ins == "div") {
            if (ReadReg(line->at(3)) == 0) {
                std::cout << "DIV BY 0";
                return 0;
            }
            int var = ReadReg(line->at(2)) / ReadReg(line->at(3));
            WriteReg(line->at(1), var);
        }
        if (ins == "mul") {
            int var = ReadReg(line->at(2)) * ReadReg(line->at(3));
            WriteReg(line->at(1), var);
        }
        if (ins == "print") {
            std::cout << "Out: " << ReadReg(line->at(1)) << std::endl;
        }
        if (ins == "j") {
            int jPos = ReadReg(line->at(1));
            eax = jPos - 2;
        }
        if (ins == "bgt" || ins == "bgtr") {
            if (ReadReg(line->at(2)) > ReadReg(line->at(3))) {
                int jPos = ReadReg(line->at(1));
                if (ins == "bgt") {
                    eax = jPos - 2;
                }
                else {
                    eax += jPos-1;
                }
            }
        }
        if (ins == "lsreg") {
            std::cout << "REGDUMP" << std::endl;
            for (int i = 0; i < 64; i++) {
                int x = i % 8;
                std::cout << " r" << i << ":" << registers[i];
                if (x == 7) {
                    std::cout << std::endl;
                }
            }
            std::cout << "EAX:" << eax << std::endl;
        }
        return 0;
    }

    void Move(std::string reg, std::string var) {

        //Parse Variable
        int num = ReadReg(var);

        //std::cout << "Moving " << num << " into " << reg << std::endl;
        WriteReg(reg, num);
    }

    bool IsNumber(char c) {
        int val = c;
        return val >= 48 && val <= 57;
    }

    bool IsRegister(std::string reg) {
        return reg == "eax" || reg[0] == 'r';
    }

    int ReadReg(std::string reg) {
        if (reg == "eax") {
            return eax;
        } else if (reg[0] == 'r') {
            return *GetReg(reg);
        }
        try {
            return std::stoi(reg);
        }
        catch (...) {
            Error("CANNOT READ REGISTER");
        }
        return NULL;
    }

    void WriteReg(std::string reg, int var) {
        if (reg == "eax") {
            Error("CANNOT DIRECTLY MODIFY EAX");
        }
        *GetReg(reg) = var;
    }

    int* GetReg(std::string reg) {
        if (reg[0] == 'r') {
            int rNum = std::stoi(reg.substr(1));
            if (rNum <= 63) {
                return &registers[rNum];
            }
        } else if (reg == "eax") {
            return &eax;
        }
        Error("INVALID REGISTER " + reg);
        return &eax;
    }

    void Error(std::string msg) {
        std::cout << "[ERROR] " << msg << " [LINE " << eax+1 << "]:" << lastOp << std::endl;
        returnCode = 1;
        Running = false;
    }

    void CoreDump() {
        std::cout << std::endl;
        std::cout << "DUMPING: " << std::endl;
        std::cout << "EAX=" << eax << std::endl;
        for (int i = 0; i < 64; i++) {
            std::cout << registers[i] << " ";
        }
    }

private:
    int eax = 0; //Sys call num
    int registers[64] = {0};

};

class program {
public:

    bool initialized = false;

    int LoadFile(std::string fileName, std::vector<std::vector<std::string>> *comp) {

        std::ifstream fs(fileName, std::ifstream::binary);

        fs.seekg(0, fs.end);
        int length = fs.tellg();
        fs.seekg(0, fs.beg);
        std::cout << "Reading file " << fileName << std::endl;

        std::string buffer(length, '\0');

        fs.read(&buffer[0], length);

        std::vector<std::string> file;
        file.push_back({});

        int lineCount = 0;

        for (int i = 0; i <= length; i++) {
            if (buffer[i] == '\n' || i == length) {
                file.push_back({});
                lineCount++;
            }
            else {
                file[lineCount] += buffer[i];
            }
        }

        for (int i = 0; i < lineCount; i++) {
            std::string strBld = "";
            comp->push_back({});
            int insCount = 0;
            for (int x = 0; x <= file[i].length(); x++) {
                char c = file[i][x];
                if (c == ' ' || c == '\n' || x == file[i].length() || c == '\r') {
                    comp->back().push_back(strBld);
                    strBld = "";
                    insCount++;
                }
                else {
                    strBld += file[i][x];
                }
            }
        }
        initialized = true;
        return lineCount;
    }
private:
    std::string fileDir = "";
    
};

class interpreter {

};


int main()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    bool _RUNNING = true;
    program prog;
    cpu processor;

    std::vector<std::vector<std::string>> file;
    static clock::time_point start;
    static clock::time_point end;

    start = clock::now();
    int lineCount = prog.LoadFile("fib.mips", &file);
    int exitCode = 0;
    end = clock::now();

    duration fileLoadTime = end - start;

    start = clock::now();
    if (prog.initialized) {
        exitCode = processor.Run(&file);
    }
    end = clock::now();

    duration elapsed = end - start;
    duration totalTime = elapsed + fileLoadTime;
    std::cout << "Processor finished with code " << exitCode << std::endl;
    std::cout << "Loaded file(" << lineCount << " lines) in " << fileLoadTime.count() << "ms" << std::endl;
    std::cout << "Ran program in " << elapsed.count() << "ms" << std::endl;
    std::cout << "Total time: " << totalTime.count() << "ms" << std::endl;
    file.clear();
    return 0;
}
