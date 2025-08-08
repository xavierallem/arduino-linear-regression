#ifndef ML_h
#define ML_h

#include "Arduino.h"
#include "cppQueue.h"

class ML
{
public:
  ML(int queueSize = 100); // Default to 100 if not specified
  ~ML();
  void calculate();
  void ReadValue(int (*func)(int param), int pin, int pin2);
  void ReadValue(int (*func)(int param), int pin);
  void clean();
  int predict(int value);
  int predict();
  float getSlope();
  float getIntercept();
  bool isValidCalculation();
  void saveCoefficients(int address);
  void loadCoefficients(int address);

  typedef struct Data_points
  {
    long int X;
    long int Y;
  } Data;

private:
  float slope;
  float intercept;
  bool _isValidCalculation;
};

#endif
