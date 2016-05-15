#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initform();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initform()
{
    udpSocket = new QUdpSocket;
    udpSocket->bind(7755);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    oldline.clear();
    newline.x.clear();
    newline.y.clear();

    oldline_net.clear();
    newline_net.x.clear();
    newline_net.y.clear();

    qDebug()<<"Program Init Successfully.";
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen=painter.pen();
    pen.setWidth(2);
    pen.setColor(Qt::white);

    painter.setPen(pen);
    for(int i=0;i<oldline.count();i++)
    {
        if(oldline[i].x.count()==1)
        {
            painter.drawPoint(oldline[i].x[0]*width()/10000,
                    oldline[i].y[0]*height()/10000);
        }
        else
        {
            for(int j=0;j<oldline[i].x.count()-1;j++)
            {
                painter.drawLine(oldline[i].x[j]*width()/10000,
                                 oldline[i].y[j]*height()/10000,
                                 oldline[i].x[j+1]*width()/10000,
                        oldline[i].y[j+1]*height()/10000);
            }
        }
    }

    if(newline.x.count()==1)
    {
        painter.drawPoint(newline.x[0]*width()/10000,newline.y[0]*height()/10000);
    }
    else
    {
        for(int j=0;j<newline.x.count()-1;j++)
        {
            painter.drawLine(newline.x[j]*width()/10000,
                             newline.y[j]*height()/10000,
                             newline.x[j+1]*width()/10000,
                    newline.y[j+1]*height()/10000);
        }
    }

    for(int i=0;i<oldline_net.count();i++)
    {
        if(oldline_net[i].x.count()==1)
        {
            painter.drawPoint(oldline_net[i].x[0]*width()/10000,
                    oldline_net[i].y[0]*height()/10000);
        }
        else
        {
            for(int j=0;j<oldline_net[i].x.count()-1;j++)
            {
                painter.drawLine(oldline_net[i].x[j]*width()/10000,
                                 oldline_net[i].y[j]*height()/10000,
                                 oldline_net[i].x[j+1]*width()/10000,
                        oldline_net[i].y[j+1]*height()/10000);
            }
        }
    }

    if(newline_net.x.count()==1)
    {
        painter.drawPoint(newline_net.x[0]*width()/10000,newline_net.y[0]*height()/10000);
    }
    else
    {
        for(int j=0;j<newline_net.x.count()-1;j++)
        {
            painter.drawLine(newline_net.x[j]*width()/10000,
                             newline_net.y[j]*height()/10000,
                             newline_net.x[j+1]*width()/10000,
                    newline_net.y[j+1]*height()/10000);
        }
    }
    qDebug()<<"Picture Painted Successfully.";
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->x()>=0&&event->y()>=0)
    {
        qint32 point;
        qint16 x,y;
        point=0;
        x = event->x()*10000/width();
        y = event->y()*10000/height();
        point=((point|x)<<16)|y;
        QByteArray datagram;
        datagram.resize(32);
        memcpy(datagram.data(),&point,sizeof(point));
        udpSocket->writeDatagram(datagram.data(),datagram.size(),
                                 QHostAddress::Broadcast,7755);
        qDebug()<<"MousePressValue Send Successfully. Value:"<<x<<y;
        newline.x << x;
        newline.y << y;
        update();
    }
    else
    {
        qDebug()<<"Point Value Out Of Space.";
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x()>=0&&event->y()>=0)
    {
        qint32 point;
        qint16 x,y;
        point=0;
        x = event->x()*10000/width();
        y = event->y()*10000/height();
        point=((point|x)<<16)|y;
        QByteArray datagram;
        datagram.resize(32);
        memcpy(datagram.data(),&point,sizeof(point));
        udpSocket->writeDatagram(datagram.data(),datagram.size(),
                                 QHostAddress::Broadcast,7755);
        qDebug()<<"MouseMoveValue Send Successfully. Value:"<<x<<y;
        newline.x << x;
        newline.y << y;
        update();
    }
    else
    {
        qDebug()<<"Point Value Out Of Space.";
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    QByteArray datagram;
    qint32 endsignal;
    endsignal=0;
    datagram.resize(32);
    memcpy(datagram.data(),&endsignal,sizeof(endsignal));
    udpSocket->writeDatagram(datagram.data(),datagram.size(),
                             QHostAddress::Broadcast,7755);
    qDebug()<<"Stop Signal Send Successfully.";
    oldline << newline;
    newline.x.clear();
    newline.y.clear();
    update();
}

void Widget::readPendingDatagrams()
{
    QByteArray datagram;
    datagram.resize(32);
    QHostAddress sender;
    quint16 senderPort;
    udpSocket->readDatagram(datagram.data(), datagram.size(),
                            &sender, &senderPort);

    qint32 data_get;
    memcpy(&data_get,datagram.data(),sizeof(data_get));

    if(data_get==0)
    {
        oldline_net << newline_net;
        newline_net.x.clear();
        newline_net.y.clear();
        qDebug()<<"New Line Saved.";
    }
    else if(data_get==1)
    {
        oldline.clear();
        newline.x.clear();
        newline.y.clear();

        oldline_net.clear();
        newline_net.x.clear();
        newline_net.y.clear();
        qDebug()<<"NetData Clear Successfully.";
    }
    else
    {
        qint16 x,y;
        x=0;
        y=0;
        x=(data_get>>16)|x;
        y=data_get|y;
        newline_net.x << x;
        newline_net.y << y;
        qDebug()<<"PointValue Get Successfully. Value:"<<x<<y;
    }
    update();
}

void Widget::on_clear_clicked()
{
    oldline.clear();
    newline.x.clear();
    newline.y.clear();

    oldline_net.clear();
    newline_net.x.clear();
    newline_net.y.clear();

    QByteArray datagram;
    qint32 clearsignal;
    clearsignal=1;
    datagram.resize(32);
    memcpy(datagram.data(),&clearsignal,sizeof(clearsignal));
    udpSocket->writeDatagram(datagram.data(),datagram.size(),
                             QHostAddress::Broadcast,7755);

    qDebug()<<"LocalData Clear Successfully.";

    update();
}
