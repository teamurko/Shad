#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>
#include <cmath>
#include <set>

#define REQUIRE(cond, message) \
                if (!cond) { \
                    std::cerr << message << std::endl; \
                    exit(1); \
                }

class Triangle
{
    public:
    Triangle(size_t lenA, size_t lenB, size_t lenC)
        : a_(lenA), b_(lenB), c_(lenC)
    {
        REQUIRE(a_ > 0 && b_ > 0 && c_ > 0,
                "Triangle sides should be positive");
        if (a_ > b_) {
            std::swap(a_, b_);
        }
        if (b_ > c_) {
            std::swap(b_, c_);
        }
        if (a_ > b_) {
            std::swap(a_, b_);
        }
        normalize();
        id_ = (c_ - 1) + ((b_ - 1) + (a_ - 1) * BASE) * BASE;
    }

    size_t simpleHashModuloP(size_t dividend) const
    {
        REQUIRE(dividend > 0, "Zero modulo");
        return id_ % dividend;
    }

    size_t id() const {
        return id_;
    }

    size_t hash() const {
        return id_;
    }

    private:
    void normalize()
    {
        size_t gd = gcd(a_, gcd(b_, c_));
        a_ = a_ / gd;
        b_ = b_ / gd;
        c_ = c_ / gd;
    }

    size_t gcd(size_t first, size_t second)
    {
        if (first == 0) {
            return second;
        }
        return gcd(second % first, first);
    }

    private:
    static size_t BASE;
    size_t a_;
    size_t b_;
    size_t c_;
    size_t id_;
};

size_t Triangle::BASE = 1000;

template <class Object>
class HashTable
{
    public:
    explicit HashTable(size_t size) : size_(size), table_(size_)
    {
        REQUIRE(size > 0, "Cannot instantiate empty hash table");
    }

    bool add(const Object& obj)
    {
        size_t index = obj.hash() % size_;
        return table_[index].insert(obj).second;
    }

    // TODO return iterator

    private:
    struct Comparator {
        bool operator()(const Object& first, const Object& second) const
        {
            return first.id() < second.id();
        }
    };
    typedef std::set<Object, Comparator> Objects;
    size_t size_;
    std::vector<Objects> table_;
};

int main()
{
    std::ios_base::sync_with_stdio(false);
    size_t numTriangles;
    std::cin >> numTriangles;
    HashTable<Triangle> htable(2324447); // prime
    size_t numUnique = 0;
    for (size_t index = 0; index < numTriangles; ++index) {
        size_t lenA, lenB, lenC;
        std::cin >> lenA >> lenB >> lenC;
        if (htable.add(Triangle(lenA, lenB, lenC))) {
            numUnique++;
        }
    }
    std::cout << numUnique << std::endl;
    std::cerr << clock() / CLOCKS_PER_SEC << std::endl;
}
