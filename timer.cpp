#include "timer.h"
#include "QtDebug"
#include "QRandomGenerator"
#include "QtSql"
#include "common.h"


Timer::Timer(QObject *parent,int interval) : QObject(parent),interval(interval)
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(interval);
    qDebug()<<"init timer";
}

//随机生成数据 并插入到数据库
void Timer::timerUpdate()
{
    #if INSERT_DATA_SIM == 1

    qint16 CO2,NO2,temp,hum,PM25;
    CO2 =static_cast<int>
            (390-20*QRandomGenerator::system()->generateDouble());
    NO2 =static_cast<int>
            (25-10*QRandomGenerator::system()->generateDouble());
    temp =static_cast<int>
            (38-8*QRandomGenerator::system()->generateDouble());
    hum =static_cast<int>
            (50-20*QRandomGenerator::system()->generateDouble());
    PM25 =static_cast<int>
            (25-5*QRandomGenerator::system()->generateDouble());

    qDebug()<<"CO2:"<<CO2
            <<"NO2:"<<NO2
            <<"temp:"<<temp
            <<"hum:"<<hum
            <<"PM2.5:"<<PM25;

    QSqlTableModel model;
    model.setTable("env_datas");
    model.select();
    int row = model.rowCount();
    model.insertRows(row,1);
    model.setData(model.index(row,1),CO2);
    model.setData(model.index(row,2),NO2);
    model.setData(model.index(row,3),temp);
    model.setData(model.index(row,4),hum);
    model.setData(model.index(row,5),PM25);
    model.setData(model.index(row,6),104.7);
    model.setData(model.index(row,7),31.4);
    bool ok = model.submitAll();
    if(ok) {
        qDebug()<<"insert data ok! row:"<<model.rowCount();
        //emit updateTable(); //更新表格
    }
    #endif

}
