#ifndef NAIVE_BAYES_CLASSIFIER_H
#define NAIVE_BAYES_CLASSIFIER_H

#include "classifier_interface.h"

class OneDimSamplingProbability
{
public:
    explicit OneDimSamplingProbability(const std::vector<Feature>& sample,
                                       double windowWidth)
        : sample_(sample), windowWidth_(windowWidth)
    {
    }

    double operator()(Feature x) const;

private:
    std::vector<Feature> sample_;
    double windowWidth_;
};

class MultiDimSamplingProbability
{
public:
    explicit MultiDimSamplingProbability(
            const std::vector<Object>& sampleSet,
            const std::vector<double>& featureWeights,
            double windowWidth = 1.0);

    double operator()(const Object& object) const;

    double log(const Object& object) const;

private:
    std::vector<double> nthFeature(
            const std::vector<Object>& sampleSet, size_t index) const;

    std::vector<OneDimSamplingProbability> oneDimProbabilities_;
};


class NaiveBayesClassifier : public ClassifierInterface
{
public:
    virtual void learn(const Dataset& dataset);
    virtual void classify(Dataset* dataset);
private:
    static std::vector<Object> objectsByClass(
            const Dataset& dataset, ClassLabel label);

    double posteriorClassProbability(
            ClassLabel label, const Object& object) const;

    void classify(LabeledObject* object) const;

    void calculateFeatureWeights(const Dataset& dataset);

    double calculateFeatureWeight(const std::vector<Feature>& objects) const;

    std::vector<double> featureWeights_;
    std::vector<ClassLabel> classLabels_;
    std::map<ClassLabel, double> classWeight_;
    std::map<ClassLabel, double> classProbability_;
    std::map<ClassLabel, MultiDimSamplingProbability> samplingProbability_;
};

#endif
