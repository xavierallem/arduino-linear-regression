#ifndef NaiveBayesClassifier_h
#define NaiveBayesClassifier_h

#include "Arduino.h"

class NaiveBayesClassifier
{
public:
  // Constructor: numFeatures = number of features, numClasses = number of possible class labels
  // featureMaxValues: an array specifying the maximum possible value for each feature (for discrete features)
  // If features are continuous, you might need to discretize them or use Gaussian Naive Bayes (more complex)
  NaiveBayesClassifier(int numFeatures, int numClasses, int* featureMaxValues);
  ~NaiveBayesClassifier();

  // Train the model with a single data point
  // features: array of feature values
  // label: the class label for this data point
  void train(int* features, int label);

  // Predict the class label for new features
  int predict(int* features);

  // Reset the classifier (clear all learned data)
  void clean();

private:
  int _numFeatures;
  int _numClasses;
  int* _featureMaxValues; // Max value for each feature (used for array sizing)

  long* _classCounts; // Count of occurrences for each class
  long** _featureCounts; // Count of feature_value given class: _featureCounts[feature_idx][feature_value][class_idx]
  long _totalTrainingSamples; // Total number of samples trained

  // Helper to calculate log probability (to avoid underflow with many small probabilities)
  float logProb(long count, long total);
};

#endif
