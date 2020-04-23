#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtDebug"
//#include "QWebEngineView"
#include "QtCharts"
#include "timer.h"
#include "QFile"
#include "QFileDialog"
#include "QRegExp"
MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->configRadio();
    this->uiInit();
    this->databaseInit();

}

void MainWindow::configRadio()
{
    ports=QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator iter=ports.begin();

    for (;iter != ports.end();iter++) {
        qDebug()<<(*iter).portName();
        portName.append((*iter).portName());
    }

    ui->portBox->addItems(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setDataBits(QSerialPort::Data8);



    QObject::connect(&serial,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
}

void MainWindow::databaseInit()
{
    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName("127.0.0.1");
    this->db.setDatabaseName("env_monitor");
    this->db.setUserName("root");
    this->db.setPassword("511025");
    bool ok =this->db.open();
    if(ok) {
        qDebug()<<"dababase open ok!";
    }

    model  = new QSqlTableModel();
    model->setTable("env_datas");
    model->select();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(model->fieldIndex("id"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->show();
}

void MainWindow::uiInit()
{
    this->setWindowTitle(tr("Environment monitoring client"));
    ui->tabWidget->setTabText(0,"database");
    ui->tabWidget->setTabText(1,"viual");
    ui->tabWidget->setTabText(2,"source");
    ui->radioEdit->setReadOnly(true);

}

QString MainWindow::returnName(int index)
{
    switch (index) {
    case 1:return "CO2";break;
    case 2:return "NO2";break;
    case 3:return "temp";break;
    case 4:return "hum";break;
    case 5:return "PM2.5";break;
    default: return "";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_showMainWindow()
{
    this->show();
}



void MainWindow::on_cornectBtn_clicked()
{
    QString selectedPort = ui->portBox->currentText();
    int index = ui->portBox->currentIndex();

    port = ports[index];
    qDebug()<<selectedPort<<" "<<index<<" "<<port.portName();
    serial.setPort(port);
    serial.close();
    serial.open(QIODevice::ReadWrite);
    //打开成功就要将按钮变为不可更改
    if(serial.isOpen()) {
        ui->cornectBtn->setEnabled(false);
        ui->portBox->setEnabled(false);
        qDebug()<<"open serial "<<port.portName()<<" successfully!";
    }

}

void MainWindow::on_breakBtn_clicked()
{
    serial.close();
    ui->cornectBtn->setEnabled(true);
    ui->portBox->setEnabled(true);
}

void MainWindow::on_readyRead()
{
    radioData.append(serial.readAll());
    ui->radioEdit->append(radioData);
    radioData.clear();
}

void MainWindow::on_clearBtn_clicked()
{
    ui->radioEdit->clear();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int selectedIndex;
    //选择到经纬度和时间就不绘制图形
    if(index.column() > 5) {
        return;
    }

    selectedIndex = index.column();
    QString selectName = this->returnName(selectedIndex);

    chart = new QChart;
    chart->setAnimationOptions(QChart::AllAnimations);
    QLineSeries * series = new QLineSeries;
    series->setName(selectName);
    //用于操作模型的映射器
    QVXYModelMapper * mapper = new QVXYModelMapper(this);
    mapper->setXColumn(0);
    mapper->setYColumn(selectedIndex);
    mapper->setSeries(series);
    mapper->setModel(model);
    chart->addSeries(series);

    chart->createDefaultAxes();
    chartView = new QChartView(chart,ui->virualWgt);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(ui->virualWgt->size());
}

//点击按钮 更新表格
void MainWindow::on_updateTableBtn_clicked()
{
    model->setTable("env_datas");
    model->select();
    ui->tableView->setModel(model);

    qDebug()<<"update table!";
}

//导出为csv文件
void MainWindow::on_exportBtn_clicked()
{
    //创建文件对话框
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setViewMode(QFileDialog::List);
    QString filename = dialog->getSaveFileName(this,
                               tr("export dababase"),NULL,tr("CSV files (*csv)"));

    if(filename.isNull())
        return;
    //匹配后缀为.csv的文件
    QRegExp rx("*.csv");
    rx.setPatternSyntax(QRegExp::Wildcard);
    bool isMatch = rx.exactMatch(filename);
    if(!isMatch) {
        filename +=".csv";
        qDebug()<<"failed to match";
    }

    qDebug()<<filename;

    //保存文件
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return;
    qDebug()<<"open file!";
    QSqlQueryModel model;
    model.setQuery("select * from env_datas");
    for ( int i =0 ; i < model.rowCount() ; ++i) {
        QString record ="";
        for(int j = 0; j<model.columnCount() - 1 ; ++j) {
            record += model.record(i).value(j).toString()+",";
        }
        record += model.record(i).value(model.columnCount()-1).toString() + "\r\n";
        QTextStream out(&file);
        out << record;

    }
    file.close();
}
