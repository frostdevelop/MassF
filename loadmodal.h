#ifndef LOADMODAL_H
#define LOADMODAL_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class loadModal : public QDialog
{
    Q_OBJECT
public:
    explicit loadModal(QWidget *parent = nullptr);
    void setprogress(unsigned short val);
    void setmsg(QString msg);
    void settitle(QString msg);
private:
    QProgressBar *bar;
    QLabel *title;
    QLabel *message;
};

#endif // LOADMODAL_H
