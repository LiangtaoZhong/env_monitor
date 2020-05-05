#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtDebug"
#include "QtCharts"
#include "timer.h"
#include "QFile"
#include "QFileDialog"
#include "QRegExp"
#include "QDateTime"
#include "common.h"
#include "chartview.h"
#include "QPixmap"
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

void MainWindow::on_refreshBtn_clicked()
{
    ui->portBox->clear();
    portName.clear();
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

}


void MainWindow::on_cornectBtn_clicked()
{
    QString selectedPort = ui->portBox->currentText();
    qDebug()<<"select port: "<<selectedPort;
    if(selectedPort.isNull())
        return;

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
    QObject::connect(&serial,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    connect(this,SIGNAL(parseData(QByteArray&)),this,SLOT(on_parseData(QByteArray&)));
    connect(this,SIGNAL(updateDatabase(Msg&)),this,SLOT(on_updateDatabase(Msg&)));
}


void MainWindow::on_breakBtn_clicked()
{
    serial.close();
    ui->cornectBtn->setEnabled(true);
    ui->portBox->setEnabled(true);
}

void MainWindow::on_readyRead()
{
    int packageSize = 17;
    static int readyReadSize = packageSize;
    static QByteArray temp;

    temp.append(serial.read(readyReadSize));
    if(temp.size() < packageSize) {
        readyReadSize = packageSize - temp.size();
    } else {
        readyReadSize = packageSize;
        emit parseData(temp);
        temp.clear();

    }

}



void MainWindow::on_parseData(QByteArray &data)
{
#define COMBINE_DATA(i,j)  static_cast<unsigned short>((static_cast<unsigned char>( raw_data.data()[i] )* 256)  \
                            + static_cast<unsigned char>(raw_data.data()[j]))
    char buf[100];
    raw_data.clear();
    memset(&msg,0,sizeof(msg));
    raw_data.append(data);

    if(raw_data.data()[0] == 0x3c && raw_data.data()[1] == 0x02) {

        msg.device = raw_data.data()[0] = 0x3c;
        msg.version = raw_data.data()[1] = 0x02;
        msg.CO2 =  COMBINE_DATA(2,3);
        msg.CH2O = COMBINE_DATA(4,5);
        msg.TVOC = COMBINE_DATA(6,7);
        msg.PM2_5 = COMBINE_DATA(8,9);
        msg.PM10 = COMBINE_DATA(10,11);
        msg.TEMP = raw_data.data()[12] + (float)raw_data.data()[13]*0.1f;
        msg.HUM = (raw_data.data()[14] + (float)raw_data.data()[15]*0.1f)*0.01f;
        msg.sum = raw_data.data()[16];

        if(ui->InsertCheckBox->isChecked())
            emit updateDatabase(msg);
        sprintf(buf, ">> CO2: %3dppm, PM2.5: %3dug/m3, PM10: %3dug/m3, Temperature: %.2f ℃, Humidity: %.2f%%",
                msg.CO2, msg.PM2_5, msg.PM10, msg.TEMP, msg.HUM*100);
        ui->radioEdit->append(buf);
    }
}

void MainWindow::on_updateDatabase(Msg &msg)
{
    QSqlTableModel model;
    model.setTable("env_datas");
    model.select();
    int row = model.rowCount();
    model.insertRows(row,1);
    model.setData(model.index(row,1),msg.CO2);
    model.setData(model.index(row,2),msg.PM2_5);
    model.setData(model.index(row,3),msg.PM10);
    model.setData(model.index(row,4),msg.TEMP);
    model.setData(model.index(row,5),msg.HUM);

    bool ok = model.submitAll();
    if(ok) {
        qDebug()<<"insert data ok! row:"<<model.rowCount();
    }
}


void MainWindow::on_clearBtn_clicked()
{
    ui->radioEdit->clear();
}

void MainWindow::databaseInit()
{
    this->db = QSqlDatabase::addDatabase("QMYSQL");
#if USE_LOCALHOST_DATABASE == 1
    this->db.setHostName("localhost");
    this->db.setDatabaseName("env_monitor");
    this->db.setUserName("root");
    this->db.setPassword("511025");
#else
    this->db.setHostName("139.180.218.58");
    this->db.setDatabaseName("env_monitor");
    this->db.setUserName("root");
    this->db.setPassword("root");
#endif
    bool ok =this->db.open();
    if(!ok)
        exit(0);

    this->tableInit();
}

void MainWindow::uiInit()
{
    this->setWindowTitle(tr("Environment monitoring client"));
    ui->tabWidget->setTabText(0,"database");
    ui->tabWidget->setTabText(1,"viual");
    ui->tabWidget->setTabText(2,"source");
    ui->radioEdit->setReadOnly(true);
    ui->plotLabel->setText(tr("Tips:左键移动，滚轮放缩，右键保存图片"));
    ui->tabelLabel->setText(tr("Tips:单击一列绘制波形"));
    ui->startTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->endTimeEdit->setMinimumDateTime(ui->startTimeEdit->dateTime());

}

void MainWindow::tableInit()
{
    model  = new QSqlTableModel();
    model->setTable("env_datas");
    model->select();
    ui->tableView->setModel(model);
    //ui->tableView->setColumnHidden(model->fieldIndex("id"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setToolTip(tr("选择一列绘制图形"));
    ui->tableView->show();
}

QString MainWindow::returnName(int index)
{
    switch (index) {
    case 1:return "CO2";break;
    case 2:return "PM2.5";break;
    case 3:return "PM10";break;
    case 4:return "temp";break;
    case 5:return "hum";break;

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
//放大缩小图片
void MainWindow::on_wheelChanged(QWheelEvent *event)
{
    int angle = event->angleDelta().y();
    QValueAxis *axisX = (QValueAxis*)(chart->axes().first());

    if(angle > 0) {
        int interval_len = axisX->max() - axisX->min();
        chart->axisX()->setRange(axisX->min(), axisX->min() + interval_len/2);
    } else {
        int interval_len = (axisX->max() - axisX->min()) > 1 ?(axisX->max() - axisX->min()) : 2;
        chart->axisX()->setRange(0, interval_len*2);
    }

    event->accept();
}


void MainWindow::on_mousePress(QMouseEvent *event)
{
    //实现平移图片
    if(event->button() == Qt::MidButton) {
        chart->axisX()->setRange(chartXSize.min, chartXSize.max);
    }

    if(event->button() == Qt::LeftButton) {
        lastPos = currentPos = event->x();
    }

    //保存曲线为图片
    if(event->button() == Qt::RightButton) {
        QPixmap image = QPixmap::grabWidget(chartView);
        QFileDialog * dialog = new QFileDialog(this);
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        dialog->setViewMode(QFileDialog::List);
        QString filename = dialog->getSaveFileName(this,
                                                   tr("export img"),NULL,tr("bmp files (*.bmp)"));
        if(filename.isNull())
            return;
        image.save(filename);
    }

}

void MainWindow::on_mouseMove(QMouseEvent *event)
{
    lastPos = currentPos;
    currentPos = event->x();
    int delta = lastPos - currentPos;
    chart->scroll(delta,0);
}



void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int selectedIndex;
    //选择到经纬度和时间就不绘制图形
    if(index.column() > 5 || index.column() < 1) {
        return;
    }

    selectedIndex = index.column();
    QString selectName = this->returnName(selectedIndex);

    chart = new QChart;
    //绘图动画
    //chart->setAnimationOptions(QChart::AllAnimations);
    series = new QLineSeries;
    series->setName(selectName);
    //用于操作模型的映射器
    QVXYModelMapper * mapper = new QVXYModelMapper(this);
    mapper->setXColumn(0);
    mapper->setYColumn(selectedIndex);
    mapper->setSeries(series);
    mapper->setModel(model);
    chart->addSeries(series);
    chart->createDefaultAxes();

    chartView = new ChartView(chart,ui->virualWgt);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(ui->virualWgt->size());

    QValueAxis *axisX = (QValueAxis*)(chart->axes().first());

    chartXSize.min =axisX->min();
    chartXSize.max = axisX->max();

    chartView->setToolTip(tr("左键平移，滚轮放大，右键保存图片"));
    connect(chartView,SIGNAL(wheelChanged(QWheelEvent *)), this,
            SLOT(on_wheelChanged(QWheelEvent *)));

    connect(chartView,SIGNAL(mousePress(QMouseEvent *)), this,
            SLOT(on_mousePress(QMouseEvent *)));

    connect(chartView,SIGNAL(mouseMove(QMouseEvent *)), this,
            SLOT(on_mouseMove(QMouseEvent *)));

}

//点击按钮 更新表格
void MainWindow::on_updateTableBtn_clicked()
{
    model->setTable("env_datas");
    model->select();
    ui->tableView->setModel(model);

   // ui->tableView->setColumnHidden(model->fieldIndex("id"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
                                               tr("export dababase"),NULL,tr("CSV files (*.csv)"));

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

    QTextStream out(&file);
    QSqlQueryModel model;

    out<<"id,CO2,NO2,temp,hum,PM2.5,longitude,latitude,time\n";
    model.setQuery("select * from env_datas");
    for ( int i =0 ; i < model.rowCount() ; ++i) {
        QString record ="";
        for(int j = 0; j<model.columnCount() - 1 ; ++j) {
            record += model.record(i).value(j).toString()+",";
        }
        record += model.record(i).value(model.columnCount()-1).toString() + "\n";

        out << record;

    }
    file.close();
}

//前面的时间不能小于后面的时间
void MainWindow::on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    ui->endTimeEdit->setMinimumDateTime(dateTime);
}


