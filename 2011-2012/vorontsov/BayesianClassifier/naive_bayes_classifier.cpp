#include "naive_bayes_classifier.h"

#include <boost/foreach.hpp>
#include <cmath>

double OneDimSamplingProbability::operator()(Feature x) const
{
    int numInWindow = 0;
    BOOST_FOREACH(double sampleElement, sample_) {
        if (fabs(sampleElement - x) * 2.0 < windowWidth_) {
            numInWindow += 1;
        }
    }
    return static_cast<double>(numInWindow) / windowWidth_ /
                                            sample_.size();
}

MultiDimSamplingProbability::MultiDimSamplingProbability(
        const std::vector<Object>& sampleSet,
        const std::vector<double>& featureWeights,
        double windowWidth)
{
    REQUIRE(!sampleSet.empty(), "Provide non empty sample set");
    REQUIRE(featureWeights.size() == sampleSet.back().size(),
            "Feature weights collection size should equal "
            << "object features number");
    size_t numFeatures = featureWeights.size();
    for (size_t index = 0; index < numFeatures; ++index) {
        oneDimProbabilities_.push_back(
                OneDimSamplingProbability(
                    nthFeature(sampleSet, index),
                    windowWidth / 50.0 / featureWeights[index]));
    }
}

double MultiDimSamplingProbability::operator()(const Object& object) const
{
    REQUIRE(object.size() == oneDimProbabilities_.size(),
            "Incorrect feature size");
    double result = 1.0;
    for (size_t index = 0; index < object.size(); ++index) {
        result *= oneDimProbabilities_[index](object[index]);
    }
    return result;
}

double MultiDimSamplingProbability::log(const Object& object) const
{
    REQUIRE(object.size() == oneDimProbabilities_.size(),
            "Incorrect feature size : " << object.size() << " "
            << oneDimProbabilities_.size());
    static const double EPS = 1e-10;
    double result = 0.0;
    for (size_t index = 0; index < object.size(); ++index) {
        result += ::log(oneDimProbabilities_[index](object[index]) + EPS);
    }
    return result;
}

std::vector<double> MultiDimSamplingProbability::nthFeature(
        const std::vector<Object>& sampleSet, size_t index) const
{
    REQUIRE(sampleSet.back().size() > index,
            "No feature with index " << index);
    std::vector<double> result;
    result.reserve(sampleSet.size());
    BOOST_FOREACH(const Object& object, sampleSet) {
        result.push_back(object[index]);
    }
    return result;
}

void NaiveBayesClassifier::calculateFeatureWeights(const Dataset& dataset)
{
    REQUIRE(!dataset.empty(), "Dataset is empty");
    size_t numFeatures = dataset.back().features.size();
    for (size_t featureIndex = 0; featureIndex < numFeatures;
                                                    ++featureIndex) {
        std::vector<Feature> objects(dataset.size());
        for (size_t index = 0; index < dataset.size(); ++index) {
            objects[index] = dataset[index].features[featureIndex];
        }
        featureWeights_.push_back(calculateFeatureWeight(objects));
    }
}

double NaiveBayesClassifier::calculateFeatureWeight(
                             const std::vector<Feature>& objects) const
{
    double rangeLength = *std::max_element(objects.begin(), objects.end()) -
                         *std::min_element(objects.begin(), objects.end());
    return 1.0 / (1.0 + rangeLength);
}

void NaiveBayesClassifier::learn(const Dataset& dataset)
{
    featureWeights_.clear();
    classProbability_.clear();
    classWeight_.clear();
    classLabels_.clear();
    samplingProbability_.clear();

    calculateFeatureWeights(dataset);

    std::map<ClassLabel, int> classElementsNumber;
    BOOST_FOREACH(const LabeledObject& object, dataset) {
        ++classElementsNumber[object.class_label];
    }

    typedef std::map<ClassLabel, int>::value_type MapElementValue;
    BOOST_FOREACH(const MapElementValue& item, classElementsNumber) {
        classProbability_[item.first] =
            static_cast<double>(item.second) / dataset.size();
        classWeight_[item.first] = 1.0; // default, change it
        classLabels_.push_back(item.first);
    }

    BOOST_FOREACH(const MapElementValue& item, classElementsNumber) {
        samplingProbability_.insert(
                std::make_pair(item.first,
                        MultiDimSamplingProbability(
                            objectsByClass(dataset, item.first),
                            featureWeights_, 0.7)));
    }
}

std::vector<Object> NaiveBayesClassifier::objectsByClass(
        const Dataset& dataset, ClassLabel label)
{
    std::vector<Object> objects;
    BOOST_FOREACH(const LabeledObject& object, dataset) {
        if (object.class_label == label) {
            objects.push_back(object.features);
        }
    }
    return objects;
}

double NaiveBayesClassifier::posteriorClassProbability(
                ClassLabel label, const Object& object) const
{
    static const double EPS = 1e-10;
    return log(mapAt(classWeight_, label) + EPS) +
           log(mapAt(classProbability_, label) + EPS) +
           mapAt(samplingProbability_, label).log(object);
}

void NaiveBayesClassifier::classify(Dataset* dataset)
{
    BOOST_FOREACH(LabeledObject& object, *dataset) {
        classify(&object);
    }
}

void NaiveBayesClassifier::classify(LabeledObject* object) const
{
    static const ClassLabel UNDEFINED =
                                std::numeric_limits<ClassLabel>::max();
    ClassLabel optLabel = UNDEFINED;
    double targetValue;
    BOOST_FOREACH(ClassLabel label, classLabels_) {
        double candidateValue =
                    posteriorClassProbability(label, object->features);
        if (optLabel == UNDEFINED || (targetValue < candidateValue)) {
            optLabel = label;
            targetValue = candidateValue;
        }
    }
    REQUIRE(optLabel != UNDEFINED, "Object cannot be classified");
    object->class_label = optLabel;
}

