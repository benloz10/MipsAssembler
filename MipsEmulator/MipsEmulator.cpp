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

    int Run(const std::vector<std::vector<std::string>> *comp) {
        Running = true;
        std::cout << "RUNNING PROGRAM" << std::endl;
        for (eax = 0; eax < comp->size(); eax++) {
            if (!Running)
                return returnCode;
            Execute(&comp->at(eax));
        }
        return returnCode;
    }

    int Execute(const std::vector<std::string> *line) {
        lastOp = "";
        for (int i = 0; i < line->size(); i++) {
            lastOp += line->at(i) + " ";
        }
        const std::string ins = line->at(0);

        //WRITING
        if (ins == "#") {
            return 0;
        }
        if (ins == "mv") {
            Move(line->at(1), line->at(2));
        }

        //MATHEMATICS
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
        if (ins == "sqrt") {
            int var = sqrt(ReadReg(line->at(2)));
            WriteReg(line->at(1), var);
        }
        if (ins == "floor") {
            int var = floor(ReadReg(line->at(2)));
            WriteReg(line->at(1), var);
        }
        if (ins == "ceil") {
            int var = ceil(ReadReg(line->at(2)));
            WriteReg(line->at(1), var);
        }

        //COMPARISON
        if (ins == "and") {
            bool var = ReadReg(line->at(2)) != 0 && ReadReg(line->at(3)) != 0;
            if (var) {
                WriteReg(line->at(1), 1);
            } else {
                WriteReg(line->at(1), 0);
            }
        }
        if (ins == "or") {
            bool var = ReadReg(line->at(2)) != 0 || ReadReg(line->at(3)) != 0;
            if (var) {
                WriteReg(line->at(1), 1);
            }
            else {
                WriteReg(line->at(1), 0);
            }
        }
        if (ins == "nor") {
            bool var = ReadReg(line->at(2)) == 0 && ReadReg(line->at(3)) == 0;
            if (var) {
                WriteReg(line->at(1), 1);
            }
            else {
                WriteReg(line->at(1), 0);
            }
        }
        if (ins == "xor") {
            bool var = ReadReg(line->at(2)) + ReadReg(line->at(3)) == 1;
            if (var) {
                WriteReg(line->at(1), 1);
            }
            else {
                WriteReg(line->at(1), 0);
            }
        }

        //JUMPING
        if (ins == "j" || ins == "jr" || ins == "jal" || ins == "jral") {
            if (ins == "jal" || ins == "jral") {
                ra = eax + 2;
            }
            int jPos = ReadReg(line->at(1));
            if (ins == "j" || ins == "jal") {
                eax = jPos - 2;
            } else {
                eax += jPos - 1;
            }
        }

        //BRANCHING (sets ra)
        if (ins == "beq" || ins == "beqr") {
            if (ReadReg(line->at(1)) == ReadReg(line->at(2))) {
                ra = eax + 2;
                int jPos = ReadReg(line->at(3));
                if (ins == "beq") {
                    eax = jPos - 2;
                }
                else {
                    eax += jPos - 1;
                }
            }
        }
        if (ins == "bne" || ins == "bner") {
            if (ReadReg(line->at(1)) != ReadReg(line->at(2))) {
                ra = eax + 2;
                int jPos = ReadReg(line->at(3));
                if (ins == "bne") {
                    eax = jPos - 2;
                }
                else {
                    eax += jPos - 1;
                }
            }
        }
        if (ins == "blt" || ins == "bltr") {
            if (ReadReg(line->at(1)) < ReadReg(line->at(2))) {
                ra = eax + 2;
                int jPos = ReadReg(line->at(3));
                if (ins == "blt") {
                    eax = jPos - 2;
                }
                else {
                    eax += jPos - 1;
                }
            }
        }
        if (ins == "bgt" || ins == "bgtr") {
            if (ReadReg(line->at(1)) > ReadReg(line->at(2))) {
                ra = eax + 2;
                int jPos = ReadReg(line->at(3));
                if (ins == "bgt") {
                    eax = jPos - 2;
                }
                else {
                    eax += jPos-1;
                }
            }
        }

        //OUTPUT
        if (ins == "print") {
            std::cout << "Out: " << ReadReg(line->at(1)) << std::endl;
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

    void Move(const std::string reg, const std::string var) {

        //Parse Variable
        int num = ReadReg(var);

        //std::cout << "Moving " << num << " into " << reg << std::endl;
        WriteReg(reg, num);
    }

    bool IsNumber(const char c) {
        int val = c;
        return val >= 48 && val <= 57;
    }

    bool IsRegister(const std::string reg) {
        return reg == "eax" || reg[0] == 'r';
    }

    int ReadReg(const std::string reg) {
        if (reg == "ra") {
            return ra;
        } else if (reg == "eax") {
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

    void WriteReg(const std::string reg, const int var) {
        *GetReg(reg) = var;
    }

    int* GetReg(const std::string reg) {
        if (reg == "ra") {
            return &ra;
        } else if (reg == "eax") {
            return &eax;
        } else if (reg[0] == 'r') {
            int rNum = std::stoi(reg.substr(1));
            if (rNum <= 63) {
                return &registers[rNum];
            }
        }
        Error("INVALID REGISTER " + reg);
        return &eax;
    }

    void Error(const std::string msg) {
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
    int ra = 0; //Branch Return
    int registers[64] = {0}; //...
};

class program {
public:

    bool initialized = false;

    int LoadFile(const std::string fileName, std::vector<std::vector<std::string>> *comp) {

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


namespace fs = std::filesystem;

std::string autocompleteFile(const std::string &compareString, const std::vector<std::string> &fileNameList) {
    for (std::string name : fileNameList) {
        if (name.length() >= compareString.length()) {
            if (0 == name.compare(0, compareString.length(), compareString)) {
                return name;
            }
        }
    }
    return "";
}

bool checkEnding(const std::string &fileName, const std::string &ending) {
    if (fileName.length() >= ending.length()) {
        return (0 == fileName.compare(fileName.length() - ending.length(), ending.length(), ending));
    }
    return false;
}

int main()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    bool _RUNNING = true;
    program prog;
    cpu processor;

    struct stat sb;
    bool selecting = true;

    std::string input;
    std::string selectedFile;
    std::vector<std::string> fileNames;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        fs::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();

        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
            const fs::path fileName = fs::relative(path);
            if (checkEnding(fileName.string(), ".mips")) {
                fileNames.push_back(fileName.string());
            }
        }
    }
    while (_RUNNING) {
        while (selecting) {

            std::cout << "MIPS files found:" << std::endl;

            for (std::string name : fileNames) {
                std::cout << "    " << name << std::endl;
            }

            std::cout << "Please enter the name you'd like to execute. (Enter \"exit\" to quit.)" << std::endl;

            std::cin >> input;

            if (input == "exit") {
                std::cout << "Exiting..." << std::endl;
                return 0;
            }

            std::string fullName = autocompleteFile(input, fileNames);

            if (fs::exists(fs::current_path().string() + "\\" + fullName)) {
                selectedFile = fullName;
                selecting = false;
            }
            else {
                std::cout << "That file doesn't exist." << std::endl << "Valid files: " << std::endl;
                for (std::string str : fileNames) {
                    std::cout << str;
                }
            }
        }

        std::vector<std::vector<std::string>> file;
        static clock::time_point start;
        static clock::time_point end;

        start = clock::now();
        int lineCount = prog.LoadFile(selectedFile, &file);
        int exitCode = 0;
        end = clock::now();

        const duration fileLoadTime = end - start;

        start = clock::now();
        if (prog.initialized) {
            exitCode = processor.Run(&file);
        }
        end = clock::now();

        const duration elapsed = end - start;
        const duration totalTime = elapsed + fileLoadTime;
        std::cout << "Processor finished with code " << exitCode << std::endl;
        std::cout << "Loaded file(" << lineCount << " lines) in " << fileLoadTime.count() << "ms" << std::endl;
        std::cout << "Ran program in " << elapsed.count() << "ms" << std::endl;
        std::cout << "Total time: " << totalTime.count() << "ms" << std::endl;
        file.clear();
        selecting = true;
    }

    return 0;
}
