#include "stringtools.h"

stringtools::stringtools() {}
std::string stringtools::trimEnds(const  std::string str){
    unsigned short start=0,end=str.size();
    while(start < str.size() && std::isspace(str[start])){
        start++;
    }
    do{
        end--;
    } while(std::isspace(str[end]) && end >= start);
    return str.substr(start,1+end-start);
}

std::string stringtools::trimL(const std::string str){
    unsigned short start=0;
    while(start < str.size() && std::isspace(str[start])){
        start++;
    }
    return str.substr(start);
}

std::string stringtools::trimR(const std::string str){
    unsigned short end=str.size();
    do{
        end--;
    } while(std::isspace(str[end]) && end > 0);
    return str.substr(0,1+end);
}

unsigned long stringtools::checksum(std::string str){
    unsigned long fullsum=0;
    for(size_t i=0;i<str.length();i++){
        fullsum += static_cast<unsigned>(str[i]);
    }
    return fullsum;
};
