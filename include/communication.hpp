//
// Created by Zai_Hui on 2026/2/27.
//

#ifndef ANALOG_SENSOR_COMMUNICATION_HPP
#define ANALOG_SENSOR_COMMUNICATION_HPP

#include <QtSerialPort/QSerialPort.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QTimer>
#include <QUdpSocket>
#include <queue>

class Communication : public QObject {
    Q_OBJECT
// -------------------------------------初始化模块----------------------------------------- //
public:
    // 总初始化
    bool init() {
        if (initSerialPort() == false) {
            qDebug() << "Communication init(): initSerialPort(): error";
            return false;
        }
        if (initUdp() == false) {
            qDebug() << "Communication init(): initUdp(): error";
            return false;
        }
        qDebug() << "Communication init(): success";
        return true;
    }
    // 串口通信初始化
    bool initSerialPort() {
        _serialPort = new QSerialPort;
        // 设置 波特率 数据位 校验位 停止位 串口名
        _serialPort->setBaudRate(QSerialPort::Baud9600);
        _serialPort->setDataBits(QSerialPort::Data8);
        _serialPort->setParity(QSerialPort::NoParity);
        _serialPort->setStopBits(QSerialPort::OneStop);
        _serialPort->setPortName("COM1");
        if (_serialPort->open(QSerialPort::ReadWrite) == false) {
            qDebug() << "initSerialPort(): open error";
            qDebug() << _serialPort->errorString();
            return false;
        }
        // 超时定时器
        _timeOut = new QTimer;
        _timeOut->setInterval(500);
        _timeOut->setSingleShot(true);
        // 关联信号
        connect(_serialPort, &QSerialPort::readyRead, this, &Communication::readFromSerialPort);
        connect(_timeOut, &QTimer::timeout, this, &Communication::timeOut);
        return true;
    }
    // UDP通信初始化
    bool initUdp() {
        _udpSocket = new QUdpSocket;
        if (_udpSocket->bind(QHostAddress::Any, _port) == false) {
            qDebug() << "initUdp: bind error";
            qDebug() << _udpSocket->errorString();
            return false;
        }
        // 关联信号
        connect(_udpSocket, &QUdpSocket::readyRead, this, &Communication::readFromUdp);
        return true;
    }

// -----------------------------------读取&写入接口模块-------------------------------------- //
    // 获取接收到的数据
    QByteArray read();
    // 发送数据
    bool write(const QByteArray& data, uint8_t type);

// ------------------------------------数据读取处理模块-------------------------------------- //
private slots:
    // 读取串口数据
    void readFromSerialPort() {;}
    // 读取UDP数据
    void readFromUdp() {;}

// ------------------------------------数据发送处理模块-------------------------------------- //
private:
    // UDP数据发送
    void sendByUdp(QByteArray data);
    // 串口数据发送(主机->从机)
    void sendBySerialToSlave(QByteArray data);
    // 串口数据发送(从机->主机)
    void sendBySerialToHost(QByteArray data);

// -------------------------------------信号定义模块---------------------------------------- //
signals:
    // 可读信号
    void readFrame();
    // 超时信号
    void timeOut();

// -------------------------------------通信成员变量---------------------------------------- //
private:
    // 串口通信成员变量
    QSerialPort* _serialPort = nullptr; // QT串口对象
    QTimer* _timeOut = nullptr;         // 超时计时器

    // UDP通信成员变量
    QUdpSocket *_udpSocket = nullptr;   // UDP套接字对象
    uint16_t _port = 0x0000;            // 绑定端口号

    // 接收缓冲区成员变量
    std::queue<QByteArray> _recvQueue;  // 接收缓冲区
    std::queue<QByteArray> _sendQueue;  // 发送缓冲区
    std::queue<uint8_t> _typeQueue;     // 发送方式队列
};

#endif //ANALOG_SENSOR_COMMUNICATION_HPP