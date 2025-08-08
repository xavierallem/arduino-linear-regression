#include "cppQueue.h"
#include "Arduino.h"
#include "ML.h"
#include <EEPROM.h> // Include EEPROM library

#define IMPLEMENTATION LIFO

// Global queue pointer for ML class
cppQueue *q_ptr; 

ML::ML(int queueSize) {
    q_ptr = new cppQueue(sizeof(ML::Data), queueSize, IMPLEMENTATION);
    slope = 0.0;
    intercept = 0.0;
    _isValidCalculation = false;
}

ML::~ML() {
    clean();
    delete q_ptr;
}

void ML::calculate()
{
    int size = q_ptr->sizeOf();
    _isValidCalculation = false; // Assume invalid until proven otherwise

    if (size < 2) { // Need at least 2 points for linear regression
        slope = 0.0;
        intercept = 0.0;
        return;
    }

    float sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;

    for (int i = 0; i < size; i++)
    {
        Data data;
        q_ptr->peekIdx(&data, i);
        sumX += data.X;
        sumX2 += (float)data.X * data.X;
        sumY += data.Y;
        sumXY += (float)data.X * data.Y;
    }

    float denominator = (size * sumX2 - sumX * sumX);
    if (denominator == 0) { // Avoid division by zero, e.g., all X values are the same
        slope = 0.0;
        intercept = sumY / size; // If X is constant, Y is just the average of Y values
        _isValidCalculation = true; // Valid calculation for constant X
        return;
    }

    slope = (size * sumXY - sumX * sumY) / denominator;
    intercept = (sumY - slope * sumX) / size;
    _isValidCalculation = true;
}

int ML::predict(int value)
{
    return (int)(value * slope + intercept);
}

int ML::predict()
{
    long int value = millis();
    return (int)(value * slope + intercept);
}

void ML::clean()
{
    q_ptr->clean();
}

void ML::ReadValue(int (*func)(int param), int pin, int pin2)
{
    ML::Data data;
    data.X = func(pin);
    data.Y = func(pin2);

    if (q_ptr->isFull()) {
        q_ptr->pop(&data); // Pop oldest data if queue is full
    }
    q_ptr->push(&data);
}

void ML::ReadValue(int (*func)(int param), int pin)
{
    ML::Data data;
    data.X = func(pin);
    data.Y = millis(); // Use millis() for the second parameter

    if (q_ptr->isFull()) {
        q_ptr->pop(&data); // Pop oldest data if queue is full
    }
    q_ptr->push(&data);
}

float ML::getSlope() {
    return slope;
}

float ML::getIntercept() {
    return intercept;
}

bool ML::isValidCalculation() {
    return _isValidCalculation;
}

void ML::saveCoefficients(int address) {
    EEPROM.put(address, slope);
    EEPROM.put(address + sizeof(float), intercept);
    EEPROM.put(address + 2 * sizeof(float), _isValidCalculation); // Save calculation status
}

void ML::loadCoefficients(int address) {
    EEPROM.get(address, slope);
    EEPROM.get(address + sizeof(float), intercept);
    EEPROM.get(address + 2 * sizeof(float), _isValidCalculation); // Load calculation status
}
