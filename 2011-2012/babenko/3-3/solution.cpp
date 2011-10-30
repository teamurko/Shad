#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

typedef size_t Id;
typedef std::vector<Id> Ids;
typedef std::vector<std::vector<size_t> > RandomUITable;
typedef std::vector<Ids> Graph;

class DisjointSetUnion
{
public:
    DisjointSetUnion(size_t size) : parent_(size)
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
    public:
    PerfectHashFactory() { }

    void initialize(const std::vector<size_t>& numbers, size_t factor = 3)
    {
        numbers_ = numbers;
        size_ = numbers.size() * factor;
        graph_.resize(size_);
        visited_.resize(size_);
        vertexLabels_.resize(size_);
        numEdges_ = numbers_.size();

        static const Id UNDEFINED = std::numeric_limits<Id>::max();

        buildGraph();
        size_t uniqueHashValue = 0;
        for (size_t vertexIndex = 0; vertexIndex < size_; ++vertexIndex) {
            if (!visited_[vertexIndex]) {
                vertexLabels_[vertexIndex] = 0;
                traverse(vertexIndex, UNDEFINED, &uniqueHashValue);
            }
        }
        clearGraph();
        // found hashing tables h1_, h2_, and vertexLabels_
    }

    size_t hash(size_t number) const
    {
        size_t hf = (vertexLabels_[hash(h1_, number)]
                + vertexLabels_[hash(h2_, number)]);
        if (hf >= numbers_.size()) {
            hf -= numbers_.size();
        }
        return hf;
    }

    private:
    // returns false iff loop is generated
    bool buildGraph()
    {
        bool foundPerfectHash = false;
        DisjointSetUnion dsu(size_);
        do {
            dsu.init();
            h1_ = generateRandomUITable();
            h2_ = generateRandomUITable();
            bool foundCycle = false;
            for (size_t index = 0; index < numbers_.size(); ++index) {
                Id vertexU = hash(h1_, numbers_[index]);
                Id vertexV = hash(h2_, numbers_[index]);
                if (!dsu.merge(vertexU, vertexV)) {
                    foundCycle = true;
                    break;
                }
            }
            foundPerfectHash = !foundCycle;
        }
        while (!foundPerfectHash);

        for (size_t index = 0; index < numbers_.size(); ++index) {
            Id vertexU = hash(h1_, numbers_[index]);
            Id vertexV = hash(h2_, numbers_[index]);
            graph_[vertexU].push_back(vertexV);
            graph_[vertexV].push_back(vertexU);
        }
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

    size_t hash(const RandomUITable& hashingFunction, size_t number) const
    {
        size_t result = 0;
        for (size_t posIndex = 0; posIndex < NUM_POSITIONS; ++posIndex) {
            size_t intPart = number / BASE;
            result += hashingFunction[posIndex][number - intPart * BASE];
            number = intPart;
        }
        return result % size_;
    }

    RandomUITable generateRandomUITable()
    {
        RandomUITable table(NUM_POSITIONS, std::vector<size_t>(BASE));
        size_t p = rand() * rand() % 300 + 123;
        for (size_t i = 0; i < BASE; ++i) {
            table[0][i] = i;
            if (table[0][i] >= size_) {
                table[0][i] %= size_;
            }
        }
        for (size_t posIndex = 1; posIndex < NUM_POSITIONS; ++posIndex) {
            for (size_t digitIndex = 0; digitIndex < BASE; ++digitIndex) {
                table[posIndex][digitIndex] =
                    table[posIndex - 1][digitIndex] * p;
                if (table[posIndex][digitIndex] >= size_) {
                    table[posIndex][digitIndex] %= size_;
                }
            }
        }
        return table;
    }

    private:
    static size_t NUM_POSITIONS;
    static size_t BASE;
    size_t size_;
    std::vector<size_t> numbers_;
    Graph graph_;
    RandomUITable h1_;
    RandomUITable h2_;
    std::vector<bool> visited_;
    Ids vertexLabels_;
    size_t numEdges_;
};

size_t PerfectHashFactory::NUM_POSITIONS = 10;
size_t PerfectHashFactory::BASE = 10;

class PersistentSet
{
    public:
    PersistentSet() { }

    void initialize(const std::vector<int>& numbers)
    {
        std::vector<size_t> unsignedNumbers(numbers.size());
        for (size_t index = 0; index < numbers.size(); ++index) {
            unsignedNumbers[index] = static_cast<size_t>(
                                        numbers[index] -
                                        std::numeric_limits<int>::min());
        }
        factory_.initialize(unsignedNumbers);
        mixedNumbers_.resize(unsignedNumbers.size());
        for (size_t index = 0; index < mixedNumbers_.size(); ++index) {
            mixedNumbers_[factory_.hash(unsignedNumbers[index])] =
                                                unsignedNumbers[index];
        }
    }

    bool contains(int number) const
    {
        size_t nonNegative = number - std::numeric_limits<int>::min();
        size_t index = factory_.hash(nonNegative);
        // subtract minimal possible in value to obtain nonnegative value
        // to fit size_t type
        return mixedNumbers_[index] == nonNegative;
    }

    private:

    size_t size_;
    PerfectHashFactory factory_;
    std::vector<size_t> mixedNumbers_;
};

int main()
{
    std::ios_base::sync_with_stdio(false);
    size_t numNumbers;
    std::cin >> numNumbers;
    std::vector<int> numbers(numNumbers);
    for (size_t i = 0; i < numNumbers; ++i) {
        std::cin >> numbers[i];
    }
    PersistentSet perSet;
    perSet.initialize(numbers);
    std::cerr << static_cast<double>(clock()) / CLOCKS_PER_SEC << std::endl;

    size_t numQueries;
    std::cin >> numQueries;
    for (size_t i = 0; i < numQueries; ++i) {
        int number;
        std::cin >> number;
        if (perSet.contains(number)) {
            std::cout << "Yes" << std::endl;
        }
        else {
            std::cout << "No" << std::endl;
        }
    }
    std::cerr << static_cast<double>(clock()) / CLOCKS_PER_SEC << std::endl;
}
