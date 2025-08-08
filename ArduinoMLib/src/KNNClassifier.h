#ifndef KNNClassifier_h
#define KNNClassifier_h

#include "Arduino.h"
#include "cppQueue.h"

class KNNClassifier
{
public:
  // Constructor: numFeatures = number of features per data point, k = number of neighbors, maxDataPoints = max training samples
  KNNClassifier(int numFeatures, int k, int maxDataPoints);
  ~KNNClassifier();

  // Add a training data point: features is an array of int, label is the class label
  bool addTrainingData(int* features, int label);

  // Predict the label for new features
  int predict(int* features);

  // Clear all training data
  void clean();

private:
  struct KNNData {
    int* features; // Array of features
    int label;     // Class label
  };

  cppQueue* _trainingDataQueue;
  int _numFeatures;
  int _k;
  int _maxDataPoints;
};

#endif
