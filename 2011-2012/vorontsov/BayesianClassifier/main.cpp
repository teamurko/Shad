#include "classifiers.h"

#include <boost/foreach.hpp>

void printUsage(const std::string& name)
{
    std::cerr << "Usage: " << name << " train-set-filename"
              << " test-set-filename dataset(0|1)" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        printUsage(argv[0]);
        exit(1);
    }
    std::string trainSetFilename(argv[1]);
    std::string testSetFilename(argv[2]);

    Dataset train;
    Dataset test;
    loadDataset(trainSetFilename, true, &train);
    loadDataset(testSetFilename, false, &test);

    if (argv[3][0] == '0') {
        ParzenWindowClassifier classifier(0.5);
        classifier.learn(train);
        classifier.classify(&test);
        BOOST_FOREACH(const LabeledObject& object, test) {
            std::cout << object.class_label << std::endl;
        }
    }
    else {
        ParzenWindowClassifier classifier(0.56);
        classifier.learn(train);
        classifier.classify(&test);
    }

    BOOST_FOREACH(const LabeledObject& object, test) {
        std::cout << object.class_label << std::endl;
    }

    return 0;
}
