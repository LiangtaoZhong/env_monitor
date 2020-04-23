#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include "QTimer"


class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = nullptr,int interval=1000);
    QTimer *timer;
private:

    int interval;
signals:
    //void updateTable();
public slots:
    void timerUpdate();
};

#endif // TIMER_H
