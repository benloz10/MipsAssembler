#ifndef _PROGRAM_
#define _PROGRAM_
#include<iostream>
#include<vector>
#include<fstream>

class program {
public:
	bool initialized = true;
	int LoadFile(const std::string fileName, std::vector<std::vector<std::string>>* comp);
private:
	std::string fileDir = "";
};

#endif