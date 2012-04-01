#ifndef QUALITY_RATING_H
#define QUALITY_RATING_H

#include "classifiers.h"

#include <boost/foreach.hpp>

class PredictionQuality
{
public:
    double rateQFold(Dataset dataset,
                     ClassifierInterface& classifier,
                     size_t blockSize) const
    {
        std::random_shuffle(dataset.begin(), dataset.end());
        size_t numBlocks = (dataset.size() + blockSize - 1) / blockSize;
        size_t numCorrect = 0;
        for (size_t blockIndex = 0; blockIndex < numBlocks; ++blockIndex) {
            size_t startBlockPosition = blockIndex * blockSize;
            size_t endBlockPosition =
                    std::min(startBlockPosition + blockSize, dataset.size());
            Dataset train;
            train.reserve(dataset.size());
            Dataset test;
            test.reserve(blockSize);
            for (size_t index = 0; index < dataset.size(); ++index) {
                if (startBlockPosition <= index && index < endBlockPosition) {
                    test.push_back(dataset[index]);
                } else {
                    train.push_back(dataset[index]);
                }
            }
            classifier.learn(train);
            classifier.classify(&test);
            for (size_t index = 0; index < test.size(); ++index) {
                numCorrect += test[index].class_label ==
                            dataset[startBlockPosition + index].class_label;
            }
        }
        return static_cast<double>(numCorrect) / dataset.size();
    }

    double rateTQFold(const Dataset& dataset,
                      ClassifierInterface& classifier,
                      size_t blockSize,
                      size_t numIterations) const
    {
        double quality = 0.0;
        for (size_t index = 0; index < numIterations; ++index) {
            quality += rateQFold(dataset, classifier, blockSize);
        }
        return quality / numIterations;
    }

    double rateLOO(const Dataset& dataset,
                   ClassifierInterface& classifier, size_t count) const
    {
        size_t numCorrect = 0;
        count = std::min(count, dataset.size());
        for (size_t loIndex = 0; loIndex < count; ++loIndex) {
            Dataset trainSet(dataset.size() - 1);
            size_t tsIndex = 0;
            for (size_t index = 0; index < dataset.size(); ++index) {
                if (index != loIndex) {
                    trainSet[tsIndex++] = dataset[index];
                }
            }

            classifier.learn(trainSet);

            Dataset testSet;
            LabeledObject loObject = dataset[loIndex];
            testSet.push_back(loObject);

            ClassLabel correctLabel = loObject.class_label;

            classifier.classify(&testSet);
            numCorrect += correctLabel == testSet.front().class_label;
        }
        return static_cast<double>(numCorrect) / count;
    }

    double rateTraining(const Dataset& dataset,
                        ClassifierInterface& classifier) const
    {
        classifier.learn(dataset);
        Dataset testSet = dataset;
        classifier.classify(&testSet);

        size_t numCorrect = 0;
        for (size_t index = 0; index < dataset.size(); ++index) {
            numCorrect +=
                dataset[index].class_label == testSet[index].class_label;
        }
        return static_cast<double>(numCorrect) / dataset.size();
    }

};

#endif
