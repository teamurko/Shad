#ifndef CLASSIFIER_INTERFACE_H__
#define CLASSIFIER_INTERFACE_H__

#include <vector>
#include <string>

typedef double Feature;

struct Object
{
    std::vector<Feature> features;
    int class_label;
};

typedef std::vector<Object> Dataset;

void LoadDataset(const std::string& filename, Dataset* dataset);

class ClassifierInterface
{
public:
    virtual void Learn(const Dataset& dataset) = 0;
    virtual void Classify(Dataset* dataset) = 0;
};

#endif
