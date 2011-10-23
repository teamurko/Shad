#include <iostream>
#include <set>

size_t gcd(size_t a, size_t b)
{
    if (a == 0) return b;
    return gcd(b % a, a);
}

size_t normalize(size_t a, size_t b, size_t c)
{
    if (a > b) std::swap(a, b);
    if (b > c) std::swap(b, c);
    if (a > b) std::swap(a, b);
    size_t d = gcd(a, gcd(b, c));
    a /= d;
    b /= d;
    c /= d;
    --a, --b, --c;
    return a * 1000 * 1000 + b * 1000 + c;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    size_t numTriangles;
    std::cin >> numTriangles;
    std::set<size_t> ids;
    for(size_t i = 0; i < numTriangles; ++i) {
        size_t a, b, c;
        std::cin >> a >> b >> c;
        ids.insert(normalize(a, b, c));
    }
    std::cout << ids.size() << std::endl;
}
