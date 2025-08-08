#ifndef Perceptron_h
#define Perceptron_h

#include "Arduino.h"

class Perceptron
{
public:
  // Constructor: numFeatures = number of input features, learningRate = how fast weights adjust
  Perceptron(int numFeatures, float learningRate = 0.1);
  ~Perceptron();

  // Train the perceptron with a single data point
  // features: array of input features
  // expectedOutput: the target binary output (0 or 1)
  void train(int* features, int expectedOutput);

  // Predict the output for new features
  // Returns 0 or 1
  int predict(int* features);

  // Reset weights to zero
  void clean();

private:
  int _numFeatures;
  float _learningRate;
  float* _weights; // Weights for each feature
  float _bias;     // Bias term

  // Activation function (step function)
  int activate(float sum);
};

#endif
