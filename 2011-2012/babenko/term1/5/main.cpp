#include <iostream>
#include <vector>
#include <limits>

enum ParkingEvent {
    EMPTY_PLACE = -2, FULL_PARKING = -1, PLACE_FREED = 0, PLACE_OCCUPIED = 1};

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

struct OperationResult
{
    ParkingEvent event;
    size_t carOccupiedPlace;
};

OperationResult
processArrived(int operation, size_t parkingSize, FenwickTree* tree)
{
    int position = operation - 1;
    size_t place = findFirstEmpty(*tree, position,
                               position + parkingSize);
    OperationResult result;
    if (place != UNDEFINED_INDEX) {
        tree->update(place, 1);
        if (place < parkingSize) {
            tree->update(place + parkingSize, 1);
        }
        else {
            tree->update(place - parkingSize, 1);
        }
        result.event = PLACE_OCCUPIED;
        result.carOccupiedPlace = place % parkingSize;
    }
    else {
        result.event = FULL_PARKING;
    }
    return result;
}

OperationResult
processLeaving(int operation, size_t parkingSize, FenwickTree* tree)
{
    int position = -operation - 1;
    OperationResult result;
    if ((*tree)(position, position + 1) > 0) {
        tree->update(position, -1);
        tree->update(position + parkingSize, -1);
        result.event = PLACE_FREED;
    }
    else {
        result.event = EMPTY_PLACE;
    }
    return result;
}

void solve(size_t parkingSize, const std::vector<int>& operations,
           std::vector<OperationResult>* result)
{
    FenwickTree tree(2 * parkingSize);
    for (size_t index = 0; index < operations.size(); ++index) {
        int operation = operations[index];
        if (operation > 0) {
            result->push_back(
                        processArrived(operation, parkingSize, &tree));
        }
        else {
            result->push_back(
                        processLeaving(operation, parkingSize, &tree));
        }
    }
}

void outputData(const std::vector<OperationResult>& responses)
{
    for (size_t index = 0; index < responses.size(); ++index) {
        if (responses[index].event == PLACE_OCCUPIED) {
            std::cout << responses[index].carOccupiedPlace + 1 << std::endl;
        }
        else {
            std::cout << responses[index].event << std::endl;
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    size_t parkingSize;
    std::vector<int> operations;
    readData(parkingSize, &operations);
    std::vector<OperationResult> queryResponses;
    solve(parkingSize, operations, &queryResponses);
    outputData(queryResponses);
    return 0;
}
