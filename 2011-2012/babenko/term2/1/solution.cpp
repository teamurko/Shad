#include <queue>
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <utility>

#define REQUIRE(cond, message) \
    if (!(cond)) { \
        std::cerr << message << std::endl; \
        assert(false); \
    }

typedef size_t Id;
typedef std::vector<Id> Ids;

struct Edge
{
    Id first;
    Id second;
    size_t weight;
    Id id;
};

struct Arc
{
    Arc(Id toIn, Id idIn) : to(toIn), id(idIn) { }
    Id to;
    Id id;
};

typedef std::vector<Edge> Edges;

class DisjointSet
{
public:
    explicit DisjointSet(size_t size) : groupIds_(size)
    {
        for (size_t index = 0; index < groupIds_.size(); ++index) {
            groupIds_[index] = index;
        }
    }
    void merge(Id first, Id second)
    {
        first = groupId(first);
        second = groupId(second);
        REQUIRE(first != second, "Cannot merge same groups.");
        if (rand() & 1) {
            groupIds_[first] = second;
        } else {
            groupIds_[second] = first;
        }
    }
    Id groupId(Id one)
    {
        if (groupIds_[one] != one) {
            groupIds_[one] = groupId(groupIds_[one]);
        }
        return groupIds_[one];
    }
    bool hasOneGroup()
    {
        REQUIRE(!groupIds_.empty(), "Disjoint set is empty,"
                << "cannot determine if there is only one group");
        Id element = groupId(0);
        for (Id index = 0; index < groupIds_.size(); ++index) {
            if (groupId(index) != element) {
                return false;
            }
        }
        return true;
    }
private:
    Ids groupIds_;
};

class Graph
{
public:
    typedef std::vector<Arc> Arcs;
    typedef Arcs IncidentArcs;

    Graph(size_t numVertices, const Edges& edges)
        : numVertices_(numVertices), edges_(edges)
    {
        buildGraph();
    }

    const IncidentArcs& incidentArcs(Id vertex) const
    {
        return graph_.at(vertex);
    }

    const Arc& arc(Id vertex, size_t index) const
    {
        return incidentArcs(vertex).at(index);
    }

    size_t numVertices() const
    {
        return numVertices_;
    }

    size_t numEdges() const
    {
        return edges_.size();
    }

    const Edges& edges() const
    {
        return edges_;
    }

    // Returns spanning tree edges as the first element of
    // the resulting pair, and returns other edges as the second
    // element.
    std::pair<Edges, Edges> spanningTree() const
    {
        Edges spanningEdges;
        Edges others;
        DisjointSet dset(numVertices());
        for (size_t edgeId = 0; edgeId < edges_.size(); ++edgeId) {
            const Edge& edge = edges_[edgeId];
            if (dset.groupId(edge.first) != dset.groupId(edge.second)) {
                spanningEdges.push_back(edge);
                dset.merge(edge.first, edge.second);
            }
            else {
                others.push_back(edge);
            }
        }
        REQUIRE(dset.hasOneGroup(),
                "Spanning edges do not form connected graph.");
        return std::make_pair(spanningEdges, others);
    }

private:
    void buildGraph()
    {
        graph_.resize(numVertices_);
        for (size_t index = 0; index < edges_.size(); ++index) {
            addDirectedEdge(edges_[index].first,
                            edges_[index].second, edges_[index].id);
            addDirectedEdge(edges_[index].second,
                            edges_[index].first, edges_[index].id);
        }
    }

    void addDirectedEdge(size_t from, size_t to, size_t id)
    {
        graph_.at(from).push_back(Arc(to, id));
    }

private:
    size_t numVertices_;
    const Edges edges_;
    std::vector<IncidentArcs> graph_;
};

void readData(size_t& numVertices, Edges* edges)
{
    REQUIRE(edges->empty(), "Edges container should be empty.");
    std::cin >> numVertices;
    size_t numEdges;
    std::cin >> numEdges;
    for (Id index = 0; index < numEdges; ++index) {
        Edge edge;
        std::cin >> edge.first >> edge.second >> edge.weight;
        edge.id = index;
        edges->push_back(edge);
    }
}

Edges to0Notation(Edges edges)
{
    for (Id index = 0; index < edges.size(); ++index) {
        --edges[index].first;
        --edges[index].second;
    }
    return edges;
}

class ProbableEdgeCutSolver
{
public:
    explicit ProbableEdgeCutSolver(const Graph& graph, size_t numBits = 60)
        : graph_(graph), numBits_(numBits)
    {
        REQUIRE(numBits <= 64, "Cannot handle more than 64 bits "
                << "because of 64 bit type restriction");
    }

    void solve()
    {
        buildRootedSpanningTree();
        generateUniRandPartialCirculations();
        completeBinaryCirculations();
        collectCutEdges();
    }

    void collectCutEdges()
    {
        const Edges& edges = graph_.edges();
        for (size_t index = 0; index < edges.size(); ++index) {
            const Edge& edge = edges[index];
            if (edgesBits_[edge.id] == 0) {
                cutEdges_.push_back(edge);
            }
        }
    }

    void completeBinaryCirculations()
    {
        std::vector<long long> vertexDegreeParity(graph_.numVertices());
        for (size_t index = 0; index < nonTreeEdges_.size(); ++index) {
            const Edge& edge = nonTreeEdges_[index];
            for (size_t bit = 0; bit < numBits_; ++bit) {
                if (takeBit(edgesBits_[edge.id], bit)) {
                    xorBit(vertexDegreeParity[edge.first], bit);
                    xorBit(vertexDegreeParity[edge.second], bit);
                }
            }
        }
        for (size_t index = 0; index < orderedVertices_.size(); ++index) {
            Id vertex = orderedVertices_[index];
            if (vertex == root_) {
                checkBinaryCirculations(vertexDegreeParity);
            } else {
                for (size_t bit = 0; bit < numBits_; ++bit) {
                    if (takeBit(vertexDegreeParity[vertex], bit)) {
                        xorBit(vertexDegreeParity[vertex], bit);
                        const Arc& arc = rootedSpanningTreeParents_[vertex];
                        xorBit(vertexDegreeParity[arc.to], bit);
                        xorBit(edgesBits_[arc.id], bit);
                    }
                }
            }
        }
    }

    void checkBinaryCirculations(
                            const std::vector<long long>& vertexDegreeParity)
    {
        for (size_t bit = 0; bit < numBits_; ++bit) {
            REQUIRE(!takeBit(vertexDegreeParity[root_], bit),
                    "Completed circulation " << bit << " is not binary.");
        }
    }

    void xorBit(long long& bitset, size_t index)
    {
        REQUIRE(index < 64, "Bitset index is out of range");
        bitset ^= 1LL << index;
    }

    bool takeBit(long long bitset, size_t index)
    {
        REQUIRE(index < 64, "Bitset index is out of range");
        return (bitset >> index) & 1;
    }

    void generateUniRandPartialCirculations()
    {
        for (size_t index = 0; index < nonTreeEdges_.size(); ++index) {
            const Edge& edge = nonTreeEdges_[index];
            for (size_t bit = 0; bit < numBits_; ++bit) {
                edgesBits_[edge.id] <<= 1;
                edgesBits_[edge.id] |= rand() & 1;
            }
        }
    }

    void buildRootedSpanningTree()
    {
        std::pair<Edges, Edges> result = graph_.spanningTree();
        nonTreeEdges_ = result.second;
        Graph tree(graph_.numVertices(), result.first);
        buildRootedTree(tree);
    }

    void buildRootedTree(const Graph& tree)
    {
        root_ = 0;
        std::queue<Id> verticesQueue;
        verticesQueue.push(root_);
        std::vector<bool> used(tree.numVertices());
        used[root_] = true;
        while (!verticesQueue.empty()) {
            Id vertex = verticesQueue.front();
            orderedVertices_.push_back(vertex);
            verticesQueue.pop();
            const Graph::Arcs& arcs = tree.incidentArcs(vertex);
            for (size_t arcIndex = 0; arcIndex < arcs.size(); ++arcIndex) {
                const Arc& arc = arcs[arcIndex];
                if (!used[arc.to]) {
                    rootedSpanningTreeParents_[arc.to] = Arc(vertex, arc.id);
                    used[arc.to] = true;
                    verticesQueue.push(arc.to);
                }
            }
        }
        std::reverse(orderedVertices_.begin(), orderedVertices_.end());
    }

    Edges cutEdges() const
    {
        return cutEdges_;
    }

    void setNumBits(size_t numBits)
    {
        numBits_ = numBits;
    }

private:
    Graph graph_;
    size_t numBits_;
    std::vector<long long> edgesBits_;
    Graph::Arcs rootedSpanningTreeParents_;
    Id root_;
    Edges nonTreeEdges_;
    Ids orderedVertices_;
    Edges cutEdges_;
};


void solve(size_t numVertices, const Edges& edges)
{
    Graph graph(numVertices, edges);
    ProbableEdgeCutSolver solver(graph, 60);
    solver.solve();
    Edges cutEdges = solver.cutEdges();
    size_t minWeight = 1e9+111;
    for (size_t index = 0; index < cutEdges.size(); ++index) {
        minWeight = std::min(minWeight, cutEdges[index].weight);
    }
    std::cout << minWeight << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    size_t numVertices;
    Edges edges;
    readData(numVertices, &edges);
    solve(numVertices, to0Notation(edges));

    return 0;
}
