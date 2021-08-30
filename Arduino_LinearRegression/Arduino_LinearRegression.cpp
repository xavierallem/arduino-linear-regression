#include "cppQueue.h"
#include "Arduino.h"
#include "Arduino_LinearRegression.h"

#define IMPLEMENTATION LIFO

cppQueue q(sizeof(ML::Data), 100, IMPLEMENTATION); // Instantiate queue


void ML::calculate()
{
    int i, size = q.sizeOf();
    float sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0, a, b;

    for (i = 0; i < size; i++)
    {
        Data data;
        q.peekIdx(&data, i);
        sumX = sumX + data.X;
        sumX2 = sumX2 + data.X * data.X;
        sumY = sumY + data.Y;
        sumXY = sumXY + data.X * data.Y;
    }

    X = (size * sumXY - sumX * sumY) / (size * sumX2 - sumX * sumX);
    Y = (sumY - b * sumX) / size;
    
    

}
static int ML::predict(int value)
{
return value*X+ Y;
}
static int ML::predict()
{
    long int value=(long)millis();
return value*X+ Y;
}

void ML::clean()
{
    q.clean();
}

void ML::ReadValue(int (*func)(int param), int pin, int pin2)
{
   ML::Data data;
   if(q.isFull)
        q.pop(&data);
    
    while (!q.isFull())
    {


        i = func(pin);
        j = func(pin2);
        delay(200);

        data.X = func(pin);
        data.Y = func(pin2);
        q.push(&data);
        delay(500);
        
    }
    
}
void ML::ReadValue(int (*func)(int param), int pin)
{

   ML::Data data;
   if(q.isFull)
        q.pop(&data);
    
    while (!q.isFull())
    {


        i = func(pin);
        j = (long)millis();
        delay(200);

        data.X = func(pin);
        data.Y = func(pin2);
        q.push(&data);
        delay(500);
        
    }
    
}
ML::~ML(){
    X=0;
    Y=0;
}
