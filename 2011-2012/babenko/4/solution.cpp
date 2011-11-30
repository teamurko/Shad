#include <iostream>
#include <vector>
#include <limits>
#include <string>
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
            Element<Heap>* source = NULL,
            size_t index = UNDEFINED_INDEX) :
        key_(key), heap_(heap), source_(source), index_(index)
    { }

    template <class SimilarElement>
    Element(const SimilarElement& element)
    {
        key_ = element.key();
        heap_ = reinterpret_cast<Heap*>(element.heap());
        source_ = reinterpret_cast<Element<Heap>*>(element.source());
        index_ = element.index();
    }

    int key() const { return key_; }

    int key() { return key_; }

    size_t index() const { return index_; }

    size_t index() { return index_; }

    void setIndex(size_t newIndex) { index_ = newIndex; }

    Heap* heap() const { return heap_; }

    Heap* heap() { return heap_; }

    Element<Heap>* source() const { return source_; }

    Element<Heap>* source() { return source_; }

    void setHeap(Heap* heap) { heap_ = heap; }

    void setSource(Element<Heap>* source) { source_ = source; }

    private:
    int key_;
    Heap* heap_;
    Element<Heap>* source_;
    size_t index_;
};

template <class Element>
class HeapElementComparator
{
    public:
    virtual bool operator()(const Element& first,
                            const Element& second) const
    {
        throw std::runtime_error("Operation is not supported");
    }

    ~HeapElementComparator() { }
};

template <class Element>
class HeapElementLess : public HeapElementComparator<Element>
{
    public:
    virtual bool operator()(const Element& first,
                            const Element& second) const
    {
        return first.key() < second.key();
    }
};

template <class Element>
class HeapElementGreater : public HeapElementComparator<Element>
{
    public:
    virtual bool operator()(const Element& first,
                            const Element& second) const
    {
        return first.key() > second.key();
    }
};

// This class does not own elements
template <template <class> class Element, template <class> class Comparator>
class BinaryHeap
{
    public:

    typedef Element<BinaryHeap> HeapElement;

    void push(HeapElement element)
    {
        element.setHeap(this);
        element.setIndex(heap_.size());
        heap_.push_back(element);
        siftUp(heap_.size() - 1);
    }

    HeapElement pop()
    {
        require(!heap_.empty(), "Cannot get min element from empty heap");

        HeapElement result = *heap_.begin();
        swap(*heap_.begin(), heap_.back());
        heap_.pop_back();

        siftDown(0);

        result.setIndex(UNDEFINED_INDEX);
        result.setHeap(NULL);

        return result;
    }

    HeapElement remove(size_t index)
    {
        require(index < heap_.size(), "Index is out of range");

        swap(heap_[index], heap_.back());

        HeapElement result = heap_.back();
        heap_.pop_back();

        siftDown(index);

        return result;
    }

    const HeapElement& top() const
    {
        require(!heap_.empty(), "Cannot get min element from empty heap");
        return *heap_.begin();
    }

    bool empty() const { return heap_.size() == 0; }

    size_t size() const { return heap_.size(); }

    size_t size() { return heap_.size(); }

    private:

    void siftUp(size_t position)
    {
        while (position > 0 &&
               comparator_(heap_[parent(position)], heap_[position])) {

            HeapElement& current = heap_[position];
            HeapElement& next = heap_[parent(position)];

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
            HeapElement& greatest = heap_[nextPosition];


            if (rightChild(position) < heap_.size() &&
                comparator_(greatest, heap_[rightChild(position)])) {
                nextPosition = rightChild(position);
                greatest = heap_[nextPosition];
            }

            HeapElement& current = heap_[position];
            if (comparator_(current, greatest)) {
                swap(current, greatest);
            }
            else {
                canBeSifted = false;
            }
            position = nextPosition;
        }
    }

    void swap(HeapElement& first, HeapElement& second) const
    {
        size_t index = first.index();
        first.setIndex(second.index());
        second.setIndex(index);

        std::swap(first, second);
    }

    size_t parent(size_t pos) const { return (pos - 1) >> 1; }

    size_t leftChild(size_t pos) const { return (pos << 1) + 1; }

    size_t rightChild(size_t pos) const { return (pos + 1) << 1; }

    typename std::vector<HeapElement> heap_;
    Comparator<HeapElement> comparator_;
};

typedef BinaryHeap<Element, HeapElementComparator> CommonHeap;
typedef CommonHeap::HeapElement HeapElement;
typedef Element<CommonHeap> CommonElement;

void processInputData(size_t* kthOrderStatistic,
                      std::string* operations,
                      std::vector<HeapElement>* elements)
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
}

void processSolution(size_t kthOrderStatistic,
                     const std::string& operations,
                     std::vector<HeapElement> elements)
{
    typedef BinaryHeap<Element, HeapElementLess> HeapLess;
    typedef BinaryHeap<Element, HeapElementGreater> HeapGreater;

    // initialize sources to update elements data
    for (size_t index = 0; index < elements.size(); ++index) {
        elements[index].setSource(&elements[index]);
    }

    HeapLess kHeap;
    HeapGreater overflowHeap;

    // initial positions of pointers, the right one is exclusive
    size_t pointerL = 0;
    size_t pointerR = pointerL + 1;

    // push the first element to kHeap, as kthOrderStatistic > 0
    kHeap.push(static_cast<HeapLess::HeapElement>(*elements.begin()));

    for (size_t operationIndex = 0; operationIndex < operations.size();
                ++operationIndex) {
        bool needTransfer = false;
        if (operations[operationIndex] == 'L') {

            HeapElement element = elements[pointerL++];
            CommonHeap* heap = element.heap();
            heap->remove(element.index());
            if (kHeap.size() < kthOrderStatistic &&
                !overflowHeap.empty()) {
                needTransfer = true;
            }

        }
        else if (operations[operationIndex] == 'R') {

            overflowHeap.push(static_cast<HeapGreater::HeapElement>(
                                                    elements[pointerR++]));

            if (kHeap.size() < kthOrderStatistic ||
                    // quite dangerous piece of code, as it makes additional
                    // cohesion of in the program; why not to use comparator?
                    kHeap.top().key() > overflowHeap.top().key()) {
                needTransfer = true;
            }
        }
        else {
            throw std::runtime_error("Unknown operation");
        }
        if (needTransfer) {
            HeapElement greatest = kHeap.pop();
            kHeap.push(overflowHeap.pop());
            overflowHeap.push(greatest);
        }

        // output kth order statistic if it exists
        if (kHeap.size() == kthOrderStatistic) {
            std::cout << kHeap.top().key() << std::endl;
        }
        else {
            std::cout << -1 << std::endl;
        }
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

}
