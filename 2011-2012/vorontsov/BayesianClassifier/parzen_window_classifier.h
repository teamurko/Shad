#ifndef PARZEN_WINDOW_CLASSIFIER_H
#define PARZEN_WINDOW_CLASSIFIER_H

#include "classifier_interface.h"

class ParzenWindowClassifier : public ClassifierInterface
{
public:
    ParzenWindowClassifier(double windowWidth) : windowWidth_(windowWidth) { }

    virtual void learn(const Dataset& dataset);
    virtual void classify(Dataset* dataset);

private:
    double posteriorClassProbability(
            ClassLabel label, const Object& object) const;

    void classify(LabeledObject* object) const;

    void calculateFeatureWeights(const Dataset& dataset);

    double calculateFeatureWeight(const std::vector<Feature>& objects) const;

    double average(ClassLabel label, size_t featureIndex) const;

    double kernelE(double x) const;

    double kernelQ(double x) const;

    double distance(const Object& x, const Object& y) const;

    double windowWidth_;
    Dataset dataset_;
    std::vector<double> featureWeights_;
    std::vector<ClassLabel> classLabels_;
    std::map<ClassLabel, double> classWeight_;
    std::map<ClassLabel, double> classProbability_;
};

#endif
