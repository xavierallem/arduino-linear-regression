# Project Overview

This is an Arduino library for performing simple linear regression using the least squares method. It's designed to collect data points (up to 100) from analog sensors or time-based readings, calculate the regression coefficients, and then predict unknown values. It utilizes a Queue library (`cppQueue`) for managing data points.

## Building and Running

This is an Arduino library. To use it:

1.  Download the library (typically as a ZIP file).
2.  Add it to the Arduino IDE's library manager (Sketch > Include Library > Add .ZIP Library...).
3.  Include the `Arduino_LinearRegression.h` header in your Arduino sketch:
    ```cpp
    #include <Arduino_LinearRegression.h>
    ```
4.  Compile and upload your sketch to an Arduino board using the Arduino IDE or Arduino CLI.

## Development Conventions

*   The library is written in C++ for the Arduino platform.
*   It includes standard Arduino headers (`Arduino.h`) and relies on the `cppQueue` library for data management.
*   Data points are stored in a `cppQueue` with a fixed capacity of 100.
*   The `ML` class encapsulates the linear regression logic.
*   The `calculate()` function computes the slope (`X`) and y-intercept (`Y`) of the regression line using the collected data.
*   `predict()` functions are provided to estimate values based on the calculated regression model.
*   `ReadValue()` functions handle data acquisition, supporting either two analog sensor inputs or one analog sensor input paired with `millis()` for the second variable.
*   A `clean()` function is available to clear the data queue and free up space.
