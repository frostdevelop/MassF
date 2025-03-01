#include "loadmodal.h"
#include <QVBoxLayout>

loadModal::loadModal(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Load...");
    setModal(true);
    //setStyleSheet("Q"); mb it inherits

    QFont font = QFont("Verdana");
    bar = new QProgressBar(this);
    font.setBold(true);
    font.setPointSize(15);
    bar->setRange(0,100);
    bar->setValue(0);
    bar->setMinimumHeight(30);
    bar->setMinimumWidth(300);
    bar->setFont(font);
    font.setPointSize(25);
    title = new QLabel(this);
    title->setText("Loading...");
    title->setFont(font);
    font.setBold(false);
    font.setPointSize(12);
    message = new QLabel(this);
    message->setText("...");
    message->setFont(font);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addWidget(message);
    layout->addWidget(bar);

    setLayout(layout);
}
void loadModal::setprogress(unsigned short val){
    bar->setValue(val);
}
void loadModal::settitle(QString msg){
    title->setText(msg);
}
void loadModal::setmsg(QString msg){
    message->setText(msg);
}
