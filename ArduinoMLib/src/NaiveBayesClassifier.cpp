#include "NaiveBayesClassifier.h"
#include "Arduino.h"
#include <math.h> // For log function

NaiveBayesClassifier::NaiveBayesClassifier(int numFeatures, int numClasses, int* featureMaxValues)
  : _numFeatures(numFeatures), _numClasses(numClasses), _featureMaxValues(featureMaxValues)
{
  _classCounts = new long[_numClasses];
  _featureCounts = new long*[_numFeatures];
  _totalTrainingSamples = 0;

  for (int i = 0; i < _numClasses; i++) {
    _classCounts[i] = 0;
  }

  for (int i = 0; i < _numFeatures; i++) {
    // Allocate for feature values * numClasses to store counts for each class
    _featureCounts[i] = new long[_featureMaxValues[i] * _numClasses];
    for (int j = 0; j < _featureMaxValues[i] * _numClasses; j++) {
      _featureCounts[i][j] = 0;
    }
  }
}

NaiveBayesClassifier::~NaiveBayesClassifier()
{
  clean();
  delete[] _classCounts;
  for (int i = 0; i < _numFeatures; i++) {
    delete[] _featureCounts[i];
  }
  delete[] _featureCounts;
  // _featureMaxValues is passed, not owned, so don't delete
}

void NaiveBayesClassifier::train(int* features, int label)
{
  if (label < 0 || label >= _numClasses) return; // Invalid label

  _classCounts[label]++;
  _totalTrainingSamples++;

  for (int i = 0; i < _numFeatures; i++) {
    if (features[i] >= 0 && features[i] < _featureMaxValues[i]) {
      _featureCounts[i][features[i] * _numClasses + label]++;
    }
  }
}

float NaiveBayesClassifier::logProb(long count, long total)
{
  // Add 1 for Laplace smoothing to avoid zero probabilities
  return log((float)(count + 1) / (total + _numClasses)); // +_numClasses for denominator in Laplace smoothing
}

int NaiveBayesClassifier::predict(int* features)
{
  if (_totalTrainingSamples == 0) return -1; // No training data

  float maxLogProbability = -FLT_MAX; // Use float max for comparison
  int predictedLabel = -1;

  for (int i = 0; i < _numClasses; i++) {
    // Calculate log prior probability for the current class
    float currentClassLogProb = log((float)_classCounts[i] / _totalTrainingSamples);

    // Calculate log likelihood for each feature given the current class
    for (int j = 0; j < _numFeatures; j++) {
      if (features[j] >= 0 && features[j] < _featureMaxValues[j]) {
        currentClassLogProb += logProb(_featureCounts[j][features[j] * _numClasses + i], _classCounts[i]);
      } else {
        // Handle unseen feature values: use Laplace smoothing for this case as well
        currentClassLogProb += logProb(0, _classCounts[i]);
      }
    }

    if (currentClassLogProb > maxLogProbability) {
      maxLogProbability = currentClassLogProb;
      predictedLabel = i;
    }
  }

  return predictedLabel;
}

void NaiveBayesClassifier::clean()
{
  for (int i = 0; i < _numClasses; i++) {
    _classCounts[i] = 0;
  }
  for (int i = 0; i < _numFeatures; i++) {
    for (int j = 0; j < _featureMaxValues[i] * _numClasses; j++) {
      _featureCounts[i][j] = 0;
    }
  }
  _totalTrainingSamples = 0;
}
