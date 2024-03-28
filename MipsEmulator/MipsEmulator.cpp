#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>

#include "program.h"
#include "processor.h"

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
        std::cout << "\n--END OF OUTPUT--" << std::endl;
        std::cout << "Processor finished with code " << exitCode << std::endl;
        std::cout << "Loaded file(" << lineCount << " lines) in " << fileLoadTime.count() << "ms" << std::endl;
        std::cout << "Ran program in " << elapsed.count() << "ms" << std::endl;
        std::cout << "Total time: " << totalTime.count() << "ms" << std::endl;
        file.clear();
        selecting = true;
    }

    return 0;
}
