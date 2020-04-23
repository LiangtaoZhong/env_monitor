#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "QSerialPort"
#include "QSerialPortInfo"
#include "QtSql"
#include "QtCharts"
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
    QString returnName(int index);
    ~MainWindow();

public slots:
    void on_showMainWindow();
    //void on_updateTable();
private slots:
    void on_cornectBtn_clicked();
    void on_breakBtn_clicked();
    void on_readyRead();
    void on_clearBtn_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_updateTableBtn_clicked();

    void on_exportBtn_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPortInfo port;
    QList<QSerialPortInfo> ports;
    QSerialPort serial;
    QStringList portName;
    QByteArray radioData;
    QSqlTableModel *model;
    QChart* chart;
    QChartView * chartView;

public:
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
