#include <algorithm>
#include <set>
#include <queue>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cassert>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
            assert(false); \
        } \
    } while (false)

std::vector<size_t> zFunction(const std::string& word)
{
    size_t length = word.length();
    std::vector<size_t> result(length);
    for (size_t i = 1, l = 0, r = 0; i < length; ++i) {
        if (i <= r)
            result[i] = std::min(r - i + 1, result[i - l]);
        while (i + result[i] < length &&
               word[result[i]] == word[i + result[i]]) {
            ++result[i];
        }
        if (i + result[i] - 1 > r) {
            l = i;
            r = i + result[i] - 1;
        }
    }
    return result;
}

void readData(std::string* word)
{
    std::cin >> *word;
}

size_t solve(const std::string& word)
{
    std::vector<size_t> zf = zFunction(word);
    size_t answer = 1;
    for (size_t i = 1; i < word.size(); ++i) {
        if (zf[i] + i == word.size() && word.size() % i == 0) {
            answer = word.size() / i;
            break;
        }
    }
    return answer;
}

void outData(size_t answer)
{
    std::cout << answer << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    std::string word;
    readData(&word);
    size_t result = solve(word);
    outData(result);

    return 0;
}
