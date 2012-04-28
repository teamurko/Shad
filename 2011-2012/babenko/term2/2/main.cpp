#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cassert>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
            assert(false); \
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

void dfs(Id vertex, const Graph& graph, std::vector<bool>* used, Ids* visited)
{
    used->at(vertex) = true;
    for (Id arcId = 0; arcId < graph[vertex].size(); ++arcId) {
        const Arc& arc = graph[vertex][arcId];
        if (arc.label == EPSILON && !used->at(arc.to)) {
            dfs(arc.to, graph, used, visited);
        }
    }
    visited->push_back(vertex);
}

Graph transposed(const Graph& graph)
{
    Graph transposedGraph(graph.size());
    for (Id from = 0; from < graph.size(); ++from) {
        for (Id arcIndex = 0; arcIndex < graph.at(from).size(); ++arcIndex) {
            const Arc& arc = graph.at(from).at(arcIndex);
            transposedGraph.at(arc.to).push_back(Arc(from, arc.label));
        }
    }
    return transposedGraph;
}

// Returns number of components.
size_t shrinkByEpsilonArcs(const Graph& graph, Ids* componentIndex)
{
    std::vector<bool> used(graph.size());
    Ids verticesInPostDfsOrder;
    for (Id vertex = 0; vertex < graph.size(); ++vertex) {
        if (!used[vertex]) {
            dfs(vertex, graph, &used, &verticesInPostDfsOrder);
        }
    }
    used.assign(graph.size(), false);
    componentIndex->assign(graph.size(), 0);
    std::reverse(verticesInPostDfsOrder.begin(),
                 verticesInPostDfsOrder.end());
    size_t numComponents = 0;
    Graph transposedGraph = transposed(graph);
    Ids visited;
    for (Id index = 0; index < verticesInPostDfsOrder.size(); ++index) {
        Id vertex = verticesInPostDfsOrder[index];
        if (!used[vertex]) {
            dfs(vertex, transposedGraph, &used, &visited);
            for (Id i = 0; i < visited.size(); ++i) {
                componentIndex->at(visited[i]) = numComponents;
            }
            visited.clear();
            ++numComponents;
        }
    }
    return numComponents;
}

Graph condensedGraph(const Graph& graph, Ids* componentIndex)
{
    size_t numComponents = shrinkByEpsilonArcs(graph, componentIndex);
    Graph resultGraph(numComponents);
    for (Id from = 0; from < graph.size(); ++from) {
        for (Id arcIndex = 0; arcIndex < graph.at(from).size(); ++arcIndex) {
            const Arc& arc = graph.at(from).at(arcIndex);
            Id newFrom = componentIndex->at(from);
            Id newTo = componentIndex->at(arc.to);
            if (arc.label != EPSILON || newFrom != newTo) {
                resultGraph.at(newFrom).push_back(Arc(newTo, arc.label));
            }
        }
    }
    return resultGraph;
}

class Automaton {
public:
    Automaton() : initVertex_(-1) { }

    Automaton(const Graph& graph, const Ids& terminals, Id initVertex)
        : initVertex_(initVertex), isTerminal_(graph.size())
    {
        for (Id index = 0; index < terminals.size(); ++index) {
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

    static const int ALPHABET_SIZE = 27;

    void buildTransitions(const Graph& graph)
    {
        transitions_.assign(graph.size(), std::vector<Ids>(ALPHABET_SIZE));
        for (Id vertex = 0; vertex < graph.size(); ++vertex) {
            const Arcs& arcs = graph.at(vertex);
            for (Id arcId = 0; arcId < arcs.size(); ++arcId) {
                const Arc& arc = arcs[arcId];
                transitions_[vertex][getLabelId(arc.label)].push_back(arc.to);
            }
        }
    }


private:
    Id getLabelId(char character) const
    {
        if (character == EPSILON) {
            return ALPHABET_SIZE - 1;
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
    struct State
    {
        size_t length;
        Id nextVertex;
        Id nextPosition;
    };

    size_t longestPathLength(Id vertex, Id position) {
        if (longestPathLength_[vertex][position] != UNDEFINED) {
            return longestPathLength_[vertex][position];
        }

        State result = {NONEXISTENT, NONEXISTENT, NONEXISTENT};
        if (automaton_.isTerminal(vertex)) {
            result.length = 0;
        }
        const Ids& epsAdjacent = automaton_.next(vertex, EPSILON);
        for (Id index = 0; index < epsAdjacent.size(); ++index) {
            size_t candidateValue =
                            longestPathLength(epsAdjacent[index], position);
            if (candidateValue != NONEXISTENT) {
                update(candidateValue, epsAdjacent[index], position, &result);
            }
        }
        if (position < word_.length()) {
            const Ids& adjacent = automaton_.next(vertex, word_[position]);
            for (Id index = 0; index < adjacent.size(); ++index) {
                size_t candidateValue =
                            longestPathLength(adjacent[index], position + 1);
                if (candidateValue != NONEXISTENT) {
                    update(candidateValue + 1, adjacent[index], position + 1,
                           &result);
                }
            }
        }

        nextVertex_[vertex][position] = result.nextVertex;
        nextPosition_[vertex][position] = result.nextPosition;
        return longestPathLength_[vertex][position] = result.length;
    }

    void update(size_t candidateLength, Id nextVertex, Id nextPosition,
                State* result)
    {
        if (result->length == NONEXISTENT ||
                                    result->length < candidateLength) {
            result->length = candidateLength;
            result->nextVertex = nextVertex;
            result->nextPosition = nextPosition;
        }
    }

    std::string restoreSubstring() {
        size_t maxLength = 0;
        Id argMaxPosition = 0;
        for (Id position = 0; position < word_.length(); ++position) {
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
    std::vector<std::vector<Id> > nextPosition_;
    std::string answer_;
    bool calculated_;
};

void buildGraph(size_t numVertices, const Edges& edges, Graph* graph)
{
    graph->clear();
    graph->resize(numVertices);
    for (Id index = 0; index < edges.size(); ++index) {
        const Edge& edge = edges[index];
        graph->at(edge.from).push_back(Arc(edge.to, edge.label));
    }
}

void buildCondensedAutomaton(size_t numVertices, const Edges& edges,
                             const Ids& terminals, Automaton* automaton)
{
    Graph graph;
    buildGraph(numVertices, edges, &graph);

    Ids componentIndex;
    Graph condensedGraph = ::condensedGraph(graph, &componentIndex);

    static const Id INIT_VERTEX = 0;

    Ids condensedTerminals;

    for (Id index = 0; index < terminals.size(); ++index) {
        condensedTerminals.push_back(componentIndex.at(terminals[index]));
    }

    std::sort(condensedTerminals.begin(), condensedTerminals.end());
    condensedTerminals.erase(std::unique(condensedTerminals.begin(),
                                         condensedTerminals.end()),
                             condensedTerminals.end());

    *automaton = Automaton(condensedGraph,
                           condensedTerminals,
                           componentIndex.at(INIT_VERTEX));
}

void readData(size_t& numVertices, Edges* edges,
              Ids* terminals, std::string* word)
{
    size_t numEdges, numTerminals;
    std::cin >> numVertices >> numEdges >> numTerminals;

    terminals->clear();
    terminals->resize(numTerminals);
    for (Id index = 0; index < numTerminals; ++index) {
        std::cin >> terminals->at(index);
    }

    edges->clear();
    edges->reserve(numEdges);

    for (Id edgeId = 0; edgeId < numEdges; ++edgeId) {
        Id from, to;
        char label;
        std::cin >> from >> label >> to;
        edges->push_back(Edge(from, to, label));
    }
    std::cin >> *word;
}

std::string findLongestAcceptedSubstring(
        const Automaton& automaton, const std::string& word)
{
    LongestSubstringFinder solver(automaton, word);
    return solver.find();
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

    std::string answer = findLongestAcceptedSubstring(automaton, word);
    writeData(answer);

    return 0;
}
