#include <list>
#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <stdexcept>

void require(bool cond, const std::string& message)
{
    if (!cond) {
        throw std::runtime_error(message);
    }
}

// does not tail recursion optimization work here?
size_t gcd(size_t first, size_t second)
{
    if (first == 0) {
        return second;
    }
    return gcd(second % first, first);
}


class Triangle
{
public:
    Triangle(size_t lenA, size_t lenB, size_t lenC)
    {
        require(lenA > 0 && lenB > 0 && lenC > 0,
                "Triangle sides should be positive");
        sideLengths_.push_back(lenA);
        sideLengths_.push_back(lenB);
        sideLengths_.push_back(lenC);
        std::sort(sideLengths_.begin(), sideLengths_.end());
    }

    Triangle(const std::vector<size_t>& lengths) : sideLengths_(lengths)
    {
        require(sideLengths_.size() == 3, "Triangle should have three sides");
        require(sideLengths_[0] > 0 && sideLengths_[1] > 0 &&
                sideLengths_[2] > 0, "Triangle sides should be positive");
    }

    Triangle canonical() const
    {
        size_t gd = 0;
        for (size_t i = 0; i < sideLengths_.size(); ++i) {
            gd = gcd(gd, sideLengths_[i]);
        }
        std::vector<size_t> lengths(asVector());
        for (size_t i = 0; i < lengths.size(); ++i) {
            lengths[i] /= gd;
        }
        return Triangle(lengths);
    }

    std::vector<size_t> asVector() const
    {
        return sideLengths_;
    }

    size_t simpleHashModuloP(size_t dividend) const
    {
        require(dividend > 0, "Zero modulo");
        return hash() % dividend;
    }

    size_t hash() const
    {
        size_t result = 0;
        for (size_t i = 0; i < sideLengths_.size(); ++i) {
            result = result * BASE + (sideLengths_[i] - 1);
        }
        return result;
    }

    bool operator==(const Triangle& other) const
    {
        return sideLengths_ == other.sideLengths_;
    }

private:
    static size_t BASE;
    std::vector<size_t> sideLengths_; // sorted
};

size_t Triangle::BASE = 1000;

namespace std {
template<>
struct equal_to<Triangle> :
    binary_function<Triangle, Triangle, bool>
{
    bool operator()(const Triangle& first, const Triangle& second) const
    {
        return first == second;
    }
};
}

template <class Object>
class HashTable
{
public:
    explicit HashTable(size_t size) : table_(size)
    {
        require(size > 0, "Cannot instantiate empty hash table");
    }

    bool add(const Object& obj)
    {
        std::binder2nd<std::equal_to<Object> > comparator =
                            std::bind2nd(std::equal_to<Object>(), obj);
        size_t index = obj.hash() % table_.size();
        if (std::find_if(table_[index].begin(),
                         table_[index].end(),
                         comparator) ==
                    table_[index].end()) {
            table_[index].insert(table_[index].begin(), obj);
            return true;
        }
        return false;
    }

private:
    typedef std::list<Object> Objects;
    std::vector<Objects> table_;
};

void readData(std::vector<Triangle>* triangles)
{
    size_t numTriangles;
    std::cin >> numTriangles;
    for (size_t index = 0; index < numTriangles; ++index)
    {
        size_t lenA, lenB, lenC;
        std::cin >> lenA >> lenB >> lenC;
        triangles->push_back(Triangle(lenA, lenB, lenC));
    }
}

bool isPrime(size_t number)
{
    if (number < 2) {
        return false;
    }
    for (size_t divisor = 0; divisor * divisor <= number; ++divisor) {
        if (number % divisor == 0) {
            return false;
        }
    }
    return true;
}

size_t nextPrime(size_t number)
{
    while (!isPrime(++number));
    return number;
}

void solve(const std::vector<Triangle>& triangles)
{
    HashTable<Triangle> htable(nextPrime(triangles.size())); // prime
    size_t numUnique = 0;
    for (size_t index = 0; index < triangles.size(); ++index) {
        if (htable.add(triangles[index].canonical())) {
            numUnique++;
        }
    }
    std::cout << numUnique << std::endl;
    std::cerr << static_cast<double>(clock()) / CLOCKS_PER_SEC << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::vector<Triangle> triangles;
    readData(&triangles);
    solve(triangles);

    return 0;
}
