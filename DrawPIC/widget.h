#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QUdpSocket>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

private:
    struct line
    {
        QVector<qint16> x;
        QVector<qint16> y;
    };

    line newline,newline_net;
    QVector<line> oldline,oldline_net;

    QUdpSocket *udpSocket;

private:
    void initform();

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void readPendingDatagrams();

    void on_clear_clicked();
};

#endif // WIDGET_H
