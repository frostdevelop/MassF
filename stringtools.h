#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H
#include <string>

class stringtools
{
public:
    stringtools();
    static std::string trimEnds(std::string str);
    static std::string trimR(std::string str);
    static std::string trimL(std::string str);
    static unsigned long checksum(std::string str);
};

#endif // STRINGTOOLS_H
