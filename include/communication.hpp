//
// Created by Zai_Hui on 2026/2/27.
//

#ifndef ANALOG_SENSOR_COMMUNICATION_HPP
#define ANALOG_SENSOR_COMMUNICATION_HPP

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QUdpSocket>
#include <queue>

class Communication : public QObject {
    Q_OBJECT
// -------------------------------------初始化模块----------------------------------------- //
public:
    // 总初始化
    bool init();
    // 串口通信初始化
    bool initSerialPort();
    // UDP通信初始化
    bool initUdp();

// -----------------------------------读取&写入接口模块-------------------------------------- //
    // 获取接收到的数据
    QByteArray read();
    // 发送数据
    bool write(const QByteArray& data, uint8_t type);

// ------------------------------------数据读取处理模块-------------------------------------- //
private slots:
    // 读取串口数据
    void readFromSerialPort();
    // 读取UDP数据
    void readFromUdp();

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

// -------------------------------------通信成员变量---------------------------------------- //
private:
    // 串口通信成员变量
    QSerialPort* _serialPort = nullptr; // QT串口对象

    // UDP通信成员变量
    QUdpSocket *_udpSocket = nullptr;   // UDP套接字对象

    // 接收缓冲区成员变量
    std::queue<QByteArray> _recvQueue;  // 接收缓冲区
    std::queue<QByteArray> _sendQueue;  // 发送缓冲区
    std::queue<uint8_t> _typeQueue;     // 发送方式队列
};

#endif //ANALOG_SENSOR_COMMUNICATION_HPP