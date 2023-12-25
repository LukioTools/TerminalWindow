#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

namespace FileManager{
    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            throw std::invalid_argument("File Not Found"); 
        }

        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string content(fileSize, '\0');
        file.read(&content[0], fileSize);

        file.close();

        return content;
    }

}