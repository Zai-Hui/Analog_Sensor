//
// Created by Zai_Hui on 2026/3/6.
//

#ifndef ANALOG_SENSOR_PRESENTATION_HPP
#define ANALOG_SENSOR_PRESENTATION_HPP
#include <QObject>
#include <queue>

class Presentation : public QObject {
    Q_OBJECT
// -------------------------------------初始化模块----------------------------------------- //
public:
    void init() {

    }
// ------------------------------------用户接口模块---------------------------------------- //
public:
    // 解码操作入口 判断数据格式类型
    void decode(const QByteArray& data) {

    }
    // 编码操作入口
    void encode(const QByteArray& data, const uint8_t type) {

    }
    // 获取解码后数据
    QByteArray getDecode() {

    }
    // 获取编码后数据
    QByteArray getEncode() {

    }
// --------------------------------------解码模块----------------------------------------- //
private:
    // 处理modbus格式的数据
    void decodeByModbus() {

    }
    // 处理csv格式的数据
    void decodeByCsv() {

    }
    // 处理json格式的数据
    void decodeByJson() {

    }
// --------------------------------------译码模块----------------------------------------- //
private:
    // 译码为modbus格式
    void encodeByModbus() {
    }
    // 译码为csv格式
    void encodeByCsv() {

    }
    // 译码为json格式
    void encodeByJson() {

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