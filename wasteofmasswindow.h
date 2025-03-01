#ifndef WASTEOFMASSWINDOW_H
#define WASTEOFMASSWINDOW_H

#include "wasteofengine.h"
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QListWidgetItem>
#include "loadmodal.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class WasteofMassWindow;
}
QT_END_NAMESPACE

class WasteOfEngine;

class WasteofMassWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WasteofMassWindow(QWidget *parent = nullptr);
    ~WasteofMassWindow();
    void showTempStatus(QString text);
    void setLoading(bool isLoading,QString msg);
    void updModalProg(unsigned short val);

private slots:
    void on_login_button_token_clicked();

    void on_login_button_login_clicked();

    void on_logout_button_clicked();

    void on_trendinguser_follow_btn_clicked();

    void on_list_add_btn_clicked();

    //void on_list_list_itemDoubleClicked(QListWidgetItem *item);

    void on_list_list_itemChanged(QListWidgetItem *item);

    void on_list_follow_btn_clicked();

    friend void followUser(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString username,unsigned short *status,unsigned short *progress,size_t lenusers);
    //friend void fetchUser(WasteofMassWindow *pwindow,QNetworkAccessManager *manager,QString username,QString endpoint,unsigned short *retries,unsigned short *page,bool auth);
    friend size_t followerFetchCallback(WasteofMassWindow *window, QJsonObject &data, unsigned short type);
    //friend size_t followingFetchCallback(WasteofMassWindow *window, QJsonObject &data);
    friend void checkFollow(WasteofMassWindow *window,QNetworkAccessManager *manager,QString username,unsigned short *status,unsigned short *progress,size_t lenusers,bool isUnfollow);
    friend size_t commentFetchCallback(WasteofMassWindow *window,QNetworkAccessManager *manager,QJsonObject &data,short unsigned mode);
    friend void followTrending(WasteofMassWindow *window,bool isUnfollow);
    friend void WasteOfEngine::loadConf(WasteofMassWindow *window);
    friend void WasteOfEngine::saveConf(WasteofMassWindow *window,bool isSigned);
    void followQList(QListWidget *list,bool isUnfollow);
    void changeEvent(QEvent *event);
    /*
    void showModal(QString msg,QString title);
    void updModal(QString msg);
    void closeModal();
    */

    void on_clone_follow_btn_clicked();

    void on_clone_submit_btn_clicked();

    void on_post_submit_btn_clicked();

    void on_post_follow_btn_clicked();

    void on_clone_follow_list_itemChanged(QListWidgetItem *item);

    void on_post_follow_list_itemChanged(QListWidgetItem *item);

    void on_wall_follow_list_itemChanged(QListWidgetItem *item);

    void on_comment_follow_list_itemChanged(QListWidgetItem *item);

    void on_list_clear_btn_clicked();

    void on_clone_clear_btn_clicked();

    void on_post_clear_btn_clicked();

    void on_wall_clear_btn_clicked();

    void on_comment_clear_btn_clicked();

    void on_wall_submit_btn_clicked();

    void on_wall_follow_btn_clicked();

    void on_comment_submit_btn_clicked();

    void on_comment_follow_btn_clicked();

    void on_list_load_btn_clicked();

    void on_list_exp_btn_clicked();

    void on_list_unfollow_btn_clicked();

    void on_trendinguser_unfollow_btn_clicked();

    void on_clone_unfollow_btn_clicked();

    void on_post_unfollow_btn_clicked();

    void on_wall_unfollow_btn_clicked();

    void on_comment_unfollow_btn_clicked();

    void on_trendinguser_submit_btn_clicked();

    void on_trendinguser_follow_list_itemChanged(QListWidgetItem *item);

    void on_trendinguser_clear_btn_clicked();

    void on_pushButton_clicked();

private:
    Ui::WasteofMassWindow *ui;
    loadModal *modal;
};
#endif // WASTEOFMASSWINDOW_H
