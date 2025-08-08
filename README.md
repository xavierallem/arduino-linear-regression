# ArduinoMLib: Machine Learning for Arduino

## Description

This library provides implementations of simple **Linear Regression**, **K-Nearest Neighbors (KNN)**, **Naive Bayes Classifier**, **Perceptron**, and **Logistic Regression** algorithms for Arduino. All are designed to be computationally lightweight, suitable for microcontroller environments.

*   **Linear Regression:** Implements the least squares method to collect data points, calculate regression coefficients (slope and intercept), and predict unknown values. It uses a configurable-size queue to manage data points. It also supports saving and loading the calculated coefficients to/from EEPROM for persistent storage.
*   **K-Nearest Neighbors (KNN):** A simple classification algorithm that classifies new data points based on the majority class among their `k` nearest neighbors in the training data. It also uses a configurable-size queue to store training samples. Due to memory constraints on typical Arduino boards, persisting the entire KNN training dataset to EEPROM is generally not feasible. For larger datasets, consider using an SD card module.
*   **Naive Bayes Classifier:** A probabilistic classifier suitable for discrete features. It's efficient in terms of computation and memory, making it a good choice for Arduino. It learns probabilities from training data to predict the class of new samples.
*   **Perceptron:** A fundamental binary classification algorithm, representing the simplest form of a neural network. It learns a linear decision boundary and is very efficient for inference.
*   **Logistic Regression:** A statistical model used for binary classification. It outputs a probability that a given input belongs to a certain class, which can then be thresholded to make a classification decision. It's computationally efficient and suitable for Arduino.

## Prerequisite

*   Basic Knowledge On ML
*   [Queue Library (cppQueue)](https://github.com/SMFSW/Queue)

## Installation

1.  Download the library as a ZIP file.
2.  In the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...` and select the downloaded ZIP file.

## Usage

Include the main library header in your Arduino sketch:

```cpp
#include <ArduinoMLib.h>
```

This will include the `ML` class (for Linear Regression), the `KNNClassifier` class, the `NaiveBayesClassifier` class, the `Perceptron` class, and the `LogisticRegression` class, whose source files are located in the `src` subdirectory of this library.

---

### Linear Regression (ML Class)

#### Constructor

You can specify the maximum number of data points to store. If not specified, it defaults to 100.

```cpp
ML regressor(50); // Creates a regressor that stores up to 50 data points
ML defaultRegressor(); // Creates a regressor that stores up to 100 data points
```

#### Functions

*   **`void ReadValue(int (*func)(int param), int pin, int pin2)`**
    *   Reads analog values from two pins and adds them as a data point (X, Y) to the internal queue.
    *   It is non-blocking; call it periodically in your `loop()` function to collect data.
    *   If the queue is full, the oldest data point is automatically removed before adding the new one.

*   **`void ReadValue(int (*func)(int param), int pin)`**
    *   Reads an analog value from one pin (X) and uses `millis()` as the second parameter (Y).
    *   It is non-blocking; call it periodically in your `loop()` function to collect data.
    *   If the queue is full, the oldest data point is automatically removed before adding the new one.

*   **`void calculate()`**
    *   Calculates the **slope** and **intercept** of the linear regression line based on the data points currently in the queue.
    *   It sets an internal flag indicating whether the calculation was successful (e.g., requires at least 2 data points, avoids division by zero).

*   **`int predict(int value)`**
    *   Predicts the Y value for a given X `value` using the calculated slope and intercept.

*   **`int predict()`**
    *   Predicts the Y value using `millis()` as the X value and the calculated slope and intercept.

*   **`float getSlope()`**
    *   Returns the calculated slope of the regression line.

*   **`float getIntercept()`**
    *   Returns the calculated intercept of the regression line.

*   **`bool isValidCalculation()`**
    *   Returns `true` if the last call to `calculate()` resulted in a valid slope and intercept (i.e., enough data points and no division by zero).

*   **`void clean()`**
    *   Clears all data points from the internal queue, freeing up space.

*   **`void saveCoefficients(int address)`**
    *   Saves the calculated `slope`, `intercept`, and `_isValidCalculation` status to EEPROM starting at the specified `address`.
    *   Requires enough EEPROM space (e.g., `sizeof(float) * 2 + sizeof(bool)` bytes).

*   **`void loadCoefficients(int address)`**
    *   Loads the `slope`, `intercept`, and `_isValidCalculation` status from EEPROM starting at the specified `address`.

#### Example Sketch (Conceptual for Linear Regression)

```cpp
#include <ArduinoMLib.h>
#include <EEPROM.h>

ML regressor(50); // Create an ML object with a queue size of 50
const int EEPROM_ADDRESS = 0; // Starting EEPROM address for coefficients

void setup() {
  Serial.begin(9600);

  // Try to load previously saved coefficients
  regressor.loadCoefficients(EEPROM_ADDRESS);
  if (regressor.isValidCalculation()) {
    Serial.println("Loaded coefficients from EEPROM.");
    Serial.print("Slope: ");
    Serial.print(regressor.getSlope());
    Serial.print(", Intercept: ");
    Serial.println(regressor.getIntercept());
  } else {
    Serial.println("No valid coefficients found in EEPROM. Will train new model.");
  }
}

void loop() {
  // Example 1: Reading from two analog pins
  regressor.ReadValue(analogRead, A0, A1); 

  // Example 2: Reading from one analog pin and using time (millis())
  // regressor.ReadValue(analogRead, A0);

  // Perform calculation periodically, e.g., every 1000ms
  static unsigned long lastCalcTime = 0;
  if (millis() - lastCalcTime > 1000) {
    regressor.calculate();
    if (regressor.isValidCalculation()) {
      Serial.print("Slope: ");
      Serial.print(regressor.getSlope());
      Serial.print(", Intercept: ");
      Serial.println(regressor.getIntercept());

      // Save coefficients to EEPROM after a successful calculation
      regressor.saveCoefficients(EEPROM_ADDRESS);
      Serial.println("Coefficients saved to EEPROM.");

    } else {
      Serial.println("Not enough data or invalid calculation.");
    }
    lastCalcTime = millis();
  }

  delay(100); // Small delay to avoid reading too fast
}
```

---

### K-Nearest Neighbors (KNNClassifier Class)

#### Constructor

Initializes the KNN classifier.

*   `numFeatures`: The number of features (dimensions) for each data point.
*   `k`: The number of nearest neighbors to consider for classification.
*   `maxDataPoints`: The maximum number of training data points to store.

```cpp
KNNClassifier classifier(2, 3, 10); // 2 features, k=3, max 10 training points
```

#### Functions

*   **`bool addTrainingData(int* features, int label)`**
    *   Adds a new training data point to the classifier.
    *   `features`: An array of integers representing the features of the data point.
    *   `label`: The integer class label for this data point.
    *   Returns `true` on success, `false` if memory allocation fails.
    *   If the queue is full, the oldest data point is removed to make space for the new one.

*   **`int predict(int* features)`**
    *   Predicts the class label for a new, unseen data point.
    *   `features`: An array of integers representing the features of the data point to classify.
    *   Returns the predicted class label (integer) or -1 if no training data is available or memory allocation fails.

*   **`void clean()`**
    *   Clears all training data from the classifier, freeing up associated memory.

#### Example Sketch (Conceptual for KNN)

```cpp
#include <ArduinoMLib.h>

// Assuming 2 features (e.g., sensor readings) and 3 classes (0, 1, 2)
KNNClassifier myKNN(2, 3, 10); // 2 features, k=3, max 10 training points

void setup() {
  Serial.begin(9600);

  // Add some training data (features, label)
  int features1[] = {100, 200}; // Class 0
  myKNN.addTrainingData(features1, 0);

  int features2[] = {110, 210}; // Class 0
  myKNN.addTrainingData(features2, 0);

  int features3[] = {500, 600}; // Class 1
  myKNN.addTrainingData(features3, 1);

  int features4[] = {510, 620}; // Class 1
  myKNN.addTrainingData(features4, 1);

  int features5[] = {900, 950}; // Class 2
  myKNN.addTrainingData(features5, 2);

  int features6[] = {920, 930}; // Class 2
  myKNN.addTrainingData(features6, 2);
}

void loop() {
  // Example: Predict for a new data point
  int newFeatures[] = {105, 205};
  int predictedClass = myKNN.predict(newFeatures);

  Serial.print("New data point {105, 205} predicted as class: ");
  Serial.println(predictedClass);

  int newFeatures2[] = {505, 605};
  predictedClass = myKNN.predict(newFeatures2);
  Serial.print("New data point {505, 605} predicted as class: ");
  Serial.println(predictedClass);

  delay(5000); // Predict every 5 seconds
}
```

---

### Naive Bayes Classifier

#### Constructor

Initializes the Naive Bayes classifier.

*   `numFeatures`: The number of features (dimensions) for each data point.
*   `numClasses`: The number of possible class labels.
*   `featureMaxValues`: An array where `featureMaxValues[i]` specifies the maximum possible integer value for feature `i`. This is crucial for allocating internal memory for discrete feature counts.

```cpp
// Example: 2 features, 3 classes. Feature 0 max value is 100, Feature 1 max value is 200.
int nbFeatureMaxValues[] = {100, 200};
NaiveBayesClassifier myNB(2, 3, maxFeatureVals);
```

#### Functions

*   **`void train(int* features, int label)`**
    *   Trains the model with a single data point.
    *   `features`: An array of integers representing the feature values of the data point.
    *   `label`: The integer class label for this data point.

*   **`int predict(int* features)`**
    *   Predicts the class label for a new, unseen data point.
    *   `features`: An array of integers representing the features of the data point to classify.
    *   Returns the predicted class label (integer) or -1 if no training data is available.

*   **`void clean()`**
    *   Resets the classifier by clearing all learned data (counts and probabilities).

#### Example Sketch (Conceptual for Naive Bayes)

```cpp
#include <ArduinoMLib.h>

// Define feature max values (e.g., sensor readings from 0-1023)
// Assuming 2 features, each with a max value of 1024 (for analogRead)
// And 2 classes (0 or 1)
int nbFeatureMaxValues[] = {1024, 1024};
NaiveBayesClassifier myNB(2, 2, nbFeatureMaxValues);

void setup() {
  Serial.begin(9600);

  // Train the Naive Bayes classifier
  // Data for Class 0
  int features0_1[] = {100, 150};
  myNB.train(features0_1, 0);
  int features0_2[] = {120, 140};
  myNB.train(features0_2, 0);

  // Data for Class 1
  int features1_1[] = {800, 850};
  myNB.train(features1_1, 1);
  int features1_2[] = {820, 830};
  myNB.train(features1_2, 1);

  Serial.println("Naive Bayes training complete.");
}

void loop() {
  // Predict for new data points
  int newFeaturesNB1[] = {110, 145};
  int predictedClassNB1 = myNB.predict(newFeaturesNB1);
  Serial.print("New data point {110, 145} predicted as class: ");
  Serial.println(predictedClassNB1);

  int newFeaturesNB2[] = {810, 840};
  int predictedClassNB2 = myNB.predict(newFeaturesNB2);
  Serial.print("New data point {810, 840} predicted as class: ");
  Serial.println(predictedClassNB2);

  delay(5000); // Predict every 5 seconds
}
```

---

### Perceptron

#### Constructor

Initializes the Perceptron classifier.

*   `numFeatures`: The number of input features (dimensions) for each data point.
*   `learningRate`: (Optional) A float value controlling how much the weights are adjusted during training. Defaults to 0.1.

```cpp
Perceptron myPerceptron(2, 0.1); // 2 features, learning rate 0.1
```

#### Functions

*   **`void train(int* features, int expectedOutput)`**
    *   Trains the Perceptron with a single data point.
    *   `features`: An array of integers representing the input features.
    *   `expectedOutput`: The target binary output for this data point (0 or 1).

*   **`int predict(int* features)`**
    *   Predicts the binary output (0 or 1) for new, unseen features.
    *   `features`: An array of integers representing the input features to classify.

*   **`void clean()`**
    *   Resets all weights and the bias of the Perceptron to zero.

#### Example Sketch (Conceptual for Perceptron)

```cpp
#include <ArduinoMLib.h>

// Example: Perceptron for a simple AND gate (2 inputs, 1 output)
Perceptron andGate(2, 0.1); // 2 features, learning rate 0.1

void setup() {
  Serial.begin(9600);

  // Training data for AND gate:
  // (0,0) -> 0
  // (0,1) -> 0
  // (1,0) -> 0
  // (1,1) -> 1

  // Train for a few iterations (adjust as needed for convergence)
  for (int i = 0; i < 50; i++) {
    int features00[] = {0, 0};
    andGate.train(features00, 0);

    int features01[] = {0, 1};
    andGate.train(features01, 0);

    int features10[] = {1, 0};
    andGate.train(features10, 0);

    int features11[] = {1, 1};
    andGate.train(features11, 1);
  }

  Serial.println("Perceptron training complete.");
}

void loop() {
  // Test predictions
  int testFeatures00[] = {0, 0};
  Serial.print("0 AND 0 = ");
  Serial.println(andGate.predict(testFeatures00));

  int testFeatures01[] = {0, 1};
  Serial.print("0 AND 1 = ");
  Serial.println(andGate.predict(testFeatures01));

  int testFeatures10[] = {1, 0};
  Serial.print("1 AND 0 = ");
  Serial.println(andGate.predict(testFeatures10));

  int testFeatures11[] = {1, 1};
  Serial.print("1 AND 1 = ");
  Serial.println(andGate.predict(testFeatures11));

  delay(5000); // Test every 5 seconds
}
```

---

### Logistic Regression

#### Constructor

Initializes the Logistic Regression classifier.

*   `numFeatures`: The number of input features (dimensions) for each data point.
*   `learningRate`: (Optional) A float value controlling how much the weights are adjusted during training. Defaults to 0.1.

```cpp
LogisticRegression myLR(2, 0.01); // 2 features, learning rate 0.01
```

#### Functions

*   **`void train(int* features, int expectedOutput)`**
    *   Trains the Logistic Regression model with a single data point.
    *   `features`: An array of integers representing the input features.
    *   `expectedOutput`: The target binary output for this data point (0 or 1).

*   **`float predictProbability(int* features)`**
    *   Predicts the probability (between 0.0 and 1.0) that the input features belong to class 1.
    *   `features`: An array of integers representing the input features.

*   **`int predictClass(int* features, float threshold = 0.5)`**
    *   Predicts the binary class (0 or 1) for new, unseen features based on a given `threshold` (defaults to 0.5).
    *   `features`: An array of integers representing the input features.

*   **`void clean()`**
    *   Resets all weights and the bias of the Logistic Regression model to zero.

#### Example Sketch (Conceptual for Logistic Regression)

```cpp
#include <ArduinoMLib.h>

// Example: Logistic Regression for a simple classification task
// (e.g., classifying based on two sensor readings)
LogisticRegression myLR(2, 0.1); // 2 features, learning rate 0.1

void setup() {
  Serial.begin(9600);

  // Training data:
  // Class 0: low values for both features
  // Class 1: high values for both features

  // Train for a few iterations
  for (int i = 0; i < 100; i++) {
    int features0_1[] = {10, 20};
    myLR.train(features0_1, 0);

    int features0_2[] = {15, 25};
    myLR.train(features0_2, 0);

    int features1_1[] = {80, 90};
    myLR.train(features1_1, 1);

    int features1_2[] = {85, 95};
    myLR.train(features1_2, 1);
  }

  Serial.println("Logistic Regression training complete.");
}

void loop() {
  // Test predictions
  int testFeatures1[] = {12, 22};
  Serial.print("Features {12, 22} -> Probability: ");
  Serial.print(myLR.predictProbability(testFeatures1));
  Serial.print(", Class: ");
  Serial.println(myLR.predictClass(testFeatures1));

  int testFeatures2[] = {82, 92};
  Serial.print("Features {82, 92} -> Probability: ");
  Serial.print(myLR.predictProbability(testFeatures2));
  Serial.print(", Class: ");
  Serial.println(myLR.predictClass(testFeatures2));

  delay(5000); // Test every 5 seconds
}
```
