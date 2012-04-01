#ifndef CLASSIFIERS_H
#define CLASSIFIERS_H

#include "classifier_interface.h"

// 1) Naive Bayes
// 2) Parzen Window
// 3) QDA
// 4) LDA
// 5) EM-RBF


#include <map>
#include <iostream>
#include <cassert>

#define REQUIRE(cond, message) \
    if (!(cond)) { \
        std::cerr << message << std::endl; \
        assert(false); \
    }

typedef int ClassLabel;

template <class Key, class Value>
const Value& mapAt(const std::map<Key, Value>& map, const Key& key)
{
    typename std::map<Key, Value>::const_iterator it = map.find(key);
    REQUIRE(it != map.end(), "Map does not contain object with key " << key);
    return it->second;
}

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


class ParzenWindowClassifier : public ClassifierInterface
{
public:
    ParzenWindowClassifier(double windowWidth) : windowWidth_(windowWidth) { }

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

class LDAClassifier : public ClassifierInterface
{
public:
    virtual void learn(const Dataset& dataset);
    virtual void classify(Dataset* dataset);
};

#endif
