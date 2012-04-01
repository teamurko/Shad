#include "classifiers.h"
#include "quality_rating.h"

#include <boost/foreach.hpp>

void printUsage(const std::string& name)
{
    std::cerr << "Usage: " << name << " train-set-filename" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        printUsage(argv[0]);
        exit(1);
    }
    std::string trainSetFilename(argv[1]);

    Dataset train;
    loadDataset(trainSetFilename, true, &train);

    PredictionQuality quality;
    for (double h = 0.53; h < 0.59; h += 0.005) {
        std::cerr << "Window width : " << h << std::endl;
        ParzenWindowClassifier classifier(h);
        srand(1);
        std::cerr << "Train quality : "
                  << quality.rateTraining(train, classifier) << std::endl;
        std::cerr << "Q-fold quality : "
                  << quality.rateQFold(train, classifier, train.size() / 4)
                  << std::endl;
    }
//    std::cerr << "LOO quality : " << quality.rateLOO(train, classifier, 10)
//              << std::endl;


    return 0;
}
