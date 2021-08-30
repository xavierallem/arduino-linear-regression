#ifndef Arduino_LinearRegression_h
#define Arduino_LinearRegression_h

#include "Arduino.h"

class ML
{
public:
  ML();
  ~ML();
  void calculate();
  void ReadValue(int (*func)(int param), int pin, int pin2);
  void ReadValue(int (*func)(int param), int pin);
  void clean();
  static int predict(int value);
  static int predict();
  typedef struct Data_points
  {
    long int X;
    long int Y;
  } Data;

private:
  int X;
  int Y;
};

#endif
