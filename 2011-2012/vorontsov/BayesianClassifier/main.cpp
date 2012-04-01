#include "classifiers.h"

#include <boost/foreach.hpp>

void printUsage(const std::string& name)
{
    std::cerr << "Usage: " << name << " train-set-filename"
              << " test-set-filename" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        printUsage(argv[0]);
        exit(1);
    }
    std::string trainSetFilename(argv[1]);
    std::string testSetFilename(argv[2]);

    Dataset train;
    Dataset test;
    loadDataset(trainSetFilename, true, &train);
    loadDataset(testSetFilename, false, &test);
    NaiveBayesClassifier classifier;
    classifier.learn(train);
    classifier.classify(&test);
    BOOST_FOREACH(const LabeledObject& object, test) {
        const Features& features = object.features;
        BOOST_FOREACH(Feature feature, features) {
            std::cout << feature << ",";
        }
        std::cout << object.class_label << std::endl;
    }

    return 0;
}
