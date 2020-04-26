#include "widget.h"
#include "ui_widget.h"
#include "QtSql"
#include "QtDebug"
#include "QMessageBox"
#include "common.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("client-login in"));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_exitBtn_clicked()
{
    exit(0);
}

void Widget::on_loginBtn_clicked()
{
    bool isOk=false;
    QString edit_user = ui->useridEdit->text();
    QString edit_passwd = ui->passEdit->text();
    QSqlQuery query;


    query.exec("select * from user");

    while(query.next()) {
        QString user = query.value(1).toString();
        QString passwd = query.value(2).toString();
        if(edit_user == user && edit_passwd == passwd) {
            isOk = true;
            qDebug()<<"login successfully!";
            this->close();
            emit showMainWindow();
        }
    }

#if NEED_LOGIN == 1
    if(!isOk){
        qDebug()<<"password incorrent!";
        QMessageBox::warning(this,"failed to login in ","Password errors or User is not in database");
    }
#else
    emit showMainWindow();
    this->close();
#endif
}
