#include <algorithm>
#include <set>
#include <queue>
#include <iostream>
#include <vector>
#include <string>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
        } \
    } while (false)

const size_t ALPH_SIZE = 26;

typedef std::vector<std::vector<size_t> > Graph;

struct Production
{
    char nonTerminal;
    std::string output;
};

typedef std::vector<Production> Productions;

bool isTerminal(char c)
{
    REQUIRE(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'),
            "Unknown symbol : " << c);
    return 'a' <= c && c <= 'z';
}

size_t nonTerminalId(char c)
{
    REQUIRE('A' <= c && c <= 'Z', "Unknown non-terminal : " << c);
    return c - 'A';
}

size_t terminalId(char c)
{
    REQUIRE('a' <= c && c <= 'z', "Unknown terminal : " << c);
    return c - 'a';
}

char terminalChar(size_t terminalId)
{
    REQUIRE(terminalId < ALPH_SIZE, "Unknown terminal id : " << terminalId);
    return 'a' + terminalId;
}

bool isEpsilon(const std::string& word)
{
    return word == "$";
}

void readData(Productions* productions)
{
    size_t numProductions;
    std::cin >> numProductions;
    for (size_t index = 0; index < numProductions; ++index) {
        std::string line;
        std::cin >> line;
        Production production;
        production.nonTerminal = line.at(0);
        production.output = line.substr(3, line.length());
        productions->push_back(production);
    }
}

void outData(const std::string& answer)
{
    std::string sorted(answer);
    std::sort(sorted.begin(), sorted.end());
    std::cout << sorted << std::endl;
}

void findEmptyReachableNonTerminals(const Productions& productions,
                                    std::vector<bool>* canBeEmpty)
{
    const size_t numProds = productions.size();
    std::vector<size_t> outputReduction(numProds);
    std::queue<size_t> canBeEmptyNonTerminals;
    std::vector<bool> processed(ALPH_SIZE);
    for (size_t index = 0; index < productions.size(); ++index) {
        const Production& production = productions[index];
        if (isEpsilon(production.output)) {
            size_t nonTerminal = nonTerminalId(production.nonTerminal);
            canBeEmptyNonTerminals.push(nonTerminal);
            processed[nonTerminal] = true;
        }
    }
    while (!canBeEmptyNonTerminals.empty()) {
        size_t nonTerminal = canBeEmptyNonTerminals.front();
        canBeEmptyNonTerminals.pop();
        canBeEmpty->at(nonTerminal) = true;
        for (size_t index = 0; index < numProds; ++index) {
            const Production& prod = productions[index];
            const std::string& output = prod.output;
            if (isEpsilon(output)) {
                continue;
            }
            for (size_t outputIndex = 0; outputIndex < output.size();
                                         ++outputIndex) {
                if (!isTerminal(output[outputIndex]) &&
                        nonTerminalId(output[outputIndex]) == nonTerminal) {
                    ++outputReduction[index];
                }
            }
            if (outputReduction[index] == output.size()) {
                size_t nonTerminal = nonTerminalId(prod.nonTerminal);
                if (!processed[nonTerminal]) {
                    canBeEmptyNonTerminals.push(nonTerminal);
                    processed[nonTerminal] = true;
                }
            }
        }
    }
}

void leaveSignificantOutputLetters(
        const std::vector<bool>& canBeEmpty,
        Productions* productions)
{
    for (size_t index = 0; index < productions->size(); ++index) {
        std::string& output = productions->at(index).output;
        if (isEpsilon(output)) {
            continue;
        }
        size_t firstNonEmptyOrTerminalIndex = 0;
        while (firstNonEmptyOrTerminalIndex < output.size() &&
               !isTerminal(output[firstNonEmptyOrTerminalIndex]) &&
               canBeEmpty[nonTerminalId(
                                    output[firstNonEmptyOrTerminalIndex])]) {
            ++firstNonEmptyOrTerminalIndex;
        }
        if (firstNonEmptyOrTerminalIndex < output.size()) {
            ++firstNonEmptyOrTerminalIndex;
        }
        output.erase(firstNonEmptyOrTerminalIndex);
    }
}

void buildGraph(const Productions& prods, Graph* graph)
{
    graph->clear();
    graph->reserve(ALPH_SIZE);
    std::vector<std::set<size_t> > tempGraph(ALPH_SIZE);
    for (size_t index = 0; index < prods.size(); ++index) {
        size_t to = nonTerminalId(prods[index].nonTerminal);
        const std::string& output = prods[index].output;
        if (isEpsilon(output)) {
            continue;
        }
        for (size_t outputIndex = 0; outputIndex < output.size();
                                     ++outputIndex) {
            if (!isTerminal(output[outputIndex])) {
                size_t from = nonTerminalId(output[outputIndex]);
                tempGraph[from].insert(to);
            }
        }
    }
    for (size_t terminal = 0; terminal < ALPH_SIZE; ++terminal) {
        const std::set<size_t>& adjacent = tempGraph[terminal];
        graph->push_back(
                    std::vector<size_t>(adjacent.begin(), adjacent.end()));
    }
}

std::vector<bool> findTerminatedNonTerminals(
        const std::vector<bool>& canBeEmpty,
        const Productions& prods)
{
    std::vector<size_t> nonTerminalCount(prods.size());
    for (size_t index = 0; index < prods.size(); ++index) {
        const std::string& output = prods[index].output;
        if (isEpsilon(output)) {
            continue;
        }
        for (size_t i = 0; i < output.size(); ++i) {
            if (!isTerminal(output[i])) {
                ++nonTerminalCount[index];
            }
        }
    }

    std::vector<bool> result(ALPH_SIZE);
    std::queue<size_t> terminatedQueue;
    std::vector<bool> processed(ALPH_SIZE);

    for (size_t index = 0; index < prods.size(); ++index) {
        const std::string& output = prods[index].output;
        if (isEpsilon(output)) {
            size_t id = nonTerminalId(prods[index].nonTerminal);
            processed[id] = true;
            terminatedQueue.push(id);
            continue;
        }
        bool areAllTerminals = true;
        for (size_t i = 0; i < output.size(); ++i) {
            if (!isTerminal(output[i])) {
                areAllTerminals = false;
                break;
            }
        }
        if (areAllTerminals) {
            size_t id = nonTerminalId(prods[index].nonTerminal);
            if (!processed[id]) {
                processed[id] = true;
                terminatedQueue.push(id);
            }
        }
    }

    while (!terminatedQueue.empty()) {
        size_t nonTerminal = terminatedQueue.front();
        terminatedQueue.pop();
        result[nonTerminal] = true;
        for (size_t index = 0; index < prods.size(); ++index) {
            const Production& prod = prods[index];
            const std::string& output = prod.output;
            if (isEpsilon(output)) {
                continue;
            }
            for (size_t i = 0; i < output.size(); ++i) {
                if (!isTerminal(output[i]) &&
                            nonTerminalId(output[i]) == nonTerminal) {
                    --nonTerminalCount[index];
                }
            }
            if (nonTerminalCount[index] == 0) {
                size_t id = nonTerminalId(prod.nonTerminal);
                if (!processed[id]) {
                    processed[id] = true;
                    terminatedQueue.push(id);
                }
            }
        }
    }

    return result;
}

Productions findSignificantProductions(const std::vector<bool>& terminated,
                                       const Productions& prods)
{
    Productions result;
    for (size_t index = 0; index < prods.size(); ++index) {
        const std::string& output = prods[index].output;
        if (isEpsilon(output)) {
            continue;
        }
        bool isFake = false;
        for (size_t i = 0; i < output.size(); ++i) {
            if (!isTerminal(output[i]) &&
                !terminated.at(nonTerminalId(output[i]))) {
                isFake = true;
                break;
            }
        }
        if (!isFake) {
            result.push_back(prods[index]);
        }
    }
    return result;
}

void solve(const Productions& productions, std::string* answer)
{
    std::vector<bool> canBeEmpty(ALPH_SIZE);
    findEmptyReachableNonTerminals(productions, &canBeEmpty);

    std::vector<bool> terminated =
                        findTerminatedNonTerminals(canBeEmpty, productions);

    Productions prods =
                findSignificantProductions(terminated, productions);

    leaveSignificantOutputLetters(canBeEmpty, &prods);

    Graph graph;
    buildGraph(prods, &graph);

    std::vector<bool> isInQueue(ALPH_SIZE);
    std::vector<std::vector<bool> > canStartWith(ALPH_SIZE,
                                    std::vector<bool>(ALPH_SIZE));

    std::queue<size_t> updatedQueue;

    for (size_t index = 0; index < prods.size(); ++index) {
        const std::string& output = prods[index].output;
        char lastLetter = output.at(output.size() - 1);
        if (!isEpsilon(output) && isTerminal(lastLetter) &&
                        terminated[nonTerminalId(prods[index].nonTerminal)]) {
            size_t nonTerminal = nonTerminalId(prods[index].nonTerminal);
            canStartWith.at(nonTerminal).at(terminalId(lastLetter)) = true;
            if (!isInQueue[nonTerminal]) {
                isInQueue[nonTerminal] = true;
                updatedQueue.push(nonTerminal);
            }
        }
    }

    while (!updatedQueue.empty()) {
        size_t nonTerminal = updatedQueue.front();
        updatedQueue.pop();
        isInQueue[nonTerminal] = false;
        const std::vector<size_t>& adjacent = graph.at(nonTerminal);
        for (size_t alph = 0; alph < ALPH_SIZE; ++alph) {
            if (canStartWith[nonTerminal][alph]) {
                for (size_t index = 0; index < adjacent.size(); ++index) {
                    size_t toNonTerminal = adjacent[index];
                    if (!terminated[toNonTerminal]) {
                        continue;
                    }
                    if (!canStartWith[toNonTerminal][alph]) {
                        canStartWith[toNonTerminal][alph] = true;
                        if (!isInQueue[toNonTerminal]) {
                            isInQueue[toNonTerminal] = true;
                            updatedQueue.push(toNonTerminal);
                        }
                    }
                }
            }
        }
    }

    const size_t START_NON_TERMINAL = nonTerminalId('S');
    answer->clear();
    for (size_t alph = 0; alph < ALPH_SIZE; ++alph) {
        if (canStartWith[START_NON_TERMINAL][alph]) {
            answer->push_back(terminalChar(alph));
        }
    }
}

int main()
{
    Productions productions;
    readData(&productions);
    std::string answer;
    solve(productions, &answer);
    outData(answer);
    return 0;
}
