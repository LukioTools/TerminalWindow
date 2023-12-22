#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace FileManager{
    std::string readFile(const std::string& filePath) {
        // Open the file
        std::ifstream file(filePath, std::ios::binary);

        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return {};  // Return an empty string to indicate failure
        }

        // Get the size of the file
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // Read the entire file into a string
        std::string content(fileSize, '\0');
        file.read(&content[0], fileSize);

        // Close the file
        file.close();

        return content;
    }

}