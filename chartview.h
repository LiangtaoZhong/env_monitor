#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QObject>
#include <QWidget>
#include <QtCharts>
#include "QWheelEvent"
class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent = nullptr);
    ~ChartView();

signals:
    void wheelChanged(QWheelEvent *event);
    void mousePress(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
private slots:

    virtual void wheelEvent( QWheelEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:
    bool isPressed=false;
};

#endif // CHARTVIEW_H
