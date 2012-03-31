#ifndef BAYES_CLASSIFIER_H
#define BAYES_CLASSIFIER_H

#include "bayes_classifier.h"

#include <iostream>
#include <sstream>
#include <fstream>

void BayesClassifier::Learn(const Dataset& dataset)
{

}

void BayesClassifier::Classify(Dataset* dataset)
{

}

void LoadDataset(const std::string& filename, Dataset* dataset)
{
    std::ifstream ifs(filename.c_str());
    std::string line;
    while (getline(ifs, line)) {
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream istr(line);
        Object object;
        Feature feature;
        while (istr >> feature) {
            object.features.push_back(feature);
        }
        object.class_label = object.features.back() + 0.5;
        object.features.pop_back();
        dataset->push_back(object);
    }
}

#endif
