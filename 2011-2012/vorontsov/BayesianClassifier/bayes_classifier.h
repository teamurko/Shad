#include "classifier_interface.h"

class BayesClassifier : public ClassifierInterface
{
public:
    virtual void Learn(const Dataset& dataset) = 0;
    virtual void Classify(Dataset* dataset) = 0;
};
