#include <string>
#include <vector>
#include <fstream>
#include <cstring>
//#include <stdexcept>
#include <filesystem>
#include <cstdint>
#include <array>
#include <iostream>
#include "usarchive.h"
#include "datatool.h"
#include "stringtools.h"
#define IOSEND std::ios::end
#define IOSBIN std::ios::binary
/*
Header info:
namesize (2byte)
filename (500byte or variable)
bytes of file (8byte)
checksum (4byte)
*/
usarchive::usarchive(){
    total_size = 2;
    err = 0;
};
usarchive::usarchive(std::ifstream &&file){
    open(file);
    file.close();
};
usarchive::usarchive(const std::string &filename) : usarchive(std::ifstream(filename)) {};
void usarchive::addFile(usa::file file){
    files.push_back(file);
};
void usarchive::addFile(const std::string &name){
    std::filesystem::path rpath = stringtools::trimEnds(name);
    if(rpath.is_absolute()){err=5;return;}
    usa::file filedat;
    std::ifstream infile(rpath.string(),IOSBIN);
    if(!infile.is_open()){infile.close();err=6;return;} //throw std::invalid_argument("USARCHIVE: File not open!");
    //size_t filesize;
    filedat.namesize = rpath.string().size();
    filedat.name = new unsigned char[filedat.namesize];
    strncpy(reinterpret_cast<char*>(filedat.name),rpath.string().c_str(),filedat.namesize); //sizeof(filedat.name)
    //filedat.name[sizeof(filedat.name)-1] = '\0';
    filedat.size = infile.tellg();
    infile.seekg(0,IOSEND);
    filedat.size = static_cast<uint_fast64_t>(infile.tellg())-filedat.size; //infile.gcount()
    //std::cout << filedat.size << std::endl;
    filedat.data = new unsigned char[filedat.size];
    infile.seekg(0);
    /*
    for(size_t i=0;i<filedat.size;i++){
        filedat.data[i] =
    }
    */
    if(infile.read(reinterpret_cast<char *>(filedat.data),filedat.size)){
        files.push_back(filedat);
        total_size += filedat.namesize+14+filedat.size;
    }else{
        delete[] filedat.data;
        delete[] filedat.name;
        err = 3;
        //throw std::runtime_error("USARCHIVE: File reading failed");
    }
    infile.close();
};
usa::file usarchive::removeFile(const int ind){
    //martodo
    usa::file selfile = files[ind];
    files.erase(files.begin()+ind); //Needs to be iterator
    total_size -= selfile.namesize+14+selfile.size;
    return selfile;
};
usa::file usarchive::removeFile(const std::string &filename){
    for(size_t i=0;i<files.size();i++){
        if(std::string(reinterpret_cast<char *>(files[i].name),files[i].namesize) == filename){
            usa::file selfile = files[i];
            //mar:todo remove from vector
            files.erase(files.begin()+i); //files.begin()+
            total_size -= selfile.namesize+14+selfile.size;
            return selfile;
        }
    };
    return usa::file{};
};
usa::file usarchive::popFile(){
    usa::file lastfile = files.back();
    files.pop_back();
    total_size -= lastfile.namesize+14+lastfile.size;
    return lastfile;
};
size_t usarchive::size(){return files.size();}
usa::file usarchive::getFile(size_t i){return files[i];};
void usarchive::putName(size_t i,std::string name){total_size = total_size+name.size()-files[i].namesize;memcpy(files[i].name,name.c_str(),name.size());files[i].namesize = name.size();};
void usarchive::putData(size_t i,char* data,size_t size){total_size = total_size+size-files[i].size;memcpy(files[i].data,data,size);};
void usarchive::writeTo(const std::string &filename){
    std::ofstream outfile(filename,std::ios::binary);
    /*
    weird replit issue
    char* testvar = new char(-1);
    outfile.write(testvar,1);
    delete testvar;
    */
    writeTo(outfile);
    outfile.close();
};
void usarchive::writeTo(std::ofstream &file){
    if(!file.is_open()){
        file.close();
        err = 8;
        return;
    }
    uint16_t version(0);
    //std::cout << "Writing to file..." << std::endl;
    file.write(reinterpret_cast<const char*>(&version),2);
    for(unsigned long i=0;i<files.size();i++){
        //std::cout << i << std::endl;
        /*
        char namesizebytes[2];
        std::memcpy(namesizebytes,dtt::ufast162byte(files[i].namesize).data(),2);
        file.write(reinterpret_cast<char *>(&namesizebytes),2); //reinterpret_cast<char*>(
        */
        std::array<char,2> namesizebytes = dtt::ufast162byte(files[i].namesize);
        file.write(namesizebytes.data(),2);
        file.write(reinterpret_cast<char *>(files[i].name),files[i].namesize);
        /*
        char sizebytes[8];
        std::memcpy(sizebytes,dtt::ufast642byte(files[i].size).data(),8);
        file.write(reinterpret_cast<char *>(&sizebytes),8);
        */
        std::array<char,8> sizebytes = dtt::ufast642byte(files[i].size);
        file.write(sizebytes.data(),8);
        //optimize below
        /*
        dtt::ufast16bytes namesizeconv;
        namesizeconv.value = files[i].namesize;
        dtt::ufast64bytes filesizeconv;
        filesizeconv.value = files[i].size;
        */
        //uint_fast32_t fullchksum=namesizeconv.bytes[0]+namesizeconv.bytes[1]+filesizeconv.bytes[0]+filesizeconv.bytes[1]+filesizeconv.bytes[2]+filesizeconv.bytes[3]+filesizeconv.bytes[4]+filesizeconv.bytes[5]+filesizeconv.bytes[6]+filesizeconv.bytes[7];
        auto& unamesizebytes = reinterpret_cast<std::array<unsigned char, 2>&>(namesizebytes);
        auto& usizebytes = reinterpret_cast<std::array<unsigned char, 8>&>(sizebytes);
        uint_fast32_t fullchksum=unamesizebytes[0]+unamesizebytes[1]+usizebytes[0]+usizebytes[1]+usizebytes[2]+usizebytes[3]+usizebytes[4]+usizebytes[5]+usizebytes[6]+usizebytes[7];
        for(uint_fast16_t j=0;j<files[i].namesize;j++){
            //segfault ≧﹏≦
            fullchksum+=files[i].name[j];//static_cast<uint_fast8_t>(
        }
        /*
        char chksumbytes[4];
        std::memcpy(chksumbytes,dtt::ufast322byte(fullchksum).data(),4);
        file.write(reinterpret_cast<char *>(&chksumbytes),4); //reinterpret_cast<char*>(
        */
        /*
        std::array<char, 4> test = dtt::ufast322byte(fullchksum); //unsigned processing failed
        std::cout << static_cast<int>(static_cast<uint8_t>(test[0])) << std::endl << static_cast<int>(static_cast<uint8_t>(test[1])) << std::endl << static_cast<int>(static_cast<uint8_t>(test[2])) << std::endl << static_cast<int>(static_cast<uint8_t>(test[3])) << std::endl;
        */
        file.write(dtt::ufast322byte(fullchksum).data(),4);
        //std::cout << fullchksum << std::endl;
        /*
        char* testvar = new char(-13);
        //file.write(test.data()+3,1);
        file.write(testvar,1);
        delete testvar;
        */
        //return;
        //std::cout << static_cast<int>(files[i].data[0]) << std::endl;
        file.write(reinterpret_cast<char *>(files[i].data),files[i].size);
    }
    //std::cout << "Afterwr" << std::endl;
};
usa::databuf usarchive::writeBuf(){
    //size_t totalsize;
    size_t cursor = 2;
    usa::databuf fulldata;
    fulldata.size = total_size;
    fulldata.data = new unsigned char[fulldata.size];
    uint16_t version(0);
    std::memcpy(fulldata.data,reinterpret_cast<const char*>(&version),2);
    for(unsigned long i=0;i<files.size();i++){
        /*
        char filensize[2];
        std::memcpy(filensize,dtt::ufast162byte(files[i].namesize).data(),2);
        std::memcpy(fulldata.data+cursor,&filensize,2);
        */
        std::memcpy(fulldata.data+cursor,dtt::ufast162byte(files[i].namesize).data(),2);
        cursor+=2;
        std::memcpy(fulldata.data+cursor,files[i].name,files[i].namesize);
        cursor+=files[i].namesize;
        /*
        char filesize[8];
        std::memcpy(filesize,dtt::ufast642byte(files[i].size).data(),8);
        std::memcpy(fulldata.data+cursor,&filesize,8);
        */
        std::memcpy(fulldata.data+cursor,dtt::ufast642byte(files[i].size).data(),8);
        cursor+=8;
        uint_fast32_t fullchksum=0;
        for(uint_fast32_t j=cursor-10-files[i].namesize;j<cursor;j++){
            fullchksum += fulldata.data[j];
        }
        /*
        char chksumbytes[4];
        std::memcpy(chksumbytes,dtt::ufast322byte(fullchksum).data(),4);
        std::memcpy(fulldata.data+cursor,&chksumbytes,4);
        */
        std::memcpy(fulldata.data+cursor,dtt::ufast322byte(fullchksum).data(),4);
        cursor += 4;
        std::memcpy(fulldata.data+cursor,files[i].data,files[i].size);
        cursor+=files[i].size;
    }
    return fulldata;
};
void usarchive::unArchive(const std::string &path){
    std::filesystem::path rpath = stringtools::trimEnds(path);
    if(!std::filesystem::is_directory(rpath)){err=4;return;}
    if(!std::filesystem::exists(path)){std::filesystem::create_directories(rpath);} //err=4;return;
    for(size_t i=0;i<files.size();i++){
        std::filesystem::path fpath = rpath / std::string(reinterpret_cast<char *>(files[i].name),files[i].namesize);
        std::filesystem::path fppath = fpath.parent_path();
        if(fpath.has_parent_path() && !std::filesystem::exists(fppath)){std::filesystem::create_directories(fppath);}
        std::ofstream outfile(fpath,IOSBIN);
        if(!outfile.is_open()){err=3;outfile.close();return;}
        outfile.write(reinterpret_cast<char *>(files[i].data),files[i].size);
        outfile.close();
    }
}
void usarchive::open(std::ifstream &file){
    /*
    usa::file filedat;
    */
    //SHOULD I USE UNIONS FOR EVERY CONVERSION???
    if(!file.is_open()){
        err = 7;
        //file.close();
        return;
    }
    err = 0;
    total_size = file.tellg();
    file.seekg(0,IOSEND);
    total_size = static_cast<size_t>(file.tellg())-total_size; //WHY A +2?? file.gcount()
    file.seekg(0);
    char verbuf[2];
    file.read(verbuf,2);
    uint_fast16_t version = dtt::byte2ufast16(verbuf);
    if(version != 0){
        err = 1;
        //file.close();
        return;
    }
    while(true){
        usa::file filedat; //potential problem being locally allocated
        char nsizebuf[2];
        file.read(nsizebuf,2);
        if(file.eof()){break;}
        filedat.namesize = dtt::byte2ufast16(nsizebuf);
        filedat.name = new unsigned char[filedat.namesize]; //char *rnamebuf
        file.read(reinterpret_cast<char *>(filedat.name),filedat.namesize);
        if(file.eof()){delete[] filedat.name;err=8;break;}
        unsigned char filesizebuf[8];
        file.read(reinterpret_cast<char*>(filesizebuf),8);
        if(file.eof()){delete[] filedat.name;err=8;break;}
        filedat.size = dtt::byte2ufast64(reinterpret_cast<char*>(filesizebuf));
        unsigned char chksumbuf[4];
        file.read(reinterpret_cast<char*>(chksumbuf),4);
        if(file.eof()){delete[] filedat.name;err=8;break;}
        uint_fast32_t chksum = dtt::byte2ufast32(reinterpret_cast<char*>(chksumbuf));
        //uint_fast32_t rchksum = nsizebuf[0]+nsizebuf[1];
        //std::cout << "rck: " << rchksum << std::endl;
        //unsigned char *filesizebuf2 = reinterpret_cast<unsigned char*>(filesizebuf);
        uint_fast32_t rchksum = nsizebuf[0]+nsizebuf[1]+filesizebuf[0]+filesizebuf[1]+filesizebuf[2]+filesizebuf[3]+filesizebuf[4]+filesizebuf[5]+filesizebuf[6]+filesizebuf[7]; //filesizebuf prob
        //std::cout << "rck: " << rchksum << std::endl;
        for(uint_fast16_t i=0;i<filedat.namesize;i++){
            rchksum += filedat.name[i];
        }
        if(rchksum != chksum){
            //resolved std::cout << static_cast<int>(static_cast<uint8_t>(chksumbuf[0])) << std::endl << static_cast<int>(static_cast<uint8_t>(chksumbuf[1])) << std::endl << static_cast<int>(static_cast<uint8_t>(chksumbuf[2])) << std::endl << static_cast<int>(static_cast<uint8_t>(chksumbuf[3])) << std::endl;
            //std::cout << chksum << std::endl;
            //std::cout << rchksum << std::endl;
            err = 2;
            delete[] filedat.name;
            //file.close();
            return;
        }
        filedat.data = new unsigned char[filedat.size]; //char *fullfiledata
        file.read(reinterpret_cast<char *>(filedat.data),filedat.size);
        if(file.eof()){delete[] filedat.name;delete[] filedat.data;err=8;break;}
        files.push_back(filedat);
        //file.eof() && break;
    }
}
unsigned short usarchive::error(){
    return err;
}
