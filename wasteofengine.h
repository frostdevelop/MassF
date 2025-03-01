#ifndef WASTEOFENGINE_H
#define WASTEOFENGINE_H
/*
 * was originally smth else
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
*/
#include <QAbstractButton>
#include <QListWidget>
#include <QListWidgetItem>
//#include <openssl/sha.h>

class WasteofMassWindow;

//Add a config file which can be written to.
//Add file and other stuff verification through sha fingerprints (tar gz fingerprints)
class WasteOfEngine
{

public:
    WasteOfEngine();
    ~WasteOfEngine();
    static void removeNullList(QListWidgetItem *item,QListWidget *list,QAbstractButton **button,unsigned short len);
    static QString usernam;
    static QString usertoken;
    static bool isToken; //if login using token
    static void saveConf(WasteofMassWindow *window,bool isSigned);
    static void loadConf(WasteofMassWindow *window);
    static int verifyFS();
    static std::string getFSHash();
    //static std::string trimEnds(std::string str);
private:
};
#endif // WASTEOFENGINE_H
