#include "wasteofmasswindow.h"
#include "wasteofengine.h"
#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);
    QSplashScreen splash(QPixmap(":/img/asset/splash.png"));
    splash.show();
    splash.showMessage("Initializing software...",Qt::AlignRight | Qt::AlignTop,Qt::white);
    WasteofMassWindow w;
    //w.show();
    QMessageBox::information(&splash,QString::fromStdString(WasteOfEngine::getFSHash()),"info");
    /*
    std::vector<size_t> tempv = WasteOfEngine::getFSHash();
    QString temp;
    for(unsigned i=0;i<tempv.size();i++){
        temp += QString::number(tempv[i]) + ", ";
    }
    QMessageBox::information(&splash,temp,"info");
    */
    splash.showMessage("Validating software...",Qt::AlignRight | Qt::AlignTop,Qt::white);
    int validsoft = WasteOfEngine::verifyFS();
    //qDebug() << validsoft;
    switch(validsoft){
        case 0:
            //Valid
            break;
        case 1:
            //Invalid;
            QMessageBox::critical(&splash,"Invalid Software!","Your installation of MASSF is invalid!");
            a.quit();
            return 0;
            break;
        case 2:
            QMessageBox::critical(&splash,"Network error!","Software validation could not be completed!");
            a.quit();
            return 0;
            break;
        case 3:
            QMessageBox::critical(&splash,"Hash error!","Software validation could not be completed!");
            a.quit();
            return 0;
            break;
        case 4:
            QMessageBox::critical(&splash,"Server error!","Hmm... It appears something is wrong with the server's certificate...");
            a.quit();
            return 0;
            break;
    }
    splash.showMessage("Loading Config...",Qt::AlignRight | Qt::AlignTop,Qt::white);
    WasteOfEngine::loadConf(&w);
    /*
    QTimer::singleShot(2500,&splash,SLOT(close()));
    QTimer::singleShot(2500,&w,SLOT(show()));
    */
    splash.close();
    w.show();
    //splash.finish(&w);
    return a.exec();
}
