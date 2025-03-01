#include <stdint.h>
#include <array>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include "datatool.h"
#include <QDebug>

uint_fast16_t dtt::byte2ufast16(char bytes[2], const bool isLittle){
    unsigned char* ubytes = reinterpret_cast<unsigned char*>(bytes);
    if(isLittle){
        return static_cast<uint_fast16_t>(ubytes[0]) | (static_cast<uint_fast16_t>(ubytes[1]) << 8);
    }else{
        return (static_cast<uint_fast16_t>(ubytes[0]) << 8) | static_cast<uint_fast16_t>(ubytes[1]);
    }
}

uint_fast32_t dtt::byte2ufast32(char bytes[4], const bool isLittle){
    unsigned char* ubytes = reinterpret_cast<unsigned char*>(bytes);
    if(isLittle){
        return static_cast<uint_fast32_t>(ubytes[0]) | (static_cast<uint_fast32_t>(ubytes[1]) << 8) | (static_cast<uint_fast32_t>(ubytes[2]) << 16) | (static_cast<uint_fast32_t>(ubytes[3]) << 24);
    }else{
        return (static_cast<uint_fast32_t>(ubytes[0]) << 24) | (static_cast<uint_fast32_t>(ubytes[1]) << 16) | (static_cast<uint_fast32_t>(ubytes[2]) << 8) | static_cast<uint_fast32_t>(ubytes[3]);
    }
}

uint_fast64_t dtt::byte2ufast64(char bytes[8], const bool isLittle){
    unsigned char* ubytes = reinterpret_cast<unsigned char*>(bytes);
    if(isLittle){
        return static_cast<uint_fast64_t>(ubytes[0]) | (static_cast<uint_fast64_t>(ubytes[1]) << 8) | (static_cast<uint_fast64_t>(ubytes[2]) << 16) | (static_cast<uint_fast64_t>(ubytes[3]) << 24) | (static_cast<uint_fast64_t>(ubytes[4]) << 32) | (static_cast<uint_fast64_t>(ubytes[5]) << 40) | (static_cast<uint_fast64_t>(ubytes[6]) << 48) | (static_cast<uint_fast64_t>(ubytes[7]) << 56);
    }else{
        return static_cast<uint_fast64_t>(ubytes[7]) | (static_cast<uint_fast64_t>(ubytes[6]) << 8) | (static_cast<uint_fast64_t>(ubytes[5]) << 16) | (static_cast<uint_fast64_t>(ubytes[4]) << 24) | (static_cast<uint_fast64_t>(ubytes[3]) << 32) | (static_cast<uint_fast64_t>(ubytes[2]) << 40) | (static_cast<uint_fast64_t>(ubytes[1]) << 48) | (static_cast<uint_fast64_t>(ubytes[0]) << 56);
    }
}

std::array<char,2> dtt::ufast162byte(const uint_fast16_t value, const bool isLittle){
    std::array<char,2> arr;
    if(isLittle){
        arr[0] = (value & 0xFF);
        arr[1] = ((value>>8) & 0xFF);
    }else{
        arr[0] = ((value>>8) & 0xFF);
        arr[1] = (value & 0xFF);
    }
    return arr;
}

std::array<char,4> dtt::ufast322byte(const uint_fast32_t value, const bool isLittle){
    std::array<char,4 > arr;
    if(isLittle){
        arr[0] = (value & 0xFF);
        arr[1] = ((value>>8) & 0xFF);
        arr[2] = ((value>>16) & 0xFF);
        arr[3] = ((value>>24) & 0xFF);
    }else{
        arr[3] = (value & 0xFF);
        arr[2] = ((value>>8) & 0xFF);
        arr[1] = ((value>>16) & 0xFF);
        arr[0] = ((value>>24) & 0xFF);
    }
    return arr;
}

std::array<char,8> dtt::ufast642byte(const uint_fast64_t value, const bool isLittle){
    std::array<char,8> arr;
    if(isLittle){
        arr[0] = (value & 0xFF);
        arr[1] = ((value>>8) & 0xFF);
        arr[2] = ((value>>16) & 0xFF);
        arr[3] = ((value>>24) & 0xFF);
        arr[4] = ((value>>32) & 0xFF);
        arr[5] = ((value>>40) & 0xFF);
        arr[6] = ((value>>48) & 0xFF);
        arr[7] = ((value>>56) & 0xFF);
    }else{
        arr[7] = (value & 0xFF);
        arr[6] = ((value>>8) & 0xFF);
        arr[5] = ((value>>16) & 0xFF);
        arr[4] = ((value>>24) & 0xFF);
        arr[3] = ((value>>32) & 0xFF);
        arr[2] = ((value>>40) & 0xFF);
        arr[1] = ((value>>48) & 0xFF);
        arr[0] = ((value>>56) & 0xFF);
    }
    return arr;
}

std::string dtt::byte2hex(unsigned char* bytes,size_t length){
    std::ostringstream temp;
    qDebug() << length;
    temp << std::hex;
    for(size_t i=0;i<length;i++){
        temp << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return temp.str();
}

std::string dtt::b64encode(const char *data, const size_t length) {
    //This is the same algorithm I used for fem8 lol
    std::string out; //(ceil((length * 8) / 6), '\x00')
    unsigned char mode = 0;
    unsigned char bufval = 0;
    for(size_t i=0;i<length;i++){
        std::cout << data[i] << std::endl;
        // std::cout << static_cast<int>(data[i]) << std::endl;
        std::cout << length << std::endl;
        // std::cout << static_cast<int>(mode) << std::endl;
        switch(mode){
        case 0:
            out.push_back(valtob64char((data[i] & 0xFC) >> 2));
            bufval = (data[i] & 0x03) << 4;
            break;
        case 1:
            out.push_back(valtob64char(bufval | ((data[i] & 0xF0) >> 4)));
            bufval = (data[i] & 0x0F) << 2;
            break;
        case 2:
            out.push_back(valtob64char(bufval | ((data[i] & 0xC0) >> 6)));
            out.push_back(valtob64char(data[i] & 0x3F));
            break;
        }
        mode = mode<2 ? mode+1 : 0;
    }
    switch(mode){
    case 1:
    case 2:
        out.push_back(valtob64char(bufval));
        break;
    }
    while(out.size() % 4) out.push_back('=');
    return out;
};

std::vector<char> dtt::b64decode(const std::string& data){
    std::vector<char> out;
    unsigned char mode = 0;
    unsigned char bufval = 0;
    char val;
    for(size_t i=0;i<data.size() && data[i] != '=';i++){
        switch(mode){
        case 0:
            bufval = b64chartoval(data[i]) << 2;
            break;
        case 1:
            val = b64chartoval(data[i]);
            out.push_back(bufval | (val >> 4));
            bufval = (val & 0x0F) << 4;
            break;
        case 2:
            val = b64chartoval(data[i]);
            out.push_back(bufval | (val >> 2));
            bufval = (val & 0x03) << 6;
            break;
        case 3:
            val = b64chartoval(data[i]);
            out.push_back(bufval | val);
            break;
        }
        mode = mode<3 ? mode+1 : 0;
    }
    //if(mode<3){out.push_back(bufval);}
    return out;
};

inline char dtt::valtob64char(char c){
    return (c<26 ? c+65 : (c<52 ? c+71 : (c<62 ? c-4 : (c>62 ? '/' : '+'))));
}

inline char dtt::b64chartoval(char c){
    return (c<47 ? 62 : (c<48 ? 63 : (c<65 ? c+4 : (c<97 ? c-65 : c-71))));
}
