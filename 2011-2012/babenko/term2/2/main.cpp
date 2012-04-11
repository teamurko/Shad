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
const size_t NON_EXISTENT = -1;


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

class Automaton {
public:
    Automaton() : numVertices_(0), initVertex_(-1) { }

    Automaton(int numVertices, const Edges& edges, Ids terminals)
        : numVertices_(numVertices), initVertex_(0), edges_(edges),
        graph_(numVertices), isTerminal_(numVertices)
    {
        for (size_t edgeId = 0; edgeId < edges.size(); ++edgeId) {
            const Edge& edge = edges[edgeId];
            graph_[edge.from].push_back(Arc(edge.to, edge.label));
        }
        for (size_t index = 0; index < terminals.size(); ++index) {
            isTerminal_[terminals[index]] = true;
        }
    }
    const Ids& next(Id vertex, char label) const
    {
        return transitions_[vertex][getLabelId(label)];
    }

    Id initVertex() const
    {
        return initVertex_;
    }

    void shrinkByEpsilonArcs()
    {
        used_ = std::vector<bool>(numVertices_);
        for (Id vertex = 0; vertex < numVertices_; ++vertex) {
            if (!used_[vertex]) {
                dfs(vertex, graph_);
            }
        }
        fill(used_.begin(), used_.end(), false);
        Graph transposedGraph(numVertices_);
        for (size_t edgeId = 0; edgeId < edges_.size(); ++edgeId) {
            const Edge& edge = edges_[edgeId];
            transposedGraph[edge.to].push_back(Arc(edge.from, edge.label));
        }
        int numComponents = 0;
        componentIndex_ = Ids(numVertices_);
        std::reverse(verticesInOrder_.begin(), verticesInOrder_.end());
        for (size_t index = 0; index < verticesInOrder_.size(); ++index) {
            Id vertex = verticesInOrder_[index];
            if (!used_[vertex]) {
                dfs(vertex, transposedGraph, numComponents++);
            }
        }
        rebuildGraph(numComponents);
    }

    bool isTerminal(Id vertex) const
    {
        return isTerminal_[vertex];
    }

    size_t numVertices() const {
        return numVertices_;
    }

    void buildTransitions()
    {
        transitions_ = std::vector<std::vector<Ids> >(
                                    numVertices_, std::vector<Ids>(ALPH_NUM));
        for (Id vertex = 0; vertex < numVertices_; ++vertex) {
            const Arcs& arcs = graph_[vertex];
            for (size_t arcId = 0; arcId < arcs.size(); ++arcId) {
                const Arc& arc = arcs[arcId];
                transitions_[vertex][getLabelId(arc.label)].push_back(arc.to);
            }
        }
    }

    static const int ALPH_NUM = 27;

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

    void rebuildGraph(int numComponents) {
        graph_ = std::vector<Arcs>(numComponents);
        for (size_t edgeId = 0; edgeId < edges_.size(); ++edgeId) {
            Edge& edge = edges_[edgeId];
            edge.from = componentIndex_[edge.from];
            edge.to = componentIndex_[edge.to];
            if (edge.label != EPSILON || edge.from != edge.to) {
                graph_[edge.from].push_back(Arc(edge.to, edge.label));
            }
        }
        std::vector<bool> isTerminal(numComponents);
        for (Id vertex = 0; vertex < numVertices_; ++vertex) {
            if (isTerminal_[vertex]) {
                isTerminal[componentIndex_[vertex]] = true;
            }
        }
        isTerminal_ = isTerminal;
        numVertices_ = numComponents;
        initVertex_ = componentIndex_[initVertex_];
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

    Id numVertices_;
    Id initVertex_;
    Edges edges_;
    Graph graph_;
    Ids verticesInOrder;
    Ids componentIndex_;
    std::vector<std::vector<Ids> > transitions_;
    std::vector<bool> isTerminal_;
    Ids verticesInOrder_;
    std::vector<bool> used_;
};

void readAutomaton(Automaton* automaton)
{
    size_t numVertices, numEdges, numTerminals;
    std::cin >> numVertices >> numEdges >> numTerminals;

    Ids terminals(numTerminals);
    for (size_t index = 0; index < numTerminals; ++index) {
        std::cin >> terminals[index];
    }

    Edges edges;
    edges.reserve(numEdges);

    for (size_t edgeId = 0; edgeId < numEdges; ++edgeId) {
        Id from, to;
        char label;
        std::cin >> from >> label >> to;
        edges.push_back(Edge(from, to, label));
    }
    *automaton = Automaton(numVertices, edges, terminals);
}

class LongestSubstringFinder
{
public:
    LongestSubstringFinder(const Automaton& automaton,
                           const std::string& word)
        : automaton_(automaton), word_(word),
        longestPathLength_(automaton.numVertices(),
                           std::vector<size_t>(word.size() + 1, UNDEFINED)),
        nextVertex_(automaton.numVertices(),
                    std::vector<size_t>(word.size() + 1, NON_EXISTENT)),
        nextPosition_(automaton.numVertices(),
                      std::vector<size_t>(word.size() + 1, NON_EXISTENT)),
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

        size_t result = NON_EXISTENT;
        size_t nextVertex = NON_EXISTENT;
        size_t nextPosition = NON_EXISTENT;
        if (automaton_.isTerminal(vertex)) {
            result = 0;
        }
        const Ids& epsAdjacent = automaton_.next(vertex, EPSILON);
        for (size_t index = 0; index < epsAdjacent.size(); ++index) {
            size_t candidateValue =
                            longestPathLength(epsAdjacent[index], position);
            if (candidateValue != NON_EXISTENT) {
                if (result == NON_EXISTENT || result < candidateValue) {
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
                if (candidateValue != NON_EXISTENT) {
                    if (result == NON_EXISTENT ||
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
            if (pathLength != NON_EXISTENT && maxLength < pathLength) {
                maxLength = pathLength;
                argMaxPosition = position;
            }
        }
        std::string result;
        if (maxLength > 0) {
            Id vertex = automaton_.initVertex();
            Id position = argMaxPosition;
            while (nextVertex_[vertex][position] != NON_EXISTENT) {
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

void readData(Automaton* automaton, std::string* word)
{
    readAutomaton(automaton);
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
    Automaton automaton;
    std::string word;
    readData(&automaton, &word);

    automaton.shrinkByEpsilonArcs();
    automaton.buildTransitions();

    std::string answer;
    solve(automaton, word, &answer);

    writeData(answer);

    return 0;
}
