#ifndef CLASSIFIER_INTERFACE_H
#define CLASSIFIER_INTERFACE_H

#include <vector>
#include <string>

typedef double Feature;
typedef std::vector<Feature> Features;

struct LabeledObject
{
    Features features;
    int class_label;
};

typedef Features Object;

typedef std::vector<LabeledObject> Dataset;

void loadDataset(const std::string& filename, bool test, Dataset* dataset);

class ClassifierInterface
{
public:
    virtual void learn(const Dataset& dataset) = 0;
    virtual void classify(Dataset* dataset) = 0;
};

#endif
