#include <limits>
#include <set>
#include <queue>
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <utility>

#define NDEBUG

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
            assert(false); \
        } \
    } while (false);

typedef size_t Id;
typedef std::vector<Id> Ids;

const Id UNDEFINED = std::numeric_limits<Id>::max();

struct Edge
{
    Id first;
    Id second;
    size_t weight;
    Id id;
};

struct Arc
{
    Arc() : to(-1), id(-1) { }
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

// Returns spanning tree edges as the first element of
// the resulting pair, and returns other edges as the second
// element.
std::pair<Edges, Edges> spanningTree(const Graph& graph)
{
    Edges spanningEdges;
    Edges others;
    DisjointSet dset(graph.numVertices());
    const Edges& edges = graph.edges();
    for (size_t edgeId = 0; edgeId < edges.size(); ++edgeId) {
        const Edge& edge = edges[edgeId];
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

void readData(size_t& numVertices, Edges* edges)
{
    REQUIRE(edges->empty(), "Edges container should be empty.");
    std::cin >> numVertices;
    size_t numEdges;
    std::cin >> numEdges;
    for (Id index = 0; index < numEdges; ++index) {
        Edge edge;
        std::cin >> edge.first >> edge.second >> edge.weight;
        --edge.first;
        --edge.second;
        edge.id = index;
        edges->push_back(edge);
    }
}

class ProbabilisticEdgesCutSolver
{
public:
    explicit ProbabilisticEdgesCutSolver(const Graph& graph, size_t numBits = 60)
        : graph_(graph), numBits_(numBits)
    {
        REQUIRE(numBits <= 64, "Cannot handle more than 64 bits "
                << "because of 64 bit type restriction");
    }

    void solve()
    {
        buildRootedSpanningTree();
        generateUniformRandomPartialCirculations();
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
            vertexDegreeParity[edge.first] ^= edgesBits_[edge.id];
            vertexDegreeParity[edge.second] ^= edgesBits_[edge.id];
        }
        for (size_t index = 0; index < orderedVertices_.size(); ++index) {
            Id vertex = orderedVertices_[index];
            if (vertex == root_) {
                checkBinaryCirculations(vertexDegreeParity);
            } else {
                const Arc& arc = rootedSpanningTreeParents_[vertex];
                vertexDegreeParity[arc.to] ^= vertexDegreeParity[vertex];
                edgesBits_[arc.id] ^= vertexDegreeParity[vertex];
                vertexDegreeParity[vertex] ^= vertexDegreeParity[vertex];
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

    bool takeBit(long long bitset, size_t index)
    {
        REQUIRE(index < 64, "Bitset index is out of range");
        return (bitset >> index) & 1;
    }

    void generateUniformRandomPartialCirculations()
    {
        edgesBits_.resize(graph_.numEdges());
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
        std::pair<Edges, Edges> result = spanningTree(graph_);
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
        rootedSpanningTreeParents_.resize(tree.numVertices());
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
    const Graph& graph_;
    size_t numBits_;
    std::vector<long long> edgesBits_;
    Graph::Arcs rootedSpanningTreeParents_;
    Id root_;
    Edges nonTreeEdges_;
    Ids orderedVertices_;
    Edges cutEdges_;
};

class EdgesCutSolver
{
public:
    explicit EdgesCutSolver(const Graph& graph)
        : graph_(graph), up_(graph_.numVertices()),
        id_(graph_.numVertices()), used_(graph_.numVertices()),
        timer_(0)
    {
    }

    void solve()
    {
        dfs(0, UNDEFINED);
    }

    void dfs(Id vertex, Id parent)
    {
        up_[vertex] = id_[vertex] = timer_++;
        used_[vertex] = true;
        const Graph::IncidentArcs& arcs = graph_.incidentArcs(vertex);
        for (size_t index = 0; index < arcs.size(); ++index) {
            const Arc& arc = arcs[index];
            if (arc.to != parent) {
                if (used_[arc.to]) {
                    up_[vertex] = std::min(up_[vertex], id_[arc.to]);
                } else {
                    dfs(arc.to, vertex);
                    up_[vertex] = std::min(up_[vertex], up_[arc.to]);
                    if (up_[arc.to] > id_[vertex]) {
                        cutEdges_.push_back(graph_.edges()[arc.id]);
                    }
                }
            }
        }
    }

    const Edges& cutEdges() const
    {
        return cutEdges_;
    }

private:
    const Graph& graph_;
    Ids up_;
    Ids id_;
    std::vector<bool> used_;
    Id timer_;
    Edges cutEdges_;
};

Ids extractIdsSorted(const Edges& edges)
{
    Ids result;
    result.reserve(edges.size());
    for (size_t index = 0; index < edges.size(); ++index) {
        result.push_back(edges[index].id);
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool checkSameEdges(const Edges& first, const Edges& second)
{
    return extractIdsSorted(first) == extractIdsSorted(second);
}

void solve(size_t numVertices, const Edges& edges, Edges* cutEdges)
{
    Graph graph(numVertices, edges);
    ProbabilisticEdgesCutSolver solver(graph, 10);
    solver.solve();
    *cutEdges = solver.cutEdges();
}

void writeData(const Edges& cutEdges)
{
    if (cutEdges.empty()) {
        std::cout << -1 << std::endl;
    } else {
        size_t minWeight = cutEdges.back().weight;
        for (size_t index = 0; index + 1 < cutEdges.size(); ++index) {
            minWeight = std::min(minWeight, cutEdges[index].weight);
        }
        std::cout << minWeight << std::endl;
    }
}

void testWithRandomGraph()
{
    size_t numVertices = rand() % 1000 + 10;
    DisjointSet ds(numVertices);
    std::set<std::pair<Id, Id> > used;
    Edges edges;
    while (!ds.hasOneGroup()) {
        Id u = rand() % numVertices;
        Id v = rand() % numVertices;
        if (u == v) {
            continue;
        }
        if (ds.groupId(u) == ds.groupId(v)) {
            continue;
        }
        if (used.count(std::make_pair(u, v))) {
            continue;
        }
        used.insert(std::make_pair(u, v));
        used.insert(std::make_pair(v, u));
        size_t weight = rand() % 100;
        Edge edge = {u, v, weight, edges.size()};
        edges.push_back(edge);
        ds.merge(u, v);
    }
    size_t rest = (numVertices - 1) * (numVertices - 2) / 2;
    size_t numAdd = rand() % rest;
    for (size_t index = 0; index < numAdd; ++index) {
        Id u = rand() % numVertices;
        Id v = rand() % numVertices;
        if (u == v) {
            continue;
        }
        if (used.count(std::make_pair(u, v))) {
            continue;
        }
        used.insert(std::make_pair(u, v));
        used.insert(std::make_pair(v, u));
        size_t weight = rand() % 100;
        Edge edge = {u, v, weight, edges.size()};
        edges.push_back(edge);
    }
    Graph graph(numVertices, edges);
    ProbabilisticEdgesCutSolver psolver(graph, 60);
    psolver.solve();
    EdgesCutSolver solver(graph);
    solver.solve();
    if (!checkSameEdges(psolver.cutEdges(), solver.cutEdges())) {
        std::cerr << "Cut edges provided by probable solution "
                  << "differ from ones provided by deterministic solution";
        std::cerr << "Edges : " << std::endl;
        for (size_t i = 0; i < edges.size(); ++i) {
            const Edge& edge = edges[i];
            std::cerr << edge.first << " " << edge.second << std::endl;
        }
        std::cerr << "Cut edges probable : " << std::endl;
        for (size_t i = 0; i < psolver.cutEdges().size(); ++i) {
            std::cerr << psolver.cutEdges()[i].id << std::endl;
        }
        std::cerr << "Cut edges : " << std::endl;
        for (size_t i = 0; i < solver.cutEdges().size(); ++i) {
            std::cerr << solver.cutEdges()[i].id << std::endl;
        }
        exit(1);
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

#ifdef DEBUG
    for (size_t testIndex = 0; testIndex < 100; ++testIndex) {
        testWithRandomGraph();
    }
#else
    size_t numVertices;
    Edges edges;
    readData(numVertices, &edges);
    Edges cutEdges;
    solve(numVertices, edges, &cutEdges);
    writeData(cutEdges);
#endif

    return 0;
}
