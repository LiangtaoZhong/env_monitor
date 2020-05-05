#include "chartview.h"
#include "QtDebug"
#include "QWheelEvent"
ChartView::ChartView(QChart *chart, QWidget *parent):
    QChartView(chart,parent)
{

}

ChartView::~ChartView()
{

}

void ChartView::wheelEvent(QWheelEvent *event)
{

    //qDebug()<<"event: "<<event->angleDelta();
    emit wheelChanged(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        isPressed = true;
    emit mousePress(event);

}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if(isPressed)
        emit mouseMove(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
}
