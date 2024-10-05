#ifndef UTILS_HPP
#define UTILS_HPP
#endif

#pragma once
#include <fstream>

std::string rstrip(std::string string){
    std::string trimmedString;
    for(auto it : string){
        if(it != ' '){
            trimmedString += it;
        }
    }
    return trimmedString;
}

std::string readString(std::ifstream &file, int stringSize){
    std::vector<char> string(stringSize + 1, '\0');
    file.read(&string[0], stringSize);

    return std::string(&string[0]);
}

uint64_t readBigEndianInteger(std::ifstream &file, int numBytes){

    uint64_t integer = 0;
    for(int i = 0; i < numBytes; i++){
        integer = (integer << 8) | (file.get() & 0xFF);
    }

    return integer;
}

std::string readStock(std::ifstream &file){
    std::string stockName = readString(file, 8);
    return rstrip(stockName);
}

