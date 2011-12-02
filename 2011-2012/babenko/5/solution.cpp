#include <iostream>
#include <vector>
#include <limits>

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

void readData(size_t* parkingSize, std::vector<int>* operations)
{
    size_t operationsNumber;
    std::cin >> *parkingSize >> operationsNumber;
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

void solve(size_t parkingSize, const std::vector<int>& operations)
{
    FenwickTree tree(2 * parkingSize);
    for (size_t index = 0; index < operations.size(); ++index) {
        int operation = operations[index];
        // someone arrived
        if (operation > 0) {
            int position = operation - 1;
            size_t place = findFirstEmpty(tree, position,
                                       position + parkingSize);
            if (place != UNDEFINED_INDEX) {
                tree.update(place, 1);
                if (place < parkingSize) {
                    tree.update(place + parkingSize, 1);
                }
                else {
                    tree.update(place - parkingSize, 1);
                }
                std::cout << place % parkingSize + 1 << std::endl;
            }
            else {
                std::cout << -1 << std::endl;
            }
        }
        // someone leaves
        else {
            int position = -operation - 1;
            if (tree(position, position + 1) > 0) {
                tree.update(position, -1);
                tree.update(position + parkingSize, -1);
                std::cout << 0 << std::endl;
            }
            else {
                std::cout << -2 << std::endl;
            }
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    size_t parkingSize;
    std::vector<int> operations;
    readData(&parkingSize, &operations);
    solve(parkingSize, operations);
    return 0;
}
