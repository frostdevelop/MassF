#include "wasteofmasswindow.h"
#include "wasteofengine.h"
#include "ui_wasteofmasswindow.h"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcessEnvironment>
//#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
//#include <fstream>
#include <filesystem>
#include "snowconf.h"
#include "usarchive.h"
#include "datatool.h"

QString WasteOfEngine::usernam = "";
QString WasteOfEngine::usertoken = "";
bool WasteOfEngine::isToken = false;
const std::string paths[] = {"../../release/","../../release/asset","../../release/generic","../../release/iconengines","../../release/imageformats","../../release/networkinformation","../../release/platforms","../../release/styles","../../release/tls","../../release/translations"};
//const std::string paths[] = {"./","./asset","./generic","./iconengines","./imageformats","./networkinformation","./platforms","./styles","./tls","./translations"};

WasteOfEngine::WasteOfEngine(){}
void WasteOfEngine::removeNullList(QListWidgetItem *item,QListWidget *list,QAbstractButton **button,const unsigned short len){
    if(item->text() == ""){
        //ui->list_list->removeItemWidget(item);
        /*
    for(short i=ui->list_list->count()-1;i>=0;i--){
        QListWidgetItem *citem = ui->list_list->item(i);
        qDebug() << citem->text();
        if(citem->text() == ""){
            citem = ui->list_list->takeItem(i);
            if(citem){delete citem;}
        }
    }
    */
        QListWidgetItem *citem = list->takeItem(list->currentRow());
        if(citem){
            delete citem;
            if(list->count() == 0){
                //button->setEnabled(false);
                /*
            size_t testlen = sizeof(button) / sizeof(button[0]);
            qDebug() << testlen;
            */
                for(unsigned short i=0;i<len;i++){
                    button[i]->setEnabled(false);
                }
            }
        }
        /*
    QList<QListWidgetItem *> empties = ui->list_list->findItems("",Qt::MatchExactly);
    for(unsigned short i=0;i<empties.size();i++){
        QListWidgetItem =
    }
    */
    }
};

void WasteOfEngine::loadConf(WasteofMassWindow *window){
    /*
    if(std::filesystem::exists("./data/massf.conf")){
        std::ifstream conf("./data/massf.conf");
        if(!conf.is_open()){
            QMessageBox::critical(window,"File Error","Reading config file inaccessable!");
        }else{
            std::string line;
            while(std::getline(conf,line)){

            }
        }
        conf.close();
    }
    */
    /*
    qDebug() << std::filesystem::current_path().string();
    qDebug() << std::filesystem::exists("./data");
    std::ifstream conft("./data/massf.sno");
    qDebug() << conft.is_open();
    conft.close();
    */
    try{
        rsnowconf conf("./data/massf.sno");
        snow::snowVal *data;
        unsigned short mode = 0;
        bool nError = true;
        bool isSigned = false;
        while ((data = conf.getNVar()) != nullptr && nError) {
            switch (data->type) {
            case snow::snowVal::HEADER:
                if(data->name == "MASSFAUTH"){
                    mode = 1;
                }else{
                    nError = false;
                    QMessageBox::critical(window,"Config Error","INVALID HEADER: "+QString::fromStdString(data->name));
                }
                break;
            case snow::snowVal::STRING:
                switch(mode){
                case 1:
                    if(data->name == "usertoken"){
                        //qDebug() << data->strVal;
                        usertoken = QString::fromStdString(data->strVal);
                        //qDebug() << usertoken;
                    }else if(data->name == "usernam"){
                        usernam = QString::fromStdString(data->strVal);
                    }else{
                        nError = false;
                        QMessageBox::critical(window,"Config Error","INVALID VAR: "+QString::fromStdString(data->name));
                    }
                }
                break;
            case snow::snowVal::BOOL:
                switch(mode){
                case 1:
                    //qDebug() << data->name;
                    if(data->name == "isToken"){
                        isToken = data->boolVal;
                    }else if(data->name == "isSigned"){
                        isSigned = data->boolVal;
                    }else{
                        nError = false;
                        QMessageBox::critical(window,"Config Error","INVALID VAR: "+QString::fromStdString(data->name));
                    }
                }
                break;
            case snow::snowVal::INT:
            case snow::snowVal::FLOAT:
                nError = false;
                QMessageBox::critical(window,"Config Error","INVALID TYPE: "+QString::number(data->type));
                break;
            }
            delete data;
            data = nullptr;
        }
        if(nError && isSigned){
            if(isToken){
                window->ui->login_tabs->setCurrentIndex(0);
                window->ui->token_login_input->setText(usertoken);
            }else{
                window->ui->login_tabs->setCurrentIndex(1);
                window->ui->username_login_input->setText(usernam);
                window->ui->login_tabs->setEnabled(false);
            }
            window->ui->statusbar->showMessage("Welcome back, " + usernam);
            window->ui->logout_button->setEnabled(true);
        }
        //qDebug() << nError;
        conf.close();
    }catch(const std::exception &e){
        QMessageBox::critical(window,"File Error","Loading config file error: "+QString(e.what()));
    }
};

void WasteOfEngine::saveConf(WasteofMassWindow *window,const bool isSigned){
    /*
    std::ofstream conf("./data/massf.conf");
    if(!conf.is_open()){
        QMessageBox::critical(window,"File Error","Writing config file inaccessable!");
    }else{
        conf << "#Hello user lol this is the funny config file\n$MASSFAUTH$\nisToken = "+std::to_string(isToken)+"\nusertoken = "+usertoken.toStdString()+"\nusernam = "+usernam.toStdString();
    }
    conf.close();
    */
    try{
        wsnowconf conf("./data/massf.sno");
        conf.writeComment("Hello user lol this is the funny config file");
        conf.writeHeader("MASSFAUTH");
        conf.writeVar("isSigned",isSigned);
        conf.writeVar("isToken",isToken);
        conf.writeVar("usertoken",usertoken.toStdString());
        conf.writeVar("usernam",usernam.toStdString());
        conf.close();
        //qDebug() << "Gaming!";
    }catch(const std::exception &e){
        QMessageBox::critical(window,"File Error","Writing config file inaccessable: "+QString(e.what()));
    }
};

int WasteOfEngine::verifyFS(){
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if(!std::filesystem::exists("./data/massf.sno")){return false;}
    //qDebug() << env.value("HMACKEY");
    usarchive archive;
    for(unsigned short i=0;i<10;i++){
        for (const auto& entry : std::filesystem::directory_iterator(paths[i])) {
            //qDebug() << entry.path().string().substr(6);
            if(std::filesystem::is_regular_file(entry.path())){
                //qDebug() << entry.path().string();
                archive.addFile(entry.path().string());
            }
        }
    }
    //qDebug() << archive.error();
    usa::databuf archivedata = archive.writeBuf();
    /*
    SHA256_CTX hashctx;
    SHA256_Init(&hashctx);
    */
    unsigned char archivehash[SHA256_DIGEST_LENGTH];
    size_t pos(0);

    EVP_MD_CTX *hashctx = EVP_MD_CTX_new();
    if(EVP_DigestInit_ex(hashctx, EVP_sha256(), nullptr) != 1 && EVP_DigestSignInit(hashctx, nullptr, EVP_sha256(), nullptr, nullptr) != 1 && EVP_DigestSignUpdate(hashctx, env.value("HMACKEY").toStdString().c_str(), env.value("HMACKEY").length()) != 1){
        EVP_MD_CTX_free(hashctx);
        return 3;
    };
    //unsigned char buf[8192];
    while(pos<archivedata.size){
        const size_t dataleft = archivedata.size-pos;
        //qDebug() << dataleft;
        if(EVP_DigestUpdate(hashctx,archivedata.data+pos,((dataleft>=8192) ? 8192 : dataleft)) != 1){
            EVP_MD_CTX_free(hashctx);
            return 3;
        };
        pos+=8192;
    }
    EVP_DigestFinal(hashctx,archivehash,nullptr);
    EVP_MD_CTX_free(hashctx);

    /*
    while(pos<archivedata.size){
        const size_t dataleft = archivedata.size-pos;
        qDebug() << dataleft;
        SHA256_Update(&hashctx,archivedata.data+pos,((dataleft>=8192) ? 8192 : dataleft));
        pos+=8192;
    }
    SHA256_Final(archivehash,&hashctx);
    */

    // for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++){
    //     qDebug() << std::to_string(static_cast<int>(archivehash[i]));
    // }

    std::string encodedHash = dtt::byte2hex(reinterpret_cast<unsigned char*>(archivehash),SHA256_DIGEST_LENGTH);
    //qDebug() << encodedHash;
    QEventLoop reqWait;
    QNetworkAccessManager netmanager;
    QNetworkRequest req(QUrl("https://cdn.frostco.org/cert/massf.pem"));
    QNetworkReply *hashreply = netmanager.get(req);

    QObject::connect(hashreply,&QNetworkReply::finished,&reqWait,&QEventLoop::quit);
    reqWait.exec();

    if(hashreply->error() != QNetworkReply::NoError){
        hashreply->deleteLater();
        netmanager.deleteLater();
        return 2;
    }
    QString hashpem = hashreply->readAll();
    hashreply->deleteLater();
    /*
     * Donno why I chose to try rsa encryption lol (HMAC IS BETTER)
    req = QNetworkRequest(QUrl("https://cdn.frostco.org/ver/pubkey.pem"));
    QNetworkReply *keyreply = netmanager.get(req);
    QObject::connect(keyreply,&QNetworkReply::finished,&reqWait,&QEventLoop::quit);
    reqWait.exec();
    if(keyreply->error() != QNetworkReply::NoError){
        keyreply->deleteLater();
        netmanager.deleteLater();
        return 2;
    }
    QString keypem = keyreply->readAll();
    keyreply->deleteLater();
    */
    netmanager.deleteLater();
    //qDebug() << hashpem;
    if(!(hashpem.startsWith("-----BEGIN SIGNATURE-----\n") && hashpem.endsWith("\n-----END SIGNATURE-----\n"))){
        return 4;
    }
    std::vector<char> decsig = dtt::b64decode(hashpem.mid(26,hashpem.size()-25).toStdString());
    std::string outsig(decsig.begin(),decsig.end());
    if(outsig != encodedHash){return 1;}
    //Haha regex l bozo
    return 0;
}

std::string WasteOfEngine::getFSHash(){
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    //std::vector<size_t> names;
    if(!std::filesystem::exists("./data/massf.sno")){return "";}
    usarchive archive;
    for(unsigned short i=0;i<10;i++){
        for (const auto& entry : std::filesystem::directory_iterator(paths[i])) { //../../release/
            //qDebug() << entry.path().string().substr(6);
            if(std::filesystem::is_regular_file(entry.path())){ // && entry.path().string() != "../../release/MassFollower.exe"
                //qDebug() << "./"+entry.path().string().substr(14);
                archive.addFile(entry.path().string());
                archive.putName(archive.size()-1,"./"+entry.path().string().substr(14));
                // usa::file temp = archive.getFile(archive.size()-1);
                // std::string tempname(reinterpret_cast<char*>(temp.name),temp.namesize);
                // qDebug() << tempname;
                // qDebug() << temp.namesize;
                // qDebug() << temp.size;
            }
        }
    }
    usa::databuf archivedata = archive.writeBuf();
    unsigned char archivehash[SHA256_DIGEST_LENGTH];
    size_t pos(0);
    EVP_MD_CTX *hashctx = EVP_MD_CTX_new();
    if(EVP_DigestInit_ex(hashctx, EVP_sha256(), nullptr) != 1 && EVP_DigestSignInit(hashctx, nullptr, EVP_sha256(), nullptr, nullptr) != 1 && EVP_DigestSignUpdate(hashctx, env.value("HMACKEY").toStdString().c_str(), env.value("HMACKEY").length()) != 1){
        EVP_MD_CTX_free(hashctx);
        return "";
    }
    //unsigned char buf[8192];
    while(pos<archivedata.size){
        const size_t dataleft = archivedata.size-pos;
        //qDebug() << dataleft;
        if(EVP_DigestUpdate(hashctx,archivedata.data+pos,((dataleft>=8192) ? 8192 : dataleft)) != 1){
            EVP_MD_CTX_free(hashctx);
            return "";
        };
        pos+=8192;
    }
    EVP_DigestFinal(hashctx,archivehash,nullptr);
    EVP_MD_CTX_free(hashctx);
    std::string encodedHash = dtt::byte2hex(reinterpret_cast<unsigned char*>(archivehash),SHA256_DIGEST_LENGTH);
    // names.push_back(archivedata.size);
    // for(int i=0;i<archive.size();i++){
    //     usa::file temp = archive.getFile(i);
    //     names.push_back(temp.namesize);
    //     names.push_back(temp.size);
    // }
    return encodedHash;
}

