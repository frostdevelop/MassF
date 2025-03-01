#include "wasteofmasswindow.h"
#include "ui_wasteofmasswindow.h"
#include "loadmodal.h"
#include "wasteofengine.h"
#include <QMessageBox>
#include <QListWidgetItem>
//#include <filesystem>
#include <fstream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>
#include <QChronoTimer>

using namespace std::chrono_literals;

/*
QString usertoken = "";
QString usernam = "";
*/
enum class usermodeenum : unsigned short {FOLLOWER,FOLLOWING,POSTS};
QString usermode[3] = {"/followers","/following","/posts"};

WasteofMassWindow::WasteofMassWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WasteofMassWindow)
{
    ui->setupUi(this);
    modal = new loadModal(this);
    this->setWindowFlags((Qt::Window | Qt::WindowStaysOnTopHint) & ~Qt::WindowTitleHint);
    /*
    QPixmap img(QString::fromStdString(std::filesystem::current_path().string()+"\\rupturewasteof.png"));
    qDebug() << std::filesystem::current_path().string()+"\\rupturewasteof.png";
    //QMessageBox::information(this,"AAA",QString::fromStdString(std::filesystem::current_path().string()));
    //QGraphicsScene *scene = new QGraphicsScene(this);
    //ui->logoView->setScene(scene);
    //ui->logoView->scene()->addPixmap(img);
    ui->logoimg->setPixmap(img);
    */
    //mb i'm stpid
}

WasteofMassWindow::~WasteofMassWindow()
{
    delete ui;
    delete modal;
}

void WasteofMassWindow::changeEvent(QEvent *event){
    if (event->type() == QEvent::ActivationChange)
    {
        if(this->isActiveWindow())
        {
            if(this->windowOpacity() < 1){
                unsigned short *times = new unsigned short(14);
                QChronoTimer *anim = new QChronoTimer(15ms,this);
                connect(anim, &QChronoTimer::timeout, this, [this,anim,times](){
                    if(*times == 0){
                        delete times;
                        anim->stop();
                        anim->deleteLater();
                    }else{
                        (*times)--;
                        this->setWindowOpacity(1-0.25*(float(*times)/14));
                    }
                });
                anim->start();
            }
        }
        else
        {
            if(this->windowOpacity() == 1){
                unsigned short *times = new unsigned short(14);
                QChronoTimer *anim = new QChronoTimer(15ms,this);
                connect(anim, &QChronoTimer::timeout, this, [this,anim,times](){
                    if(*times == 0){
                        delete times;
                        anim->stop();
                        anim->deleteLater();
                    }else{
                        (*times)--;
                        this->setWindowOpacity(0.75+0.25*(float(*times)/14));
                    }
                });
                anim->start();
            }
        }
    }
};

/*
void WasteofMassWindow::showModal(QString msg,QString title="Load..."){
    modal->settitle(title);
    modal->setmsg(msg);
    modal->show();
};
void WasteofMassWindow::updModal(QString msg){
    modal->setmsg(msg);
};
void WasteofMassWindow::closeModal(){
    modal->close();
};
*/

void WasteofMassWindow::updModalProg(unsigned short val){
    modal->setprogress(val);
};

void WasteofMassWindow::showTempStatus(QString text){ui->statusbar->showMessage(text,1000);};
void WasteofMassWindow::setLoading(bool isLoading,QString msg=""){
    if(isLoading){
        modal->setprogress(0);
        ui->topbarlogo->setEnabled(false);
        modal->settitle("Load...");
        modal->setmsg(msg);
        if(modal->isHidden()) modal->show();
    }else{
        ui->topbarlogo->setEnabled(true);
        modal->close();
    }
};

//Well this is fantastic, ui is a private member (haha friend function l bozo)
void followUser(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString username,unsigned short *status,unsigned short *progress,size_t lenusers){
    qDebug() << "https://api.wasteof.money/users/" + username + "/followers";
    QNetworkRequest request(QUrl("https://api.wasteof.money/users/" + username + "/followers"));
    request.setRawHeader("Authorization",WasteOfEngine::usertoken.toUtf8());
    QNetworkReply *followrep = manager->post(request,nullptr);
    pwindow->connect(followrep,&QNetworkReply::finished,pwindow,[pwindow,progress,lenusers,manager,followrep,status](){
        unsigned short itemper = 100 / lenusers;
        *progress += itemper;
        pwindow->ui->statusbar->showMessage(QString::fromStdString("Following... " + std::to_string(*progress) + "%"));
        pwindow->modal->setprogress(*progress);
        if(followrep->error()!=QNetworkReply::NoError){
            //qDebug() << *progress;
            //qDebug() << static_cast<unsigned>(100 / lenusers);
            //qDebug() << "Follow Network Error: " + std::to_string(static_cast<int>(followrep->error()));
            qDebug() << "Follow Network Error: " + followrep->errorString();
            *status = followrep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        if(*progress == itemper*lenusers){
            //qDebug() << "Disposed progress";
            if(*status != 0){
                QMessageBox::critical(pwindow,"Error",QString::fromStdString("Some of the follows had an error: Status " + std::to_string(*status)));
                pwindow->ui->statusbar->showMessage(QString::fromStdString("STATUS " + std::to_string(*status)),1000);
            }else{
                QMessageBox::information(pwindow,"Success","Following finished with no errors!");
                pwindow->ui->statusbar->showMessage("Following finished with no errors.",1000);
            }
            pwindow->setLoading(false);
            manager->deleteLater();
            delete progress;
            delete status;
        }
    });
    return;
}

void checkFollow(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString username,unsigned short *status,unsigned short *progress,size_t lenusers,bool isUnfollow){
    qDebug() << "https://api.wasteof.money/users/" + username + "/followers/" + WasteOfEngine::usernam;
    QNetworkRequest request(QUrl("https://api.wasteof.money/users/" + username + "/followers/" + WasteOfEngine::usernam));
    request.setRawHeader("User-Agent","MASSF 1.0");
    QNetworkReply *reply = manager->get(request);
    pwindow->connect(reply,&QNetworkReply::finished,pwindow,[pwindow,manager,reply,username,status,progress,lenusers,isUnfollow](){
        if(reply->error()==QNetworkReply::NoError){
            QString data = reply->readAll();
            //qDebug() << data;
            //bool isFollowed =  reply->readAll() == "true";
            if(isUnfollow ? data == "true" : data == "false"){
                followUser(pwindow,manager,username,status,progress,lenusers);
                return;
            }else if(isUnfollow ? data != "false" : data !="true"){
                qDebug() << "UNRECOGNIZED FORMAT: " << data;
                *status = 600;
            }
        }else{
            qDebug() << "Follow Network Error: " + reply->errorString();
            *status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        unsigned short itemper = 100 / lenusers;
        *progress += itemper;
        if(*progress == itemper*lenusers){
            if(*status != 0){
                QMessageBox::critical(pwindow,"Error",QString::fromStdString("Some of the follows had an error: Status " + std::to_string(*status)));
                pwindow->ui->statusbar->showMessage(QString::fromStdString("STATUS " + std::to_string(*status)),1000);
            }else{
                QMessageBox::information(pwindow,"Success","Following finished with no errors!");
                pwindow->ui->statusbar->showMessage("Following finished with no errors.",1000);
            }
            pwindow->setLoading(false);
            manager->deleteLater();
            delete progress;
            delete status;
        }else{
            pwindow->modal->setprogress(*progress);
            pwindow->ui->statusbar->showMessage(QString::fromStdString("Following... " + std::to_string(*progress) + "%"));
        }
    });
}
//page should be nullptr if unused.
//QString endpoint
//QListWidget *list
void fetchUser(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString username,unsigned short type,unsigned short *retries,unsigned short *page,bool auth,size_t (*callback)(WasteofMassWindow*,QJsonObject&,unsigned short),void (*completion)(WasteofMassWindow*)){
    qDebug() << "https://api.wasteof.money/users/" + username + usermode[type] + "?page=" + QString::number(*page);
    QNetworkRequest request(QUrl("https://api.wasteof.money/users/" + username + usermode[type] + "?page=" + QString::number(*page)));
    //short-circuiting not applicable due to void return
    auth ? request.setRawHeader("Authorization",WasteOfEngine::usertoken.toUtf8()) : void();
    QNetworkReply *userrep = manager->get(request);
    //I hate this request object
    //shoot I need a friend function for the lambda (but also they call each other so infeasable)
    pwindow->connect(userrep,&QNetworkReply::finished,pwindow,[pwindow,manager,userrep,username,type,retries,page,auth,callback,completion](){
        if(userrep->error() == QNetworkReply::NoError){
            QJsonObject data = QJsonDocument::fromJson(userrep->readAll()).object();
            //Use function pointer? i'll return length of array back.
            //list->addItem(QListWidgetItem(data["user"]));
            size_t len = callback(pwindow,data,type);
            if(data["last"].toBool() || len==0){
                completion(pwindow);
                pwindow->setLoading(false);
                manager->deleteLater();
                delete retries;
                delete page;
            }else{
                *retries = 3;
                (*page)++;
                fetchUser(pwindow,manager,username,type,retries,page,auth,callback,completion);
            }
        }else{
            if(retries == 0){
                QMessageBox::critical(pwindow,"Error!","Max retries exceeded");
                pwindow->showTempStatus("Max retries exceeded");
                manager->deleteLater();
                delete retries;
                delete page;
                pwindow->setLoading(false);
            }else{
                (*retries)--;
            }
        }
    });
    //pwindow->connect(userrep,&QNetworkReply::finished,pwindow,fetchUserHandle(pwindow,manager,username,type,retries,page,auth,userrep));
    return;
}

/*
void followTrending(WasteofMassWindow *window,bool isUnfollow){
    if(WasteOfEngine::usertoken == ""){
        QMessageBox::critical(window,"Following Error","Please sign in first!");
        window->ui->statusbar->showMessage("Please sign in first!",1000);
        return;
    }
    QNetworkAccessManager *manager = new QNetworkAccessManager(window);
    QNetworkRequest request(QUrl("https://api.wasteof.money/explore/users/top"));
    request.setRawHeader("User-Agent", "MASSF 1.0");
    QNetworkReply *reply = manager->get(request);
    window->connect(reply,&QNetworkReply::finished,window,[window,manager,reply,isUnfollow](){
        if(reply->error()==QNetworkReply::NoError){
            //qDebug() << reply->readAll();
            QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
            if(data.isArray()){
                QJsonArray users = data.array();
                unsigned short *progress = new unsigned short(0);
                unsigned short *status = new unsigned short(0);
                //qDebug() << users.size();
                //qDebug() << static_cast<size_t>(users.size());
                size_t lenusers = static_cast<size_t>(users.size());
                for(size_t i=0;i<lenusers;i++){
                    //Add check for alr following;
                    QJsonObject user = users[i].toObject();
                    checkFollow(window,manager,user["name"].toString(),status,progress,lenusers,isUnfollow);
                    //
                    qDebug() << "https://api.wasteof.money/users/" + user["name"].toString() + "/followers";
                    QNetworkRequest request(QUrl("https://api.wasteof.money/users/" + user["name"].toString() + "/followers"));
                    request.setRawHeader("Authorization",WasteOfEngine::usertoken.toUtf8());
                    QNetworkReply *followrep = manager->post(request,nullptr);
                    connect(followrep,&QNetworkReply::finished,this,[this,progress,lenusers,manager,followrep,status](){
                        unsigned short itemper = 100 / lenusers;
                        *progress += itemper;
                        ui->statusbar->showMessage(QString::fromStdString("Following... " + std::to_string(*progress) + "%"));
                        if(followrep->error()!=QNetworkReply::NoError){
                            //qDebug() << *progress;
                            //qDebug() << static_cast<unsigned>(100 / lenusers);
                            //qDebug() << "Follow Network Error: " + std::to_string(static_cast<int>(followrep->error()));
                            qDebug() << "Follow Network Error: " + followrep->errorString();
                            *status = followrep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        }
                        if(*progress == itemper*lenusers){
                            qDebug() << "Disposed progress";
                            if(*status != 0){
                                QMessageBox::critical(this,"Error",QString::fromStdString("Some of the follows had an error: Status " + std::to_string(*status)));
                                ui->statusbar->showMessage(QString::fromStdString("STATUS " + std::to_string(*status)),1000);
                            }else{
                                QMessageBox::information(this,"Success","Following finished with no errors!");
                                ui->statusbar->showMessage("Following finished with no errors.",1000);
                            }
                            manager->deleteLater();
                            delete progress;
                            delete status;
                        }
                    });
                    //
                }
            }else{
                QMessageBox::critical(window,"Trending Error","Invalid response format");
                window->ui->statusbar->showMessage("Invalid response format",1000);
            }
        }else{
            QMessageBox::critical(window,"Trending Error","There was an error in the network: " + reply->errorString());
            window->ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
        }
        reply->deleteLater();
    });
    window->ui->statusbar->showMessage("Requesting Users, please wait...");
}
*/

//Add duplicate check?
void fetchComments(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString ID,unsigned short *retries,unsigned short *page,unsigned short mode,size_t (*callback)(WasteofMassWindow*,QNetworkAccessManager*,QJsonObject&,unsigned short),void (*completion)(WasteofMassWindow*)){
    //QString url = iswall ? "https://api.wasteof.money/users/" + ID + "/wall?page=" + QString::number(*page) : "https://api.wasteof.money/posts/" + ID + "/comments?page=" + QString::number(*page);
    QString url;
    switch(mode){
    case 0:
        url = "https://api.wasteof.money/posts/" + ID + "/comments?page=" + QString::number(*page);
        break;
    case 1:
        url = "https://api.wasteof.money/users/" + ID + "/wall?page=" + QString::number(*page);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        url = "https://api.wasteof.money/comments/" + ID + "/replies?page=" + QString::number(*page);
        break;
    }
    qDebug() << url;
    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = manager->get(request);
    pwindow->updModalProg(50);
    pwindow->connect(reply,&QNetworkReply::finished,pwindow,[pwindow,manager,reply,ID,retries,page,callback,completion,mode](){
        if(reply->error() == QNetworkReply::NoError){
            QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
            size_t len = callback(pwindow,manager,data,mode);
            if(data["last"].toBool() || len==0){
                pwindow->updModalProg(100);
                completion != nullptr ? completion(pwindow) : void();
                //manager->deleteLater();
                delete retries;
                delete page;
                pwindow->setLoading(false);
            }else{
                *retries = 3;
                (*page)++;
                fetchComments(pwindow,manager,ID,retries,page,mode,callback,completion);
            }
        }else{
            if(retries == 0){
                QMessageBox::critical(pwindow,"Comments Error!","Max retries exceeded");
                pwindow->showTempStatus("Max retries exceeded");
                manager->deleteLater();
                delete retries;
                delete page;
                pwindow->setLoading(false);
            }else{
                (*retries)--;
            }
        }
    });
    return;
}

void WasteofMassWindow::followQList(QListWidget *list,bool isUnfollow){
    if(WasteOfEngine::usertoken == ""){
        QMessageBox::critical(this,"Following Error","Please sign in first!");
        ui->statusbar->showMessage("Please sign in first!",1000);
        return;
    }
    setLoading(true,"Following users...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    size_t lenusers = list->count();
    unsigned short *progress = new unsigned short(0);
    unsigned short *status = new unsigned short(0);
    for(size_t i=0;i<lenusers;i++){
        QListWidgetItem *citem = list->item(i);
        checkFollow(this,manager,citem->text(),status,progress,lenusers,isUnfollow);
        //qDebug() << i;
    }
}

/*
void WasteofMassWindow::on_list_list_itemDoubleClicked(QListWidgetItem *item)
{
    ui->list_list->removeItemWidget(item);
}
*/

void WasteofMassWindow::on_list_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[4] = {ui->list_follow_btn,ui->list_unfollow_btn,ui->list_clear_btn,ui->list_exp_btn};
    WasteOfEngine::removeNullList(item,ui->list_list,buttons,4);
}

void WasteofMassWindow::on_trendinguser_follow_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[3] = {ui->trendinguser_follow_btn,ui->trendinguser_unfollow_btn,ui->trendinguser_clear_btn};
    WasteOfEngine::removeNullList(item,ui->trendinguser_follow_list,buttons,3);
}

/*
void WasteofMassWindow::on_list_input_returnPressed()
{
    this->on_list_add_btn_clicked();
}
*/

void WasteofMassWindow::on_list_follow_btn_clicked()
{
    this->followQList(ui->list_list,false);
}

void WasteofMassWindow::on_clone_follow_btn_clicked()
{
    this->followQList(ui->clone_follow_list,false);
}

void WasteofMassWindow::on_post_follow_btn_clicked()
{
    this->followQList(ui->post_follow_list,false);
}

void WasteofMassWindow::on_wall_follow_btn_clicked()
{
    this->followQList(ui->wall_follow_list,false);
}

void WasteofMassWindow::on_comment_follow_btn_clicked()
{
   this->followQList(ui->comment_follow_list,false);
}

void WasteofMassWindow::on_list_unfollow_btn_clicked()
{
   this->followQList(ui->list_list,true);
}

void WasteofMassWindow::on_clone_unfollow_btn_clicked()
{
   this->followQList(ui->clone_follow_list,true);
}

void WasteofMassWindow::on_post_unfollow_btn_clicked()
{
   this->followQList(ui->post_follow_list,true);
}

void WasteofMassWindow::on_wall_unfollow_btn_clicked()
{
   this->followQList(ui->wall_follow_list,true);
}

void WasteofMassWindow::on_comment_unfollow_btn_clicked()
{
   this->followQList(ui->comment_follow_list,true);
}

void WasteofMassWindow::on_trendinguser_follow_btn_clicked()
{
    //followTrending(this,false);
    this->followQList(ui->trendinguser_follow_list,false);
}

void WasteofMassWindow::on_trendinguser_unfollow_btn_clicked()
{
    //followTrending(this,true);
    this->followQList(ui->trendinguser_follow_list,true);
}

void WasteofMassWindow::on_clone_follow_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[3] = {ui->clone_follow_btn,ui->clone_unfollow_btn,ui->clone_clear_btn};
    WasteOfEngine::removeNullList(item,ui->clone_follow_list,buttons,3);
}


void WasteofMassWindow::on_post_follow_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[3] = {ui->post_follow_btn,ui->post_unfollow_btn,ui->post_clear_btn};
    WasteOfEngine::removeNullList(item,ui->post_follow_list,buttons,3);
}

void WasteofMassWindow::on_wall_follow_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[3] = {ui->wall_follow_btn,ui->wall_unfollow_btn,ui->wall_clear_btn};
    WasteOfEngine::removeNullList(item,ui->wall_follow_list,buttons,3);
}


void WasteofMassWindow::on_comment_follow_list_itemChanged(QListWidgetItem *item)
{
    QAbstractButton* buttons[3] = {ui->comment_follow_btn,ui->comment_unfollow_btn,ui->comment_clear_btn};
    WasteOfEngine::removeNullList(item,ui->comment_follow_list,buttons,3);
}

void WasteofMassWindow::on_list_clear_btn_clicked()
{
    ui->list_follow_btn->setEnabled(false);
    ui->list_unfollow_btn->setEnabled(false);
    ui->list_clear_btn->setEnabled(false);
    ui->list_exp_btn->setEnabled(false);
}


void WasteofMassWindow::on_clone_clear_btn_clicked()
{
    ui->clone_follow_btn->setEnabled(false);
    ui->clone_unfollow_btn->setEnabled(false);
    ui->clone_clear_btn->setEnabled(false);
}


void WasteofMassWindow::on_post_clear_btn_clicked()
{
    ui->post_follow_btn->setEnabled(false);
    ui->post_unfollow_btn->setEnabled(false);
    ui->post_clear_btn->setEnabled(false);
}


void WasteofMassWindow::on_wall_clear_btn_clicked()
{
    ui->wall_follow_btn->setEnabled(false);
    ui->wall_unfollow_btn->setEnabled(false);
    ui->wall_clear_btn->setEnabled(false);
}


void WasteofMassWindow::on_comment_clear_btn_clicked()
{
    ui->comment_follow_btn->setEnabled(false);
    ui->comment_unfollow_btn->setEnabled(false);
    ui->comment_clear_btn->setEnabled(false);
}

void WasteofMassWindow::on_trendinguser_clear_btn_clicked()
{
    ui->trendinguser_clear_btn->setEnabled(false);
    ui->trendinguser_follow_btn->setEnabled(false);
    ui->trendinguser_unfollow_btn->setEnabled(false);
}

void WasteofMassWindow::on_login_button_token_clicked()
{
    setLoading(true,"Logging in...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.wasteof.money/session"));
    request.setRawHeader("User-Agent", "MASSF 1.0");
    request.setRawHeader("Authorization",ui->token_login_input->text().toUtf8());
    connect(manager,&QNetworkAccessManager::finished,this,[this,manager](QNetworkReply *reply){
        if(reply->error()==QNetworkReply::NoError){
            QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
            if(data.contains("user")){
                WasteOfEngine::isToken = true;
                WasteOfEngine::usertoken = ui->token_login_input->text();
                WasteOfEngine::usernam = data["user"].toObject()["name"].toString();
                WasteOfEngine::saveConf(this,true);
                ui->statusbar->showMessage("Welcome back, "+WasteOfEngine::usernam,1000);
                ui->logout_button->setEnabled(true);
                QMessageBox::information(this,"Welcome","Welcome back, "+WasteOfEngine::usernam);
                //ui->login_tabs->setEnabled(false);
            }else{
                QMessageBox::critical(this,"Login Error","User not found!");
                ui->statusbar->showMessage("User not found!",1000);
            }
        }else{
            QMessageBox::critical(this,"Login Error","There was an error in the network: " + reply->errorString());
            ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
        }
        reply->deleteLater();
        manager->deleteLater();
        setLoading(false);
    });
    ui->statusbar->showMessage("Requesting Token Info, please wait...");
    manager->get(request);
}


void WasteofMassWindow::on_login_button_login_clicked()
{
    setLoading(true,"Fetching token details...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.wasteof.money/session"));
    request.setRawHeader("User-Agent", "MASSF 1.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject reqobj;
    //QMap <QString, QString> reqobj;
    reqobj["username"] = ui->username_login_input->text();
    reqobj["password"] = ui->password_login_input->text();
    QJsonDocument reqdoc(reqobj);
    connect(manager,&QNetworkAccessManager::finished,this,[this,manager](QNetworkReply *reply){
        updModalProg(100);
        if(reply->error()==QNetworkReply::NoError){
            QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
            if(!data.contains("error")){
                WasteOfEngine::isToken = false;
                WasteOfEngine::usertoken = data["token"].toString();
                WasteOfEngine::usernam = ui->username_login_input->text();
                WasteOfEngine::saveConf(this,true);
                //qDebug() << WasteOfEngine::usertoken;
                ui->login_tabs->setEnabled(false);
                ui->logout_button->setEnabled(true);
                ui->statusbar->showMessage("Welcome back, "+WasteOfEngine::usernam,1000);
                QMessageBox::information(this,"Welcome","Welcome back, "+WasteOfEngine::usernam);
            }else{
                QMessageBox::critical(this,"Login Error",data["error"].toString());
                ui->statusbar->showMessage(data["error"].toString(),1000);
            }
        }else{
            QMessageBox::critical(this,"Login Error","There was an error in the network: " + reply->errorString());
            ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
        }
        setLoading(false);
        reply->deleteLater();
        manager->deleteLater();
    });
    ui->statusbar->showMessage("Retreiving token, please wait...");
    manager->post(request,reqdoc.toJson(QJsonDocument::Compact));
    updModalProg(50);
    //qDebug() << reqdoc.toJson(QJsonDocument::Compact);
}


void WasteofMassWindow::on_logout_button_clicked()
{
    setLoading(true,"Logging out...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.wasteof.money/session"));
    request.setRawHeader("User-Agent", "MASSF 1.0");
    request.setRawHeader("Authorization", WasteOfEngine::usertoken.toUtf8());
    manager->deleteResource(request);
    updModalProg(50);
    connect(manager,&QNetworkAccessManager::finished,this,[this,manager](QNetworkReply *reply){
        updModalProg(100);
        if(reply->error()==QNetworkReply::NoError){
            QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
            if(!data.contains("error")){
                WasteOfEngine::usertoken = "";
                WasteOfEngine::usernam = "";
                WasteOfEngine::isToken = false;
                WasteOfEngine::saveConf(this,false);
                QMessageBox::information(this,"Success",data["ok"].toString());
                ui->login_tabs->setEnabled(true);
                ui->logout_button->setEnabled(false);
                ui->statusbar->showMessage(data["ok"].toString(),1000);
            }else{
                QMessageBox::critical(this,"Logout Error",data["error"].toString());
                ui->statusbar->showMessage(data["error"].toString(),1000);
            }
        }else{
            QMessageBox::critical(this,"Logout Error","There was an error in the network: " + reply->errorString());
            ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
        }
        setLoading(false);
        reply->deleteLater();
        manager->deleteLater();
    });
    ui->statusbar->showMessage("Logging out, please wait...");
}

/*
void WasteofMassWindow::on_follow_tabs_currentChanged(int index)
{
    //qDebug() << index;
    switch(index){
        case 1:
            qDebug() << "Switched to trending";
    }
}
*/

void WasteofMassWindow::on_list_add_btn_clicked()
{
    if(ui->list_input->text() != ""){
        QListWidgetItem *item = new QListWidgetItem(ui->list_input->text());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->list_list->addItem(item);
        ui->list_input->setText("");
        ui->list_follow_btn->setEnabled(true);
        ui->list_unfollow_btn->setEnabled(true);
        ui->list_clear_btn->setEnabled(true);
        ui->list_exp_btn->setEnabled(true);
    }
    /*
    else{
        ui->login_tabs->setTabVisible(1,true);
    }
    */
}

size_t followerFetchCallback(WasteofMassWindow *window, QJsonObject &data, unsigned short type){
    QJsonArray users = type==0 ? data["followers"].toArray() : data["following"].toArray();
    size_t len = static_cast<size_t>(users.size());
    for(size_t i=0;i<len;i++){
        QJsonObject user = users[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(user["name"].toString());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        window->ui->clone_follow_list->addItem(item);
    }
    return len;
}

/*
size_t followingFetchCallback(WasteofMassWindow *window, QJsonObject &data){
    QJsonArray users = data["following"].toArray();
    size_t len = static_cast<size_t>(users.size());
    for(size_t i=0;i<len;i++){
        QJsonObject user = users[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(user["name"].toString());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        window->ui->clone_follow_list->addItem(item);
    }
    return len;
}
*/

void WasteofMassWindow::on_clone_submit_btn_clicked()
{
    QString username = ui->clone_username_inp->text();
    if(username != ""){
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        ui->clone_follow_btn->setEnabled(true);
        ui->clone_unfollow_btn->setEnabled(true);
        ui->clone_clear_btn->setEnabled(true);
        if(ui->clone_follower_chk->isChecked()){
            ui->statusbar->showMessage("Fetching followers...");
            unsigned short *page = new unsigned short(1);
            unsigned short *retries = new unsigned short(3);
            /*
             * Whoops doesn't work lol
            auto callbacklamb = [this](QJsonObject gg) -> int {return 5;};
            std::function<int(QJsonObject)> callback = callbacklamb;
            */
            setLoading(true,"Fetching followers...");
            fetchUser(this,manager,username,0,retries,page,false,followerFetchCallback,[](WasteofMassWindow *window){QMessageBox::information(window,"Success!","Follower fetch completed!");window->showTempStatus("Follower fetch completed!");window->setLoading(false);});
        }
        if(ui->clone_following_chk->isChecked()){
            ui->statusbar->showMessage("Fetching following...");
            unsigned short *page = new unsigned short(1);
            unsigned short *retries = new unsigned short(3);
            setLoading(true,"Fetching following...");
            fetchUser(this,manager,username,1,retries,page,false,followerFetchCallback,[](WasteofMassWindow *window){QMessageBox::information(window,"Success!","Following fetch completed!");window->showTempStatus("Following fetch completed!");window->setLoading(false);});
        }
    }else{
        QMessageBox::critical(this,"Clone Error","Empty username field!");
        ui->statusbar->showMessage("Empty username field!",1000);
    }
}

size_t commentFetchCallback(WasteofMassWindow *window,QNetworkAccessManager *manager,QJsonObject &data,short unsigned mode){
    QJsonArray users = data["comments"].toArray();
    size_t len = static_cast<size_t>(users.count());
    for(unsigned short i=0;i<len;i++){
        QJsonObject post = users[i].toObject();
        QString username = post["poster"].toObject()["name"].toString();
        switch(mode){
        case 0:
        case 3:
            //window->ui->post_follow_list->findItems(username,Qt::MatchExactly).count()==0 ? window->ui->post_follow_list->addItem(item) : void();
            if(window->ui->post_follow_list->findItems(username,Qt::MatchExactly).count()==0){
                QListWidgetItem *item = new QListWidgetItem(username);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                window->ui->post_follow_list->addItem(item);
            }
            if(window->ui->post_reply_chk->isChecked() && post["hasReplies"].toBool()){
                unsigned short *page = new unsigned short(1);
                unsigned short *retries = new unsigned short(3);
                fetchComments(window,manager,post["_id"].toString(),retries,page,3,commentFetchCallback,nullptr);
            }
            break;
        case 1:
        case 4:
            //window->ui->wall_follow_list->findItems(username,Qt::MatchExactly).count()==0 ? window->ui->wall_follow_list->addItem(item) : void();
            if(window->ui->wall_follow_list->findItems(username,Qt::MatchExactly).count()==0){
                QListWidgetItem *item = new QListWidgetItem(username);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                window->ui->wall_follow_list->addItem(item);
            }
            if(window->ui->wall_reply_chk->isChecked() && post["hasReplies"].toBool()){
                unsigned short *page = new unsigned short(1);
                unsigned short *retries = new unsigned short(3);
                fetchComments(window,manager,post["_id"].toString(),retries,page,4,commentFetchCallback,nullptr);
            }
            break;
        case 2:
        case 5:
            //window->ui->comment_follow_list->findItems(username,Qt::MatchExactly).count()==0 ? window->ui->comment_follow_list->addItem(item) : void();
            if(window->ui->comment_follow_list->findItems(username,Qt::MatchExactly).count()==0){
                QListWidgetItem *item = new QListWidgetItem(username);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                window->ui->comment_follow_list->addItem(item);
            }
            if(window->ui->comment_reply_chk->isChecked() && post["hasReplies"].toBool()){
                unsigned short *page = new unsigned short(1);
                unsigned short *retries = new unsigned short(3);
                fetchComments(window,manager,post["_id"].toString(),retries,page,5,commentFetchCallback,nullptr);
            }
            break;
        }
        //iswall ? window->ui->wall_follow_list->addItem(item) : window->ui->post_follow_list->addItem(item);
    }
    return len;
}

void WasteofMassWindow::on_post_submit_btn_clicked()
{
    QString postID = ui->post_postid->text();
    if(postID != ""){
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        ui->post_follow_btn->setEnabled(true);
        ui->post_unfollow_btn->setEnabled(true);
        ui->post_clear_btn->setEnabled(true);
        if(ui->post_poster_chk->isChecked()){
            ui->statusbar->showMessage("Fetching post info...");
            QNetworkRequest request(QUrl("https://api.wasteof.money/posts/" + postID));
            request.setRawHeader("User-Agent", "MASSF 1.0");
            QNetworkReply *reply = manager->get(request);
            connect(reply,&QNetworkReply::finished,this,[this,manager,reply](){
                if(reply->error()==QNetworkReply::NoError){
                    //qDebug() << reply->readAll();
                    QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
                    if(data.isObject()){
                        QListWidgetItem *item = new QListWidgetItem(data.object()["poster"].toObject()["name"].toString());
                        item->setFlags(item->flags() | Qt::ItemIsEditable);
                        ui->post_follow_list->addItem(item);
                        ui->statusbar->showMessage("Successfully fetched poster.",1000);
                        QMessageBox::information(this,"Success","Successfully fetched poster.");
                    }else{
                        ui->statusbar->showMessage("Invalid response format",1000);
                        QMessageBox::critical(this,"Poster Error","Invalid response format");
                    }
                }else{
                    ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
                    QMessageBox::critical(this,"Poster Error","There was an error in the network: " + reply->errorString());
                }
                reply->deleteLater();
                manager->deleteLater();
            });
        }
        if(ui->post_comment_chk->isChecked()){
            ui->statusbar->showMessage("Fetching commenters...");
            unsigned short *page = new unsigned short(1);
            unsigned short *retries = new unsigned short(3);
            setLoading(true,"Fetching post commenters");
            fetchComments(this,manager,postID,retries,page,0,commentFetchCallback,[](WasteofMassWindow *window){QMessageBox::information(window,"Success!","Post comment fetch completed!");window->showTempStatus("Comment fetch completed!");window->setLoading(false);});
        }
    }else{
        ui->statusbar->showMessage("Empty post ID field!",1000);
        QMessageBox::critical(this,"Post Error","Empty post ID field!");
    }
}

void WasteofMassWindow::on_wall_submit_btn_clicked()
{
    ui->wall_follow_btn->setEnabled(true);
    ui->wall_unfollow_btn->setEnabled(true);
    ui->wall_clear_btn->setEnabled(true);
    QString username = ui->wall_username_inp->text();
    if(username != ""){
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        ui->statusbar->showMessage("Fetching usernames...");
        unsigned short *page = new unsigned short(1);
        unsigned short *retries = new unsigned short(3);
        setLoading(true,"Fetching wall commenters...");
        fetchComments(this,manager,username,retries,page,1,commentFetchCallback,[](WasteofMassWindow *window){QMessageBox::information(window,"Success!","Wall comment fetch completed!");window->showTempStatus("Wall fetch completed!");window->setLoading(false);});
    }else{
        ui->statusbar->showMessage("Empty username field!",1000);
        QMessageBox::critical(this,"Wall Error","Empty username field!");
    }
}

void WasteofMassWindow::on_comment_submit_btn_clicked()
{
    QString commID = ui->comment_id_inp->text();
    if(commID != ""){
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        ui->comment_follow_btn->setEnabled(true);
        ui->comment_unfollow_btn->setEnabled(true);
        ui->comment_clear_btn->setEnabled(true);
        unsigned short *page = new unsigned short(1);
        unsigned short *retries = new unsigned short(3);
        ui->statusbar->showMessage("Fetching repliers...");
        setLoading(true,"Fetching repliers...");
        fetchComments(this,manager,commID,retries,page,2,commentFetchCallback,[](WasteofMassWindow *window){QMessageBox::information(window,"Success!","Reply fetch completed!");window->showTempStatus("Reply fetch completed!");window->setLoading(false);});
    }else{
        ui->statusbar->showMessage("Empty comment ID field!",1000);
        QMessageBox::critical(this,"Comment Error","Empty comment ID field!");
    }
}

/*
bool isUTF(std::ifstream &check){
    char buff[3];
    check.read(buff,3);
    if(buff[0]==(char)0xEF && buff[1]==(char)0xBB && buff[2]==(char)0xBF){return true;}else{check.seekg(0);return false;};
}
*/

void WasteofMassWindow::on_list_load_btn_clicked()
{
    setLoading(true,"Processing files...");
    QString fileName = QFileDialog::getOpenFileName(this, "Open Users CSV", "", "Csv Files (*.csv);;All Files (*)");
    if(fileName.isEmpty()){
        QMessageBox::critical(this,"Users Error","File i/o error!");
        return;
    }
    std::ifstream userfile(fileName.toStdString());
    if(!userfile.is_open()){
        QMessageBox::critical(this,"File Error","File inaccessable!");
    }else{
        /*
        std::ostringstream temp;
        temp << userfile.rdbuf();
        content = temp.str();
        */
        std::string user;
        bool fired = false;
        while(std::getline(userfile,user,',')){
            if(ui->list_list->findItems(QString::fromStdString(user),Qt::MatchExactly).count()==0){
                QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(user));
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                ui->list_list->addItem(item);
            }
            fired = true;
        }
        if(fired){ui->list_follow_btn->setEnabled(true);ui->list_unfollow_btn->setEnabled(true);ui->list_clear_btn->setEnabled(true);ui->list_exp_btn->setEnabled(true);}
    }
    userfile.close();
    setLoading(false);
}

void WasteofMassWindow::on_list_exp_btn_clicked()
{
    setLoading(true,"Processing files...");
    std::string fullcontent = "";
    size_t len = ui->list_list->count();
    for(unsigned short i=0;i<len;i++){
        fullcontent+=ui->list_list->item(i)->text().toStdString() + (i==len-1 ? "" : ",");
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Users CSV", "users.csv", "Csv Files (*.csv);;All Files (*)");
    std::ofstream userfile(fileName.toStdString());
    if(!userfile.is_open()){
        QMessageBox::critical(this,"Users Error","File i/o error!");
    }else{
        userfile << fullcontent;
    }
    userfile.close();
    setLoading(false);
}



void WasteofMassWindow::on_trendinguser_submit_btn_clicked()
{
    setLoading(true,"Fetching trending users...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.wasteof.money/explore/users/top"));
    request.setRawHeader("User-Agent", "MASSF 1.0");
    QNetworkReply *reply = manager->get(request);
    connect(reply,&QNetworkReply::finished,this,[this,manager,reply](){
        updModalProg(100);
        if(reply->error()==QNetworkReply::NoError){
            QJsonDocument data = QJsonDocument::fromJson(reply->readAll());
            if(data.isArray()){
                QJsonArray users = data.array();
                size_t lenusers = static_cast<size_t>(users.size());
                for(size_t i=0;i<lenusers;i++){
                    QString fetchusername = users[i].toObject()["name"].toString();
                    if(ui->trendinguser_follow_list->findItems(fetchusername,Qt::MatchExactly).count()==0){
                        QListWidgetItem *item = new QListWidgetItem(fetchusername);
                        item->setFlags(item->flags() | Qt::ItemIsEditable);
                        ui->trendinguser_follow_list->addItem(item);
                    }
                }
                if(lenusers > 0){
                    ui->trendinguser_clear_btn->setEnabled(true);
                    ui->trendinguser_follow_btn->setEnabled(true);
                    ui->trendinguser_unfollow_btn->setEnabled(true);
                }
                QMessageBox::information(this,"Success","Fetched trending users!");
                ui->statusbar->showMessage("Fetched trending users!",1000);
            }else{
                QMessageBox::critical(this,"Trending Error","Invalid response format");
                ui->statusbar->showMessage("Invalid response format",1000);
            }
        }else{
            QMessageBox::critical(this,"Trending Error","There was an error in the network: " + reply->errorString());
            ui->statusbar->showMessage("Error: " + reply->errorString(),1000);
        }
        setLoading(false);
        reply->deleteLater();
        manager->deleteLater();
    });
    updModalProg(50);
    ui->statusbar->showMessage("Requesting Users, please wait...");
}

void WasteofMassWindow::on_pushButton_clicked()
{
    this->close();
}
