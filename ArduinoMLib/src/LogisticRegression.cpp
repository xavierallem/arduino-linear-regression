#include "LogisticRegression.h"
#include "Arduino.h"
#include <math.h> // For exp() function

LogisticRegression::LogisticRegression(int numFeatures, float learningRate)
  : _numFeatures(numFeatures), _learningRate(learningRate)
{
  _weights = new float[_numFeatures];
  // Initialize weights and bias to zeros
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] = 0.0;
  }
  _bias = 0.0;
}

LogisticRegression::~LogisticRegression()
{
  delete[] _weights;
}

float LogisticRegression::sigmoid(float x)
{
  return 1.0 / (1.0 + exp(-x));
}

void LogisticRegression::train(int* features, int expectedOutput)
{
  float predictionProbability = predictProbability(features);
  float error = expectedOutput - predictionProbability;

  // Update weights and bias using gradient descent
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] += _learningRate * error * features[i];
  }
  _bias += _learningRate * error;
}

float LogisticRegression::predictProbability(int* features)
{
  float linearCombination = _bias;
  for (int i = 0; i < _numFeatures; i++) {
    linearCombination += _weights[i] * features[i];
  }
  return sigmoid(linearCombination);
}

int LogisticRegression::predictClass(int* features, float threshold)
{
  float probability = predictProbability(features);
  return (probability >= threshold) ? 1 : 0;
}

void LogisticRegression::clean()
{
  for (int i = 0; i < _numFeatures; i++) {
    _weights[i] = 0.0;
  }
  _bias = 0.0;
}
