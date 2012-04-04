#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include <iostream>
#include <vector>

typedef std::vector<double> Point;

void print(const Point& point)
{
    std::cout.precision(10);
    std::cout << std::fixed;
    for (size_t i = 0; i < point.size(); ++i) {
        if (i) std::cout << " ";
        std::cout << point[i];
    }
    std::cout << std::endl;
}

int main()
{
    boost::mt19937 rng;
    boost::normal_distribution<> nd(0.0, 1.0);
    boost::variate_generator<boost::mt19937&,
        boost::normal_distribution<> > nor(rng, nd);

    const size_t dimSize = 2;
    const size_t numElements = 1000;
    const size_t numClusters = 2;
    std::vector<Point> means(numClusters);
    means[0] = Point(dimSize, 1.0);
    means[1] = Point(dimSize);
    std::vector<double> deviations;
    deviations.push_back(2.0);
    deviations.push_back(3.0);

    std::cout << numElements << " " << numClusters << " "
              << dimSize << std::endl;

    for (size_t i = 0; i < numElements; ++i) {
        int r = rand() & 1;
        Point point(dimSize);
        for (size_t j = 0; j < dimSize; ++j) {
            point[j] = means[r][j] + nor() * deviations[r];
        }
        print(point);
    }

    return 0;
}
