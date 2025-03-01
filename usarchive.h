#include <string>
#include <vector>
#include <cstdint> //stdint.h
#include <fstream>
#ifndef USARCHIVE_H
#define USARCHIVE_H

namespace usa {
struct file{
    unsigned char *name; //[512]
    uint_fast16_t namesize;
    uint_fast64_t size; //unsigned long long
    unsigned char *data;
};
struct databuf{
    unsigned char *data;
    size_t size;
};
}
// Why did I use references? cuz they fast l bozo lol
class usarchive
{
public:
    usarchive();
    usarchive(const std::string &filename);
    usarchive(std::ifstream &&file);
    void addFile(usa::file file);
    void addFile(const std::string &name);
    usa::file removeFile(const int ind);
    usa::file removeFile(const std::string &filename);
    usa::file popFile();
    void writeTo(const std::string &filename);
    void writeTo(std::ofstream &file);
    void unArchive(const std::string &path);
    void putName(size_t i,std::string name);
    void putData(size_t i,char* data,size_t size);
    size_t size();
    usa::file getFile(size_t i);
    usa::databuf writeBuf();
    unsigned short error();
    void open(std::ifstream &file);
private:
    std::vector<usa::file> files;
    size_t total_size;
    unsigned short err;
};

#endif // USARCHIVE_H
