//
// Created by Zai_Hui on 2026/3/6.
//

#ifndef ANALOG_SENSOR_PRESENTATION_HPP
#define ANALOG_SENSOR_PRESENTATION_HPP
#include <QObject>
#include <queue>
#include <QDebug>

class Presentation : public QObject {
    Q_OBJECT
// -------------------------------------初始化模块----------------------------------------- //
public:
    void init() {

    }
// ------------------------------------用户接口模块---------------------------------------- //
public:
    // 解码操作入口 判断数据格式类型 0x01:csv 0x02:json
    void decode(const QByteArray& data) {
        if (data.size() <= 4) {
            qDebug() << "presentation:decode: data size is too small";
            return;
        }
        else if (static_cast<uint8_t>(data[0]) == 0xFF && data[1] == 0x01) {
            decodeByCsv(data.mid(2, data.size() - 2));
        }
        else if (static_cast<uint8_t>(data[0]) == 0xFF && data[1] == 0x02) {
            decodeByJson(data.mid(2, data.size() - 2));
        }
        else {
            decodeByModbus(data);
        }
    }
    // 编码操作入口 //0x01:modbus 0x02:csv 0x03:json
    void encode(const QByteArray& data, const uint8_t type) {
        if (type == 0x01) {
            encodeByModbus(data);
        }
        else if (type == 0x02) {
            encodeByCsv(data);
        }
        else if (type == 0x03) {
            encodeByJson(data);
        }
        else {
            qDebug() << "presentation:encode: type error";
        }
    }
    // 获取解码后数据
    QByteArray getDecode(uint8_t& type) {
        if (_dataDecode.empty() == false) {
            std::pair<QByteArray, uint8_t> data = _dataDecode.front();
            _dataDecode.pop();
            type = data.second;
            return data.first;
        }
        qDebug() << "presentation:getDecode: no data need to get";
        return QByteArray();
    }
    // 获取编码后数据
    QByteArray getEncode(uint8_t& type, uint8_t& sendType) {
        if (_dataEncode.empty() == false) {
            std::pair<QByteArray, uint8_t> data = _dataEncode.front();
            sendType = _dataSendType.front();
            _dataEncode.pop();
            _dataSendType.pop();
            type = data.second;
            return data.first;
        }
        qDebug() << "presentation:getEncode: no data need to get";
        return QByteArray();
    }
// --------------------------------------解码模块----------------------------------------- //
private:
    // 处理modbus格式的数据
    void decodeByModbus(const QByteArray& data) {

    }
    // 处理csv格式的数据
    void decodeByCsv(const QByteArray& data) {

    }
    // 处理json格式的数据
    void decodeByJson(const QByteArray& data) {

    }
// --------------------------------------译码模块----------------------------------------- //
private:
    // 译码为modbus格式
    void encodeByModbus(const QByteArray& data) {
    }
    // 译码为csv格式
    void encodeByCsv(const QByteArray& data) {

    }
    // 译码为json格式
    void encodeByJson(const QByteArray& data) {

    }
// --------------------------------------信号定义----------------------------------------- //
signals:
    // 编码完成待发送信号
    void readySend();
    // 解码完成等待处理信号
    void readyProcess();
// -----------------------------------解码译码成员变量-------------------------------------- //
public:
    // 解码缓冲区
    std::queue<std::pair<QByteArray, uint8_t>> _dataDecode;
    // 译码缓冲区
    std::queue<std::pair<QByteArray, uint8_t>> _dataEncode;
    std::queue<uint8_t> _dataSendType;
    // 数据处理回调方法
    using func = std::function<void()>;
    func* processByModbus = nullptr;
    func* processByCsv = nullptr;
    func* processByJson = nullptr;
};

#endif //ANALOG_SENSOR_PRESENTATION_HPP