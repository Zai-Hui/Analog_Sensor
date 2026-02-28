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
    void start() {
        connect(communication, &Communication::readyFrame, this, [this]() {
            uint8_t type;
            std::pair<QHostAddress, uint16_t> addr;
            QByteArray data = communication->read(&type, &addr);
            qDebug() << "type:" << type;
            qDebug() << "addr:" << addr.first << ":" << addr.second;
            qDebug() << "data:" << data.toHex();
        });
    }
private:
    Communication* communication;
};



#endif //ANALOG_SENSOR_TEST__COMMUNICATION_HPP