#ifndef LogisticRegression_h
#define LogisticRegression_h

#include "Arduino.h"

class LogisticRegression
{
public:
  // Constructor: numFeatures = number of input features, learningRate = how fast weights adjust
  LogisticRegression(int numFeatures, float learningRate = 0.1);
  ~LogisticRegression();

  // Train the model with a single data point
  // features: array of input features
  // expectedOutput: the target binary output (0 or 1)
  void train(int* features, int expectedOutput);

  // Predict the probability of class 1 for new features
  // Returns a float between 0.0 and 1.0
  float predictProbability(int* features);

  // Predict the binary class (0 or 1) for new features
  int predictClass(int* features, float threshold = 0.5);

  // Reset weights to zero
  void clean();

private:
  int _numFeatures;
  float _learningRate;
  float* _weights; // Weights for each feature
  float _bias;     // Bias term

  // Sigmoid activation function
  float sigmoid(float x);
};

#endif
