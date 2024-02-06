#include "processor.h"
int cpu::Run(const std::vector<std::vector<std::string>>* comp) {
    Running = true;
    std::cout << "RUNNING PROGRAM" << std::endl;
    for (eax = 0; eax < comp->size(); eax++) {
        if (!Running)
            return returnCode;
        Execute(&comp->at(eax));
    }
    return returnCode;
}

int cpu::Execute(const std::vector<std::string>* line) {
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
        }
        else {
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
        }
        else {
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
                eax += jPos - 1;
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

void cpu::Move(const std::string reg, const std::string var) {

    //Parse Variable
    int num = ReadReg(var);

    //std::cout << "Moving " << num << " into " << reg << std::endl;
    WriteReg(reg, num);
}

bool cpu::IsNumber(const char c) {
    int val = c;
    return val >= 48 && val <= 57;
}

bool cpu::IsRegister(const std::string reg) {
    return reg == "eax" || reg[0] == 'r';
}

int cpu::ReadReg(const std::string reg) {
    if (reg == "ra") {
        return ra;
    }
    else if (reg == "eax") {
        return eax;
    }
    else if (reg[0] == 'r') {
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

void cpu::WriteReg(const std::string reg, const int var) {
    *GetReg(reg) = var;
}

int* cpu::GetReg(const std::string reg) {
    if (reg == "ra") {
        return &ra;
    }
    else if (reg == "eax") {
        return &eax;
    }
    else if (reg[0] == 'r') {
        int rNum = std::stoi(reg.substr(1));
        if (rNum <= 63) {
            return &registers[rNum];
        }
    }
    Error("INVALID REGISTER " + reg);
    return &eax;
}

void cpu::Error(const std::string msg) {
    std::cout << "[ERROR] " << msg << " [LINE " << eax + 1 << "]:" << lastOp << std::endl;
    returnCode = 1;
    Running = false;
}

void cpu::CoreDump() {
    std::cout << std::endl;
    std::cout << "DUMPING: " << std::endl;
    std::cout << "EAX=" << eax << std::endl;
    for (int i = 0; i < 64; i++) {
        std::cout << registers[i] << " ";
    }
}