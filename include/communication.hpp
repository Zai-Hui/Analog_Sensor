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
            qDebug() << "initUdp(): bind error";
            qDebug() << _udpSocket->errorString();
            return false;
        }
        // 关联信号
        connect(_udpSocket, &QUdpSocket::readyRead, this, &Communication::readFromUdp);
        return true;
    }

// -----------------------------------读取&写入接口模块-------------------------------------- //
public:
    // 获取接收到的数据
    QByteArray read(uint8_t* type, std::pair<QHostAddress, uint16_t>* addr = nullptr) {
        QByteArray data;
        if (_recvQueue.empty() == true) {
            qDebug() << "communication::read(): queue is empty";
            return data;
        }
        data.append(_recvQueue.front());
        *type = _recvTypeQueue.front();
        _recvQueue.pop();
        _recvTypeQueue.pop();
        if (*type == 0x03) {
            if (addr == nullptr) {
                qDebug() << "communication::read(): addr is null";
                return data;
            }
            _udpRecvQueue.push(*addr);
        }
        return data;
    }
    // 发送数据
    bool write(const QByteArray& data, uint8_t type, std::pair<QHostAddress, uint16_t>* addr = nullptr) {
        if (type == 0x01) {
            _sendQueue.push(data);
            _sendTypeQueue.push(type);
        }
        else if (type == 0x02) {
            _sendQueue.push(data);
            _sendTypeQueue.push(type);
        }
        else if (type == 0x03) {
            _sendQueue.push(data);
            _sendTypeQueue.push(type);
            _udpSendQueue.push(*addr);
        }
        else {
            qDebug() << "type: " << type << ":" << data.toHex() << ":error";
            return false;
        }
        send();
    }

// ------------------------------------数据读取处理模块-------------------------------------- //
private slots:
    // 读取串口数据
    void readFromSerialPort() {
        _timeOut->stop();
        QByteArray data = _serialPort->readAll();
        qDebug() << "----------------------------------";
        qDebug() << "COM1-接收: " << data.toHex();
        if (crc16Checksum(data) == true) {
            _recvQueue.push(data.mid(0, data.size() - 2));
            _recvTypeQueue.push(0x01);
            emit readyFrame();
        }
    }
    // 读取UDP数据
    void readFromUdp() {
        int size = static_cast<int>(_udpSocket->pendingDatagramSize());
        QByteArray data;
        data.resize(size);
        std::pair<QHostAddress, uint16_t> addr;
        _udpSocket->readDatagram(data.data(), size, &addr.first, &addr.second);
        qDebug() << "----------------------------------";
        qDebug() << "Udp-接收: " << data.toHex();
        qDebug() << "from: " << addr.first.toString() << ":" << addr.second;
        _udpRecvQueue.push(addr);
        _recvQueue.push(data);
        _recvTypeQueue.push(0x02);
        emit readyFrame();
    }
public:
    // crc16 校验
    static bool crc16Checksum(const QByteArray& data) {
        uint16_t crc16 = static_cast<unsigned char>(data[data.size() - 1]);
        crc16 = (crc16 << 8) | static_cast<unsigned char>(data[data.size() - 2]);
        if (crc16 == crc16Calculate(data.mid(0, data.size() - 2))) {
            qDebug() << "crc16 校验通过";
            return true;
        }
        else {
            qDebug() << "crc16 校验失败";
            return false;
        }
    }
    // crc16 校验码计算
    static uint16_t crc16Calculate(const QByteArray& crcData) {
        uint16_t crc = 0xFFFF;
        for (int i = 0; i < crcData.length(); i++) {
            crc = crc ^ (crcData[i] & 0x00FF);
            for (int num = 0; num < 8; num++) {
                bool flag = crc & 0x0001;
                crc = crc >> 1;
                if (flag) crc = crc ^ 0xA001;
            }
        }
        return crc;
    }

// ------------------------------------数据发送处理模块-------------------------------------- //
private:
    // 主发送函数
    void send() {
        while (_isSending == false && _sendQueue.empty() == false) {
            _isSending = true;
            uint8_t type = _sendTypeQueue.front();
            QByteArray data = _sendQueue.front();
            _sendTypeQueue.pop();
            _sendQueue.pop();
            if (type == 0x01) {
                sendBySerialToHost(data);
                _isSending = false;
            }
            else if (type == 0x02) {
                sendBySerialToSlave(data);
            }
            else if (type == 0x03) {
                std::pair<QHostAddress, uint16_t> addr = _udpSendQueue.front();
                _udpSendQueue.pop();
                sendByUdp(data, addr);
                _isSending = false;
            }
            else {
                qDebug() << "communication: send(): send type error: " << type;
            }
        }
    }
    // UDP数据发送
    void sendByUdp(const QByteArray& data, const std::pair<QHostAddress, uint16_t>& addr) {
        _udpSocket->writeDatagram(data, addr.first, addr.second);
        qDebug() << "----------------------------------";
        qDebug() << "Udp-发送: " << data.toHex();
        qDebug() << "to: " << addr.first.toString() << ":" << addr.second;
    };
    // 串口数据发送
    void sendBySerialPort(QByteArray data) {
        uint16_t crc = crc16Calculate(data);
        data.append(static_cast<char>(crc >> 8));
        data.append(static_cast<char>(crc & 0xFF));
        _serialPort->write(data);
        qDebug() << "----------------------------------";
        qDebug() << "COM1-发送: " << data.toHex();
    }
    // 串口数据发送(主机->从机)
    void sendBySerialToSlave(const QByteArray& data) {
        sendBySerialPort(data);
        _timeOut->start();
    }
    // 串口数据发送(从机->主机)
    void sendBySerialToHost(const QByteArray& data) {
        sendBySerialPort(data);
    }
private slots:
    // 串口数据发送(主机->从机)超时处理
    void sendBySerialTimeout() {
        qDebug() << "---------------------------------";
        qDebug() << "从机响应超时";
        _isSending = false;
    }

// -------------------------------------信号定义模块---------------------------------------- //
signals:
    // 可读信号
    void readyFrame();
    // 超时信号
    void timeOut();

// -------------------------------------通信成员变量---------------------------------------- //
private:
    bool _isSending = false;                                        // 当前发送状态

    // 串口通信成员变量
    QSerialPort* _serialPort = nullptr;                             // QT串口对象
    QTimer* _timeOut = nullptr;                                     // 超时计时器

    // UDP通信成员变量
    QUdpSocket *_udpSocket = nullptr;                               // UDP套接字对象
    uint16_t _port = 10001;                                         // 绑定端口号
    std::queue<std::pair<QHostAddress, uint16_t>> _udpSendQueue;    // 接收地址信息
    std::queue<std::pair<QHostAddress, uint16_t>> _udpRecvQueue;    // 发送地址信息

    // 接收缓冲区成员变量
    std::queue<QByteArray> _recvQueue;                              // 接收缓冲区
    std::queue<QByteArray> _sendQueue;                              // 发送缓冲区
    std::queue<uint8_t> _recvTypeQueue;                             // 接收方式队列
    std::queue<uint8_t> _sendTypeQueue;                             // 发送方式队列
};

#endif //ANALOG_SENSOR_COMMUNICATION_HPP