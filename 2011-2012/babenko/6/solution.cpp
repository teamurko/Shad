#include <iostream>
#include <vector>
#include <stdexcept>

void require(bool cond, const std::string& message)
{
    if (!cond) {
        throw std::runtime_error(message);
    }
}

#define SIZE(node) ((node) == NULL ? 0 : (node)->size())
#define EMPTY_TREE NULL

template <class Key=int, class Value=char>
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
    typedef Node* NodePointer;

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

    static NodePointer instance(const Element& element) const
    {
        return new Node(element);
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
class CartesianTree
{
public:
    typedef NodePointer Tree;
    typedef std::pair<Tree, Tree> TreesPair;

    CartesianTree(const std::vector<Element>& elements)
    {
        build();
    }

    ~CartesianTree()
    {
        std::queue<NodePointer> nodes;
        nodes.push(root_);
        while (!nodes.empty()) {
            NodePointer node = nodes.front();
            nodes.pop();
            if (node.leftChild() != EMPTY_TREE) {
                nodes.push(node.leftChild());
            }
            if (node.rightChild() != EMPTY_TREE) {
                nodes.push(node.rightChild());
            }
            delete node;
        }
    }

    void insert(size_t index, const Element& element)
    {
        TreesPair splitPair = split(root_, index);
        Tree innerTree = Node.instance(element);
        root_ = merge(splitPair.first,
                      merge(innerTree, splitPair.second));
    }

    void append(const Element& element)
    {
        insert(size(), element);
    }

    const Tree find(size_t index) const
    {
        return find(root_, index);
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
            return find(tree->rightChild(), index - tree->leftChild() - 1);
        }
    }

    TreesPair split(Tree tree, size_t index) const
    {
        if (tree == EMPTY_TREE) {
            require(index == 0, "Index is out of range");
            return EMPTY_TREE;
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
        if (leftTree->key() > rightTree->key()) {
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
        // TODO
    }

    typedef Node<Element>::NodePointer NodePointer;
    Tree root_;
};

template <class T>
class Vector
{
public:
    typedef Vector<T> Type;

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
        throw std::runtime_error("Unsupported operation exception");
    }

    Type disjoin(size_t index)
    {

    }

private:
    int random() const
    {
        return (rand() << 16) + rand();
    }

    typedef Element<int, T> ElementType;
    typedef CartesianTree<ElementType> Tree;
    Tree tree_;
};

int main()
{
    return 0;
}
