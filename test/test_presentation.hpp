//
// Created by Zai_Hui on 2026/3/16.
//

#ifndef ANALOG_SENSOR_TEST_PRESENTATION_HPP
#define ANALOG_SENSOR_TEST_PRESENTATION_HPP

#include "../include/presentation.hpp"

class test_presentation : public QObject{
    Q_OBJECT
public:
    test_presentation() {
        _presentation = new Presentation();
    }
    void test_encode() {
        connect(_presentation, &Presentation::readySend, this, [this]()->void {
            uint8_t type;
            QByteArray data = _presentation->getEncode(type);
            qDebug() << QString(data);
            qDebug() << "Send type" << QString::number(type);
        });
        QVector<QStringList> dataCsv({{"id", "name", "age"}, {"da", "333", "az"}});
        _presentation->encodeByCsv(dataCsv, (uint8_t) 1);
        std::vector<QStringList> dataJson({{"1", "3", "4"}, {"da", "333", "az"}});
        _presentation->encodeByJson(dataJson, (uint8_t) 1);
    }
    ~test_presentation() override {
        if (_presentation)
            delete _presentation;
    }
    Presentation* _presentation;
};

#endif //ANALOG_SENSOR_TEST_PRESENTATION_HPP