#include "cppQueue.h"
#include "Arduino.h"
#include "KNNClassifier.h"
#include <limits.h> // For INT_MAX

#define IMPLEMENTATION LIFO

// KNNClassifier Implementation

KNNClassifier::KNNClassifier(int numFeatures, int k, int maxDataPoints)
  : _numFeatures(numFeatures), _k(k), _maxDataPoints(maxDataPoints)
{
  _trainingDataQueue = new cppQueue(sizeof(KNNData), maxDataPoints, IMPLEMENTATION);
}

KNNClassifier::~KNNClassifier()
{
  // Free memory for features arrays within each KNNData struct
  KNNData data;
  while (!_trainingDataQueue->isEmpty()) {
    _trainingDataQueue->pop(&data);
    delete[] data.features;
  }
  delete _trainingDataQueue;
}

bool KNNClassifier::addTrainingData(int* features, int label)
{
  if (_trainingDataQueue->isFull()) {
    // Optionally, remove the oldest data point to make space
    KNNData oldestData;
    _trainingDataQueue->pop(&oldestData);
    delete[] oldestData.features; // Free memory of old features array
  }

  KNNData newData;
  newData.features = new int[_numFeatures];
  if (newData.features == NULL) return false; // Memory allocation failed

  for (int i = 0; i < _numFeatures; i++) {
    newData.features[i] = features[i];
  }
  newData.label = label;

  return _trainingDataQueue->push(&newData);
}

int KNNClassifier::predict(int* features)
{
  if (_trainingDataQueue->isEmpty()) return -1; // No training data

  // Store distances and corresponding labels
  struct Neighbor {
    float distance;
    int label;
  };

  // Use a simple array to store neighbors and sort them
  // This is not efficient for large k or maxDataPoints, but simple for Arduino
  Neighbor* neighbors = new Neighbor[_trainingDataQueue->sizeOf()];
  if (neighbors == NULL) return -1; // Memory allocation failed

  int currentNeighbors = 0;

  KNNData trainingData;
  for (int i = 0; i < _trainingDataQueue->sizeOf(); i++) {
    _trainingDataQueue->peekIdx(&trainingData, i);

    float distance = 0;
    for (int j = 0; j < _numFeatures; j++) {
      float diff = (float)features[j] - trainingData.features[j];
      distance += diff * diff; // Squared Euclidean distance
    }
    // No need to take sqrt for comparison, saves computation

    neighbors[currentNeighbors].distance = distance;
    neighbors[currentNeighbors].label = trainingData.label;
    currentNeighbors++;
  }

  // Sort neighbors by distance (bubble sort for simplicity on Arduino)
  for (int i = 0; i < currentNeighbors - 1; i++) {
    for (int j = 0; j < currentNeighbors - i - 1; j++) {
      if (neighbors[j].distance > neighbors[j+1].distance) {
        Neighbor temp = neighbors[j];
        neighbors[j] = neighbors[j+1];
        neighbors[j+1] = temp;
      }
    }
  }

  // Perform majority vote among the k nearest neighbors
  // Use a simple frequency map (array) for labels
  // Assuming labels are non-negative and within a reasonable range
  // Max label value needs to be considered for array size
  // For simplicity, let's assume labels are small integers (e.g., 0-9)
  // A more robust solution would use a dynamic map or sort labels and count

  // Find max label to determine array size for vote counting
  int maxLabel = 0;
  for(int i = 0; i < currentNeighbors; i++) {
      if (neighbors[i].label > maxLabel) {
          maxLabel = neighbors[i].label;
      }
  }

  // Allocate vote counts dynamically or use a fixed max if known
  // For now, let's assume maxLabel is not excessively large
  int* voteCounts = new int[maxLabel + 1];
  if (voteCounts == NULL) {
      delete[] neighbors;
      return -1; // Memory allocation failed
  }
  for(int i = 0; i <= maxLabel; i++) {
      voteCounts[i] = 0;
  }

  int effectiveK = (_k < currentNeighbors) ? _k : currentNeighbors;
  for (int i = 0; i < effectiveK; i++) {
    if (neighbors[i].label >= 0 && neighbors[i].label <= maxLabel) {
        voteCounts[neighbors[i].label]++;
    }
  }

  int predictedLabel = -1;
  int maxVotes = -1;
  for (int i = 0; i <= maxLabel; i++) {
    if (voteCounts[i] > maxVotes) {
      maxVotes = voteCounts[i];
      predictedLabel = i;
    }
  }

  delete[] neighbors;
  delete[] voteCounts;

  return predictedLabel;
}

void KNNClassifier::clean()
{
  KNNData data;
  while (!_trainingDataQueue->isEmpty()) {
    _trainingDataQueue->pop(&data);
    delete[] data.features;
  }
  _trainingDataQueue->clean();
}
