#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#ifndef DATATOOL_H
#define DATATOOL_H
namespace dtt{
    union ufast16bytes {
        uint_fast16_t value;
        uint8_t bytes[2];
    };
    union ufast32bytes {
        uint_fast32_t value;
        uint8_t bytes[4];
    };
    union ufast64bytes {
        uint_fast64_t value;
        uint8_t bytes[8];
    };
    uint_fast16_t byte2ufast16(char bytes[2], const bool isLittle=false);
    uint_fast32_t byte2ufast32(char bytes[4], const bool isLittle=false);
    uint_fast64_t byte2ufast64(char bytes[8], const bool isLittle=false);
    std::array<char,2> ufast162byte(const uint_fast16_t value, const bool isLittle=false);
    std::array<char,4> ufast322byte(const uint_fast32_t value, const bool isLittle=false);
    std::array<char,8> ufast642byte(const uint_fast64_t value, const bool isLittle=false);
    std::string byte2hex(unsigned char* bytes,size_t length);
    std::string b64encode(const char* data, const size_t length);
    std::vector<char> b64decode(const std::string& data);
    inline char valtob64char(char c);
    inline char b64chartoval(char c);
    //const char b64Char[5]; nvm
}
#endif // DATATOOL_H
