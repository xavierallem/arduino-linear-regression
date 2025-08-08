#include "Perceptron.h"
#include "Arduino.h"

Perceptron::Perceptron(int numFeatures, float learningRate)
  : _numFeatures(numFeatures), _learningRate(learningRate)
{
  _weights = new float[_numFeatures];
  // Initialize weights and bias to small random values or zeros
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] = 0.0; // Or random(0, 100) / 100.0 - 0.5 for small random
  }
  _bias = 0.0;
}

Perceptron::~Perceptron()
{
  delete[] _weights;
}

int Perceptron::activate(float sum)
{
  return (sum >= 0) ? 1 : 0;
}

void Perceptron::train(int* features, int expectedOutput)
{
  int prediction = predict(features);
  int error = expectedOutput - prediction;

  // Update weights and bias
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] += _learningRate * error * features[i];
  }
  _bias += _learningRate * error;
}

int Perceptron::predict(int* features)
{
  float sum = _bias;
  for (int i = 0; i < _numFeatures; i++) {
    sum += _weights[i] * features[i];
  }
  return activate(sum);
}

void Perceptron::clean()
{
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] = 0.0;
  }
  _bias = 0.0;
}
