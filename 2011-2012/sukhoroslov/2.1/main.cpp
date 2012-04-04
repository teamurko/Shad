#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>
#include <set>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
            assert(false); \
        } \
    } while (false)

typedef std::vector<double> Features;
typedef std::vector<Features> Objects;

void chooseRandomMeans(const std::vector<Features>& objects,
                       size_t numClusters,
                       std::vector<Features>* means)
{
    REQUIRE(objects.size() >= numClusters,
            "Number of objects should be at least" <<
            " of number of clusters");
    REQUIRE(numClusters == means->size(),
            "means should have size of numClusters");
    std::set<Features> taken;
    for (size_t index = 0; index < numClusters; ++index) {
        if (taken.count(objects[index])) continue;
        means->at(index) = objects[index];
        taken.insert(objects[index]);
    }
}

bool approximateEqual(const Features& first, const Features& second)
{
    const static double EPS = 1e-7;
    REQUIRE(first.size() == second.size(),
            "Features sizes differ");
    for (size_t index = 0; index < first.size(); ++index) {
        if (fabs(first[index] - second[index]) > EPS) return false;
    }
    return true;
}

bool approximateEqual(const std::vector<Features>& first,
                       const std::vector<Features>& second)
{
    REQUIRE(first.size() == second.size(),
            "Cannot compare vectores of different sizes");
    for (size_t index = 0; index < first.size(); ++index) {
        if (!approximateEqual(first[index], second[index])) return false;
    }
    return true;
}

double distance(const Features& first, const Features& second)
{
    REQUIRE(first.size() == second.size(),
            "Features sizes differ");
    double result = 0.0;
    for (size_t index = 0; index < first.size(); ++index) {
        result += pow(first[index] - second[index], 2.0);
    }
    return sqrt(result);
}

size_t findNearestIndex(const Objects& means, const Features& object)
{

}

void add(Features& first, const Features& second)
{
    REQUIRE(first.size() == second.size(),
            "Features sizes differ");
    for (size_t index = 0; index < first.size(); ++index) {
        first[index] += second[index];
    }
}

void div(Features& object, double divider)
{
    for (size_t index = 0; index < object.size(); ++index) {
        object[index] /= divider;
    }
}

void findMeans(const Objects& objects, const std::vector<size_t>& clusterIndex,
               Objects* means)
{
    for (size_t index = 0; index < means->size(); ++index) {
        means->at(index) = Features(objects.back().size());
    }
    std::vector<size_t> clusterSize(means->size());
    for (size_t index = 0; index < objects.size(); ++index) {
        add(means->at(clusterIndex[index]), objects[index]);
        ++clusterSize[clusterIndex[index]];
    }
    for (size_t index = 0; index < means->size(); ++index) {
        div(means->at(index), clusterSize[index]);
    }
}

void kMeans(const std::vector<Features>& objects, size_t numClusters,
            std::vector<size_t>* clusterIndex)
{
    std::vector<Features> means(numClusters);
    chooseRandomMeans(objects, numClusters, &means);
    clusterIndex->resize(objects.size());
    bool areMeansChanged = true;
    while (areMeansChanged) {
        for (size_t index = 0; index < objects.size(); ++index) {
            clusterIndex->at(index) = findNearestIndex(means, objects[index]);
        }
        std::vector<Features> updatedMeans(numClusters);
        findMeans(objects, *clusterIndex, &updatedMeans);
        if (approximateEqual(means, updatedMeans)) {
            areMeansChanged = false;
        }
    }
}

void readData(std::vector<Features>* objects, size_t& numClusters)
{
    size_t numObjects, dimSize;
    std::cin >> numObjects >> numClusters >> dimSize;
    objects->reserve(numObjects);
    for (size_t i = 0; i < numObjects; ++i) {
        Features f(dimSize);
        for (size_t j = 0; j < dimSize; ++j) {
            std::cin >> f[j];
        }
        objects->push_back(f);
    }
}

void outputData(const std::vector<Features>& objects,
                const std::vector<size_t>& clusterIndex)
{
    std::cout.precision(10);
    std::cout << std::fixed;
    REQUIRE(objects.size() == clusterIndex.size(),
            "Sizes of objects and clusterIndex are not equal");
    for (size_t index = 0; index < objects.size(); ++index) {
        const Features& features = objects[index];
        for (size_t featureIndex = 0; featureIndex < features.size();
                                      ++featureIndex) {
            std::cout << features[featureIndex] << " ";
        }
        std::cout << clusterIndex[index] << std::endl;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::vector<Features> objects;
    size_t numClusters;
    readData(&objects, numClusters);
    std::vector<size_t> clusterIndex;
    kMeans(objects, numClusters, &clusterIndex);
    outputData(objects, clusterIndex);

    return 0;
}
