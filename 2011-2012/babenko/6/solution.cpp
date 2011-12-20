#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <memory>

void require(bool cond, const std::string& message)
{
    if (!cond) {
        throw std::runtime_error(message);
    }
}

#define SIZE(node) ((node) == NULL ? 0 : (node)->size())
#define EMPTY_TREE NULL

template <class Key, class Value>
class Element
{
public:
    typedef Key KeyType;
    typedef Value ValueType;
    Element(Key key, Value value) : key_(key), value_(value) { }

    Key key() const { return key_; }

    Value value() const { return value_; }

private:
    Key key_;
    Value value_;
};

template <class Element>
class Node
{
public:
    typedef Node<Element> Type;
    typedef Type* NodePointer;

    explicit Node(const Element& element, NodePointer leftChild = EMPTY_TREE,
                  NodePointer rightChild = EMPTY_TREE,
                  NodePointer parent = EMPTY_TREE)
        : element_(element), leftChild_(leftChild),
        rightChild_(rightChild), parent_(parent), size_(1) { }

    const Element& element() const { return element_; }

    void setLeftChild(NodePointer node)
    {
        leftChild_ = node;
        updateSize();
    }

    NodePointer leftChild() const { return leftChild_; }

    void setRightChild(NodePointer node)
    {
        rightChild_ = node;
        updateSize();
    }

    NodePointer rightChild() const { return rightChild_; }

    size_t size() const { return size_; }

    void setSize(size_t size) { size_ = size; }

    bool isLeaf() const
    {
        return leftChild_ == EMPTY_TREE &&
               rightChild_ == EMPTY_TREE;
    }

    static NodePointer instance(const Element& element)
    {
        return new Node<Element>(element);
    }

private:
    void updateSize()
    {
        size_ = SIZE(leftChild_) + SIZE(rightChild_) + 1;
    }

    Element element_;
    NodePointer leftChild_;
    NodePointer rightChild_;
    NodePointer parent_;
    size_t size_;
};


template <class Element>
class CartesianTree;

// for conversion
template <class Element>
class CartesianTreeRef
{
public:
    typedef typename Node<Element>::NodePointer NodePointer;
    typedef typename Node<Element>::Type Node;
    typedef NodePointer Tree;
    typedef std::auto_ptr<Node> Root;

    explicit CartesianTreeRef(Root root)
    : tree_(root.release()) { }

    Tree get() const
    {
        return tree_;
    }

private:
    Tree tree_;
};

template <class Element>
class CartesianTree
{
public:
    typedef typename Node<Element>::NodePointer NodePointer;
    typedef typename Node<Element>::Type Node;
    typedef NodePointer Tree;
    typedef std::pair<Tree, Tree> TreesPair;
    typedef CartesianTree<Element> Type;
    typedef std::auto_ptr<Node> Root;

    explicit CartesianTree() { }

    explicit CartesianTree(const std::vector<Element>& elements)
    {
        build();
    }

    // it does own pointer like std::auto_ptr
    explicit CartesianTree(Tree tree) : root_(tree) { }

    explicit CartesianTree(Type& other) : root_(other.release()) { }

    CartesianTree(CartesianTreeRef<Element> ref)
    : root_(ref.get()) { }

    operator CartesianTreeRef<Element>()
    {
        return CartesianTreeRef<Element>(root_);
    }

    Tree release()
    {
        return root_.release();
    }

    ~CartesianTree()
    {
        if (root_.get() == EMPTY_TREE) {
            return;
        }
        std::queue<NodePointer> nodes;
        nodes.push(root_.release());
        while (!nodes.empty()) {
            NodePointer node = nodes.front();
            nodes.pop();
            if (node->leftChild() != EMPTY_TREE) {
                nodes.push(node->leftChild());
            }
            if (node->rightChild() != EMPTY_TREE) {
                nodes.push(node->rightChild());
            }
            delete node;
        }
    }

    void insert(size_t index, const Element& element)
    {
        TreesPair splitPair = split(root_.release(), index);
        Tree innerTree = Node::instance(element);
        root_.reset(merge(splitPair.first,
                    merge(innerTree, splitPair.second)));
    }

    void append(const Element& element)
    {
        insert(size(), element);
    }

    const Tree find(size_t index) const
    {
        return find(root_.get(), index);
    }

    size_t size() const { return SIZE(root_.get()); }

    Type split(size_t index)
    {
        TreesPair result = split(root_.release(), index);
        root_.reset(result.first);
        return Type(result.second);
    }

    void merge(Type tree)
    {
        root_.reset(merge(root_.release(), tree.release()));
    }

private:
    const Tree find(Tree tree, size_t index) const
    {
        require(tree != EMPTY_TREE, "Index is out of range");
        if (tree->isLeaf()) {
            return tree;
        }
        if (SIZE(tree->leftChild()) > index) {
            return find(tree->leftChild(), index);
        }
        else if (SIZE(tree->leftChild()) == index) {
            return tree;
        }
        else {
            return find(tree->rightChild(),
                        index - SIZE(tree->leftChild()) - 1);
        }
    }

    TreesPair split(Tree tree, size_t index) const
    {
        if (tree == EMPTY_TREE) {
            require(index == 0, "Index is out of range");
            return TreesPair(EMPTY_TREE, EMPTY_TREE);
        }
        if (SIZE(tree->leftChild()) + 1 <= index) {
            TreesPair splitPair = split(tree->rightChild(),
                                        index - SIZE(tree->leftChild()) - 1);
            tree->setRightChild(splitPair.first);
            return TreesPair(tree, splitPair.second);
        }
        else {
            TreesPair splitPair = split(tree->leftChild(), index);
            tree->setLeftChild(splitPair.second);
            return TreesPair(splitPair.first, tree);
        }
    }

    Tree merge(Tree leftTree, Tree rightTree) const
    {
        if (leftTree == EMPTY_TREE) {
            return rightTree;
        }
        if (rightTree == EMPTY_TREE) {
            return leftTree;
        }
        if (leftTree->element().key() > rightTree->element().key()) {
            leftTree->setRightChild(
                        merge(leftTree->rightChild(), rightTree));
            return leftTree;
        }
        else {
            rightTree->setLeftChild(
                        merge(leftTree, rightTree->leftChild()));
            return rightTree;
        }
    }

    void build()
    {
        // TODO: add linear time build from collection
        throw std::runtime_error("Unsupported operation");
    }

    std::auto_ptr<Node> root_;
};


template <class T>
class Vector
{
public:
    typedef Vector<T> Type;
    typedef Element<int, T> ElementType;
    typedef CartesianTree<ElementType> Tree;

    Vector() { }

    void append(T value)
    {
        ElementType element(random(), value);
        tree_.append(element);
    }

    T at(size_t index) const
    {
        return tree_.find(index)->element().value();
    }

    void put(size_t index, T value)
    {
        // TODO: implement replace element operation
        throw std::runtime_error("Unsupported operation exception");
    }

    void append(Type other)
    {
        tree_.merge(other.release());
    }

    void rotate(size_t begin, size_t end, size_t shift)
    {
        require(begin <= end,
                "Incorrect elements subsegment for rotate operation");
        if (begin + 1 < end) {
            shift %= end - begin;
            Tree middle = tree_.split(begin);
            Tree tail = middle.split(end - begin);
            Tree middlePart = middle.split(shift);
            middlePart.merge(middle);
            tree_.merge(middlePart);
            tree_.merge(tail);
        }
    }

    size_t size() const { return tree_.size(); }

    friend std::ostream& operator<<(std::ostream& os, const Type& vector)
    {
        for (size_t i = 0; i < vector.size(); ++i) {
            os << vector.at(i);
        }
        return os;
    }

private:
    // no copyable
    explicit Vector(const Type& other) { }

    int random() const
    {
        return (rand() << 16) + rand();
    }

    Tree tree_;
};

struct Query
{
    size_t start;
    size_t end;
    size_t shift;
};

int main()
{
    std::ios_base::sync_with_stdio(false);
    Vector<char> sequence;
    std::string message;
    std::cin >> message;
    for (size_t i = 0; i < message.size(); ++i) {
        sequence.append(message[i]);
    }
    size_t numQueries;
    std::cin >> numQueries;
    std::vector<Query> queries(numQueries);

    for (size_t queryIndex = 0; queryIndex < numQueries; ++queryIndex) {
        size_t startIndex, endIndex, shift;
        std::cin >> startIndex >> endIndex >> shift;
        --startIndex;

        queries[queryIndex].start = startIndex;
        queries[queryIndex].end = endIndex;
        queries[queryIndex].shift = shift;
    }

    std::reverse(queries.begin(), queries.end());
    for (size_t queryIndex = 0; queryIndex < numQueries; ++queryIndex) {
        const Query query = queries[queryIndex];
        sequence.rotate(query.start, query.end, query.shift);
    }

    std::cout << sequence << std::endl;

    return 0;
}
