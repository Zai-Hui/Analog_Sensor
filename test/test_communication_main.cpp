//
// Created by Zai_Hui on 2026/2/27.
//

#include"test_communication.hpp"
#include <QApplication>
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    Test test;
    test.start_recv();
    test.start_send();
    return QCoreApplication::exec();
}