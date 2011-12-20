#include <iostream>
#include <vector>
#include <limits>

enum {EMPTY_PLACE = -2, FULL_PARKING, PLACE_FREED};

const size_t UNDEFINED_INDEX = std::numeric_limits<size_t>::max();

class FenwickTree
{
public:
    explicit FenwickTree(size_t size) : tree_(size) { }

    int operator()(size_t beginIndex, size_t endIndex) const
    {
        int result = 0;
        if (endIndex > 0) result += sum(endIndex - 1);
        if (beginIndex > 0) result -= sum(beginIndex - 1);
        return result;
    }

    void update(size_t index, int addValue)
    {
        while (index < tree_.size()) {
            tree_[index] += addValue;
            index = next(index);
        }
    }

private:
    int sum(int index) const
    {
        int result = 0;
        while (index >= 0) {
            result += tree_[index];
            index = prev(index) - 1;
        }
        return result;
    }
    int prev(int index) const { return index & (index + 1); }

    int next(int index) const { return index | (index + 1); }

    std::vector<int> tree_;
};

void readData(size_t& parkingSize, std::vector<int>* operations)
{
    size_t operationsNumber;
    std::cin >> parkingSize >> operationsNumber;
    operations->resize(operationsNumber);
    for (size_t index = 0; index < operations->size(); ++index) {
        char sign;
        std::cin >> sign;
        int number;
        std::cin >> number;
        if (sign == '-') {
            number = -number;
        }
        (*operations)[index] = number;
    }
}

size_t findFirstEmpty(const FenwickTree& tree, size_t begin, size_t end)
{
    while (begin + 1 < end) {
        size_t middle = (begin + end) / 2;
        if (tree(begin, middle) == middle - begin) {
            begin = middle;
        }
        else {
            end = middle;
        }
    }
    if (tree(begin, end) > 0) {
        return UNDEFINED_INDEX;
    }
    else {
        return begin;
    }
}

void processArrived(int operation, size_t parkingSize,
                    FenwickTree* tree, std::vector<int>* result)
{
    int position = operation - 1;
    size_t place = findFirstEmpty(*tree, position,
                               position + parkingSize);
    if (place != UNDEFINED_INDEX) {
        tree->update(place, 1);
        if (place < parkingSize) {
            tree->update(place + parkingSize, 1);
        }
        else {
            tree->update(place - parkingSize, 1);
        }
        result->push_back(place % parkingSize + 1);
    }
    else {
        result->push_back(FULL_PARKING);
    }
}

void processLeaving(int operation, size_t parkingSize,
                    FenwickTree* tree, std::vector<int>* result)
{
    int position = -operation - 1;
    if ((*tree)(position, position + 1) > 0) {
        tree->update(position, -1);
        tree->update(position + parkingSize, -1);
        result->push_back(PLACE_FREED);
    }
    else {
        result->push_back(EMPTY_PLACE);
    }
}

void solve(size_t parkingSize, const std::vector<int>& operations,
           std::vector<int>* result)
{
    FenwickTree tree(2 * parkingSize);
    for (size_t index = 0; index < operations.size(); ++index) {
        int operation = operations[index];
        if (operation > 0) {
            processArrived(operation, parkingSize, &tree, result);
        }
        else {
            processLeaving(operation, parkingSize, &tree, result);
        }
    }
}

void outputData(const std::vector<int>& responses)
{
    for (size_t index = 0; index < responses.size(); ++index) {
        std::cout << responses[index] << std::endl;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    size_t parkingSize;
    std::vector<int> operations;
    readData(parkingSize, &operations);
    std::vector<int> queryResponses;
    solve(parkingSize, operations, &queryResponses);
    outputData(queryResponses);
    return 0;
}
