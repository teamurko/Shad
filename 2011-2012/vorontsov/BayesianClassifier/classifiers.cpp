#include "classifiers.h"

#include <sstream>
#include <fstream>

void loadDataset(const std::string& filename, bool train, Dataset* dataset)
{
    std::ifstream ifs(filename.c_str());
    std::string line;
    while (getline(ifs, line)) {
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream istr(line);
        LabeledObject object;
        Feature feature;
        while (istr >> feature) {
            object.features.push_back(feature);
        }
        if (train) {
            object.class_label = object.features.back() + 0.5;
            object.features.pop_back();
        }
        dataset->push_back(object);
    }
}
