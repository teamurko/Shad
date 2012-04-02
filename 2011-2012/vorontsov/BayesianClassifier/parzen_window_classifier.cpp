#include "parzen_window_classifier.h"

#include <boost/foreach.hpp>
#include <cmath>

double ParzenWindowClassifier::kernelE(double x) const
{
    if (fabs(x) > 1) {
        return 0.0;
    }
    return 0.75 * fabs(1 - x * x);
}

double ParzenWindowClassifier::kernelQ(double x) const
{
    if (fabs(x) > 1) {
        return 0.0;
    }
    return 15 * pow(1 - x * x, 2.0) / 16;
}

void ParzenWindowClassifier::calculateFeatureWeights(const Dataset& dataset)
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

double ParzenWindowClassifier::calculateFeatureWeight(
                             const std::vector<Feature>& objects) const
{
    double rangeLength = *std::max_element(objects.begin(), objects.end()) -
                         *std::min_element(objects.begin(), objects.end());
    return 1.0 / (1.0 + rangeLength);
}

double ParzenWindowClassifier::average(
        ClassLabel label, size_t featureIndex) const
{
    size_t size = 0;
    double value = 0.0;
    BOOST_FOREACH(const LabeledObject& object, dataset_) {
        if (object.class_label == label) {
            value += object.features[featureIndex] *
                     featureWeights_[featureIndex];
            ++size;
        }
    }
    return value / size;
}

void ParzenWindowClassifier::learn(const Dataset& dataset)
{
    dataset_ = dataset;

    featureWeights_.clear();
    classProbability_.clear();
    classWeight_.clear();
    classLabels_.clear();

    calculateFeatureWeights(dataset);
    if (featureWeights_.size() >= 0) {
        std::vector<double> t(featureWeights_.size());
        for (size_t i = 0; i < t.size(); ++i) {
            double a1 = average(0, i);
            double a2 = average(1, i);
            t[i] = fabs(a1 - a2);
        }
        for (size_t i = 0; i < t.size(); ++i) {
            featureWeights_[i] *= t[i];
        }
    }

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
}

void ParzenWindowClassifier::classify(Dataset* dataset)
{
    BOOST_FOREACH(LabeledObject& object, *dataset) {
        classify(&object);
    }
}

void ParzenWindowClassifier::classify(LabeledObject* object) const
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

double ParzenWindowClassifier::posteriorClassProbability(
                    ClassLabel label, const Object& object) const
{
    double result = 0.0;
    size_t classSize = 0;
    BOOST_FOREACH(const LabeledObject& labeledObject, dataset_) {
        if (labeledObject.class_label == label) {
            ++classSize;
            result += kernelQ(
                    distance(object, labeledObject.features) / windowWidth_);
        }
    }
    return result * mapAt(classWeight_, label) *
           mapAt(classProbability_, label) / classSize;
}

double ParzenWindowClassifier::distance(const Object& x, const Object& y) const
{
    double result = 0.0;
    for (size_t featureIndex = 0; featureIndex < featureWeights_.size();
                                                            ++featureIndex) {
        result += featureWeights_[featureIndex] *
                  pow(x[featureIndex] - y[featureIndex], 2.0);
    }
    return std::sqrt(result);
}

