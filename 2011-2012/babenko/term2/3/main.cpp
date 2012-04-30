#include <functional>
#include <cassert>
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
            assert(false); \
        } \
    } while (false)

const size_t ALPHABET_SIZE = 26;

typedef std::vector<std::vector<size_t> > Graph;
typedef std::vector<bool> Flags;

struct Production
{
    char nonTerminal;
    std::string output;
};

typedef std::vector<Production> Productions;

bool isTerminal(char c)
{
    return 'a' <= c && c <= 'z';
}

bool isNonTerminal(char c)
{
    return 'A' <= c && c <= 'Z';
}

size_t nonTerminalId(char c)
{
    REQUIRE(isNonTerminal(c), "Unknown non-terminal : " << c);
    return c - 'A';
}

size_t terminalId(char c)
{
    REQUIRE(isTerminal(c), "Unknown terminal : " << c);
    return c - 'a';
}

char terminalChar(size_t terminalId)
{
    REQUIRE(terminalId < ALPHABET_SIZE,
            "Unknown terminal id : " << terminalId);
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

void outData(std::string answer)
{
    std::sort(answer.begin(), answer.begin());
    std::cout << answer << std::endl;
}

void leaveSignificantOutputLetters(
        const Flags& canBeEmpty,
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

Graph buildGraphWithArcsFromSubstitutionPart(
            const Productions& productions)
{
    Graph graph(ALPHABET_SIZE);
    for (size_t index = 0; index < productions.size(); ++index) {
        size_t to = nonTerminalId(productions[index].nonTerminal);
        const std::string& output = productions[index].output;
        if (isEpsilon(output)) {
            continue;
        }
        for (size_t outputIndex = 0; outputIndex < output.size();
                                     ++outputIndex) {
            if (!isTerminal(output[outputIndex])) {
                size_t from = nonTerminalId(output[outputIndex]);
                graph[from].push_back(to);
            }
        }
    }
    for (size_t terminal = 0; terminal < ALPHABET_SIZE; ++terminal) {
        std::vector<size_t>& adjacent = graph[terminal];
        std::sort(adjacent.begin(), adjacent.end());
        adjacent.erase(std::unique(adjacent.begin(), adjacent.end()),
                       adjacent.end());
    }
    return graph;
}

size_t nonTerminalsCount(const std::string& word)
{
    return std::count_if(word.begin(), word.end(),
                         std::not1(std::ptr_fun(isTerminal)));
}

bool hasOnlyTerminalsOrIsEpsilon(const std::string& word)
{
    return isEpsilon(word) ||
           std::count_if(word.begin(), word.end(), isTerminal) ==
                                                static_cast<int>(word.size());
}

template <typename StartVertexChecker, typename LengthCounter>
Flags findSpecificNonTerminals(
        const Productions& productions, StartVertexChecker check,
        LengthCounter count)
{
    const size_t numProductions = productions.size();
    std::vector<size_t> outputReduction(numProductions);
    for (size_t index = 0; index < productions.size(); ++index) {
        const std::string& output = productions[index].output;
        outputReduction[index] = count(output);
    }
    std::queue<size_t> specificNonTerminals;
    Flags isProcessed(ALPHABET_SIZE);
    for (size_t index = 0; index < productions.size(); ++index) {
        const Production& production = productions[index];
        if (check(production.output)) {
            size_t nonTerminal = nonTerminalId(production.nonTerminal);
            if (!isProcessed[nonTerminal]) {
                specificNonTerminals.push(nonTerminal);
                isProcessed[nonTerminal] = true;
            }
        }
    }
    Flags result(ALPHABET_SIZE);
    while (!specificNonTerminals.empty()) {
        size_t nonTerminal = specificNonTerminals.front();
        specificNonTerminals.pop();
        result.at(nonTerminal) = true;
        for (size_t index = 0; index < numProductions; ++index) {
            const Production& production = productions[index];
            const std::string& output = production.output;
            if (isEpsilon(output)) {
                continue;
            }
            for (size_t outputIndex = 0; outputIndex < output.size();
                                         ++outputIndex) {
                if (!isTerminal(output[outputIndex]) &&
                        nonTerminalId(output[outputIndex]) == nonTerminal) {
                    --outputReduction[index];
                }
            }
            if (outputReduction[index] == 0) {
                size_t nonTerminal = nonTerminalId(production.nonTerminal);
                if (!isProcessed[nonTerminal]) {
                    specificNonTerminals.push(nonTerminal);
                    isProcessed[nonTerminal] = true;
                }
            }
        }
    }
    return result;
}

Flags findTerminatedNonTerminals(const Productions& productions)
{
    return findSpecificNonTerminals(productions,
                                    hasOnlyTerminalsOrIsEpsilon,
                                    nonTerminalsCount);
}

Flags findEmptyReachableNonTerminals(const Productions& productions)
{
    return findSpecificNonTerminals(productions,
                                    isEpsilon,
                                    std::mem_fun_ref(&std::string::length));
}

Productions leaveSignificantProductions(const Flags& terminated,
                                        const Productions& productions)
{
    Productions result;
    for (size_t index = 0; index < productions.size(); ++index) {
        const std::string& output = productions[index].output;
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
            result.push_back(productions[index]);
        }
    }
    return result;
}

void findAllStartTerminals(const Productions& productions, std::string* answer)
{
    Flags canBeEmpty = findEmptyReachableNonTerminals(productions);
    Flags isTerminated = findTerminatedNonTerminals(productions);
    Productions significantProductions =
                    leaveSignificantProductions(isTerminated, productions);

    leaveSignificantOutputLetters(canBeEmpty, &significantProductions);

    Graph graph =
            buildGraphWithArcsFromSubstitutionPart(significantProductions);

    Flags isInQueue(ALPHABET_SIZE);
    std::vector<Flags> canStartWith(ALPHABET_SIZE, Flags(ALPHABET_SIZE));

    std::queue<size_t> updatedQueue;

    for (size_t index = 0; index < significantProductions.size(); ++index) {
        const Production& production = significantProductions[index];
        const std::string& output = production.output;
        char lastLetter = output.at(output.size() - 1);
        if (!isEpsilon(output) && isTerminal(lastLetter)) {
            size_t nonTerminal = nonTerminalId(production.nonTerminal);
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
        for (size_t alph = 0; alph < ALPHABET_SIZE; ++alph) {
            if (canStartWith[nonTerminal][alph]) {
                for (size_t index = 0; index < adjacent.size(); ++index) {
                    size_t toNonTerminal = adjacent[index];
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
    for (size_t alph = 0; alph < ALPHABET_SIZE; ++alph) {
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
    findAllStartTerminals(productions, &answer);

    outData(answer);
    return 0;
}
