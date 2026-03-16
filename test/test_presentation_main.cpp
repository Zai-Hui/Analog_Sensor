//
// Created by Zai_Hui on 2026/3/16.
//

#include "test_presentation.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    test_presentation test;
    test.test_encode();
    return app.exec();
}