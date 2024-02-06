#ifndef _PROCESSOR_
#define _PROCESSOR_
#include<iostream>
#include<vector>
#include<string>

class cpu {
public:
    std::string lastOp;
    bool Running = false;
    int returnCode = 0;

    int Run(const std::vector<std::vector<std::string>>* comp);

    int Execute(const std::vector<std::string>* line);

    void Move(const std::string reg, const std::string var);

    bool IsNumber(const char c);

    bool IsRegister(const std::string reg);

    int ReadReg(const std::string reg);

    void WriteReg(const std::string reg, const int var);

    int* GetReg(const std::string reg);

    void Error(const std::string msg);

    void CoreDump();

private:
    int eax = 0; //Sys call num
    int ra = 0; //Branch Return
    int registers[64] = { 0 }; //...
};


#endif