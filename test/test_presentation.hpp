//
// Created by Zai_Hui on 2026/3/16.
//

#ifndef ANALOG_SENSOR_TEST_PRESENTATION_HPP
#define ANALOG_SENSOR_TEST_PRESENTATION_HPP

#include "../include/presentation.hpp"

class test_presentation {
public:
    test_presentation() {
        _presentation = new Presentation();
    }
    ~test_presentation() {
        if (_presentation)
            delete _presentation;
    }
    Presentation* _presentation;
};

#endif //ANALOG_SENSOR_TEST_PRESENTATION_HPP