#include "program.h"

int program::LoadFile(const std::string fileName, std::vector<std::vector<std::string>>* comp) {

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