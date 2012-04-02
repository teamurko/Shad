#ifndef CLASSIFIER_INTERFACE_H
#define CLASSIFIER_INTERFACE_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>

#define REQUIRE(cond, message) \
    if (!(cond)) { \
        std::cerr << message << std::endl; \
        assert(false); \
    }

typedef double Feature;
typedef std::vector<Feature> Features;

struct LabeledObject
{
    Features features;
    int class_label;
};

typedef Features Object;

typedef std::vector<LabeledObject> Dataset;

void loadDataset(const std::string& filename, bool test, Dataset* dataset);

class ClassifierInterface
{
public:
    virtual void learn(const Dataset& dataset) = 0;
    virtual void classify(Dataset* dataset) = 0;
};

typedef int ClassLabel;

template <class Key, class Value>
const Value& mapAt(const std::map<Key, Value>& map, const Key& key)
{
    typename std::map<Key, Value>::const_iterator it = map.find(key);
    REQUIRE(it != map.end(), "Map does not contain object with key " << key);
    return it->second;
}

#endif
