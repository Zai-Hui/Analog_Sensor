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
        _dataModbus.clear();
        int index = 0;
        _host = data[index++];
        _funcCode = data[index++];
        _addr = static_cast<unsigned char>(data[index++]);
        _addr = (_addr << 8) | data[index++];
        _size_oneData = static_cast<unsigned char>(data[index++]);
        _size_oneData = (_size_oneData << 8) | data[index++];
        if (_funcCode == 0x0F || _funcCode == 0x10) {
            _len = data[index++];
            for (int i = 0; i < _len; i++) {
                _dataModbus.push_back(data[index++]);
            }
        }
    }
    // 处理csv格式的数据
    void decodeByCsv(const QByteArray& BData) {
        QString data(BData);
        // 处理 \r\n 的情况
        QStringList stringList = data.split("\n");
        while (stringList.back().isEmpty()) {
            stringList.pop_back();
        }
        for (auto & e : stringList) {
            if (e.back() == "\r") e.chop(1);
        }
        // 获取表格列名
        QStringList columnsName = getLine(stringList[0]);
        // 建立列名和列数的映射
        for (int i = 0; i < columnsName.size(); i++) {
            if (columnsName[i] != "") { _hash.insert(columnsName[i], i); }
        }
        // 将数据存到_dataCsv中
        for (int i = 1; i < stringList.size(); i++) {
            QStringList lineData = getLine(stringList[i]);
            _dataCsv.append(lineData);
        }
    }
    // 处理一行数据
    QStringList getLine(const QString& str) {
        int index = 0;
        QStringList stringList;
        while (index < str.size() && str[index] != '\n') {
            stringList.append("");
            int beginIndex = index;
            while (index < str.size() && str[index] != ',' && str[index] != '\n') {
                if (str[index] == '\"') {
                    index++;
                    while (str[index] != '\"')
                        index++;
                }
                index++;
            }
            if (index > 0 && str[index - 1] == '\"') {
                stringList.back() += str.mid(beginIndex + 1, index - beginIndex - 2);
            }
            else if (index == 0 || str[index - 1] == ',') {
                index++;
                continue;
            }
            else {
                stringList.back() += str.mid(beginIndex, index - beginIndex);
            }
            index++;
        }
        return stringList;
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
    // modbus协议成员
    uint8_t _host = 0x00;               // 从机地址
    uint8_t _funcCode = 0x00;           // 功能码
    uint16_t _addr = 0x0000;            // 数据地址
    uint16_t _size_oneData = 0x0000;    // 目标数量/单个数据
    uint8_t _len = 0x00;                // 字节计数
    std::vector<uint8_t> _dataModbus;   // 字节数据
    // csv格式成员
    QMap<QString, int> _hash;
    QVector<QStringList> _dataCsv;
};

#endif //ANALOG_SENSOR_PRESENTATION_HPP