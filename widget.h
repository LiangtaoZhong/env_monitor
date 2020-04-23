#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_exitBtn_clicked();
    void on_loginBtn_clicked();
signals:
    void showMainWindow();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
