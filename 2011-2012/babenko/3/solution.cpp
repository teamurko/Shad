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
    Triangle(size_t a, size_t b, size_t c) : a_(a), b_(b), c_(c)
    {
        REQUIRE(a > 0 && b > 0 && c > 0, "Triangle sides should be positive");
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
        size_t d = gcd(a_, gcd(b_, c_));
        a_ = a_ / d;
        b_ = b_ / d;
        c_ = c_ / d;
    }

    size_t gcd(size_t a, size_t b)
    {
        if (a == 0) {
            return b;
        }
        return gcd(b % a, a);
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
    HashTable(size_t size) : size_(size), table_(size_)
    { }

    bool add(const Object& obj)
    {
        size_t index = obj.hash() % size_;
        return table_[index].insert(obj).second;
    }

    //TODO return iterator

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
    HashTable<Triangle> htable(2324347);
    size_t numUnique = 0;
    for (size_t index = 0; index < numTriangles; ++index) {
        size_t a, b, c;
        std::cin >> a >> b >> c;
        if (htable.add(Triangle(a, b, c))) {
            numUnique++;
        }
    }
    std::cout << numUnique << std::endl;
}
