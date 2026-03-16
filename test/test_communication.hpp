//
// Created by Zai_Hui on 2026/2/28.
//

#ifndef ANALOG_SENSOR_TEST__COMMUNICATION_HPP
#define ANALOG_SENSOR_TEST__COMMUNICATION_HPP

#include "../include/communication.hpp"

class Test : public QObject {
    Q_OBJECT
public:
    Test() {
        communication = new Communication;
        communication->init();
    }
    void start_recv() {
        connect(communication, &Communication::readyFrame, this, [this]() {
            uint8_t type;
            std::pair<QHostAddress, uint16_t> addr;
            QByteArray data = communication->read(&type, &addr);
            qDebug() << "[test] type:" << type;
            qDebug() << "[test] addr:" << addr.first << ":" << addr.second;
            qDebug() << "[test] data:" << data.toHex();
        });
    }
    void start_send() {
        //while (true)
        {
            //system("pause");
            QByteArray data;
            data.append(0x05);
            data.append(0x02);
            data.append(0x01);
            data.append(0x02);
            data.append(0x01);
            data.append(0x01);
            std::pair<QHostAddress, uint16_t> addr;
            addr.first = QHostAddress("127.0.0.1");
            addr.second = 10002;
            communication->write(data, 0x02);
            communication->write(data, 0x01);
            communication->write(data, 0x03, &addr);
        }
    }
private:
    Communication* communication;
};



#endif //ANALOG_SENSOR_TEST__COMMUNICATION_HPP