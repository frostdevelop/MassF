#include <cstring>
#include <fstream>
#include <string>
//#include <variant>
#ifndef SNOWCONF_H
#define SNOWCONF_H

namespace snow {
// enum Error : unsigned short {};
struct snowVal {
    std::string name;
    enum Type : unsigned short { INT, FLOAT, STRING, HEADER, BOOL } type;
    // std::variant<int,float,std::string> value;
    union {
        long intVal;
        double floatVal;
        char strVal[255];
        bool boolVal;
    };
};
} // namespace snow

class rsnowconf {
public:
    rsnowconf(std::ifstream *file);
    rsnowconf(const std::string filename);
    ~rsnowconf();
    snow::snowVal *getNVar();
    std::string getCurrentHead();
    void close();

private:
    std::string currhead;
    std::ifstream *conf;
};

class wsnowconf {
public:
    wsnowconf(std::ofstream *file);
    wsnowconf(const std::string filename);
    ~wsnowconf();
    template <typename Num>
    void writeVar(std::string name, Num value);
    void writeComment(std::string text);
    void writeHeader(std::string text);
    void close();

private:
    std::ofstream *conf;
};

template <typename Num>
void wsnowconf::writeVar(std::string name, Num value){
    *conf << name+" = "+std::to_string(value)+'\n';
};
template <> inline void wsnowconf::writeVar<std::string>(std::string name, std::string value){
    *conf << name+" = \""+value+"\"\n";
};
template <> inline void wsnowconf::writeVar<const char*>(std::string name, const char *value){
    *conf << name+" = \""+value+"\"\n"; //writeVar(name,std::string(value));
};
template <> inline void wsnowconf::writeVar<bool>(std::string name, bool value){
    *conf << name+" = "+(value ? "true\n" : "false\n"); //writeVar(name,std::string(value));
};

#endif // SNOWCONF_H
