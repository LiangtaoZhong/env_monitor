#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "QSerialPort"
#include "QSerialPortInfo"
#include "QtSql"
#include "QtCharts"
#include "chartview.h"
#include "common.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void configRadio();
    void databaseInit();
    void uiInit();
    void tableInit();
    QString returnName(int index);
    ~MainWindow();

public slots:
    void on_showMainWindow();

private slots:
    void on_cornectBtn_clicked();
    void on_breakBtn_clicked();
    void on_readyRead();
    void on_clearBtn_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_updateTableBtn_clicked();

    void on_exportBtn_clicked();

    void on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_refreshBtn_clicked();

    void on_wheelChanged(QWheelEvent *event);
    void on_mousePress(QMouseEvent *event);
    void on_mouseMove(QMouseEvent *event);

    void on_parseData(QByteArray& data);
    void on_updateDatabase(Msg& msg);
signals:
    void  parseData(QByteArray& data);
    void  updateDatabase(Msg& msg);
private:
    Ui::MainWindow *ui;
    QSerialPortInfo port;
    QList<QSerialPortInfo> ports;
    QSerialPort serial;
    QStringList portName;
    QByteArray radioData;
    QSqlTableModel *model;
    QChart* chart;
    struct chartXSize{
        int min;
        int max;
    }chartXSize;

    int currentPos;
    int lastPos;
    ChartView * chartView;
    QLineSeries *series;

    QByteArray raw_data;
public:
    QSqlDatabase db;
    Msg msg;
};

#endif // MAINWINDOW_H
