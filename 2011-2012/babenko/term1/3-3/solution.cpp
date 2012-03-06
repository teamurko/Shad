#include <string>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

typedef size_t Id;
typedef std::vector<Id> Ids;
typedef std::vector<Ids> Graph;

class DisjointSetUnion
{
    public:
    explicit DisjointSetUnion(size_t size) : parent_(size)
    {
        init();
    }

    void init()
    {
        for (size_t i = 0; i < parent_.size(); ++i) {
            parent_[i] = i;
        }
    }

    bool merge(size_t first, size_t second)
    {
        first = up(first);
        second = up(second);
        if (first == second) {
            return false;
        }
        if (rand() & 1) {
            parent_[first] = second;
        }
        else {
            parent_[second] = first;
        }
        return true;
    }

    size_t up(size_t id)
    {
        if (parent_[id] != id) {
            parent_[id] = up(parent_[id]);
        }
        return parent_[id];
    }
    private:
    std::vector<size_t> parent_;
};

class PerfectHashFactory
{
    private:
    typedef std::vector<int> HashGenVector;

    public:
    PerfectHashFactory() { }

    void initialize(const std::vector<int>& numbers, const size_t factor = 3)
    {
        size_t size = numbers.size() * factor;
        pre11_.resize(1 << 16);
        pre12_.resize(1 << 16);
        pre21_.resize(1 << 16);
        pre22_.resize(1 << 16);
        buildPerfectHash(numbers, size);
    }

    bool buildPerfectHash(const std::vector<int>& numbers, size_t size)
    {
        size_ = size;
        size_ = nextPrime(size_);
        graph_.resize(size_);
        visited_.resize(size_);
        vertexLabels_.resize(size_);
        numEdges_ = numbers.size();

        static const Id UNDEFINED = std::numeric_limits<Id>::max();

        if (!buildGraph(numbers)) {
            return false;
        }
        size_t uniqueHashValue = 0;
        for (size_t vertexIndex = 0; vertexIndex < size_; ++vertexIndex) {
            if (!visited_[vertexIndex]) {
                vertexLabels_[vertexIndex] = 0;
                traverse(vertexIndex, UNDEFINED, &uniqueHashValue);
            }
        }
        clearGraph();
        return true;
    }

    size_t hash(int number) const
    {
        size_t hf = (vertexLabels_[hash(h1_, pre11_, pre12_, number)]
                + vertexLabels_[hash(h2_, pre21_, pre22_, number)]);
        if (hf >= numEdges_) {
            hf -= numEdges_;
        }
        return hf;
    }

    private:
    bool isPrime(size_t number) const
    {
        for (size_t i = 2; i * i <= number; ++i) {
            if (number % i == 0) {
                return false;
            }
        }
        return number > 1;
    }

    size_t nextPrime(size_t startNumber) const
    {
        while (!isPrime(startNumber)) {
            ++startNumber;
        }
        return startNumber;
    }

    void precalc(const HashGenVector& hash,
                HashGenVector& preOne,
                HashGenVector& preTwo)
    {
        for (size_t mask = 0; mask < (1 << 16); ++mask) {
            preOne[mask] = 0;
            preTwo[mask] = 0;
        }
        for (int i = 0; i < 16; ++i) {
            for (int mask = 0; mask < (1 << 16); ++mask) {
                if (mask & (1 << i)) {
                    preOne[mask] ^= hash[i];
                }
            }
        }
        /*
        for (int mask = 0; mask < (1 << 16); ++mask) {
            int res = 0;
            for (int i = 0; i < 16; ++i) {
                if (mask & (1 << i)) {
                    res ^= hash[i];
                }
            }
            assert(res == pre1[mask]);
        }
        */
        for (int i = 0; i < 16; ++i) {
            for (int mask = 0; mask < (1 << 16); ++mask) {
                if (mask & (1 << i)) {
                    preTwo[mask] ^= hash[i + 16];
                }
            }
        }
        /*
        for (int mask = 0; mask < (1 << 16); ++mask) {
            int res = 0;
            for (int i = 0; i < 16; ++i) {
                if (mask & (1 << i)) {
                    res ^= hash[i + 16];
                }
            }
            assert(res == pre2[mask]);
        }
        */
    }

    // returns false iff loop is generated
    bool buildGraph(const std::vector<int>& numbers)
    {
        bool foundPerfectHash = false;
        DisjointSetUnion dsu(size_);
        static const size_t NUM_ITERATIONS = 30000000;
        size_t iter = 0;
        do {
            dsu.init();
            h1_ = createHashGenPair();
            precalc(h1_, pre11_, pre12_);
            h2_ = createHashGenPair();
            precalc(h2_, pre21_, pre22_);
            bool foundCycle = false;
            for (size_t index = 0; index < numbers.size(); ++index) {
                Id vertexU = hash(h1_, pre11_, pre12_, numbers[index]);
                Id vertexV = hash(h2_, pre21_, pre22_, numbers[index]);
                if (!dsu.merge(vertexU, vertexV)) {
                    foundCycle = true;
                    break;
                }
            }
            foundPerfectHash = !foundCycle;
            ++iter;
        }
        while (iter < NUM_ITERATIONS && !foundPerfectHash);

        if (!foundPerfectHash) {
            return false;
        }

        for (size_t index = 0; index < numbers.size(); ++index) {
            Id vertexU = hash(h1_, pre11_, pre12_, numbers[index]);
            Id vertexV = hash(h2_, pre21_, pre22_, numbers[index]);
            graph_[vertexU].push_back(vertexV);
            graph_[vertexV].push_back(vertexU);
        }
        return true;
    }

    // traverses graph, fills vertexLabels_,
    // returns false iff multiedges found
    void traverse(Id vertex, Id ancestor, size_t* uniqueHashValue)
    {
        visited_[vertex] = true;
        for (size_t index = 0; index < graph_[vertex].size(); ++index) {
            Id neibVertex = graph_[vertex][index];
            if (neibVertex != ancestor) {
                vertexLabels_[neibVertex] =
                            (numEdges_ + *uniqueHashValue -
                             vertexLabels_[vertex]) % numEdges_;
                ++(*uniqueHashValue);
                traverse(neibVertex, vertex, uniqueHashValue);
            }
        }
    }

    void clearGraph()
    {
        graph_.clear();
        visited_.clear();
    }

    size_t hash(const HashGenVector& hashGenVector,
            const HashGenVector& preOne,
            const HashGenVector& preTwo, int number) const
    {
        static const int LOW = (1 << 16) - 1;
        int result = 0;
        result ^= preOne[number & LOW];
        result ^= preTwo[(number >> 16) & LOW];
        result += hashGenVector.back();
        if (result >= size_) {
            result %= size_;
        }
        return result;
    }

    HashGenVector createHashGenPair() const
    {
        HashGenVector result(33);
        for (size_t i = 0; i < 33; ++i) {
            result[i] = random();
        }
        return result;
    }

    int random() const
    {
        return (rand() << 16) + rand();
    }

    private:
    size_t size_;
    Graph graph_;
    HashGenVector h1_;
    HashGenVector h2_;
    std::vector<int> pre11_;
    std::vector<int> pre21_;
    std::vector<int> pre12_;
    std::vector<int> pre22_;
    std::vector<bool> visited_;
    Ids vertexLabels_;
    size_t numEdges_;
};

class PersistentSet
{
    public:
    PersistentSet() { }

    void initialize(const std::vector<int>& numbers)
    {
        factory_.initialize(numbers, 10);
        numbers_.resize(numbers.size());
        for (size_t index = 0; index < numbers_.size(); ++index) {
            numbers_[factory_.hash(numbers[index])] = numbers[index];
        }
    }

    bool contains(int number) const
    {
        size_t index = factory_.hash(number);
        return numbers_[index] == number;
    }

    private:
    size_t size_;
    PerfectHashFactory factory_;
    std::vector<int> numbers_;
};

int main()
{
    std::ios_base::sync_with_stdio(false);
    size_t numNumbers;
    std::cin >> numNumbers;
    std::vector<int> numbers(numNumbers);
    for (size_t i = 0; i < numNumbers; ++i) {
        // scanf("%d", &numbers[i]);
        std::cin >> numbers[i];
    }
    PersistentSet perSet;
    perSet.initialize(numbers);
    int buildTime = static_cast<double>(clock()) / CLOCKS_PER_SEC;
    assert(buildTime < 1.0);

    size_t numQueries;
    std::cin >> numQueries;
    std::string ans;
    for (size_t i = 0; i < numQueries; ++i) {
        int number;
        std::cin >> number;
        if (perSet.contains(number)) {
            ans += "Yes\n";
        }
        else {
            ans += "No\n";
        }
    }
    puts(ans.c_str());
    std::cerr << static_cast<double>(clock()) / CLOCKS_PER_SEC << std::endl;
}
