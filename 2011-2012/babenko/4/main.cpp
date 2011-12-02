#include <set>
#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include <cassert>
#include <stdexcept>

void require(bool cond, const std::string& message)
{
    if (!cond) {
        throw std::runtime_error(message);
    }
}

const size_t UNDEFINED_INDEX = std::numeric_limits<size_t>::max();

template <class Heap>
class Element
{
public:
    Element() : Element(0) { }

    explicit Element(int key, Heap* heap = NULL,
                     size_t index = UNDEFINED_INDEX) :
        key_(key), heap_(heap), index_(index)
    { }

    int key() const { return key_; }

    int key() { return key_; }

    size_t index() const { return index_; }

    size_t index() { return index_; }

    void setIndex(size_t index) { index_ = index; }

    Heap* heap() const { return heap_; }

    Heap* heap() { return heap_; }

    void setHeap(Heap* heap) { heap_ = heap; }

private:
    int key_;
    Heap* heap_;
    size_t index_;
};

template <class Element>
class HeapElementComparator
{
    public:
    virtual bool operator()(const Element* first,
                            const Element* second) const
    {
        throw std::runtime_error("Operation is not supported");
    }

    virtual ~HeapElementComparator() { }
};

// This class does not own elements
template <template <class> class Element, template <class> class Comparator>
class BinaryHeap
{
public:
    typedef Element<BinaryHeap> HeapElement;
    typedef Comparator<HeapElement> HeapElementComparator;

    explicit BinaryHeap(const Comparator<HeapElement>& comparator)
        : comparator_(comparator) { }

    void push(HeapElement* element)
    {
        element->setHeap(this);
        element->setIndex(heap_.size());
        heap_.push_back(element);
        siftUp(heap_.size() - 1);
    }

    HeapElement* pop()
    {
        require(!heap_.empty(), "Cannot get min element from empty heap");

        HeapElement* result = *heap_.begin();
        swap(*heap_.begin(), heap_.back());
        heap_.pop_back();

        siftDown(0);

        result->setIndex(UNDEFINED_INDEX);
        result->setHeap(NULL);

        return result;
    }

    void remove(size_t index)
    {
        require(index < heap_.size(), "Index is out of range");

        swap(heap_[index], heap_.back());

        HeapElement* result = heap_.back();
        heap_.pop_back();

        siftDown(index);
        if (index < heap_.size()) {
            siftUp(index);
        }
    }

    const HeapElement& top() const
    {
        require(!heap_.empty(), "Cannot get min element from empty heap");
        return **heap_.begin();
    }

    bool empty() const { return heap_.size() == 0; }

    size_t size() const { return heap_.size(); }

    size_t size() { return heap_.size(); }

private:
    void siftUp(size_t position)
    {
        require(position < heap_.size(), "position is out of range");

        while (position > 0 &&
               comparator_(heap_[parent(position)], heap_[position])) {

            HeapElement*& current = heap_[position];
            HeapElement*& next = heap_[parent(position)];

            swap(current, next);

            position = parent(position);
        }
    }

    void siftDown(size_t position)
    {
        bool canBeSifted = true;
        while (position < heap_.size() &&
               leftChild(position) < heap_.size() &&
               canBeSifted) {

            size_t nextPosition = leftChild(position);

            if (nextPosition + 1 < heap_.size() &&
                comparator_(heap_[nextPosition],
                            heap_[nextPosition + 1])) {
                ++nextPosition;
            }

            HeapElement*& current = heap_[position];
            HeapElement*& greatest = heap_[nextPosition];

            if (comparator_(current, greatest)) {
                swap(current, greatest);
            }
            else {
                canBeSifted = false;
            }
            position = nextPosition;
        }
    }

    void swap(HeapElement*& first, HeapElement*& second) const
    {
        size_t index = first->index();
        first->setIndex(second->index());
        second->setIndex(index);

        std::swap(first, second);
    }

    size_t parent(size_t pos) const { return (pos - 1) >> 1; }

    size_t leftChild(size_t pos) const { return (pos << 1) + 1; }

    size_t rightChild(size_t pos) const { return (pos + 1) << 1; }

    typename std::vector<HeapElement*> heap_;
    const Comparator<HeapElement>& comparator_;
};

typedef BinaryHeap<Element, HeapElementComparator> CommonHeap;
typedef CommonHeap::HeapElement HeapElement;

class HeapElementLess : public CommonHeap::HeapElementComparator
{
    public:
    virtual bool operator()(const HeapElement* first,
                            const HeapElement* second) const
    {
        return first->key() < second->key();
    }
};

class HeapElementGreater : public CommonHeap::HeapElementComparator
{
    public:
    virtual bool operator()(const HeapElement* first,
                            const HeapElement* second) const
    {
        return first->key() > second->key();
    }
};

typedef std::vector<HeapElement> HeapElements;

void processInputData(size_t* kthOrderStatistic,
                      std::string* operations,
                      HeapElements* elements)
{
    size_t numElements, numOperations;
    std::cin >> numElements >> numOperations >> *kthOrderStatistic;

    elements->reserve(numElements);

    for (size_t index = 0; index < numElements; ++index) {
        int value;
        std::cin >> value;
        elements->push_back(HeapElement(value));
    }

    std::cin >> *operations;
    operations->resize(numOperations);
}

class Solver
{
public:
    Solver(const HeapElements& elements, size_t kthOrderStatistic)
        : elements_(elements), kthOrderStatistic_(kthOrderStatistic),
          begin_(0), end_(0), kHeap_(less_), overflowHeap_(greater_) { }

    void pop()
    {
        require(begin_ + 1 < end_, "begin cannot pass ahead of end pointer");
        HeapElement& element = elements_[begin_++];
        CommonHeap* heap = element.heap();
        heap->remove(element.index());
        if (kHeap_.size() < kthOrderStatistic_ &&
                !overflowHeap_.empty()) {
            transfer();
        }
    }

    void push()
    {
        require(end_ < elements_.size(), "end pointer is out of range");
        overflowHeap_.push(&elements_[end_++]);
        if (kHeap_.size() < kthOrderStatistic_ ||
                // quite dangerous piece of code, as it makes additional
                // cohesion in the program; why not to use comparator?
                kHeap_.top().key() > overflowHeap_.top().key()) {
            transfer();
        }
    }

    int answer() const
    {
        if (kthOrderStatistic_ == kHeap_.size()) {
            return kHeap_.top().key();
        }
        return -1;
    }

private:
    void transfer()
    {
        kHeap_.push(overflowHeap_.pop());
        if (kHeap_.size() > kthOrderStatistic_) {
            overflowHeap_.push(kHeap_.pop());
        }
    }

    HeapElements elements_;
    size_t kthOrderStatistic_;
    size_t begin_;
    size_t end_;
    HeapElementLess less_;
    HeapElementGreater greater_;
    CommonHeap kHeap_;
    CommonHeap overflowHeap_;
};

void processSolution(size_t kthOrderStatistic,
                     const std::string& operations,
                     const HeapElements& elements)
{
    Solver solver(elements, kthOrderStatistic);
    solver.push();

    for (size_t operationIndex = 0; operationIndex < operations.size();
                ++operationIndex) {
        if (operations[operationIndex] == 'L') {
            solver.pop();
        }
        else if (operations[operationIndex] == 'R') {
            solver.push();
        }
        else {
            throw std::runtime_error("Unknown operation");
        }

        std::cout << solver.answer() << std::endl;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    std::string operations;
    size_t kthOrderStatistic;
    std::vector<HeapElement> elements;
    processInputData(&kthOrderStatistic, &operations, &elements);

    processSolution(kthOrderStatistic, operations, elements);

    return 0;
}