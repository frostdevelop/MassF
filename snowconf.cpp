#include "snowconf.h"
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <qDebug>
#include "stringtools.h"

rsnowconf::rsnowconf(std::ifstream *file):conf(file){
    if(!conf->is_open()){conf->close();throw std::invalid_argument("RSNOWCONF: File not open!");}
}

rsnowconf::rsnowconf(const std::string filename){
    if(!(filename.substr(filename.size()-5) != ".sno")){throw std::invalid_argument("RSNOWCONF: Invalid file extension!");}
    conf = new std::ifstream(stringtools::trimEnds(filename));
    if(!conf->is_open()){conf->close();throw std::invalid_argument("RSNOWCONF: File not open!");}
    std::string line;
    std::getline(*conf,line);
    if(line.substr(0,4) != "SNOW" || line.substr(5) != "1.0"){
        conf->close();throw std::invalid_argument("RSNOWCONF: Invalid format!");
    }
}

void rsnowconf::close(){
    conf->close();
}

rsnowconf::~rsnowconf(){
    conf->close();
    delete conf;
}

snow::snowVal* rsnowconf::getNVar(){
    snow::snowVal *data = new snow::snowVal;
    std::string line;
    while(std::getline(*conf,line)){
        if(line.size()>0){
            line = stringtools::trimEnds(line);
            switch(line[0]){
            case '$':
                if(line[line.size()-1] == '$'){
                    data->type = data->HEADER;
                    data->name = line.substr(1,line.size()-2);
                    currhead = data->name;
                    return data;
                }else{
                    delete data;
                    throw std::invalid_argument("RSNOWCONF: Invalid syntax expected header:" + line);
                }
            case '#':
                break;
            default:
                for(unsigned short i=0;i<line.size();i++){
                    if(line[i] == '='){
                        data->name = stringtools::trimR(line.substr(0,i));
                        const std::string valstr = stringtools::trimL(line.substr(i+1));
                        if(valstr.size() == 0){
                            delete data;
                            throw std::invalid_argument("RSNOWCONF: Invalid syntax expected value:" + line);
                        }else if(valstr[0] == '"'){
                            if(valstr[valstr.size()-1] == '"'){
                                data->type = data->STRING;
                                //qDebug() << valstr;
                                strncpy(data->strVal,valstr.substr(1,valstr.size()-2).c_str(),sizeof(data->strVal)-1); //valstr.size()-2
                                data->strVal[sizeof(data->strVal)-1] = '\0'; //valstr.size()
                                /*
                                qDebug() << valstr.substr(1,valstr.size()-2).c_str();
                                qDebug() << valstr.size()-2;
                                qDebug() << data->strVal;
                                */
                                return data;
                            }else{
                                delete data;
                                throw std::invalid_argument("RSNOWCONF: Invalid syntax expected string:" + line);
                            }
                        }else if(valstr == "false"){
                            data->type = data->BOOL;
                            data->boolVal = false;
                            return data;
                        }else if(valstr == "true"){
                            data->type = data->BOOL;
                            data->boolVal = true;
                            return data;
                        }else{
                            long intVal;
                            std::istringstream intstream(valstr);
                            intstream >> std::noskipws >> intVal;
                            if(intstream.eof() && !intstream.fail()){
                                data->type = data->INT;
                                data->intVal = intVal;
                                return data;
                            }else{
                                double floatVal;
                                intstream.clear();
                                intstream.seekg(0);
                                intstream >> std::noskipws >> floatVal;
                                if(intstream.eof() && !intstream.fail()){
                                    data->type = data->FLOAT;
                                    data->floatVal = floatVal;
                                    return data;
                                }else{
                                    delete data;
                                    throw std::invalid_argument("RSNOWCONF: Invalid value format:" + line);
                                }
                            }
                        }
                    }
                }
            }
        }
    };
    delete data;
    return nullptr;
}

wsnowconf::wsnowconf(std::ofstream *file):conf(file){
    if(!conf->is_open()){conf->close();throw std::invalid_argument("WSNOWCONF: File not open!");}
    *conf << "SNOW 1.0\n";
}

wsnowconf::wsnowconf(const std::string filename){
    if(!(filename.substr(filename.size()-5) != ".sno")){throw std::invalid_argument("WSNOWCONF: Invalid file extension!");}
    conf = new std::ofstream(stringtools::trimEnds(filename));
    if(!conf->is_open()){conf->close();throw std::invalid_argument("WSNOWCONF: File not open!");}
    *conf << "SNOW 1.0\n";
}

wsnowconf::~wsnowconf(){
    conf->close();
    delete conf;
}

void wsnowconf::writeHeader(const std::string text){
    *conf << "$"+text+"$\n";
}

void wsnowconf::writeComment(const std::string text){
    *conf << '#'+text+'\n';
}

void wsnowconf::close(){
    conf->close();
}
