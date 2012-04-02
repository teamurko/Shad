#ifndef QDA_CLASSIFIER_H
#define QDA_CLASSIFIER_H

#include "classifier_interface.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/foreach.hpp>
#include <cmath>

namespace bnu = boost::numeric::ublas;

typedef bnu::matrix<double> Matrix;
typedef Matrix CovarianceMatrix;
typedef bnu::vector<double> Vector;

CovarianceMatrix inversed(CovarianceMatrix matrix);

class LDAClassifier : public ClassifierInterface
{
public:
    virtual void learn(const Dataset& dataset)
    {
        featureWeights_.clear();
        classProbability_.clear();
        classWeight_.clear();
        classLabels_.clear();

        calculateFeatureWeights(dataset);

        size_t objectSize = dataset.back().features.size();

        std::map<ClassLabel, int> classElementsNumber;
        BOOST_FOREACH(const LabeledObject& object, dataset) {
            ++classElementsNumber[object.class_label];
            dataset_[object.class_label].push_back(toVector(object.features));
        }

        typedef std::map<ClassLabel, int>::value_type MapElementValue;
        BOOST_FOREACH(const MapElementValue& item, classElementsNumber) {
            classProbability_[item.first] =
                static_cast<double>(item.second) / dataset.size();
            classWeight_[item.first] = 1.0; // default, change it
            classLabels_.push_back(item.first);
        }

        std::map<ClassLabel, Vector> means;
        BOOST_FOREACH(ClassLabel label, classLabels_) {
            Vector mean(objectSize);
            BOOST_FOREACH(const Vector& object, dataset_[label]) {
                mean += object;
            }
            mean /= mapAt(classElementsNumber, label);
            means.insert(std::make_pair(label, mean));
        }
        CovarianceMatrix covMatrix(objectSize, objectSize);
        BOOST_FOREACH(const LabeledObject& labeledObject, dataset) {
            Vector object = toVector(labeledObject.features);
            const Vector& mean = mapAt(means, labeledObject.class_label);
            covMatrix += bnu::outer_prod(object, mean);
        }
        covMatrix /= dataset.size();
        CovarianceMatrix invCovMatrix = inversed(covMatrix);

        BOOST_FOREACH(ClassLabel label, classLabels_) {
            const Vector& mean = mapAt(means, label);
            beta_.insert(
                std::make_pair(label,
                    - bnu::prec_inner_prod(mean,
                                         bnu::prod(invCovMatrix, mean)) * 0.5
                    + log(mapAt(classWeight_, label))
                    + log(mapAt(classProbability_, label))));
            alpha_.insert(std::make_pair(label,
                                         bnu::prod(invCovMatrix, mean)));
            std::cerr << "Class : " << label << " : alpha : " << mapAt(alpha_, label) << std::endl;
            std::cerr << "Class : " << label << " : beta : " << mapAt(beta_, label) << std::endl;
        }
    }

    virtual void classify(Dataset* dataset)
    {
        BOOST_FOREACH(LabeledObject& object, *dataset) {
            classify(&object);
        }
    }

private:
    void classify(LabeledObject* object) const
    {
        static const ClassLabel UNDEFINED =
                                std::numeric_limits<ClassLabel>::max();
        ClassLabel optLabel = UNDEFINED;
        double value;
        BOOST_FOREACH(ClassLabel label, classLabels_) {
            double candidateValue =
                        bnu::inner_prod(
                                toVector(object->features),
                                mapAt(alpha_, label)) + mapAt(beta_, label);
            if (optLabel == UNDEFINED || (value < candidateValue)) {
                value = candidateValue;
                optLabel = label;
            }
        }
        object->class_label = optLabel;
    }

    Vector toVector(const Features& features) const
    {
        Vector result(features.size());
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = features[i];
        }
        return result;
    }

    void calculateFeatureWeights(const Dataset& dataset)
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

    double calculateFeatureWeight(const std::vector<Feature>& objects) const
    {
        double rangeLength =
                        *std::max_element(objects.begin(), objects.end()) -
                        *std::min_element(objects.begin(), objects.end());
        return 1.0 / (1.0 + rangeLength);
    }

    std::map<ClassLabel, std::vector<Vector> > dataset_;
    std::map<ClassLabel, double> beta_;
    std::map<ClassLabel, Vector> alpha_;
    std::vector<double> featureWeights_;
    std::vector<ClassLabel> classLabels_;
    std::map<ClassLabel, double> classWeight_;
    std::map<ClassLabel, double> classProbability_;
};

#endif
