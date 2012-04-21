#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
        } \
    } while (false)


const char EPSILON = '$';
const size_t UNDEFINED = -2;
const size_t NONEXISTENT = -1;

typedef size_t Id;
typedef std::vector<Id> Ids;

struct Edge
{
    Edge(Id from_, Id to_, char label_)
        : from(from_), to(to_), label(label_) { }
    Id from, to;
    char label;
};

typedef std::vector<Edge> Edges;

struct Arc
{
    Arc(Id to_, char label_) : to(to_), label(label_) { }
    Id to;
    char label;
};

typedef std::vector<Arc> Arcs;
typedef std::vector<Arcs> Graph;

class GraphCondenser
{
public:
    explicit GraphCondenser(const Graph& graph) : graph_(graph)
    {
        shrinkByEpsilonArcs();
    }

    Id componentIndex(Id vertex) const
    {
        return componentIndex_.at(vertex);
    }

    Graph condensedGraph() const
    {
        Graph resultGraph(numComponents_);
        for (Id from = 0; from < graph_.size(); ++from) {
            for (size_t arcIndex = 0; arcIndex < graph_.at(from).size();
                                                                ++arcIndex) {
                const Arc& arc = graph_.at(from).at(arcIndex);
                Id newFrom = componentIndex_[from];
                Id newTo = componentIndex_[arc.to];
                if (arc.label != EPSILON || newFrom != newTo) {
                    resultGraph.at(newFrom).push_back(Arc(newTo, arc.label));
                }
            }
        }
        return resultGraph;
    }

private:
    void shrinkByEpsilonArcs()
    {
        used_.assign(graph_.size(), false);
        for (Id vertex = 0; vertex < graph_.size(); ++vertex) {
            if (!used_[vertex]) {
                dfs(vertex, graph_);
            }
        }
        used_.assign(graph_.size(), false);
        Graph transposedGraph(graph_.size());
        for (Id from = 0; from < graph_.size(); ++from) {
            for (size_t arcIndex = 0; arcIndex < graph_.at(from).size();
                                                                ++arcIndex) {
                const Arc& arc = graph_.at(from).at(arcIndex);
                transposedGraph.at(arc.to).push_back(Arc(from, arc.label));
            }
        }
        componentIndex_ = Ids(graph_.size());
        std::reverse(verticesInOrder_.begin(), verticesInOrder_.end());
        numComponents_ = 0;
        for (size_t index = 0; index < verticesInOrder_.size(); ++index) {
            Id vertex = verticesInOrder_[index];
            if (!used_[vertex]) {
                dfs(vertex, transposedGraph, numComponents_++);
            }
        }
    }

    void dfs(Id vertex, const Graph& graph, Id component) {
        used_[vertex] = true;
        for (size_t arcId = 0; arcId < graph[vertex].size(); ++arcId) {
            const Arc& arc = graph[vertex][arcId];
            if (arc.label == EPSILON && !used_[arc.to]) {
                dfs(arc.to, graph, component);
            }
        }
        componentIndex_[vertex] = component;
    }

    void dfs(Id vertex, const Graph& graph) {
        used_[vertex] = true;
        for (size_t arcId = 0; arcId < graph[vertex].size(); ++arcId) {
            const Arc& arc = graph[vertex][arcId];
            if (arc.label == EPSILON && !used_[arc.to]) {
                dfs(arc.to, graph);
            }
        }
        verticesInOrder_.push_back(vertex);
    }

    Graph graph_;
    size_t numComponents_;
    Ids componentIndex_;
    Ids verticesInOrder_;
    std::vector<bool> used_;
};

class Automaton {
public:
    Automaton() : initVertex_(-1) { }

    Automaton(const Graph& graph, Ids terminals, Id initVertex)
        : initVertex_(initVertex), isTerminal_(graph.size())
    {
        for (size_t index = 0; index < terminals.size(); ++index) {
            isTerminal_.at(terminals[index]) = true;
        }
        buildTransitions(graph);
    }

    const Ids& next(Id vertex, char label) const
    {
        return transitions_[vertex][getLabelId(label)];
    }

    Id initVertex() const
    {
        return initVertex_;
    }

    bool isTerminal(Id vertex) const
    {
        return isTerminal_[vertex];
    }

    size_t numVertices() const {
        return transitions_.size();
    }

    static const int ALPH_NUM = 27;

    void buildTransitions(const Graph& graph)
    {
        transitions_.assign(graph.size(), std::vector<Ids>(ALPH_NUM));
        for (Id vertex = 0; vertex < graph.size(); ++vertex) {
            const Arcs& arcs = graph.at(vertex);
            for (size_t arcId = 0; arcId < arcs.size(); ++arcId) {
                const Arc& arc = arcs[arcId];
                transitions_[vertex][getLabelId(arc.label)].push_back(arc.to);
            }
        }
    }


private:
    Id getLabelId(char character) const
    {
        if (character == EPSILON) {
            return ALPH_NUM - 1;
        } else {
            REQUIRE(isalpha(character) && tolower(character) == character,
                    "Character " << character << " is not one of a-z");
            return character - 'a';
        }
    }

    Id initVertex_;
    std::vector<std::vector<Ids> > transitions_;
    std::vector<bool> isTerminal_;
};

class LongestSubstringFinder
{
public:
    LongestSubstringFinder(const Automaton& automaton,
                           const std::string& word)
        : automaton_(automaton), word_(word),
        longestPathLength_(automaton.numVertices(),
                           std::vector<size_t>(word.size() + 1, UNDEFINED)),
        nextVertex_(automaton.numVertices(),
                    std::vector<size_t>(word.size() + 1, NONEXISTENT)),
        nextPosition_(automaton.numVertices(),
                      std::vector<size_t>(word.size() + 1, NONEXISTENT)),
        calculated_(false)
    { }

    std::string find()
    {
        if (!calculated_) {
            answer_ = restoreSubstring();
        }
        return answer_;
    }

private:
    int longestPathLength(Id vertex, size_t position) {
        if (longestPathLength_[vertex][position] != UNDEFINED) {
            return longestPathLength_[vertex][position];
        }

        size_t result = NONEXISTENT;
        size_t nextVertex = NONEXISTENT;
        size_t nextPosition = NONEXISTENT;
        if (automaton_.isTerminal(vertex)) {
            result = 0;
        }
        const Ids& epsAdjacent = automaton_.next(vertex, EPSILON);
        for (size_t index = 0; index < epsAdjacent.size(); ++index) {
            size_t candidateValue =
                            longestPathLength(epsAdjacent[index], position);
            if (candidateValue != NONEXISTENT) {
                if (result == NONEXISTENT || result < candidateValue) {
                    result = candidateValue;
                    nextVertex = epsAdjacent[index];
                    nextPosition = position;
                }
            }
        }
        if (position < word_.length()) {
            const Ids& adjacent = automaton_.next(vertex, word_[position]);
            for (size_t index = 0; index < adjacent.size(); ++index) {
                size_t candidateValue =
                            longestPathLength(adjacent[index], position + 1);
                if (candidateValue != NONEXISTENT) {
                    if (result == NONEXISTENT ||
                                                result < candidateValue + 1) {
                        result = candidateValue + 1;
                        nextVertex = adjacent[index];
                        nextPosition = position + 1;
                    }
                }
            }
        }

        nextVertex_[vertex][position] = nextVertex;
        nextPosition_[vertex][position] = nextPosition;
        return longestPathLength_[vertex][position] = result;
    }

    std::string restoreSubstring() {
        size_t maxLength = 0;
        size_t argMaxPosition = 0;
        for (size_t position = 0; position < word_.length(); ++position) {
            size_t pathLength =
                        longestPathLength(automaton_.initVertex(), position);
            if (pathLength != NONEXISTENT && maxLength < pathLength) {
                maxLength = pathLength;
                argMaxPosition = position;
            }
        }
        std::string result;
        if (maxLength > 0) {
            Id vertex = automaton_.initVertex();
            Id position = argMaxPosition;
            while (nextVertex_[vertex][position] != NONEXISTENT) {
                if (position + 1 == nextPosition_[vertex][position]) {
                    result += word_[position];
                }
                Id newVertex = nextVertex_[vertex][position];
                position = nextPosition_[vertex][position];
                vertex = newVertex;
            }
        }
        REQUIRE(result.length() == maxLength,
                "Answer and string length should be equal");
        return result;
    }

    const Automaton& automaton_;
    const std::string& word_;
    std::vector<std::vector<size_t> > longestPathLength_;
    std::vector<std::vector<Id> > nextVertex_;
    std::vector<std::vector<size_t> > nextPosition_;
    std::string answer_;
    bool calculated_;
};

void buildGraph(size_t numVertices, const Edges& edges, Graph* graph)
{
    graph->clear();
    graph->resize(numVertices);
    for (size_t index = 0; index < edges.size(); ++index) {
        const Edge& edge = edges[index];
        graph->at(edge.from).push_back(Arc(edge.to, edge.label));
    }
}

void buildCondensedAutomaton(size_t numVertices, const Edges& edges,
                             const Ids& terminals, Automaton* automaton)
{
    Graph graph;
    buildGraph(numVertices, edges, &graph);

    GraphCondenser condenser(graph);
    Graph condensedGraph = condenser.condensedGraph();

    static const Id INIT_VERTEX = 0;

    Ids condensedTerminals;

    for (size_t index = 0; index < terminals.size(); ++index) {
        condensedTerminals.push_back(
                condenser.componentIndex(terminals[index]));
    }

    std::sort(condensedTerminals.begin(), condensedTerminals.end());
    condensedTerminals.erase(std::unique(condensedTerminals.begin(),
                                         condensedTerminals.end()),
                             condensedTerminals.end());

    *automaton = Automaton(condensedGraph,
                           condensedTerminals,
                           condenser.componentIndex(INIT_VERTEX));
}

void readData(size_t& numVertices, Edges* edges,
              Ids* terminals, std::string* word)
{
    size_t numEdges, numTerminals;
    std::cin >> numVertices >> numEdges >> numTerminals;

    terminals->clear();
    terminals->resize(numTerminals);
    for (size_t index = 0; index < numTerminals; ++index) {
        std::cin >> terminals->at(index);
    }

    edges->clear();
    edges->reserve(numEdges);

    for (size_t edgeId = 0; edgeId < numEdges; ++edgeId) {
        Id from, to;
        char label;
        std::cin >> from >> label >> to;
        edges->push_back(Edge(from, to, label));
    }
    std::cin >> *word;
}

void solve(const Automaton& automaton, const std::string& word,
           std::string* result)
{
    LongestSubstringFinder solver(automaton, word);
    *result = solver.find();
}

void writeData(const std::string& answer)
{
    if (answer.empty()) {
        std::cout << "No solution" << std::endl;
    } else {
        std::cout << answer << std::endl;
    }
}

int main()
{
    size_t numVertices;
    Edges edges;
    Ids terminals;
    std::string word;
    readData(numVertices, &edges, &terminals, &word);

    Automaton automaton;
    buildCondensedAutomaton(numVertices, edges, terminals, &automaton);

    std::string answer;
    solve(automaton, word, &answer);

    writeData(answer);

    return 0;
}
