#include "widget.h"
#include "mainwindow.h"
#include <QApplication>
#include "QtSql"
#include "timer.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    MainWindow mainwin;
    w.show();

    //每秒生成一些随机数
    Timer timer;

    qApp->setWindowIcon(QIcon(":/Z_icon.png"));
    QObject::connect(&w,SIGNAL(showMainWindow()),&mainwin,SLOT(on_showMainWindow()));

    return a.exec();
}
