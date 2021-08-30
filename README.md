# Simple Linear Regression in Arduino
- Download the Zip file
- Add to library

## Description

- It is simple **Linear Regression** using least square mean method.
- It uses Queue to `push` and `pop` data 
- Uses 100 data points

## Prerequisite
- Basic Knowledge On ML
- [Queue Library](https://github.com/SMFSW/Queue)

## Functions 
```c++
ReadValue(*analogRead, int pin, int pin2);
```
- This funtion will take **analog** Readings of the sensor connected to the pins. 
-`100` data points will be read for calculating from the two pins
- After hundread it will automatically udpate the values over time

```c++
ReadValue(*analogRead, int pin);
```
- Same as above but with single **analog** Reading.
- The second parameter will be `millis()`

```c++
calculate();
```
- This is used to caluclate the **Co-efficients**

```c++
predict(int value);
```
- This predicts the value of the required `unknown`  parameter by taking the `known` parameter
```c++
predict();
```
- This predicts the value of the required `unknown`  parameter by using `millis()`

```c++
clean();
```
- This will destroy the queue for freeing up space
